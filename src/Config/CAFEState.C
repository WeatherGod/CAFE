using namespace std;

#include <map>
#include <set>
#include <vector>
#include <algorithm>			// for find()
#include <string>
#include <cctype>			// for size_t
#include <cmath>			// for abs()

#include <Config/CAFEVar.h>
#include <Config/EventType.h>
#include <Config/CAFEDomain.h>
#include <Config/DataSource.h>
#include <Config/CAFEParam.h>

#include <Config/CAFEState.h>


CAFEState::CAFEState()
	:	myCAFEInfo(),
		myTimeOffsetIter(GetTimeOffsets().end()),
		myEventTypeIter(GetEventTypes().end()),
		myDataSourceIter(GetDataSources().end()),
		myExtremumIter(GetExtremumNames().end()),
		myTempEventVarNames(0),
		myEventVarIter(myTempEventVarNames.end()),
		myTempEventVarLevelNames(0),
		myEventVarLevelIter(myTempEventVarLevelNames.end())
{
}

CAFEState::CAFEState(const CAFEState &stateCopy)
	:	myCAFEInfo(stateCopy.myCAFEInfo),
                myTimeOffsetIter(),
                myEventTypeIter(),
		myDataSourceIter(),
		myExtremumIter(),
		myTempEventVarNames(stateCopy.myTempEventVarNames),
                myEventVarIter(),
		myTempEventVarLevelNames(stateCopy.myTempEventVarLevelNames),
                myEventVarLevelIter()
{
	if (stateCopy.myTimeOffsetIter != stateCopy.GetTimeOffsets().end())
	{
		myTimeOffsetIter = GetTimeOffsets().find(*stateCopy.myTimeOffsetIter);
	}
	else
	{
		myTimeOffsetIter = GetTimeOffsets().end();
	}

	if (stateCopy.myEventTypeIter != stateCopy.GetEventTypes().end())
	{
		myEventTypeIter = GetEventTypes().find(stateCopy.myEventTypeIter->first);
	}
	else
	{
		myEventTypeIter = GetEventTypes().end();
	}

	if (stateCopy.myDataSourceIter != stateCopy.GetDataSources().end())
	{
		myDataSourceIter = GetDataSources().find(stateCopy.myDataSourceIter->first);
        }
        else
        {
                myDataSourceIter = GetDataSources().end();
        }

	myExtremumIter = GetExtremumNames().begin() 
			+ (stateCopy.myExtremumIter - stateCopy.GetExtremumNames().begin());

	myEventVarIter = myTempEventVarNames.begin() 
			+ (stateCopy.myEventVarIter - stateCopy.myTempEventVarNames.begin());
	myEventVarLevelIter = myTempEventVarLevelNames.begin() 
			     + (stateCopy.myEventVarLevelIter - stateCopy.myTempEventVarLevelNames.begin());
}

CAFEState::CAFEState(const CAFEParam &cafeConfig)
	:	myCAFEInfo(cafeConfig),
		myTimeOffsetIter(GetTimeOffsets().begin()),
		myEventTypeIter(GetEventTypes().begin()),
		myDataSourceIter(GetDataSources().find(GetDefaultDataSource())),
		myExtremumIter(GetExtremumNames().begin()),
                myTempEventVarNames(0),
                myEventVarIter(),
                myTempEventVarLevelNames(0),
                myEventVarLevelIter()
{
	ResetTempEventVars();
}



CAFEState&
CAFEState::SetParams(const CAFEParam& cafeConfig)
{
	myCAFEInfo = cafeConfig;
	myTimeOffsetIter = GetTimeOffsets().begin();
	myEventTypeIter = GetEventTypes().begin();
	myDataSourceIter = GetDataSources().find(GetDefaultDataSource());
	myExtremumIter = GetExtremumNames().begin();
	ResetTempEventVars();
}

const CAFEParam&
CAFEState::GetParams() const
{
	return(myCAFEInfo);
}

CAFEState&
CAFEState::SetDefaultDataSource(const string &sourceName)
{
	const map<string, DataSource>::const_iterator sourceFind = GetDataSources().find(sourceName);

	if (sourceFind != GetDataSources().end())
	{
		myDataSourceIter = sourceFind;
		myCAFEInfo.SetDefaultDataSource(sourceName);
	}

	return(*this);
}




const int&
CAFEState::GetVerboseLevel() const
{
	return(myCAFEInfo.GetVerboseLevel());
}

const string&
CAFEState::GetConfigFilename() const
{
	return(myCAFEInfo.GetConfigFilename());
}

const string&
CAFEState::GetCAFEPath() const
{
	return(myCAFEInfo.GetCAFEPath());
}

const string&
CAFEState::GetLoginUserName() const
{
	return(myCAFEInfo.GetLoginUserName());
}

const string&
CAFEState::GetCAFEUserName() const
{
	return(myCAFEInfo.GetCAFEUserName());
}

const string&
CAFEState::GetServerName() const
{
	return(myCAFEInfo.GetServerName());
}

const string&
CAFEState::GetUntrainedNameStem() const
{
	return(myCAFEInfo.GetUntrainedNameStem());
}

const string&
CAFEState::GetTrainedNameStem() const
{
	return(myCAFEInfo.GetTrainedNameStem());
}

const CAFEDomain&
CAFEState::GetCAFEDomain() const
{
	return(myCAFEInfo.GetCAFEDomain());
}

const string&
CAFEState::GetDefaultDataSource() const
{
	return(myCAFEInfo.GetDefaultDataSource());
}

const set<int>&
CAFEState::GetTimeOffsets() const
{
	return(myCAFEInfo.GetTimeOffsets());
}

const map<string, DataSource>& 
CAFEState::GetDataSources() const
{
	return(myCAFEInfo.GetDataSources());
}

const map<string, CAFEVar>& 
CAFEState::GetCAFEVars() const
{
	return(myCAFEInfo.GetCAFEVars());
}

set<string>
CAFEState::GetCAFEVar_Names() const
{
	set<string> theNames;
	for (map<string, CAFEVar>::const_iterator aVar = GetCAFEVars().begin();
	     aVar != GetCAFEVars().end();
	     aVar++)
	{
		theNames.insert(theNames.end(), aVar->first);
	}

	return(theNames);
}

const map<string, EventType>&
CAFEState::GetEventTypes() const
{
	return(myCAFEInfo.GetEventTypes());
}

const vector<string>&
CAFEState::GetExtremumNames() const
{
	return(myCAFEInfo.GetExtremumNames());
}



// -------------------
// --- TimePeriods ---
// -------------------
size_t
CAFEState::TimePeriods_Size() const
{
	return(GetTimeOffsets().size());
}

CAFEState&
CAFEState::TimePeriods_Begin()
{
	myTimeOffsetIter = GetTimeOffsets().begin();
	return(*this);
}

CAFEState&
CAFEState::TimePeriods_Next()
{
	myTimeOffsetIter++;
	return(*this);
}

bool
CAFEState::TimePeriods_HasNext() const
{
	return(myTimeOffsetIter != GetTimeOffsets().end());
}

bool
CAFEState::TimePeriods_JumpTo(const int &offset)
{
	const set<int>::const_iterator offsetFind = GetTimeOffsets().find(offset);

	if (offsetFind != GetTimeOffsets().end())
	{
		myTimeOffsetIter = offsetFind;
		return(true);
	}
	else
	{
		return(false);
	}
}

int
CAFEState::TimePeriod_Offset() const
{
	return(*myTimeOffsetIter);
}

set<string>
CAFEState::GetTimePeriod_Names() const
{
	set<string> theNames;
	for (set<int>::const_iterator anOffset = GetTimeOffsets().begin();
	     anOffset != GetTimeOffsets().end();
	     anOffset++)
	{
		theNames.insert(theNames.end(), GiveTimePeriod(*anOffset));
	}

	return(theNames);
}

string
CAFEState::TimePeriod_Name() const
{
	return(GiveTimePeriod(*myTimeOffsetIter));
}

set<string>
CAFEState::GetUntrained_Names() const
{
        set<string> theNames;
        for (set<int>::const_iterator anOffset = GetTimeOffsets().begin();
             anOffset != GetTimeOffsets().end();
             anOffset++)
        {
                theNames.insert(theNames.end(), GetUntrainedNameStem() 
						+ "_" + GiveTimePeriod(*anOffset));
        }

        return(theNames);
}

string
CAFEState::Untrained_Name() const
{
        return(GetUntrainedNameStem() + "_" + TimePeriod_Name());
}

set<string>
CAFEState::GetTrained_Names() const
{
        set<string> theNames;
        for (set<int>::const_iterator anOffset = GetTimeOffsets().begin();
             anOffset != GetTimeOffsets().end();
             anOffset++)
        {
                theNames.insert(theNames.end(), GetTrainedNameStem() 
						+ "_" + GiveTimePeriod(*anOffset));
        }

        return(theNames);
}

string
CAFEState::Trained_Name() const
{
        return(GetTrainedNameStem() + "_" + TimePeriod_Name());
}



// ------------------
// --- EventTypes ---
// ------------------
size_t
CAFEState::EventTypes_Size() const
{
	return(GetEventTypes().size());
}

CAFEState&
CAFEState::EventTypes_Begin()
{
	myEventTypeIter = GetEventTypes().begin();
	return(ResetTempEventVars());
}

CAFEState&
CAFEState::EventTypes_Next()
{
	myEventTypeIter++;
	return(ResetTempEventVars());
}

bool
CAFEState::EventTypes_HasNext() const
{
	return(myEventTypeIter != GetEventTypes().end());
}

bool
CAFEState::EventTypes_JumpTo(const string &eventName)
{
	const map<string, EventType>::const_iterator eventFind = GetEventTypes().find(eventName);

	if (eventFind != GetEventTypes().end())
	{
		myEventTypeIter = eventFind;
		return(true);
	}
	else
	{
		return(false);
	}
}

set<string>
CAFEState::GetEventType_Names() const
{
	set<string> theNames;
	for (map<string, EventType>::const_iterator anEventType = GetEventTypes().begin();
	     anEventType != GetEventTypes().end();
	     anEventType++)
	{
		theNames.insert(theNames.end(), anEventType->first);
	}

	return(theNames);
}

string
CAFEState::EventType_Name() const
{
	return(myEventTypeIter->first);
}


// -----------------
// --- EventVars ---
// -----------------
size_t
CAFEState::EventVars_Size() const
{
	return(myTempEventVarNames.size());
}

CAFEState&
CAFEState::EventVars_Begin()
{
	myEventVarIter = myTempEventVarNames.begin();
	return(ResetTempEventVarLevels());
}

CAFEState&
CAFEState::EventVars_Next()
{
	myEventVarIter++;
	return(ResetTempEventVarLevels());
}

bool
CAFEState::EventVars_HasNext() const
{
	return(myEventVarIter != myTempEventVarNames.end());
}

bool
CAFEState::EventVars_JumpTo(const string &varName)
{
	const vector<string>::const_iterator varFind = find(myTempEventVarNames.begin(), 
							    myTempEventVarNames.end(),
							    varName);

	if (varFind != myTempEventVarNames.end())
	{
		myEventVarIter = varFind;
		return(true);
	}
	else
	{
		return(false);
	}
}

set<string>
CAFEState::EventVar_Names() const
{
	set<string> theNames;
	for (vector<string>::const_iterator aName = myTempEventVarNames.begin();
	     aName != myTempEventVarNames.end();
	     aName++)
	{
		theNames.insert(theNames.end(), *aName);
	}

	return(theNames);
}

string
CAFEState::EventVar_Name() const
{
	return(*myEventVarIter);
}

set<string>
CAFEState::DataVar_Names() const
{
        set<string> theNames;
        for (vector<string>::const_iterator aName = myTempEventVarNames.begin();
             aName != myTempEventVarNames.end();
             aName++)
        {
                theNames.insert(theNames.end(), myDataSourceIter->second.GiveDataVarName(*aName));
        }

        return(theNames);
}

string
CAFEState::DataVar_Name() const
{
	return(myDataSourceIter->second.GiveDataVarName(*myEventVarIter));
}



// ------------------
// --- CAFELevels ---
// ------------------
size_t
CAFEState::CAFELevels_Size() const
{
	return(myTempEventVarLevelNames.size());
}

CAFEState&
CAFEState::CAFELevels_Begin()
{
	myEventVarLevelIter = myTempEventVarLevelNames.begin();
	return(*this);
}

CAFEState&
CAFEState::CAFELevels_Next()
{
	myEventVarLevelIter++;
	return(*this);
}

bool
CAFEState::CAFELevels_HasNext() const
{
	return(myEventVarLevelIter != myTempEventVarLevelNames.end());
}

bool
CAFEState::CAFELevels_JumpTo(const string &levelName)
{
	const vector<string>::const_iterator levelFind = find(myTempEventVarLevelNames.begin(),
							      myTempEventVarLevelNames.end(),
							      levelName);

	if (levelFind != myTempEventVarLevelNames.end())
	{
		myEventVarLevelIter = levelFind;
		return(true);
	}
	else
	{
		return(false);
	}
}

set<string>
CAFEState::CAFELevel_Names() const
{
        set<string> theNames;
        for (vector<string>::const_iterator aName = myTempEventVarLevelNames.begin();
             aName != myTempEventVarLevelNames.end();
             aName++)
        {
                theNames.insert(theNames.end(), *aName);
        }

        return(theNames);
}

string
CAFEState::CAFELevel_Name() const
{
	return(*myEventVarLevelIter);
}

set<string>
CAFEState::DataLevel_Names() const
{
        set<string> theNames;
        for (vector<string>::const_iterator aName = myTempEventVarLevelNames.begin();
             aName != myTempEventVarLevelNames.end();
             aName++)
        {
                theNames.insert(theNames.end(), 
				myDataSourceIter->second.GiveDataLevel(*myEventVarIter,
									GetCAFEVars().find(*myEventVarIter)
									  ->second.GiveCAFELevelIndex(*aName)));
        }

        return(theNames);
}

string
CAFEState::DataLevel_Name() const
{
	return(myDataSourceIter->second.GiveDataLevel(*myEventVarIter, 
						      GetCAFEVars().find(*myEventVarIter)
							->second.GiveCAFELevelIndex(*myEventVarLevelIter)));
}

set<string>
CAFEState::CAFEField_Names() const
{
        set<string> theNames;
        for (vector<string>::const_iterator aName = myTempEventVarLevelNames.begin();
             aName != myTempEventVarLevelNames.end();
             aName++)
        {
                theNames.insert(theNames.end(),
				(aName->empty() ? *myEventVarIter
						: *myEventVarIter + "_" + *aName));
        }

        return(theNames);
}

string
CAFEState::CAFEField_Name() const
{
	// Need to see if there is a level to 
	// append onto the event variable name.
	return(myEventVarLevelIter->empty() ? *myEventVarIter
					    : *myEventVarIter + "_" + *myEventVarLevelIter);
}





// ---------------
// --- Extrema ---
// ---------------
size_t
CAFEState::Extrema_Size() const
{
	return(GetExtremumNames().size());
}

CAFEState&
CAFEState::Extrema_Begin()
{
	myExtremumIter = GetExtremumNames().begin();
	return(*this);
}

CAFEState&
CAFEState::Extrema_Next()
{
	myExtremumIter++;
	return(*this);
}

bool
CAFEState::Extrema_HasNext() const
{
	return(myExtremumIter != GetExtremumNames().end());
}

bool
CAFEState::Extrema_JumpTo(const string &extremumName)
{
	const vector<string>::const_iterator extremumFind = find(GetExtremumNames().begin(),
								 GetExtremumNames().end(),
								 extremumName);

	if (extremumFind != GetExtremumNames().end())
	{
		myExtremumIter = extremumFind;
		return(true);
	}
	else
	{
		return(false);
	}
}

string
CAFEState::Extremum_Name() const
{
	return(*myExtremumIter);
}

vector<string>
CAFEState::FieldExtremum_Names() const
{
	vector<string> theNames(0);
	theNames.reserve(Extrema_Size());

	for (vector<string>::const_iterator aName = GetExtremumNames().begin();
	     aName != GetExtremumNames().end();
	     aName++)
	{
		theNames.push_back(CAFEField_Name() + "_" + *aName);
	}

	return(theNames);
}

string
CAFEState::FieldExtremum_Name() const
{
	return(CAFEField_Name() + "_" + *myExtremumIter);
}




// ***********************************
// ******* Private Functions *********
// ***********************************
string
CAFEState::GiveTimePeriod(const int &timeOffset) const
{
	char timePeriodStr[10];
	memset(timePeriodStr, '\0', 10);

	if (timeOffset <= 0)
	{
		snprintf(timePeriodStr, 10, "Tm%.2d", abs(timeOffset));
	}
	else
	{
		snprintf(timePeriodStr, 10, "Tp%.2d", timeOffset);
	}

	return(timePeriodStr);
}

CAFEState&
CAFEState::ResetTempEventVars()
{
	if (myEventTypeIter != GetEventTypes().end())
	{
		myTempEventVarNames = myEventTypeIter->second.GiveCAFEVariableNames();
	}
	else
	{
		myTempEventVarNames.clear();
	}

	myEventVarIter = myTempEventVarNames.begin();

	return(ResetTempEventVarLevels());
}

CAFEState&
CAFEState::ResetTempEventVarLevels()
{
	if (myEventVarIter != myTempEventVarNames.end())
	{
		myTempEventVarLevelNames = myEventTypeIter->second.GiveCAFELevels(*myEventVarIter);
		if (myTempEventVarLevelNames.empty())
		{
			// This means that there are no CAFE levels at all.
			// However, there might be a default data level for it.
			// So, give the array a single empty string.
			/* TODO: This really shouldn't be here.
			         Need to consider the proper way
			         to refactor this
			*/
			myTempEventVarLevelNames.push_back("");
		}
	}
	else
	{
		myTempEventVarLevelNames.clear();
	}

	return(*this);
}


