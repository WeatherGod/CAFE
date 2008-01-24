#ifndef _CAFEVAR_C
#define _CAFEVAR_C

using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cctype>				// for size_t
#include <string>
#include <algorithm>				// for lower_bound(), binary_search(), find()

#include "Utils/CAFEException.h"		// for CAFELevel_Not_Found
#include "Config/CAFEVar.h"

#include <StrUtly.h>				// for TakeDelimitedList(), RipWhiteSpace(), StrToSize_t(), StripWhiteSpace()
#include <ConfigUtly.h>				// for ReadNoComments(), StripTags(), FoundStartTag(), FoundEndTag()

CAFEVar::CAFEVar()
	:	myCAFEVarName(""),
		myCAFELevels(),
		myIsConfigured(false)
{
}

CAFEVar::CAFEVar(const CAFEVar &varCopy)
	:	myCAFEVarName(varCopy.myCAFEVarName),
		myCAFELevels(varCopy.myCAFELevels),
		myIsConfigured(varCopy.myIsConfigured)
{
}

CAFEVar::CAFEVar(const string &varName, const map<string, size_t> &CAFELevels)
	:	myCAFEVarName(varName),
		myCAFELevels(CAFELevels),
		myIsConfigured(true)
{
}

void
CAFEVar::GetConfigInfo(string &FileLine, fstream &ReadData)
{
	const vector<string> TagWords = InitTagWords();

	bool BadObject = false;
	
	while (!FoundEndTag(FileLine, TagWords[0]) && !ReadData.eof())
	{
		if (!BadObject)
		{
			if (FoundStartTag(FileLine, TagWords[1]))		// Name
			{
				myCAFEVarName = RipWhiteSpace(StripTags(FileLine, TagWords[1]));
			}
			else if (FoundStartTag(FileLine, TagWords[2]))	//Level
			{
				string LevelInfo = StripTags(FileLine, TagWords[2]);
				if (!RipWhiteSpace(LevelInfo).empty())
				{
					vector <string> TempHold = TakeDelimitedList(LevelInfo, '=');
					StripWhiteSpace(TempHold);

					if (TempHold.size() == 2 && !TempHold[0].empty() && TempHold[0][0] == '$')
					{
						TempHold[0][0] = ' ';   // replace the dollar sign with a space
						StripWhiteSpace(TempHold[0]);
					
						if (!AddCAFELevel(TempHold[1], StrToSize_t(TempHold[0])))
						{
							BadObject = true;
							cerr << "Problem in CAFEVar object." << endl;
							cerr << "The line was: " << FileLine << endl;
						}
					}
					else
					{
						BadObject = true;
						cerr << "Problem in CAFEVar object:" << endl;
						cerr << "Incorrect format.  Levels must be denoted as '$index = LevelName'.  For example:\n";
						cerr << "<Level>$0 = 925</Level>" << endl;
						cerr << "The line was: " << FileLine << endl;
					}
				}
				else
				{
					// Empty Level tags just forces the empty name default
					// with a default level index of string::npos
					AddCAFELevel("", string::npos);
				}
			}
			else
			{
				BadObject = true;
				cerr << "Problem with CAFEVar object.. Couldn't recognize anything.\nHere is the line: " << FileLine << endl;
			}
		}//end if !BadObject

		FileLine = ReadNoComments(ReadData);
	}// end while loop

	if (!myCAFEVarName.empty() && !ReadData.eof() && !BadObject)
	{
		// NOTE: A CAFEVar can have no levels, so I am not going to check to see if there are any levels.
		//     : If there are no levels given, then a default empty level will be
		//     : automatically supplied.
		if (myCAFELevels.empty())
		{
			AddCAFELevel("", string::npos);
		}

		myIsConfigured = true;
	}
}

bool
CAFEVar::ValidConfig() const throw()
{
	return(myIsConfigured);
}

bool
CAFEVar::IsValid() const throw()
{
	// need to change this code to do some transaction checking, to make sure everything connects together.
	return(myIsConfigured);
}

const map<string, size_t>&
CAFEVar::GiveCAFELevels() const throw()
{
	return(myCAFELevels);
}

const string&
CAFEVar::GiveCAFEVarName() const throw()
{
	return(myCAFEVarName);
}


vector<string>
CAFEVar::GiveCAFELevelNames() const
// NOTE: this function returns the LevelNames in the order of the LevelNames!
{
	vector<string> levelNames;
	levelNames.reserve(GiveCAFELevels().size());

	for (map<string, size_t>::const_iterator aLevel = GiveCAFELevels().begin();
	     aLevel != GiveCAFELevels().end();
	     aLevel++)
	{
		levelNames.push_back(aLevel->first);
	}

	return(levelNames);
}

size_t
CAFEVar::GiveCAFELevelCount() const throw()
{
	return(GiveCAFELevels().size());
}

const size_t&
CAFEVar::GiveCAFELevelIndex(const string &CAFELevelName) const throw(CAFELevel_Not_Found)
// Throws CAFELevel_Not_Found if level name is not valid.
{
	const map<string, size_t>::const_iterator levelFind = myCAFELevels.find(CAFELevelName);
	if (myCAFELevels.end() == levelFind)
	{
		throw CAFELevel_Not_Found("CAFEVar::GiveCAFELevelIndex()",
					  CAFELevelName, myCAFEVarName);
	}
	else
	{
		return(levelFind->second);
	}
}

vector<size_t> 
CAFEVar::GiveCAFELevelIndicies() const
// NOTE: This function returns the official LevelIndex values in the order of the LevelNames.
// They are not sorted by their indicies!
{
	vector<size_t> levelIndicies;
	levelIndicies.reserve(myCAFELevels.size());

	for (map<string, size_t>::const_iterator aLevel = myCAFELevels.begin();
	     aLevel != myCAFELevels.end();
	     aLevel++)
	{
		levelIndicies.push_back(aLevel->second);
	}

        return(levelIndicies);
}


bool 
CAFEVar::AddCAFELevel(const string &NewCAFELevel, const size_t &CAFELevelIndex)
// Returns true if NewCAFELevel and CAFELevelIndex are a valid pair,
// NOT if the pairing were actually entered into the CAFEVar!
// if a pairing matches a pair that already exists, it is a valid pair,
// but a duplicate entry won't be added into CAFEVar.
{
	// TODO: Reconsider the error-checking.  Maybe have an error if
	//       it is an empty string and a non string::npos index?
/*
	if (NewCAFELevel.empty())
	{
		cerr << "ERROR -- NewCAFELevel is an empty string!" << endl;
                return(false);
	}
*/

	const map<string, size_t>::const_iterator levelFind = myCAFELevels.find(NewCAFELevel);

	if (myCAFELevels.end() == levelFind)
	{
		myCAFELevels.insert(myCAFELevels.end(), make_pair(NewCAFELevel, CAFELevelIndex));
		return(true);
	}
	else
	{
		throw CAFEException("CAFEVar::AddCAFELevel()",
				    "Duplicate CAFELevelNames used. No two LevelIndicies can be associated with the same CAFE Level Name");
	}
}

vector<string> CAFEVar::InitTagWords() const
{
	vector<string> TagWords(3);
	TagWords[0] = "CAFEVar";
	TagWords[1] = "Name";
	TagWords[2] = "Level";

	return(TagWords);
}

#endif
