#ifndef _VARIABLE_C
#define _VARIABLE_C

using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>				// for size_t
#include <string>
#include <algorithm>				// for binary_search(), lower_bound()

#include "Config/Variable.h"

#include <StrUtly.h>				// for RipWhiteSpace(), StripWhiteSpace(), TakeDelimitedList()
#include <ConfigUtly.h>				// for ReadNoComments(), StripTags(), FoundStartTag(), FoundEndTag()


Variable::Variable()
	:	myCAFEVariableName(""),
		myCAFELevels(0),
		myIsConfigured(false)
{
}

Variable::Variable(const Variable &varCopy)
	:	myCAFEVariableName(varCopy.myCAFEVariableName),
		myCAFELevels(varCopy.myCAFELevels),
		myIsConfigured(varCopy.myIsConfigured)
{
}

Variable::Variable(const string &CAFEName,
		   const vector<string> &CAFELevels)
	:	myCAFEVariableName(CAFEName),
		myCAFELevels(CAFELevels),
		myIsConfigured(true)
{
}

Variable::Variable(const string &CAFEName,
		   const set<string> &CAFELevels)
	:	myCAFEVariableName(CAFEName),
		myCAFELevels(CAFELevels.begin(), CAFELevels.end()),
		myIsConfigured(true)
{
}


void Variable::GetConfigInfo(string &FileLine, fstream &ReadData)
{
	const vector<string> TagWords = InitTagWords();

	bool BadObject = false;

	while (!FoundEndTag(FileLine, TagWords[0]) && !ReadData.eof())
        {
		if (!BadObject)
		{
	                if (FoundStartTag(FileLine, TagWords[1]))             // Name
        	        {
                	        myCAFEVariableName = RipWhiteSpace(StripTags(FileLine, TagWords[1]));
	                }
        	        else if (FoundStartTag(FileLine, TagWords[2]))        // Levels
			{
				vector <string> TempHold = TakeDelimitedList(StripTags(FileLine, TagWords[2]), ',');
				StripWhiteSpace(TempHold);
				for (size_t LevIndex = 0; LevIndex < TempHold.size(); LevIndex++)
				{
					if (!AddCAFELevel(TempHold[LevIndex]))
					{
						BadObject = true;
						cerr << "ERROR -- Problem in Level object in Variable object... Here is the line: " << FileLine << endl;
					}
				}
			}
			else
			{
				BadObject = true;
				cerr << "ERROR -- Problem in Variable object... Here is the line: " << FileLine << endl;
			}
		}// end if !BadObject

		FileLine = ReadNoComments(ReadData);
	}

	if (!ReadData.eof() && !myCAFEVariableName.empty() && !BadObject)
	{
		myIsConfigured = true;
	}
}

bool Variable::ValidConfig() const throw()
{
	return(myIsConfigured);
}

bool Variable::IsValid() const throw()
{
	return(myIsConfigured);
}

const string&
Variable::GiveCAFEVariableName() const throw()
{
	return(myCAFEVariableName);
}

const vector<string>&
Variable::GiveCAFELevels() const throw()
// be in alphabetical order.
{
	return(myCAFELevels);
}

size_t Variable::GiveLevelCount() const throw()
{
	return(myCAFELevels.size());
}

bool Variable::AddCAFELevel(const string &CAFELevelName)
// returns true if the Level is valid, not if the level was actually inserted.
// If a duplicate, but valid level is given, it will still return true,
// but the duplicate level will not be inserted.
{
	if (!binary_search(myCAFELevels.begin(), myCAFELevels.end(), CAFELevelName))
	{
		myCAFELevels.insert(lower_bound(myCAFELevels.begin(), myCAFELevels.end(), CAFELevelName),
				    CAFELevelName);
	}

	return(true);
}

vector<string> Variable::InitTagWords() const
{
	vector<string> TagWords(3);

	TagWords[0] = "Variable";
	TagWords[1] = "Name";
	TagWords[2] = "Levels";
	
	return(TagWords);
}

#endif
