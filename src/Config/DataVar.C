#ifndef _DATAVAR_C
#define _DATAVAR_C

using namespace std;

#include <iostream>
#include <fstream>

#include <vector>
#include <cctype>			// for size_t
#include <map>
#include <string>
#include <algorithm>			// for binary_search() and lower_bound()

#include "Utils/CAFEException.h"	// for DataLevel_Not_Found
#include "Config/DataVar.h"

#include <StrUtly.h>				// for RipWhiteSpace(), StripWhiteSpace(), TakeDelimitedList(), StrToInt(), StrToSize_t()
#include <ConfigUtly.h>				// for FoundStartTag(), FoundEndTag(), StripTags(), ReadNoComments()

DataVar::DataVar()
	:	myDataVarName(""),
		myCAFEVarName(""),
		myDataLevels(),
		myIsConfigured(false)
{
}

DataVar::DataVar(const DataVar &varCopy)
	:	myDataVarName(varCopy.myDataVarName),
		myCAFEVarName(varCopy.myCAFEVarName),
		myDataLevels(varCopy.myDataLevels),
		myIsConfigured(varCopy.myIsConfigured)
{
}

DataVar::DataVar(const string &dataName, const string &CAFEName,
		 const map<size_t, string> &dataLevels)
	:	myDataVarName(dataName),
		myCAFEVarName(CAFEName),
		myDataLevels(dataLevels),
		myIsConfigured(true)
{
}


void DataVar::GetConfigInfo(string &FileLine, fstream &ReadData)
{
	const vector<string> TagWords = InitTagWords();

	bool BadObject = false;

	while (!FoundEndTag(FileLine, TagWords[0]) && !ReadData.eof())
	{
		if (!BadObject)
		{
			if (FoundStartTag(FileLine, TagWords[1]))		//Name
			{
				myDataVarName = RipWhiteSpace(StripTags(FileLine, TagWords[1]));
			}
			else if (FoundStartTag(FileLine, TagWords[2]))	//CAFEname
			{
				myCAFEVarName = RipWhiteSpace(StripTags(FileLine, TagWords[2]));
			}
			else if (FoundStartTag(FileLine, TagWords[3]))	//Level
			{
				string levelInfo = StripTags(FileLine, TagWords[3]);
				if (!RipWhiteSpace(levelInfo).empty())
				{
					vector <string>	TempHold = TakeDelimitedList(levelInfo, '=');
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
					// Empty level tags.  So just force the default empty level.
					AddDataLevel("", string::npos);
				}
			}
		}// end if !BadObject
		
		FileLine = ReadNoComments(ReadData);
	}// end while loop

	if (!myDataVarName.empty() && !myCAFEVarName.empty() && !ReadData.eof() && !BadObject)
	{
        	myIsConfigured = true;
		// If there are no levels, then force-add a default empty level.
		if (myDataLevels.empty())
		{
			AddDataLevel("", string::npos);
		}
	}
}

bool DataVar::ValidConfig() const throw()
{
	return(myIsConfigured);
}

bool DataVar::IsValid() const throw()
{
	return(myIsConfigured);
}

const string&
DataVar::GiveDataVarName() const throw()
{
	return(myDataVarName);
}

const string&
DataVar::GiveCAFEVarName() const throw()
{
	return(myCAFEVarName);
}

const map<size_t, string>&
DataVar::GiveDataLevels() const throw()
{
	return(myDataLevels);
}

const string&
DataVar::GiveDataLevel(const size_t &CAFELevelIndex) const throw(DataLevel_Not_Found)
{
	const map<size_t, string>::const_iterator levelFind = myDataLevels.find(CAFELevelIndex);
	if (levelFind == myDataLevels.end())
        {
                throw DataLevel_Not_Found("DataVar::GiveDataLevel()", 
					  CAFELevelIndex, myDataVarName);
        }
	else
	{
		return(levelFind->second);
	}
}

size_t DataVar::GiveDataLevelCount() const throw()
{
	return(myDataLevels.size());
}

bool DataVar::AddDataLevel(const string &NewDataLevel, const size_t &CAFELevelIndex)
{
	if (myDataLevels.find(CAFELevelIndex) == myDataLevels.end())
        {
   	        myDataLevels[CAFELevelIndex] = NewDataLevel;
		return(true);
	}
        else
        {
		throw CAFEException("DataVar::AddDataLevel()",
				    "Duplicate DataLevelIndex used: " + Size_tToStr(CAFELevelIndex)
				    + ". No two datalevels can be associated with the same CAFE level.");
        }
}

vector<string> DataVar::InitTagWords() const
{
	vector<string> TagWords(4);

	TagWords[0] = "DataVar";
	TagWords[1] = "Name";
	TagWords[2] = "CAFEname";
	TagWords[3] = "Level";

	return(TagWords);
}

#endif
