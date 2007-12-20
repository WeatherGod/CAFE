#ifndef _DATASOURCE_C
#define _DATASOURCE_C

using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>					// for binary_search(), lower_bound()

#include "Config/DataSource.h"
#include "Config/DataVar.h"

#include <StrUtly.h>					// for RipWhiteSpace()
#include <ConfigUtly.h>					// for ReadNoComments(), StripTags(), FoundStartTag(), FoundEndTag()

#include <TimeUtly.h>					// for GetTime_UTC()
#include <ctime>


DataSourceID_t::DataSourceID_t()
	:	myDataSourceName("")
{
}

DataSourceID_t::DataSourceID_t(const DataSourceID_t &ACopy)
	:	myDataSourceName( ACopy.myDataSourceName )
{
}


DataSourceID_t::DataSourceID_t(const string &DataSourceName)
	:	myDataSourceName(DataSourceName)
{
}

DataSourceID_t::DataSourceID_t(const char* DataSourceName)
	:	myDataSourceName(DataSourceName)
{
}


void DataSourceID_t::operator = (const DataSourceID_t &ACopy)
{
	myDataSourceName = ACopy.myDataSourceName;
}

void DataSourceID_t::operator = (const string &dataSourceName)
{
	myDataSourceName = dataSourceName;
}


ostream& operator << (ostream& os, const DataSourceID_t &dataSource)
{
	return(os << dataSource.myDataSourceName);
}

bool operator == (const DataSourceID_t &lefty, const DataSourceID_t &righty)
{
	return(lefty.myDataSourceName == righty.myDataSourceName);
}

bool operator != (const DataSourceID_t &lefty, const DataSourceID_t &righty)
{
	return(lefty.myDataSourceName != righty.myDataSourceName);
}

bool operator < (const DataSourceID_t &lefty, const DataSourceID_t &righty)
{
	return(lefty.myDataSourceName < righty.myDataSourceName);
}









DataSource::DataSource()
	:	mySourceName(""),
		myProjectionName(""),
		myProjectionConfig(""),
		myDataVars(),
		myIsConfigured(false),
		myTagWords(0),
		myDummyDataVar(),
		myLowerTimeRange(-1),
		myUpperTimeRange(-1)
{
}

void DataSource::GetConfigInfo(string &FileLine, fstream &ReadData)
{
	InitTagWords();

	bool BadObject = false;

	while (!FoundEndTag(FileLine, myTagWords[0]) && !ReadData.eof())
	{
		if (!BadObject)
		{
			if (FoundStartTag(FileLine, myTagWords[1]))		// Name
			{
				mySourceName = RipWhiteSpace(StripTags(FileLine, myTagWords[1]));
			}
			else if (FoundStartTag(FileLine, myTagWords[2]))	// DataVar
			{
				FileLine = ReadNoComments(ReadData);
				DataVar TempDataVar;
				TempDataVar.GetConfigInfo(FileLine, ReadData);
				if (!AddDataVar(TempDataVar))
				{
					BadObject = true;
				}
			}
			else if (FoundStartTag(FileLine, myTagWords[3]))	// Projection
			{
				vector <string> ProjectionInfo = TakeDelimitedList(RipWhiteSpace(StripTags(FileLine, myTagWords[3])), "::");

				if (ProjectionInfo.size() != 2)
				{
					cerr << "ERROR: Invalid Projection string: " << FileLine << endl;
					BadObject = true;
				}

				myProjectionName = ProjectionInfo[0];
				myProjectionConfig = ProjectionInfo[1];
			}
			else if (FoundStartTag(FileLine, myTagWords[4]))	// Time
			{
				vector <string> TimeRangeInfo = TakeDelimitedList(RipWhiteSpace(StripTags(FileLine, myTagWords[4])), ",");

				if (TimeRangeInfo.size() != 2)
				{
					cerr << "ERROR: Invalid Time Range string: " << FileLine << endl;
					BadObject = true;
				}

				time_t tempTime1 = GetTimeUTC(TimeRangeInfo[0], "%Y-%m-%d %H");
				time_t tempTime2 = GetTimeUTC(TimeRangeInfo[1], "%Y-%m-%d %H");

				if (tempTime1 == (time_t)-1)
				{
					cerr << "ERROR: Invalid time string: " << TimeRangeInfo[0] << endl;
					BadObject = true;
				}

				if (tempTime2 == (time_t)-1)
				{
					cerr << "ERROR: Invalid time string: " << TimeRangeInfo[1] << endl;
					BadObject = true;
				}

				if (tempTime1 <= tempTime2)
				{
					myLowerTimeRange = tempTime1;
					myUpperTimeRange = tempTime2;
				}
				else
				{
					myLowerTimeRange = tempTime2;
					myUpperTimeRange = tempTime1;
				}
			}
			else
			{
				BadObject = true;
				cerr << "Problem in DataSource object... couldn't recognize anything.\nHere is the line: " << FileLine << endl;
			}
		}// end if !BadObject

		FileLine = ReadNoComments(ReadData);
	}// end while loop

	if (!ReadData.eof() && !mySourceName.empty() && !BadObject)
	{
		
		myIsConfigured = true;
	}

	myTagWords.resize(0);
}

bool DataSource::ValidConfig() const
{
	return(myIsConfigured);
}

bool DataSource::IsValid() const
{
	return(myIsConfigured);
}

string DataSource::GiveSourceName() const
{
	return(mySourceName);
}

string DataSource::GiveProjectionName() const
{
	return(myProjectionName);
}

string DataSource::GiveProjectionConfig() const
{
	return(myProjectionConfig);
}


size_t DataSource::GiveDataVarCount() const
{
	return(myDataVars.size());
}

string DataSource::GiveDataVarName(const string &CAFEVarName) const
{
	map<string, DataVar>::const_iterator ADataVar = myDataVars.find(CAFEVarName);
        if (ADataVar == myDataVars.end())
        {
                return(myDummyDataVar.GiveDataVarName());
        }

	return(ADataVar->second.GiveDataVarName());
}

string DataSource::GiveDataLevel(const string &CAFEVarName, const size_t &CAFELevelIndex) const
{
	map<string, DataVar>::const_iterator ADataVar = myDataVars.find(CAFEVarName);
	if (ADataVar == myDataVars.end())
        {
                return(myDummyDataVar.GiveDataLevel(string::npos));
        }

	return(ADataVar->second.GiveDataLevel(CAFELevelIndex));
}

size_t DataSource::GiveDataLevelCount(const string &CAFEVarName) const
{
	map<string, DataVar>::const_iterator ADataVar = myDataVars.find(CAFEVarName);
	if (ADataVar == myDataVars.end())
        {
                return(string::npos);
        }

        return(ADataVar->second.GiveDataLevelCount());
}


pair<time_t, time_t> DataSource::GiveTimeRange() const
{
	return(make_pair(myLowerTimeRange, myUpperTimeRange));
}

bool DataSource::AddDataVar(const DataVar &NewDataVar)
{
	if (NewDataVar.IsValid())
	{
		map <string, DataVar>::iterator DataVarMatch = myDataVars.find(NewDataVar.GiveCAFEVarName());
		if (DataVarMatch == myDataVars.end())
		{
			// the key doesn't exist yet, so I can add in a DataVar under that key value
			myDataVars[NewDataVar.GiveCAFEVarName()] = NewDataVar;
			return(true);
		}
		else
		{
			// a datavar with that keyvalue already exists.  Going to try and append info to it.
			if (DataVarMatch->second.GiveDataVarName() == NewDataVar.GiveDataVarName())
			{
				// they have the same CAFEName and the same DataVarName, will then update the stored info.
				return(true);
			}
			else
			{
				cerr << "Bad mappings!  The datavar name does not match the stored datavar name!" << endl;
				cerr << "CAFEName: " << NewDataVar.GiveCAFEVarName();
				cerr << "   Stored DataVarName: " << DataVarMatch->second.GiveDataVarName();
				cerr << "    New DataVarName: " << NewDataVar.GiveDataVarName() << endl;
				return(false);
			}
		}
	}

	return(false);
}


void DataSource::InitTagWords()
{
	if (myTagWords.size() == 0)
	{
		myTagWords.push_back("DataSource");
		myTagWords.push_back("Name");
		myTagWords.push_back("DataVar");
		myTagWords.push_back("Projection");
		myTagWords.push_back("Time");
	}
}

bool operator == (const DataSource &Lefty, const DataSource &Righty)
{
	return(Lefty.mySourceName == Righty.mySourceName);
}

bool operator != (const DataSource &Lefty, const DataSource &Righty)
{
	return(Lefty.mySourceName != Righty.mySourceName);
}

bool operator > (const DataSource &TheSource, const string &ASourceName)
{
	return(TheSource.mySourceName > ASourceName);
}

bool operator < (const DataSource &TheSource, const string &ASourceName)
{
        return(TheSource.mySourceName < ASourceName);
}

bool operator >= (const DataSource &TheSource, const string &ASourceName)
{
        return(TheSource.mySourceName >= ASourceName);
}

bool operator <= (const DataSource &TheSource, const string &ASourceName)
{
        return(TheSource.mySourceName <= ASourceName);
}

bool operator == (const DataSource &TheSource, const string &ASourceName)
{
        return(TheSource.mySourceName == ASourceName);
}

bool operator != (const DataSource &TheSource, const string &ASourceName)
{
        return(TheSource.mySourceName != ASourceName);
}

bool operator > (const string &ASourceName, const DataSource &TheSource)
{
        return(ASourceName > TheSource.mySourceName);
}

bool operator < (const string &ASourceName, const DataSource &TheSource)
{
        return(ASourceName < TheSource.mySourceName);
}

bool operator >= (const string &ASourceName, const DataSource &TheSource)
{
        return(ASourceName >= TheSource.mySourceName);
}

bool operator <= (const string &ASourceName, const DataSource &TheSource)
{
        return(ASourceName <= TheSource.mySourceName);
}

bool operator == (const string &ASourceName, const DataSource &TheSource)
{
        return(ASourceName == TheSource.mySourceName);
}

bool operator != (const string &ASourceName, const DataSource &TheSource)
{
        return(ASourceName != TheSource.mySourceName);
}

#endif
