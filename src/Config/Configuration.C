#ifndef _CONFIGURATION_C
#define _CONFIGURATION_C

using namespace std;

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>			// for find(), lower_bound()
#include <ctime>
#include <cctype>			// for size_t
#include <map>

#include <StrUtly.h>				// for TakeDelimitedList()
#include <ConfigUtly.h>				// for StripTags(), FoundStartTag(), FoundEndTag(), ReadNoComments()

#include "Config/CAFEDomain.h"
#include "Config/CAFEVar.h"
#include "Config/DataSource.h"
#include "Config/EventType.h"
#include <Projection_t.h>
#include <ProjectionFactory.h>

#include "Config/Configuration.h"


Configuration::Configuration()
	:	myDatabaseNames(0),
		myProcessedNames(0),
		myPeakValleys(0),
		myTimePeriods(0),
		myIsConfiged(false),
		myConfigFileName(""),
		myDatabaseStem(""),
		myProcessedStem(""),
		myDefaultDataSourceID(""),
		myCAFEDomain(),
		myCAFEVars(0),
		myDataSources(),
		myEventTypes(0)
{
}

Configuration::Configuration(const string &ConfigFileName)
        :       myDatabaseNames(0),
                myProcessedNames(0),
                myPeakValleys(0),
                myTimePeriods(0),
                myIsConfiged(false),
                myConfigFileName(""),
                myDatabaseStem(""),
                myProcessedStem(""),
                myDefaultDataSourceID(""),
                myCAFEDomain(),
                myCAFEVars(0),
                myDataSources(),
                myEventTypes(0)
{
	myIsConfiged = ConfigFromFile(ConfigFileName);
}


bool Configuration::ValidConfig() const
{
	return(myIsConfiged);
}

bool Configuration::IsValid() const
{
	// test connections....

	return(myIsConfiged);		// just a stub, for now...
}

vector<string> Configuration::InitTagWords() const
{
	vector<string> TagWords(9);

	TagWords[0] = "Config";
	TagWords[1] = "DatabaseStem";
	TagWords[2] = "ProcessedStem";
	TagWords[3] = "TimePeriods";
	TagWords[4] = "Domain";
	TagWords[5] = "CAFEVar";
	TagWords[6] = "Default";
	TagWords[7] = "DataSource";
	TagWords[8] = "EventType";
	
	return(TagWords);
}

bool Configuration::SetConfigFilename(const string &FileName)
{
	if (FileName.empty())
	{
		cerr << "Invalid filename... no changes made..." << endl;
		return(false);
	}
	else
	{
		myConfigFileName = FileName;
		return(true);
	}
}

bool Configuration::SetDefaultSource(const DataSourceID_t &DataSourceID)
{
	if (myDataSources.end() == myDataSources.find(DataSourceID))
	{
		return(false);
	}
	else
	{
		myDefaultDataSourceID = DataSourceID;
		return(true);
	}
}

// -------------- Class-Level Counters ------------------
size_t Configuration::DatabaseCount() const
{
        return(myDatabaseNames.size());
}

size_t Configuration::CAFEVarCount() const
{
        return(myCAFEVars.size());
}

size_t Configuration::DataSourceCount() const
{
        return(myDataSources.size());
}

size_t Configuration::EventTypeCount() const
{
        return(myEventTypes.size());
}

size_t Configuration::ExtremaCount() const
{
        return(myPeakValleys.size());
}

size_t Configuration::TimePeriodCount() const
{
	return(myTimePeriods.size());
}
// ============ end Class-Level counters ===========================

// ------------ Class-Level pointer returns -------------------------
const CAFEDomain* Configuration::GiveDomain() const
{
        return(&myCAFEDomain);
}

const Projection_t* Configuration::Give_DataSource_Projection() const
// It is the programmer's responsibility to delete the contents of the pointer when finished!
{
	return(Give_DataSource_Projection(myDefaultDataSourceID));
}

const Projection_t* Configuration::Give_DataSource_Projection(const DataSourceID_t &DataSourceID) const
{
	map<DataSourceID_t, DataSource>::const_iterator theSource = myDataSources.find( DataSourceID );

	if (myDataSources.end() == theSource)
	{
		cerr << "ERROR: Could not find the data source: " << DataSourceID << endl;
		return(new Projection_t());
        }

        const string ProjName = theSource->second.GiveProjectionName();
	const string ConfigStr = theSource->second.GiveProjectionConfig();

	ProjectionFactory TheFactory;

	return(TheFactory.ProjectionFromStr(ConfigStr, ProjName));
}

//============== end Class-Level pointer returns ===================

pair<time_t, time_t> Configuration::Give_DataSource_TimeRange() const
{
	return(Give_DataSource_TimeRange(myDefaultDataSourceID));
}

pair<time_t, time_t> Configuration::Give_DataSource_TimeRange(const DataSourceID_t &DataSourceID) const
{
	map<DataSourceID_t, DataSource>::const_iterator theSource = myDataSources.find(DataSourceID);

	if (myDataSources.end() == theSource)
	{
		return(make_pair((time_t)-1, (time_t)-1));
	}

	return(theSource->second.GiveTimeRange());
}



//-------------- Class-Level string value returners ----------------
string Configuration::GiveConfigFilename() const
{
	return(myConfigFileName);
}

vector <float> Configuration::GiveCAFEDomainBoundingBox() const
{
	return(myCAFEDomain.GiveBoundingBox());
}

string Configuration::GiveTimePeriod(const size_t &TimePeriodIndex) const
{
	if (TimePeriodIndex == string::npos || TimePeriodIndex >= myTimePeriods.size())
	{
		return("");
	}
	
	return(myTimePeriods[TimePeriodIndex]);
}

string Configuration::GiveDatabaseName(const size_t &DatabaseIndex) const
{
        if (DatabaseIndex == string::npos || DatabaseIndex >= myDatabaseNames.size())
        {
                return("");
        }

        return(myDatabaseNames[DatabaseIndex]);
}

string Configuration::GiveClusteredDatabaseName(const size_t &DatabaseIndex) const
{
        if (DatabaseIndex == string::npos || DatabaseIndex >= myProcessedNames.size())
        {
                return("");
        }

        return(myProcessedNames[DatabaseIndex]);
}

string Configuration::GiveDatabaseName(const string &TimePeriod) const
{
        return(GiveDatabaseName(GiveTimePeriodIndex(TimePeriod)));
}

string Configuration::GiveClusteredDatabaseName(const string &TimePeriod) const
{
	return(GiveClusteredDatabaseName(GiveTimePeriodIndex(TimePeriod)));
}


string Configuration::GiveEventTypeName(const size_t &EventTypeIndex) const
{
        if (EventTypeIndex == string::npos || EventTypeIndex >= myEventTypes.size())
        {
                return("");
        }

        return(myEventTypes[EventTypeIndex].GiveEventTypeName());
}

/*
string Configuration::GiveDataSourceName(const size_t &DataSourceIndex) const
{
	if (DataSourceIndex == string::npos || DataSourceIndex >= myDataSources.size())
	{
		return("");
	}

	return(myDataSources[DataSourceIndex].GiveSourceName());
}
*/

string Configuration::GiveDefaultSourceName() const
{
	map<DataSourceID_t, DataSource>::const_iterator theSource = myDataSources.find(myDefaultDataSourceID);

	if (myDataSources.end() == theSource)
	{
		return("");
	}

	return(theSource->second.GiveSourceName());
}

string Configuration::GiveExtremaName(const size_t &ExtremaIndex) const
{
        if (ExtremaIndex == string::npos || ExtremaIndex >= myPeakValleys.size())
        {
                return("");
        }

        return(myPeakValleys[ExtremaIndex]);
}
//=============== end Class-Level string value returners =============================

// ---------------- Class-Level string vector returners ------------------------------
vector <string> Configuration::GiveDatabaseNames() const
{
	return(myDatabaseNames);
}

vector <string> Configuration::GiveClusteredDatabaseNames() const
{
	return(myProcessedNames);
}

vector <string> Configuration::GiveEventTypeNames() const
{
	vector <string> EventTypeNames(myEventTypes.size(), "");

	for (size_t EventIndex = 0; EventIndex < myEventTypes.size(); EventIndex++)
	{
		EventTypeNames[EventIndex] = myEventTypes[EventIndex].GiveEventTypeName();
	}

	return(EventTypeNames);
}

vector <string> Configuration::GiveDataSourceNames() const
{
        vector <string> DataSourceNames(myDataSources.size(), "");

	size_t DataSourceIndex = 0;
	for (map<DataSourceID_t, DataSource>::const_iterator theSource = myDataSources.begin();
	     theSource != myDataSources.end();
	     theSource++, DataSourceIndex++)
	{
		DataSourceNames[DataSourceIndex] = theSource->second.GiveSourceName();
	}

        return(DataSourceNames);
}

vector <string> Configuration::GiveCAFEVarNames() const
{
        vector <string> CAFEVarNames(myCAFEVars.size(), "");

        for (size_t VarIndex = 0; VarIndex < myCAFEVars.size(); VarIndex++)
        {
                CAFEVarNames[VarIndex] = myCAFEVars[VarIndex].GiveCAFEVarName();
        }

        return(CAFEVarNames);
}

vector <string> Configuration::GiveExtremaNames() const
{
	return(myPeakValleys);
}

vector <string> Configuration::GiveTimePeriods() const
{
	return(myTimePeriods);
}
//================= end Class-Level string vector returners =========================

//----------------- Index Reporters -------------------------------------------------
/*
size_t Configuration::GiveDefaultSourceIndex() const
{
	return(myDefaultDataSourceID.GiveIndex(myDataSources));
}
*/

size_t Configuration::GiveEventTypeIndex(const string &EventTypeName) const
{
	EventTypeID_t EventTypeID = EventTypeName;
	return(EventTypeID.GiveIndex(myEventTypes));
}

/*
size_t Configuration::GiveDataSourceIndex(const string &DataSourceName) const
{
	DataSourceID_t DataSourceID = DataSourceName;
	return(DataSourceID.GiveIndex(myDataSources));
}
*/

size_t Configuration::GiveDatabaseIndex(const string &DatabaseName) const
{
	const vector<string>::const_iterator TheName = find(myDatabaseNames.begin(), myDatabaseNames.end(), DatabaseName);

	if (TheName == myDatabaseNames.end())
	{
		return(string::npos);
	}
	else
	{
		return(TheName - myDatabaseNames.begin());
	}
}

size_t Configuration::GiveClusteredDatabaseIndex(const string &ClustDatabaseName) const
{
	const vector<string>::const_iterator TheName = find(myProcessedNames.begin(), myProcessedNames.end(), ClustDatabaseName);

        if (TheName == myProcessedNames.end())
        {
                return(string::npos);
        }
        else
        {
                return(TheName - myProcessedNames.begin());
        }
}

size_t Configuration::GiveTimePeriodIndex(const string &TimePeriod) const
{
	const vector<string>::const_iterator TheName = find(myTimePeriods.begin(), myTimePeriods.end(), TimePeriod);

        if (TheName == myTimePeriods.end())
        {
                return(string::npos);
        }
        else
        {
                return(TheName - myTimePeriods.begin());
        }
}
//================= end Index Reporters ============================================

// --------------------- Container-Level counters ----------------------------------
size_t Configuration::CAFEVar_LevelCount(const string &CAFEVarName) const
{
	CAFEVarID_t CAFEVarID = CAFEVarName;
	const size_t CAFEVarIndex = CAFEVarID.GiveIndex(myCAFEVars);

	if (CAFEVarIndex == string::npos || CAFEVarIndex >= myCAFEVars.size())
	{
		return(string::npos);
	}

	return(myCAFEVars[CAFEVarIndex].GiveCAFELevelCount());
}

size_t Configuration::DataSource_VarCount(const DataSourceID_t &DataSourceID) const
{
	map<DataSourceID_t, DataSource>::const_iterator theSource = myDataSources.find(DataSourceID);

	if (myDataSources.end() == theSource)
	{
		return(string::npos);
	}

	return(theSource->second.GiveDataVarCount());
}

size_t Configuration::DataSource_DataLevelCount(const DataSourceID_t &DataSourceID, const string &CAFEVarName) const
{
	map<DataSourceID_t, DataSource>::const_iterator theSource = myDataSources.find(DataSourceID);         

	if (myDataSources.end() == theSource)
	{
		return(string::npos);
	}


        return(theSource->second.GiveDataLevelCount(CAFEVarName));
}

size_t Configuration::DataSource_VarCount() const
{
	return(DataSource_VarCount(myDefaultDataSourceID));
}

size_t Configuration::DataSource_DataLevelCount(const string &CAFEVarName) const
{
	return(DataSource_DataLevelCount(myDefaultDataSourceID, CAFEVarName));
}

size_t Configuration::EventType_VarCount(const EventTypeID_t &EventTypeID) const
{
	const size_t EventTypeIndex = EventTypeID.GiveIndex(myEventTypes);
	if (EventTypeIndex == string::npos || EventTypeIndex >= myEventTypes.size())
	{
		return(string::npos);
	}

	return(myEventTypes[EventTypeIndex].GiveVariableCount());
}

size_t Configuration::EventType_LevelCount(const EventTypeID_t &EventTypeID, const string &CAFEVarName) const
{
	const size_t EventTypeIndex = EventTypeID.GiveIndex(myEventTypes);
        if (EventTypeIndex == string::npos || EventTypeIndex >= myEventTypes.size())
        {
                return(string::npos);
        }

	return(myEventTypes[EventTypeIndex].GiveLevelCount(CAFEVarName));
}
//============================== end Container-level counters =========================

//--------------------------- Container-level value reporters -------------------------
string Configuration::Give_DataSource_DataLevel(const DataSourceID_t &DataSourceID, const string &CAFEVarName, const string &CAFELevelName) const
{
	map<DataSourceID_t, DataSource>::const_iterator theSource = myDataSources.find(DataSourceID);

        if (myDataSources.end() == theSource)
        {
                return("");
        }

	CAFEVarID_t CAFEVarID = CAFEVarName;
	const size_t CAFEVarIndex = CAFEVarID.GiveIndex(myCAFEVars);
	if (CAFEVarIndex == string::npos || CAFEVarIndex >= myCAFEVars.size())
	{
		return("");
	}

	return(theSource->second.GiveDataLevel(CAFEVarName, 
					       myCAFEVars[CAFEVarIndex].GiveCAFELevelIndex(CAFELevelName)));
}

string Configuration::Give_DataSource_DataName(const DataSourceID_t &DataSourceID, const string &CAFEVarName) const
{
        map<DataSourceID_t, DataSource>::const_iterator theSource = myDataSources.find(DataSourceID);

        if (myDataSources.end() == theSource)
        {
                return("");
        }

        return(theSource->second.GiveDataVarName(CAFEVarName));
}

string Configuration::Give_DataSource_DataLevel(const string &CAFEVarName, const string &CAFELevelName) const
{
        return(Give_DataSource_DataLevel(myDefaultDataSourceID, CAFEVarName, CAFELevelName));
}

string Configuration::Give_DataSource_DataName(const string &CAFEVarName) const
{
        return(Give_DataSource_DataName(myDefaultDataSourceID, CAFEVarName));
}

string Configuration::Give_CAFEVar_LevelName(const string &CAFEVarName, const string &CAFEVarLabel) const
// Derives the LevelName from the Label.  Very useful if a Label is given as user input.
// This function generates the possible Labels for the particular CAFEVar, and tries to match it.
{
	// LabelNames should be in alphabetical order.
	const vector <string> LabelNames = Give_CAFEVar_CAFEVarLabels(CAFEVarName);

	if (binary_search(LabelNames.begin(), LabelNames.end(), CAFEVarLabel))
	{
		const size_t LabelIndex = lower_bound(LabelNames.begin(), LabelNames.end(), CAFEVarLabel) - LabelNames.begin();

		// These LevelNames should be in alphabetical order,
		// so they will correspond with the LabelNames vector. (unless there are no levels, like MSLP and PWAT)
		// i.e. - element "Uwnd_850" in LabelNames will correspond to "850" in LevelNames.
		const vector <string> LevelNames = Give_CAFEVar_LevelNames(CAFEVarName);

		if (LevelNames.empty())
		{
			return("");		// since there are no level names, but there is a match for a label.
						// this happens for situations like MSLP, PWAT.
		}
		else
		{
			return(LevelNames[LabelIndex]);
		}
	}
	else
	{
		// Temporary error message.  Will remove later.
		cerr << "ERROR: No Match: " << CAFEVarLabel << "  with these: " << GiveDelimitedList(LabelNames, ' ') << endl;
		cerr << '*' << GiveDelimitedList(Give_CAFEVar_LevelNames(CAFEVarName), ' ') << '*' << endl;
		return("");
	}
}

vector <string> Configuration::Give_CAFEVar_LevelNames(const string &CAFEVarName) const
// NOTE: The LevelNames are in alphabetical order
{
	CAFEVarID_t CAFEVarID = CAFEVarName;
	const size_t CAFEVarIndex = CAFEVarID.GiveIndex(myCAFEVars);

	if (CAFEVarIndex == string::npos || CAFEVarIndex >= myCAFEVars.size())
	{
		return(vector<string>(0));
	}

	return(myCAFEVars[CAFEVarIndex].GiveCAFELevelNames());
}

vector <string> Configuration::Give_CAFEVar_CAFEVarLabels(const string &CAFEVarName) const
//-----------------------
// NOTE: The Labels will be in alphabetical order.
//-----------------------

// If CAFEVarName is invalid, a zero element vector is returned.
// If it is valid, then a vector of at least one element will be returned
// containing the complete label for the variable.
// For example, assume that a CAFEVar of Uwnd at levels 850, 700 and 500.
//     This function will return a 3-element vector of:
//                      "Uwnd_850"
//                      "Uwnd_700"
//                      "Uwnd_500"
//
// As another example, assume a CAFEVar of PWAT, which has no levels.
//     This function will return a 1-element vector of:
//                      "PWAT"
{
        const size_t LevCount = CAFEVar_LevelCount(CAFEVarName);

        if (LevCount != string::npos)
        {
                vector <string> CAFEVarLabels(max(LevCount, (size_t) 1), CAFEVarName);  // there should always be at least one element in this vector

		// The values in this vector are in alphabetical order!
                const vector <string> LevelNames = Give_CAFEVar_LevelNames(CAFEVarName);       // if there are no levels, then this vector
                                                                                                // will be zero-length
                                                                                                // The length of this vector always equals
                                                                                                // LevCount.

                // If there are no levels, then nothing will be appended to the CAFEVarName.
                //    The function will return, in such a case, a one-element vector with just
                //    the CAFEVarName in it.
                // If there are levels, the vector will have, for each element, the CAFEVarName
                //    and a respective CAFELevel name with an underscore between them.
                for (size_t LevIndex = 0; LevIndex < LevelNames.size(); LevIndex++)
                {
                        CAFEVarLabels[LevIndex] += '_' + LevelNames[LevIndex];
                }

                return(CAFEVarLabels);
        }
        else
        {
                return(vector<string>(0));
        }
}

/*
vector <string> Configuration::Give_DataSource_DataLevels(const DataSourceID_t &DataSourceID, const string &CAFEVarName) const
// assumes that the level count for the datavar is the same as the level count for the CAFEvar.
// This can be verified using IsValid(), which will test all the various components to make sure
// that such errors do not exist in the configuration.

{
	vector <string> DataLevelNames(0);

	CAFEVarID_t CAFEVarID = CAFEVarName;
        const size_t CAFEVarIndex = CAFEVarID.GiveIndex(myCAFEVars);
        if (CAFEVarIndex == string::npos || CAFEVarIndex >= myCAFEVars.size())
        {
		// returning an empty vector
                return(DataLevelNames);
        }

	const size_t DataSourceIndex = DataSourceID.GiveIndex(myDataSources);
        if (DataSourceIndex == string::npos || DataSourceIndex >= myDataSources.size())
        {
		// returning an empty vector
                return(DataLevelNames);
        }

	DataLevelNames.resize(myCAFEVars[CAFEVarIndex].GiveCAFELevelCount());

	const vector <size_t> CAFELevelIndicies = myCAFEVars[CAFEVarIndex].GiveCAFELevelIndicies();

	for (size_t LevelIndex = 0; LevelIndex < CAFELevelIndicies.size(); LevelIndex++)
	{
		DataLevelNames[LevelIndex] = myDataSources[DataSourceIndex].GiveDataLevel(CAFEVarName, CAFELevelIndicies[LevelIndex]);
	}

        return(DataLevelNames);
}
*/
vector <string> Configuration::Give_DataSource_DataNames(const DataSourceID_t &DataSourceID) const
// assumes that the number of CAFEVars is the same as the number of DataVars
// this can be checked using IsValid(), which will test all these kinds of connections...some day...maybe...

// The elements in the DataVarNames vector will correspond to the elements of the CAFEVarNames vector given by GiveCAFEVarNames()
{
	vector <string> DataVarNames(0);
	
        
        map<DataSourceID_t, DataSource>::const_iterator theSource = myDataSources.find(DataSourceID);

	if (myDataSources.end() == theSource)
	{
                return(DataVarNames);
        }

	DataVarNames.resize(theSource->second.GiveDataVarCount());
	const vector <string> CAFEVarNames = GiveCAFEVarNames();

	for (size_t DataVarIndex = 0; DataVarIndex < DataVarNames.size(); DataVarIndex++)
	{
		DataVarNames[DataVarIndex] = theSource->second.GiveDataVarName(CAFEVarNames[DataVarIndex]);
	}

        return(DataVarNames);
}

/*
vector <string> Configuration::Give_DataSource_DataLevels(const string &CAFEVarName) const
// assumes that the level count for the datavar is the same as the level count for the CAFEvar.
// This can be verified using IsValid(), which will test all the various components to make sure
// that such errors do not exist in the configuration.

// The elements in the DataLevelNames vector will correspond to the elements of the CAFEVarLevels vector given by Give_CAFEVar_LevelNames()
{
        return(Give_DataSource_DataLevels(myDefaultDataSourceID, CAFEVarName));
}
*/

vector <string> Configuration::Give_DataSource_DataNames() const
// assumes that the number of CAFEVars is the same as the number of DataVars
// this can be checked using IsValid(), which will test all these kinds of connections...
{
	return(Give_DataSource_DataNames(myDefaultDataSourceID));
}

vector <string> Configuration::Give_EventType_CAFEVarNames(const EventTypeID_t &EventTypeID) const
// The elements in the CAFEVarNames vector are in alphabetical order
{
	const size_t EventTypeIndex = EventTypeID.GiveIndex(myEventTypes);
	if (EventTypeIndex == string::npos || EventTypeIndex >= myEventTypes.size())
	{
		return(vector<string>(0));
	}

	return(myEventTypes[EventTypeIndex].GiveCAFEVariableNames());
}


vector <string> Configuration::Give_EventType_LevelNames(const EventTypeID_t &EventTypeID, const string &CAFEVarName) const
// The elements in the CAFELevelNames vector are in alphabetical order.
{
	const size_t EventTypeIndex = EventTypeID.GiveIndex(myEventTypes);
	if (EventTypeIndex == string::npos || EventTypeIndex >= myEventTypes.size())
	{
		return(vector<string>(0));
	}

	return(myEventTypes[EventTypeIndex].GiveCAFELevels(CAFEVarName));
}



vector <string> Configuration::Give_EventType_CAFEVarLabels(const EventTypeID_t &EventTypeID, const string &CAFEVarName) const
//---------------------
// The elements of CAFEVarLabels are in alphabetical order
//---------------------
// If EventTypeID or CAFEVarName are invalid, a zero element vector is returned.
// If both are valid, then a vector of at least one element will be returned
// containing the complete label for the variable.
// For example, assume that an event type has a CAFEVar of Uwnd at levels 850, 700 and 500.
//     This function will return a 3-element vector of:
//			"Uwnd_850"
//                      "Uwnd_700"
//                      "Uwnd_500"
//
// As another example, assume that an event type has a CAFEVar of PWAT, which has no levels.
//     This function will return a 1-element vector of:
//			"PWAT"
{
	const size_t LevCount = EventType_LevelCount(EventTypeID, CAFEVarName);

	if (LevCount != string::npos)
	{
		vector <string> CAFEVarLabels(max(LevCount, (size_t) 1), CAFEVarName);	// there should always be at least one element in this vector

		// the elements of LevelNames are in alphabetical order
		const vector <string> LevelNames = Give_EventType_LevelNames(EventTypeID, CAFEVarName);	// if there are no levels, then this vector
												// will be zero-length
												// The length of this vector always equals
												// LevCount.

		// If there are no levels, then nothing will be appended to the CAFEVarName.
		//    The function will return, in such a case, a one-element vector with just
		//    the CAFEVarName in it.
		// If there are levels, the vector will have, for each element, the CAFEVarName
		//    and a respective CAFELevel name with an underscore between them.
		for (size_t LevIndex = 0; LevIndex < LevCount; LevIndex++)
		{
			CAFEVarLabels[LevIndex] += '_' + LevelNames[LevIndex];
		}

		return(CAFEVarLabels);
	}
	else
	{
		vector <string> CAFEVarLabels(0);
		return(CAFEVarLabels);
	}
}
// ======================== end Container-level value returners ==============================================



//--------------------------- Configuration Modifiers --------------------------------------------------------
bool Configuration::ReplaceDatabaseStems(const string &NewDatabaseStem, const string &NewProcessedStem)
{
	if (!NewDatabaseStem.empty() && !NewProcessedStem.empty())
	{
		myDatabaseStem = NewDatabaseStem;
		myProcessedStem = NewProcessedStem;
		
		CompileDatabaseNames();
	
		return(true);
	}
	else
	{
		return(false);
	}
}

bool Configuration::ReplaceDomain(const CAFEDomain &NewDomain)
{
        if (NewDomain.IsValid())
        {
                myCAFEDomain = NewDomain;
                return(true);
        }
        else
        {
                return(false);
        }
}

bool Configuration::ReplaceCAFEVar(const CAFEVar &NewVar, const string &OldCAFEVarName)
{
        if (NewVar.IsValid())
        {
                if (RemoveCAFEVar(OldCAFEVarName))
                {
                        AddCAFEVar(NewVar);
                        return(true);
                }
        }

        return(false);
}

bool Configuration::ReplaceDataSource(const DataSource &NewSource, const DataSourceID_t &OldDataSourceID)
{
        if (NewSource.IsValid())
        {
		if (RemoveDataSource(OldDataSourceID))
		{
			AddDataSource(NewSource);
		        return(true);
		}
        }
        
	return(false);
}

bool Configuration::ReplaceDataSource(const DataSource &NewSource)
// automatically sets the default source ID to the new source
{
	if (NewSource.IsValid())
	{
		if (RemoveDataSource(myDefaultDataSourceID))
		{
			AddDataSource(NewSource);
			myDefaultDataSourceID = NewSource.GiveSourceName();
			return(true);
		}
	}

	return(false);
}

bool Configuration::ReplaceEventType(const EventType &NewType, const EventTypeID_t &OldEventTypeID)
{
	if (NewType.IsValid())
        {
		if (RemoveEventType(OldEventTypeID))
		{
                	AddEventType(NewType);
	                return(true);
		}
        }

	return(false);
}

bool Configuration::AddTimePeriod(const string &NewTimePeriod)
{
	if (NewTimePeriod == "")
	{
		return(false);
	}

	if (!binary_search(myTimePeriods.begin(), myTimePeriods.end(), NewTimePeriod))
	{
		myTimePeriods.insert(lower_bound(myTimePeriods.begin(), myTimePeriods.end(), NewTimePeriod), NewTimePeriod);
		CompileDatabaseNames();				// rebuilds the list of database names and processed database names
		return(true);
	}
	else
	{
		return(false);
	}	
}

bool Configuration::AddCAFEVar(const CAFEVar &NewVar)
{
	if (!NewVar.IsValid())
	{
		return(false);
	}

	if (!binary_search(myCAFEVars.begin(), myCAFEVars.end(), NewVar.GiveCAFEVarName()))
	{
		// Probably put some sort of connection testing function here...

		myCAFEVars.insert(lower_bound(myCAFEVars.begin(), myCAFEVars.end(), NewVar.GiveCAFEVarName()), NewVar);
		return(true);
	}
	else
	{
		return(false);
	}
}

bool Configuration::AddDataSource(const DataSource &NewSource)
{
	if (!NewSource.IsValid())
	{
		return(false);
	}

	pair< map< DataSourceID_t, DataSource >::const_iterator, bool > 
	insertResult = myDataSources.insert( make_pair( NewSource.GiveSourceName(),
							NewSource ) );
	return(insertResult.second);
}

bool Configuration::AddEventType(const EventType &NewType)
{
	if (!NewType.IsValid())
	{
		return(false);
	}

	if (!binary_search(myEventTypes.begin(), myEventTypes.end(), NewType.GiveEventTypeName()))
	{
		// probably put some sort of connection testing function here...

		myEventTypes.insert(lower_bound(myEventTypes.begin(), myEventTypes.end(), NewType.GiveEventTypeName()), NewType);
		return(true);
	}
	else
	{
		return(false);
	}
}

// ---------- Removal Functions ---------------------------------------
bool Configuration::RemoveTimePeriod(const size_t &TimePeriodIndex)
{
	if (TimePeriodIndex == string::npos || TimePeriodIndex >= myTimePeriods.size())
	{
		return(false);
	}

	myTimePeriods.erase(myTimePeriods.begin() + TimePeriodIndex);
	CompileDatabaseNames();
	return(true);
}

bool Configuration::RemoveCAFEVar(const string &CAFEVarName)
{
	CAFEVarID_t CAFEVarID = CAFEVarName;
	size_t CAFEVarIndex = CAFEVarID.GiveIndex(myCAFEVars);

	if (CAFEVarIndex == string::npos || CAFEVarIndex >= myCAFEVars.size())
	{
		return(false);
	}

	myCAFEVars.erase(myCAFEVars.begin() + CAFEVarIndex);
	return(true);
}

bool Configuration::RemoveDataSource(const DataSourceID_t &DataSourceID)
{
	map<DataSourceID_t, DataSource>::iterator theSource = myDataSources.find(DataSourceID);
	if (myDataSources.end() == theSource)
	{
		return(false);
	}

	myDataSources.erase(theSource);
	return(true);
}

bool Configuration::RemoveEventType(const EventTypeID_t &EventTypeID)
{
	size_t EventTypeIndex = EventTypeID.GiveIndex(myEventTypes);

	if (EventTypeIndex == string::npos || EventTypeIndex >= myEventTypes.size())
	{
		return(false);
	}

	myEventTypes.erase(myEventTypes.begin() + EventTypeIndex);
	return(true);
}
//============================ end Class Modifiers ========================================


bool Configuration::ConfigFromFile()
{
	return(ConfigFromFile(myConfigFileName));
}


bool Configuration::ConfigFromFile(const string &TheConfigFilename)
{
	fstream ConfigStream(TheConfigFilename.c_str());

	if (!ConfigStream.is_open())
	{
		cerr << "\nCould not open the config file: " << TheConfigFilename << endl;
		return(false);
	}

	myConfigFileName = TheConfigFilename;
	string FileLine = "";

	//  Essentially, this and the while loop is treating anything before the first tag as comments
	FileLine = ReadNoComments(ConfigStream);

	while (!FoundStartTag(FileLine, "Config") && !ConfigStream.eof())
	{
		FileLine = ReadNoComments(ConfigStream);
	}

	if (ConfigStream.eof())
	{
		cerr << "\nRead through the file without finding the <" << "Config" << "> tag\n\n";
		ConfigStream.close();
		return(false);
	}
	else
	{
		FileLine = ReadNoComments(ConfigStream);
		GetConfigInfo(FileLine, ConfigStream);
	}

	ConfigStream.close();

	return(myIsConfiged);
}

void Configuration::GetConfigInfo(string &FileLine, fstream &ReadData)
{
	const vector<string> TagWords = InitTagWords();
	bool BadObject = false;

	while (!FoundEndTag(FileLine, TagWords[0]) && !ReadData.eof())
	{
		if (!BadObject)		// purpose is to skip over object if there is a problem.
						// eases fixing of config file by limiting error messages to only the error that is
						// relevant rather than all subsequent messages.
		{
			if (FoundStartTag(FileLine, TagWords[1]))	// DatabaseStem
			{
				myDatabaseStem = RipWhiteSpace(StripTags(FileLine, TagWords[1]));
			}
			else if (FoundStartTag(FileLine, TagWords[2]))	// processed stem
			{
				myProcessedStem = RipWhiteSpace(StripTags(FileLine, TagWords[2]));
			}
			else if (FoundStartTag(FileLine, TagWords[3]))	// time periods
			{
				string Tempy = StripTags(FileLine, TagWords[3]);
				vector <string> TempList = TakeDelimitedList(Tempy, ',');

				for (size_t TimeIndex = 0; TimeIndex < TempList.size(); TimeIndex++)
				{
					AddTimePeriod(RipWhiteSpace(TempList[TimeIndex]));
				}
			}
			else if (FoundStartTag(FileLine, TagWords[4]))	// domain
			{
				FileLine = ReadNoComments(ReadData);
				CAFEDomain TempDomain;
				TempDomain.GetConfigInfo(FileLine, ReadData);
				if (TempDomain.ValidConfig())
				{
					myCAFEDomain = TempDomain;
				}
				else
				{
					BadObject = true;
				}
			}
			else if (FoundStartTag(FileLine, TagWords[5]))	// CAFEVar
			{
				FileLine = ReadNoComments(ReadData);
				CAFEVar TempVar;
				TempVar.GetConfigInfo(FileLine, ReadData);
				if (!AddCAFEVar(TempVar))
				{
					BadObject = true;
				}
			}
			else if (FoundStartTag(FileLine, TagWords[6]))	// default data source name
			{
				myDefaultDataSourceID = RipWhiteSpace(StripTags(FileLine, TagWords[6]));
			}
			else if (FoundStartTag(FileLine, TagWords[7]))	// Data source
			{
				FileLine = ReadNoComments(ReadData);
				DataSource TempDataSource;
				TempDataSource.GetConfigInfo(FileLine, ReadData);
				if (!AddDataSource(TempDataSource))
				{
					BadObject = true;
				}
			}
			else if (FoundStartTag(FileLine, TagWords[8]))	// EventType
			{
				FileLine = ReadNoComments(ReadData);
				EventType TempEventType;
				TempEventType.GetConfigInfo(FileLine, ReadData);
				if (!AddEventType(TempEventType))
				{
					BadObject = true;
				}
			}
			else
			{
				BadObject = true;
				cerr << "ERROR: Problem in Configuration object.  Could not recognize anything..." << endl;
				cerr << "Here is the line: " << FileLine << endl;
			}
		}// end if !BadObject

		FileLine = ReadNoComments(ReadData);
	}// end while loop

	if (!myDatabaseStem.empty() && !myProcessedStem.empty() 
	    && !myTimePeriods.empty() && !myCAFEVars.empty() 
	    && !myDataSources.empty() && !myEventTypes.empty())
	{
		myIsConfiged = true;
		CompileDatabaseNames();

		myPeakValleys.resize(4);
		myPeakValleys[0] = "Peak1";
		myPeakValleys[1] = "Peak2";
		myPeakValleys[2] = "Valley1";
		myPeakValleys[3] = "Valley2";
	}
	else
	{
		cerr << "Not valid configuration..." << endl;
		cerr << "Status:" << endl;
		cerr << "Database Stem: " << myDatabaseStem << "   Processed Stem: " << myProcessedStem << endl;
		cerr << "TimePeriods count: " << myTimePeriods.size() << endl;
		cerr << "CAFEDomain Valid? " << myCAFEDomain.IsValid() << endl;
		cerr << "CAFEVars count: " << myCAFEVars.size() << endl;
		cerr << "DataSources count: " << myDataSources.size() << endl;
		cerr << "EventTypes count: " << myEventTypes.size() << endl;
	}
}// ends GetConfigInfo()

bool Configuration::MakeConfigFile()
{
	return(MakeConfigFile(myConfigFileName));
}

bool Configuration::MakeConfigFile(const string &ConfigFileName)
{
	if (ConfigFileName.empty())
	{
		cerr << "\n\tInvalid Configuration filename: " << ConfigFileName << endl;
		return(false);
	}

	ofstream ConfigStream(ConfigFileName.c_str());

	if (!ConfigStream.is_open())
	{
		cerr << "\n\tCould not open file for writing: " << ConfigFileName << endl;
		return(false);
	}

	ConfigStream << "<Config>" << endl;
	ConfigStream << "\t<DatabaseStem>" << myDatabaseStem << "</DatabaseStem>" << endl;
	ConfigStream << "\t<ProcessedStem>" << myProcessedStem << "</ProcessedStem>" << endl;
	ConfigStream << "\t<TimePeriods>" << GiveDelimitedList(myTimePeriods, ',') << "</TimePeriods>" << endl;
	ConfigStream << "\t<Domain>" << endl;
	ConfigStream << "\t\t<Lon>" << GiveDelimitedList(FloatToStr(myCAFEDomain.GiveLons()), ',') << "</Lon>" << endl;
	ConfigStream << "\t\t<Lat>" << GiveDelimitedList(FloatToStr(myCAFEDomain.GiveLats()), ',') << "</Lat>" << endl;
	ConfigStream << "\t</Domain>" << endl;

	for (size_t CAFEVarIndex = 0; CAFEVarIndex < myCAFEVars.size(); CAFEVarIndex)
	{
		ConfigStream << "\t<CAFEVar>" << endl;
		ConfigStream << "\t\t<Name>" << myCAFEVars[CAFEVarIndex].GiveCAFEVarName() << "</Name>" << endl;

		vector <string> CAFELevelNames = myCAFEVars[CAFEVarIndex].GiveCAFELevelNames();
		vector <size_t> CAFELevelIndicies = myCAFEVars[CAFEVarIndex].GiveCAFELevelIndicies();

		for (size_t CAFELevelIndex = 0; CAFELevelIndex < CAFELevelNames.size(); CAFELevelIndex++)
		{
			ConfigStream << "\t\t<Level>$" << CAFELevelIndicies[CAFELevelIndex] << " = ";
			ConfigStream << CAFELevelNames[CAFELevelIndex] << "</Level>" << endl;
		}

		ConfigStream << "\t</CAFEVar>" << endl;
	}

	ConfigStream << "\t<Default>" << GiveDefaultSourceName() << "</Default>" << endl;
	
	vector <string> CAFEVarNames = GiveCAFEVarNames();

	for (map<DataSourceID_t, DataSource>::const_iterator theSource = myDataSources.begin();
	     theSource != myDataSources.end();
	     theSource++)
	{
		ConfigStream << "\t<DataSource>" << endl;
		ConfigStream << "\t\t<Name>" << theSource->first << "</Name>" << endl;

		for (size_t VarIndex = 0; VarIndex < CAFEVarNames.size(); VarIndex++)
		{
			ConfigStream << "\t\t<DataVar>" << endl
				     << "\t\t\t<Name>" 
				     << Give_DataSource_DataName(theSource->first, CAFEVarNames[VarIndex])
				     << "</Name>" << endl
				     << "\t\t\t<CAFEname>" << CAFEVarNames[VarIndex]
                        	     << "</CAFEname>" << endl;

			vector <size_t> CAFELevelIndicies = myCAFEVars[VarIndex].GiveCAFELevelIndicies();
			vector <string> CAFELevelNames = myCAFEVars[VarIndex].GiveCAFELevelNames();
			for (size_t LevelIndex = 0; LevelIndex < CAFELevelIndicies.size(); LevelIndex++)
			{
				ConfigStream << "\t\t\t<Level>"
					     << Give_DataSource_DataLevel(theSource->first, CAFEVarNames[VarIndex], CAFELevelNames[LevelIndex])
					     << " = $" << CAFELevelIndicies[LevelIndex]
					     << "</Level>" << endl;
			}

			ConfigStream << "\t\t</DataVar>" << endl;
		}

		ConfigStream << "\t</DataSource>" << endl;
	}

	for (size_t EventTypeIndex = 0; EventTypeIndex < myEventTypes.size(); EventTypeIndex++)
	{
		ConfigStream << "\t<EventType>" << endl;
		ConfigStream << "\t\t<TypeName>" << GiveEventTypeName(EventTypeIndex) << "</TypeName>" << endl;

		vector <string> CAFEVarNames = Give_EventType_CAFEVarNames(EventTypeIndex);
		for (size_t VarIndex = 0; VarIndex < CAFEVarNames.size(); VarIndex++)
		{
			ConfigStream << "\t\t<Variable>" << endl;
			ConfigStream << "\t\t\t<Name>" << CAFEVarNames[VarIndex] << "</Name>" << endl;
			
			if (EventType_LevelCount(EventTypeIndex, CAFEVarNames[VarIndex]) > 0)
			{
				ConfigStream << "\t\t\t<Level>";
				ConfigStream << GiveDelimitedList(Give_EventType_LevelNames(EventTypeIndex, CAFEVarNames[VarIndex]), ',');
				ConfigStream << "</Level>" << endl;
			}

			ConfigStream << "\t\t</Variable>" << endl;
		}

		ConfigStream << "\t</EventType>" << endl;
	}

	ConfigStream << "</Config>" << endl;

	ConfigStream.close();

	return(true);
}




void Configuration::CompileDatabaseNames()
{
        myDatabaseNames.resize(myTimePeriods.size());
        myProcessedNames.resize(myTimePeriods.size());

        for (int TimeIndex = 0; TimeIndex < myTimePeriods.size(); TimeIndex++)
        {
                myDatabaseNames[TimeIndex] = (myDatabaseStem + '_' + myTimePeriods[TimeIndex]);
                myProcessedNames[TimeIndex] = (myProcessedStem + '_' + myTimePeriods[TimeIndex]);
        }
}
			

#endif
