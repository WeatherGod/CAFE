#ifndef _DATASOURCE_C
#define _DATASOURCE_C

using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cctype>					// for size_t

#include "Utils/CAFEException.h"			// for DataVar_Not_Found, DataLevel_Not_Found

#include "Config/DataSource.h"
#include "Config/DataVar.h"

#include <StrUtly.h>					// for RipWhiteSpace()
#include <ConfigUtly.h>					// for ReadNoComments(), StripTags(), FoundStartTag(), FoundEndTag()

#include <TimeUtly.h>					// for GetTime_UTC()
#include <ctime>

#include <Projection_t.h>
#include <ProjectionFactory.h>



DataSource::DataSource()
	:	mySourceName(""),
		myProjectionName(""),
		myProjectionConfig(""),
		myDataVars(),
		myIsConfigured(false),
		myLowerTimeRange(-1),
		myUpperTimeRange(-1)
{
}

DataSource::DataSource(const DataSource &sourceCopy)
	:	mySourceName(sourceCopy.mySourceName),
		myProjectionName(sourceCopy.myProjectionName),
		myProjectionConfig(sourceCopy.myProjectionConfig),
		myDataVars(sourceCopy.myDataVars),
		myIsConfigured(sourceCopy.myIsConfigured),
		myLowerTimeRange(sourceCopy.myLowerTimeRange),
		myUpperTimeRange(sourceCopy.myUpperTimeRange)
{
}

DataSource::DataSource(const string &sourceName,
		       const string &projectionName, const string &projectionConf,
		       const map<string, DataVar> dataVars,
		       const time_t &timeStart, const time_t &timeEnd)
	:	mySourceName(sourceName),
		myProjectionName(projectionName),
		myProjectionConfig(projectionConf),
		myDataVars(dataVars),
		myIsConfigured(true),
		myLowerTimeRange(timeStart),
		myUpperTimeRange(timeEnd)
{
}

void DataSource::GetConfigInfo(string &FileLine, fstream &ReadData)
{
	vector<string> TagWords = InitTagWords();

	bool BadObject = false;

	while (!FoundEndTag(FileLine, TagWords[0]) && !ReadData.eof())
	{
		if (!BadObject)
		{
			if (FoundStartTag(FileLine, TagWords[1]))		// Name
			{
				mySourceName = RipWhiteSpace(StripTags(FileLine, TagWords[1]));
			}
			else if (FoundStartTag(FileLine, TagWords[2]))	// DataVar
			{
				FileLine = ReadNoComments(ReadData);
				DataVar TempDataVar;
				TempDataVar.GetConfigInfo(FileLine, ReadData);
				if (!AddDataVar(TempDataVar))
				{
					BadObject = true;
				}
			}
			else if (FoundStartTag(FileLine, TagWords[3]))	// Projection
			{
				vector <string> ProjectionInfo = TakeDelimitedList(RipWhiteSpace(StripTags(FileLine, TagWords[3])), "::");

				if (ProjectionInfo.size() != 2)
				{
					cerr << "ERROR: Invalid Projection string: " << FileLine << endl;
					BadObject = true;
				}

				myProjectionName = ProjectionInfo[0];
				myProjectionConfig = ProjectionInfo[1];
			}
			else if (FoundStartTag(FileLine, TagWords[4]))	// Time
			{
				vector <string> TimeRangeInfo = TakeDelimitedList(RipWhiteSpace(StripTags(FileLine, TagWords[4])), ",");

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
}

bool DataSource::ValidConfig() const throw()
{
	return(myIsConfigured);
}

bool DataSource::IsValid() const throw()
{
	return(myIsConfigured);
}

const string&
DataSource::GiveSourceName() const throw()
{
	return(mySourceName);
}

const string&
DataSource::GiveProjectionName() const throw()
{
	return(myProjectionName);
}

const string&
DataSource::GiveProjectionConfig() const throw()
{
	return(myProjectionConfig);
}


size_t
DataSource::GiveDataVarCount() const throw()
{
	return(myDataVars.size());
}

const string&
DataSource::GiveDataVarName(const string &CAFEVarName) const throw(DataVar_Not_Found)
{
	map<string, DataVar>::const_iterator ADataVar = myDataVars.find(CAFEVarName);
        if (ADataVar == myDataVars.end())
        {
                throw DataVar_Not_Found("DataSource::GiveDataVarName()",
					CAFEVarName, mySourceName);
        }
	else
	{
		return(ADataVar->second.GiveDataVarName());
	}
}

const string&
DataSource::GiveDataLevel(const string &CAFEVarName, const size_t &CAFELevelIndex) const throw(DataVar_Not_Found,
											       DataLevel_Not_Found)
{
	map<string, DataVar>::const_iterator ADataVar = myDataVars.find(CAFEVarName);
	if (ADataVar == myDataVars.end())
        {
                throw DataVar_Not_Found("DataSource::GiveDataLevel()",
					CAFEVarName, mySourceName);
        }
	else
	{
		try
		{
			return(ADataVar->second.GiveDataLevel(CAFELevelIndex));
		}
		catch (DataLevel_Not_Found &err)
		{
			err.SetDataSourceName(mySourceName);
			err.AddFunctionName("DataSource::GiveDataLevel()");
			throw;
		}
	}
}

size_t
DataSource::GiveDataLevelCount(const string &CAFEVarName) const throw(DataVar_Not_Found)
{
	map<string, DataVar>::const_iterator ADataVar = myDataVars.find(CAFEVarName);
	if (ADataVar == myDataVars.end())
        {
                throw DataVar_Not_Found("DataSource::GiveDataLevelCount()",
					CAFEVarName, mySourceName);
        }
	else
	{
        	return(ADataVar->second.GiveDataLevelCount());
	}
}

const map<string, DataVar>&
DataSource::GiveDataVars() const throw()
{
	return(myDataVars);
}


pair<time_t, time_t>
DataSource::GiveTimeRange() const throw()
{
	return(make_pair(myLowerTimeRange, myUpperTimeRange));
}

const Projection_t*
DataSource::GiveProjection() const
{
	ProjectionFactory theFactory;

	return(theFactory.ProjectionFromStr(myProjectionConfig, myProjectionName));
}

bool DataSource::AddDataVar(const DataVar &NewDataVar)
{
	if (NewDataVar.IsValid())
	{
		map<string, DataVar>::iterator DataVarMatch = myDataVars.find(NewDataVar.GiveCAFEVarName());
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
				// TODO: is this supposed to be done here or elsewhere?
				return(true);
			}
			else
			{
				throw CAFEException("DataSource::AddDataVar()",
						    "Bad Mappings! CAFEVarName: " + NewDataVar.GiveCAFEVarName()
						    + "  Stored DataVarName: " + DataVarMatch->second.GiveDataVarName()
						    + "  New DataVarName: " + NewDataVar.GiveDataVarName());
			}
		}
	}

	return(false);
}


vector<string> DataSource::InitTagWords() const
{
	vector<string> TagWords(5);
	
	TagWords[0] = "DataSource";
	TagWords[1] = "Name";
	TagWords[2] = "DataVar";
	TagWords[3] = "Projection";
	TagWords[4] = "Time";
	
	return(TagWords);
}

#endif
