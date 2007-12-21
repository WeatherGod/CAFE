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
		myIsConfigured(false),
		myTagWords(0)
{
}

void Variable::GetConfigInfo(string &FileLine, fstream &ReadData)
{
	InitTagWords();

	bool BadObject = false;

	while (!FoundEndTag(FileLine, myTagWords[0]) && !ReadData.eof())
        {
		if (!BadObject)
		{
	                if (FoundStartTag(FileLine, myTagWords[1]))             // Name
        	        {
                	        myCAFEVariableName = RipWhiteSpace(StripTags(FileLine, myTagWords[1]));
	                }
        	        else if (FoundStartTag(FileLine, myTagWords[2]))        // Levels
			{
				vector <string> TempHold = TakeDelimitedList(StripTags(FileLine, myTagWords[2]), ',');
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

	myTagWords.resize(0);				// don't need to waste space holding these around...
}

bool Variable::ValidConfig() const
{
	return(myIsConfigured);
}

bool Variable::IsValid() const
{
	return(myIsConfigured);
}

string Variable::GiveCAFEVariableName() const
{
	return(myCAFEVariableName);
}

vector <string> Variable::GiveCAFELevels() const
// be in alphabetical order.
{
	return(myCAFELevels);
}

string Variable::GiveCAFELevel(const size_t &LevelIndex) const
// if LevelIndex is an invalid index or a string::npos, an empty string will be returned
{
	if (LevelIndex == string::npos || LevelIndex >= myCAFELevels.size())
	{
		return("");
	}

	return(myCAFELevels[LevelIndex]);
}

size_t Variable::GiveLevelCount() const
{
	return(myCAFELevels.size());
}

bool Variable::AddCAFELevel(const string &CAFELevelName)
// returns true if the Level is valid, not if the level was actually inserted.
// If a duplicate, but valid level is given, it will still return true,
// but the duplicate level will not be inserted.
{
	if (CAFELevelName.empty())
	{
		return(false);
	}

	if (!binary_search(myCAFELevels.begin(), myCAFELevels.end(), CAFELevelName))
	{
		myCAFELevels.insert(lower_bound(myCAFELevels.begin(), myCAFELevels.end(), CAFELevelName),
				    CAFELevelName);
	}

	return(true);
}

void Variable::InitTagWords()
{
	if (myTagWords.empty())
	{
		myTagWords.push_back("Variable");
		myTagWords.push_back("Name");
		myTagWords.push_back("Levels");
	}
}

#endif
