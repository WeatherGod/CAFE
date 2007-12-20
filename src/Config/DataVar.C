#ifndef _DATAVAR_C
#define _DATAVAR_C

using namespace std;

#include <iostream>
#include <fstream>

#include <vector>
#include <map>
#include <string>
#include <algorithm>			// for binary_search() and lower_bound()

#include "Config/DataVar.h"

#include <StrUtly.h>				// for RipWhiteSpace(), StripWhiteSpace(), TakeDelimitedList(), StrToInt(), StrToUnsignedInt()
#include <ConfigUtly.h>				// for FoundStartTag(), FoundEndTag(), StripTags(), ReadNoComments()

DataVar::DataVar()
	:	myDataVarName(""),
		myCAFEVarName(""),
		myDataLevels(),
		myIsConfigured(false),
		myTagWords(0)
{
}


void DataVar::GetConfigInfo(string &FileLine, fstream &ReadData)
{
	InitTagWords();

	bool BadObject = false;

	while (!FoundEndTag(FileLine, myTagWords[0]) && !ReadData.eof())
	{
		if (!BadObject)
		{
			if (FoundStartTag(FileLine, myTagWords[1]))		//Name
			{
				myDataVarName = RipWhiteSpace(StripTags(FileLine, myTagWords[1]));
			}
			else if (FoundStartTag(FileLine, myTagWords[2]))	//CAFEname
			{
				myCAFEVarName = RipWhiteSpace(StripTags(FileLine, myTagWords[2]));
			}
			else if (FoundStartTag(FileLine, myTagWords[3]))	//Level
			{
				vector <string>	TempHold = TakeDelimitedList(StripTags(FileLine, myTagWords[3]), '=');
				StripWhiteSpace(TempHold);		// all elements have their whitespaces stripped.
				
				if (TempHold.size() == 2 && !TempHold[1].empty() && TempHold[1][0] == '$')
				{
					TempHold[1][0] = ' ';	// replace the dollar sign with a space
					StripWhiteSpace(TempHold[1]);

					if (!AddDataLevel(TempHold[0], StrToSize_t(TempHold[1])))
					{
						BadObject = true;
						cerr << "Problem in DataVar object." << endl;
						cerr << "The line was: " << FileLine << endl;
					}
				}
				else if (TempHold.size() == 1 && !TempHold[0].empty())
				{
					if (!AddDataLevel(TempHold[0], string::npos))
                                        {
                                                BadObject = true;
                                                cerr << "Problem in DataVar object." << endl;
                                                cerr << "The line was: " << FileLine << endl;
                                        }
				}
				else
				{
					BadObject = true;
					cerr << "Problem in DataVar object:" << endl;
					cerr << "Incorrect format.  Levels must be denoted as 'Level = $index'.  For example:\n";
	                                cerr << "<Level>1000,850,851 = $0</Level>" << endl;
        	                        cerr << "The line was: " << FileLine << endl;
				}
			}
			else
			{
				BadObject = true;
				cerr << "Problem in DataVar object... couldn't recognize anything.\n Here is the line: " << FileLine << endl;
			}
		}// end if !BadObject
		
		FileLine = ReadNoComments(ReadData);
	}// end while loop

	if (!myDataVarName.empty() && !myCAFEVarName.empty() && !ReadData.eof() && !BadObject)
	{
        	myIsConfigured = true;
	}

	myTagWords.resize(0);
}

bool DataVar::ValidConfig() const
{
	return(myIsConfigured);
}

bool DataVar::IsValid() const
{
	return(myIsConfigured);
}

string DataVar::GiveDataVarName() const
{
	return(myDataVarName);
}

string DataVar::GiveCAFEVarName() const
{
	return(myCAFEVarName);
}

string DataVar::GiveDataLevel(const size_t &CAFELevelIndex) const
{
	if (myDataLevels.find(CAFELevelIndex) == myDataLevels.end())
        {
                return("");
        }

	
	return(myDataLevels.find(CAFELevelIndex)->second);
}

size_t DataVar::GiveDataLevelCount() const
{
	return(myDataLevels.size());
}

bool DataVar::AddDataLevel(const string &NewDataLevel, const size_t &CAFELevelIndex)
{
	if (myDataLevels.find(CAFELevelIndex) == myDataLevels.end())
        {
		if (!NewDataLevel.empty())
                {
   	        	myDataLevels[CAFELevelIndex] = NewDataLevel;
			return(true);
                }
                else
                {
               		cerr << "ERROR -- Empty string for DataLevel value." << endl;
                }
	}
        else
        {
               	cerr << "ERROR -- Duplicate DataLevelIndex used.  No two datalevels can be associated" << endl;
                cerr << "with the same CAFE level." << endl;
        }

	return(false);		// if you can reach here, something was wrong and a level could not be added.
}

void DataVar::InitTagWords()
{
	if (myTagWords.empty())
	{
		myTagWords.push_back("DataVar");
		myTagWords.push_back("Name");
		myTagWords.push_back("CAFEname");
		myTagWords.push_back("Level");
	}
}

#endif
