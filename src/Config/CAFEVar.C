#ifndef _CAFEVAR_C
#define _CAFEVAR_C

using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>				// for lower_bound(), binary_search(), find()

#include "Config/CAFEVar.h"

#include <StrUtly.h>				// for TakeDelimitedList(), RipWhiteSpace(), StrToSize_t(), StripWhiteSpace()
#include <ConfigUtly.h>				// for ReadNoComments(), StripTags(), FoundStartTag(), FoundEndTag()

CAFEVarID_t::CAFEVarID_t()
	:	myCAFEVarIndex(string::npos),
		myCAFEVarName("")
{
}

CAFEVarID_t::CAFEVarID_t(const size_t &CAFEVarIndex)
	:	myCAFEVarIndex(CAFEVarIndex),
		myCAFEVarName("")
{
}

CAFEVarID_t::CAFEVarID_t(const string &CAFEVarName)
	:	myCAFEVarIndex(string::npos),
		myCAFEVarName(CAFEVarName)
{
}

size_t CAFEVarID_t::GiveIndex(const vector <CAFEVar> &TheCAFEVars) const
{
	if (myCAFEVarIndex != string::npos)
	{
		return(myCAFEVarIndex);
	}

	if (!myCAFEVarName.empty())
	{
		if (binary_search(TheCAFEVars.begin(), TheCAFEVars.end(), myCAFEVarName))
		{
			return(lower_bound(TheCAFEVars.begin(), TheCAFEVars.end(), myCAFEVarName) - TheCAFEVars.begin());
		}
	}

	return(string::npos);
}


CAFEVar::CAFEVar()
	:	myCAFEVarName(""),
		myCAFELevelIndicies(),
		myCAFELevelNames(),
		myIsConfigured(false),
		myTagWords(0)
{
}

void CAFEVar::GetConfigInfo(string &FileLine, fstream &ReadData)
{
	InitTagWords();

	bool BadObject = false;
	
	while (!FoundEndTag(FileLine, myTagWords[0]) && !ReadData.eof())
	{
		if (!BadObject)
		{
			if (FoundStartTag(FileLine, myTagWords[1]))		// Name
			{
				myCAFEVarName = RipWhiteSpace(StripTags(FileLine, myTagWords[1]));
			}
			else if (FoundStartTag(FileLine, myTagWords[2]))	//Level
			{
				string LevelInfo = StripTags(FileLine, myTagWords[2]);
				if (RipWhiteSpace(LevelInfo) != "")
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
					// empty level tags were found.
					// should I issue a warning?
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
		// note, a CAFEVar can have no levels, so I am not going to check to see if there are any levels.
		myIsConfigured = true;
	}

	myTagWords.resize(0);
}

bool CAFEVar::ValidConfig() const
{
	return(myIsConfigured);
}

bool CAFEVar::IsValid() const
{
	// need to change this code to do some transaction checking, to make sure everything connects together.
	return(myIsConfigured);
}

string CAFEVar::GiveCAFEVarName() const
{
	return(myCAFEVarName);
}

string CAFEVar::GiveCAFELevelName(const size_t &CAFELevelIndex) const
{
	if (CAFELevelIndex == string::npos)
	{
		return("");
	}

	vector<size_t>::const_iterator MatchPos = find(myCAFELevelIndicies.begin(), myCAFELevelIndicies.end(), CAFELevelIndex);

	if (MatchPos == myCAFELevelIndicies.end())
        {
                return("");
        }

	return(myCAFELevelNames[MatchPos - myCAFELevelIndicies.begin()]);
}

vector <string> CAFEVar::GiveCAFELevelNames() const
// NOTE: this function returns the LevelNames in the order of the LevelNames!
{
	return(myCAFELevelNames);
}

size_t CAFEVar::GiveCAFELevelCount() const
{
	return(myCAFELevelIndicies.size());
}

size_t CAFEVar::GiveCAFELevelIndex(const string &CAFELevelName) const
// returns string::npos if the list is zero size or no match was made
{
	if (!binary_search(myCAFELevelNames.begin(), myCAFELevelNames.end(), CAFELevelName))
	{
		return(string::npos);
	}

	const size_t MatchPos = lower_bound(myCAFELevelNames.begin(), myCAFELevelNames.end(), CAFELevelName) - myCAFELevelNames.begin();

	return(myCAFELevelIndicies[MatchPos]);
}

vector <size_t> CAFEVar::GiveCAFELevelIndicies() const
// NOTE: This function returns the official LevelIndex values in the order of the LevelNames.
// They are not sorted by their indicies!
{
        return(myCAFELevelIndicies);
}


bool CAFEVar::AddCAFELevel(const string &NewCAFELevel, const size_t &CAFELevelIndex)
// Returns true if NewCAFELevel and CAFELevelIndex are a valid pair,
// NOT if the pairing were actually entered into the CAFEVar!
// if a pairing matches a pair that already exists, it is a valid pair,
// but a duplicate entry won't be added into CAFEVar.

// The CAFELevels will be sorted by the LevelNames.
{
	if (NewCAFELevel.empty())
	{
		cerr << "ERROR -- NewCAFELevel is an empty string!" << endl;
                return(false);
	}

	if (!binary_search(myCAFELevelNames.begin(), myCAFELevelNames.end(), NewCAFELevel))
	{
		if (CAFELevelIndex != string::npos)
		{
			vector<string>::iterator InsertPos = lower_bound(myCAFELevelNames.begin(), myCAFELevelNames.end(), NewCAFELevel);
			const size_t InsertIndex = InsertPos - myCAFELevelNames.begin();
			myCAFELevelNames.insert(InsertPos, NewCAFELevel);
			myCAFELevelIndicies.insert(myCAFELevelIndicies.begin() + InsertIndex, CAFELevelIndex);
			return(true);
		}
		else
		{
			cerr << "ERROR -- string::npos for CAFELevelIndex." << endl;
			cerr << "Was the LevelIndex originally a valid, positive integer?" << endl;
			return(false);
		}
	}
	else
	{
		const size_t MatchPos = lower_bound(myCAFELevelNames.begin(), myCAFELevelNames.end(), NewCAFELevel) - myCAFELevelNames.begin();
		if (myCAFELevelIndicies[MatchPos] == CAFELevelIndex)
		{
			cerr << "WARNING -- Duplicate CAFELevelName/CAFELevelIndex used: " << NewCAFELevel << '/' << CAFELevelIndex << endl;
			return(true);
		}
		else
		{
			cerr << "ERROR -- Duplicate CAFELevelNames used.  No two CAFELevelIndicies can be associated" << endl;
			cerr << "with the same CAFELevelName." << endl;
			return(false);
		}
	}
}

void CAFEVar::InitTagWords()
{
	if (myTagWords.empty())
	{
		myTagWords.push_back("CAFEVar");
		myTagWords.push_back("Name");
		myTagWords.push_back("Level");
	}
}

bool operator == (const CAFEVar &Lefty, const CAFEVar &Righty)
{
	return(Lefty.myCAFEVarName == Righty.myCAFEVarName);
}

bool operator != (const CAFEVar &Lefty, const CAFEVar &Righty)
{
	return(Lefty.myCAFEVarName != Righty.myCAFEVarName);
}

bool operator > (const CAFEVar &TheVar, const string &CAFEVarName)
{
	return(TheVar.myCAFEVarName > CAFEVarName);
}

bool operator < (const CAFEVar &TheVar, const string &CAFEVarName)
{
        return(TheVar.myCAFEVarName < CAFEVarName);
}

bool operator >= (const CAFEVar &TheVar, const string &CAFEVarName)
{
        return(TheVar.myCAFEVarName >= CAFEVarName);
}

bool operator <= (const CAFEVar &TheVar, const string &CAFEVarName)
{
        return(TheVar.myCAFEVarName <= CAFEVarName);
}

bool operator == (const CAFEVar &TheVar, const string &CAFEVarName)
{
        return(TheVar.myCAFEVarName == CAFEVarName);
}

bool operator != (const CAFEVar &TheVar, const string &CAFEVarName)
{
        return(TheVar.myCAFEVarName != CAFEVarName);
}

bool operator > (const string &CAFEVarName, const CAFEVar &TheVar)
{
        return(CAFEVarName > TheVar.myCAFEVarName);
}

bool operator < (const string &CAFEVarName, const CAFEVar &TheVar)
{
        return(CAFEVarName < TheVar.myCAFEVarName);
}

bool operator >= (const string &CAFEVarName, const CAFEVar &TheVar)
{
        return(CAFEVarName >= TheVar.myCAFEVarName);
}

bool operator <= (const string &CAFEVarName, const CAFEVar &TheVar)
{
        return(CAFEVarName <= TheVar.myCAFEVarName);
}

bool operator == (const string &CAFEVarName, const CAFEVar &TheVar)
{
        return(CAFEVarName == TheVar.myCAFEVarName);
}

bool operator != (const string &CAFEVarName, const CAFEVar &TheVar)
{
        return(CAFEVarName != TheVar.myCAFEVarName);
}

#endif
