using namespace std;

#include <map>
#include <set>
#include <vector>
#include <algorithm>			// for find()
#include <string>
#include <cctype>			// for size_t

#include <Config/CAFEVar.h>
#include <Config/EventType.h>
#include <Config/CAFEDomain.h>
#include <Config/DataSource.h>
#include <Config/CAFEParam.h>


#include <Utils/CAFEUtly.h>		// for OffsetToTimePeriod()

#include <Config/CAFEState.h>



CAFEState::CAFEState()
	:	myCAFEInfo(),
		myTimeOffsetIter(GetTimeOffsets().end()),
		myEventTypeIter(GetEventTypes().end()),
		myDataSourceIter(GetDataSources().end()),
		myExtremumIter(GetExtremumNames().end()),
		myCAFEVarIter(GetCAFEVars().end()),
		myTempEventVarNames(0),
		myEventVarIter(GetEventVars().end()),
		myTempEventVarLevelNames(0),
		myEventVarLevelIter(GetEventLevels().end()),
		myCAFELevelIter(GetCAFELevels().end())
{
}

CAFEState::CAFEState(const CAFEState &stateCopy)
	:	myCAFEInfo(stateCopy.myCAFEInfo),
                myTimeOffsetIter(),
                myEventTypeIter(),
		myDataSourceIter(),
		myExtremumIter(),
		myCAFEVarIter(),
		myTempEventVarNames(stateCopy.GetEventVars()),
                myEventVarIter(),
		myTempEventVarLevelNames(stateCopy.GetEventLevels()),
                myEventVarLevelIter(),
		myCAFELevelIter()
{
	if (stateCopy.TimePeriods_HasNext())
	{
		myTimeOffsetIter = GetTimeOffsets().find(stateCopy.TimePeriod_Offset());
	}
	else
	{
		myTimeOffsetIter = GetTimeOffsets().end();
	}

	if (stateCopy.EventTypes_HasNext())
	{
		myEventTypeIter = GetEventTypes().find(stateCopy.EventType_Name());
	}
	else
	{
		myEventTypeIter = GetEventTypes().end();
	}

	if (stateCopy.DataSources_HasNext())
	{
		myDataSourceIter = GetDataSources().find(stateCopy.DataSource_Name());
        }
        else
        {
                myDataSourceIter = GetDataSources().end();
        }

	if (stateCopy.CAFEVars_HasNext())
	{
		myCAFEVarIter = GetCAFEVars().find(stateCopy.CAFEVar_Name());
	}
	else
	{
		myCAFEVarIter = GetCAFEVars().end();
	}

	myExtremumIter = GetExtremumNames().begin() 
			+ (stateCopy.myExtremumIter - stateCopy.GetExtremumNames().begin());

	myEventVarIter = GetEventVars().begin() 
			+ (stateCopy.myEventVarIter - stateCopy.GetEventVars().begin());
	myEventVarLevelIter = GetEventLevels().begin() 
			     + (stateCopy.myEventVarLevelIter - stateCopy.GetEventLevels().begin());

	if (stateCopy.CAFELevels_HasNext())
	{
		myCAFELevelIter = GetCAFELevels().find(stateCopy.CAFELevel_Name());
	}
	else
	{
		myCAFELevelIter = GetCAFELevels().end();
	}
}

CAFEState::CAFEState(const CAFEParam &cafeConfig)
	:	myCAFEInfo(cafeConfig),
		myTimeOffsetIter(GetTimeOffsets().begin()),
		myEventTypeIter(GetEventTypes().begin()),
		myDataSourceIter(GetDataSources().find(myCAFEInfo.GetDefaultDataSource())),
		myExtremumIter(GetExtremumNames().begin()),
		myCAFEVarIter(GetCAFEVars().begin()),
                myTempEventVarNames(0),
                myEventVarIter(),
                myTempEventVarLevelNames(0),
                myEventVarLevelIter(),
		myCAFELevelIter(GetCAFELevels().begin())
{
	ResetEventVars();	// Initializes myTempEventVarNames, myEventVarIter,
				//             myTempEventVarLevelNames, and myEventVarLevelIter
}



CAFEState&
CAFEState::SetParams(const CAFEParam& cafeConfig)
{
	myCAFEInfo = cafeConfig;
	TimePeriods_Begin();
	EventTypes_Begin();
	DataSources_JumpTo(myCAFEInfo.GetDefaultDataSource());
	Extrema_Begin();
	CAFEVars_Begin();
}

const CAFEParam&
CAFEState::GetParams() const
{
	return(myCAFEInfo);
}


//-----------------------------------------------------------
// Value Existance Checkers
bool
CAFEState::EventType_Check(const string &eventName) const
{
	return(GetEventTypes().end() != GetEventTypes().find(eventName));
}

bool
CAFEState::TimeOffset_Check(const int offsetHour) const
{
        return(GetTimeOffsets().end() != GetTimeOffsets().find(offsetHour));
}

bool
CAFEState::CAFEVar_Check(const string &varName) const
{
        return(GetCAFEVars().end() != GetCAFEVars().find(varName));
}

bool
CAFEState::DataSource_Check(const string &sourceName) const
{
        return(GetDataSources().end() != GetDataSources().find(sourceName));
}
//--------------------------------------------------------------



//-------------------------------------------------
// Getter Functions
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
//------------------------------------------------------


//----------------------------------------------
set<string>
CAFEState::GetCAFEFields() const
{
	return(myCAFEInfo.GetCAFEFields());
}

set<string>
CAFEState::GetEventFields() const
{
	return(myCAFEInfo.GetEventFields());
}
//-----------------------------------------------








// ********************************
// --- Dynamic Parameter Access ---
// ********************************
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

set<int>
CAFEState::TimePeriod_Offsets() const
{
	return(GetTimeOffsets());
}

const int&
CAFEState::TimePeriod_Offset() const
{
	return(*myTimeOffsetIter);
}

set<string>
CAFEState::TimePeriod_Names() const
{
	return(myCAFEInfo.GetTimePeriods());
}

string
CAFEState::TimePeriod_Name() const
{
	return(GiveTimePeriod(TimePeriod_Offset()));
}

set<string>
CAFEState::Untrained_Names() const
{
        return(myCAFEInfo.GetUntrainedNames());
}

string
CAFEState::Untrained_Name() const
{
        return(GetUntrainedNameStem() + "_" + TimePeriod_Name());
}

set<string>
CAFEState::Trained_Names() const
{
        return(myCAFEInfo.GetTrainedNames());
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
	return(ResetEventVars());
}

CAFEState&
CAFEState::EventTypes_Next()
{
	myEventTypeIter++;
	return(ResetEventVars());
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
		ResetEventVars();
		return(true);
	}
	else
	{
		return(false);
	}
}

set<string>
CAFEState::EventType_Names() const
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

const string&
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
	return(GetEventVars().size());
}

CAFEState&
CAFEState::EventVars_Begin()
{
	myEventVarIter = GetEventVars().begin();
	return(ResetEventLevels());
}

CAFEState&
CAFEState::EventVars_Next()
{
	myEventVarIter++;
	return(ResetEventLevels());
}

bool
CAFEState::EventVars_HasNext() const
{
	return(myEventVarIter != GetEventVars().end());
}

bool
CAFEState::EventVars_JumpTo(const string &varName)
{
	const vector<string>::const_iterator varFind = find(GetEventVars().begin(), 
							    GetEventVars().end(),
							    varName);

	if (GetEventVars().end() != varFind)
	{
		myEventVarIter = varFind;
		ResetEventLevels();
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
	for (vector<string>::const_iterator aName = GetEventVars().begin();
	     aName != GetEventVars().end();
	     aName++)
	{
		theNames.insert(theNames.end(), *aName);
	}

	return(theNames);
}

const string&
CAFEState::EventVar_Name() const
{
	return(*myEventVarIter);
}

map<string, string>
CAFEState::EventVar_DataVar_Names() const
{
	map<string, string> logicalPairings;

	for (vector<string>::const_iterator aVarName = GetEventVars().begin();
	     aVarName != GetEventVars().end();
	     aVarName++)
	{
		logicalPairings[*aVarName] = GetDataSource().GiveDataVarName(*aVarName);
	}

	return(logicalPairings);
}

set<string>
CAFEState::DataVar_Names() const
{
        set<string> theNames;
        for (vector<string>::const_iterator aName = GetEventVars().begin();
             aName != GetEventVars().end();
             aName++)
        {
                theNames.insert(GetDataSource().GiveDataVarName(*aName));
        }

        return(theNames);
}

const string&
CAFEState::DataVar_Name() const
{
	return(GetDataSource().GiveDataVarName(EventVar_Name()));
}



// -------------------
// --- EventLevels ---
// -------------------
size_t
CAFEState::EventLevels_Size() const
{
	return(GetEventLevels().size());
}

CAFEState&
CAFEState::EventLevels_Begin()
{
	myEventVarLevelIter = GetEventLevels().begin();
	return(*this);
}

CAFEState&
CAFEState::EventLevels_Next()
{
	myEventVarLevelIter++;
	return(*this);
}

bool
CAFEState::EventLevels_HasNext() const
{
	return(myEventVarLevelIter != GetEventLevels().end());
}

bool
CAFEState::EventLevels_JumpTo(const string &levelName)
{
	const vector<string>::const_iterator levelFind = find(GetEventLevels().begin(),
							      GetEventLevels().end(),
							      levelName);

	if (levelFind != GetEventLevels().end())
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
CAFEState::EventLevel_Names() const
{
        set<string> theNames;
        for (vector<string>::const_iterator aName = GetEventLevels().begin();
             aName != GetEventLevels().end();
             aName++)
        {
                theNames.insert(theNames.end(), *aName);
        }

        return(theNames);
}

const string&
CAFEState::EventLevel_Name() const
{
	return(*myEventVarLevelIter);
}

map<string, string>
CAFEState::EventLevel_DataLevel_Names() const
{
	map<string, string> logicalPairings;
	for (vector<string>::const_iterator aName = GetEventLevels().begin();
	     aName != GetEventLevels().end();
	     aName++)
	{
		logicalPairings[*aName] = GetDataSource().GiveDataLevel(EventVar_Name(),
									GetCAFEVars().find(EventVar_Name())
									  ->second.GiveCAFELevelIndex(*aName));
	}

	return(logicalPairings);
}

set<string>
CAFEState::DataLevel_Names() const
{
        set<string> theNames;
        for (vector<string>::const_iterator aName = GetEventLevels().begin();
             aName != GetEventLevels().end();
             aName++)
        {
                theNames.insert(GetDataSource().GiveDataLevel(EventVar_Name(),
							      GetCAFEVars().find(EventVar_Name())
								->second.GiveCAFELevelIndex(*aName)));
        }

        return(theNames);
}

string
CAFEState::DataLevel_Name() const
{
	return(GetDataSource().GiveDataLevel(EventVar_Name(), GetCAFEVars().find(EventVar_Name())
								->second.GiveCAFELevelIndex(EventLevel_Name())));
}

set<string>
CAFEState::EventField_Names() const
{
        set<string> theNames;
        for (vector<string>::const_iterator aName = GetEventLevels().begin();
             aName != GetEventLevels().end();
             aName++)
        {
                theNames.insert(theNames.end(),
				(aName->empty() ? EventVar_Name()
						: EventVar_Name() + "_" + *aName));
        }

        return(theNames);
}

string
CAFEState::EventField_Name() const
{
	// Need to see if there is a level to 
	// append onto the event variable name.  (i.e. - "MSLP" has no levels, while "UWnd_850" does)
	return(EventLevel_Name().empty() ? EventVar_Name()
					 : EventVar_Name() + "_" + EventLevel_Name());
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

vector<string>
CAFEState::Extremum_Names() const
{
	return(GetExtremumNames());
}

const string&
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
		theNames.push_back(EventField_Name() + "_" + *aName);
	}

	return(theNames);
}

string
CAFEState::FieldExtremum_Name() const
{
	return(EventField_Name() + "_" + Extremum_Name());
}


// *********************************
// --- Dynamic CAFEVar Accessors ---
// *********************************
// ----------------
// --- CAFEVars ---
// ----------------
size_t
CAFEState::CAFEVars_Size() const
{
	return(GetCAFEVars().size());
}

CAFEState&
CAFEState::CAFEVars_Begin()
{
	myCAFEVarIter = GetCAFEVars().begin();
	return(CAFELevels_Begin());
}

CAFEState&
CAFEState::CAFEVars_Next()
{
	myCAFEVarIter++;
	return(CAFELevels_Begin());
}

bool
CAFEState::CAFEVars_HasNext() const
{
	return(GetCAFEVars().end() != myCAFEVarIter);
}

bool
CAFEState::CAFEVars_JumpTo(const string &varName)
{
	const map<string, CAFEVar>::const_iterator varFind = GetCAFEVars().find(varName);

	if (GetCAFEVars().end() != varFind)
	{
		myCAFEVarIter = varFind;
		CAFELevels_Begin();
		return(true);
	}
	else
	{
		return(false);
	}
}


set<string>
CAFEState::CAFEVar_Names() const
{
	return(myCAFEInfo.GetCAFEVarNames());
}

const string&
CAFEState::CAFEVar_Name() const
{
	return(myCAFEVarIter->first);
}

map<string, string>
CAFEState::CAFEVar_CAFEData_Names() const
{
	map<string, string> logicalPairings;
	for (map<string, CAFEVar>::const_iterator aVar = GetCAFEVars().begin();
	     aVar != GetCAFEVars().end();
	     aVar++)
	{
		logicalPairings[aVar->first] = GetDataSource().GiveDataVarName(aVar->first);
	}

	return(logicalPairings);
}

set<string>
CAFEState::CAFEData_Names() const
{
	set<string> theNames;
	for (map<string, CAFEVar>::const_iterator aVar = GetCAFEVars().begin();
	     aVar != GetCAFEVars().end();
	     aVar++)
	{
		theNames.insert(GetDataSource().GiveDataVarName(aVar->first));
	}

	return(theNames);
}

const string&
CAFEState::CAFEData_Name() const
{
	return(GetDataSource().GiveDataVarName(CAFEVar_Name()));
}


// -------------------
// --- CAFE Levels ---
// -------------------
size_t
CAFEState::CAFELevels_Size() const
{
	return(GetCAFELevels().size());
}

CAFEState&
CAFEState::CAFELevels_Begin()
{
	myCAFELevelIter = GetCAFELevels().begin();
	return(*this);
}

CAFEState&
CAFEState::CAFELevels_Next()
{
	myCAFELevelIter++;
	return(*this);
}

bool
CAFEState::CAFELevels_HasNext() const
{
	return(GetCAFELevels().end() != myCAFELevelIter);
}

bool
CAFEState::CAFELevels_JumpTo(const string &levelName)
{
	const map<string, size_t>::const_iterator levelFind = GetCAFELevels().find(levelName);

	if (GetCAFELevels().end() != levelFind)
	{
		myCAFELevelIter = levelFind;
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
	for (map<string, size_t>::const_iterator aLevel = GetCAFELevels().begin();
	     aLevel != GetCAFELevels().end();
	     aLevel++)
	{
		theNames.insert(theNames.end(), aLevel->first);
	}

	return(theNames);
}

const string&
CAFEState::CAFELevel_Name() const
{
	return(myCAFELevelIter->first);
}

map<string, string>
CAFEState::CAFELevel_CAFEDataLevel_Names() const
{
	map<string, string> logicalPairings;
	for (map<string, size_t>::const_iterator aLevel = GetCAFELevels().begin();
	     aLevel != GetCAFELevels().end();
	     aLevel++)
	{
		logicalPairings[aLevel->first] = GetDataSource().GiveDataLevel(CAFEVar_Name(), aLevel->second);
	}

	return(logicalPairings);
}

set<string>
CAFEState::CAFEDataLevel_Names() const
{
	set<string> theNames;
	for (map<string, size_t>::const_iterator aLevel = GetCAFELevels().begin();
	     aLevel != GetCAFELevels().end();
	     aLevel++)
	{
		theNames.insert( GetDataSource().GiveDataLevel(CAFEVar_Name(), aLevel->second) );
	}

	return(theNames);
}

const string&
CAFEState::CAFEDataLevel_Name() const
{
	return(GetDataSource().GiveDataLevel(CAFEVar_Name(), myCAFELevelIter->second));
}


set<string>
CAFEState::CAFEField_Names() const
{
	set<string> theNames;
	for (map<string, size_t>::const_iterator aLevel = GetCAFELevels().begin();
	     aLevel != GetCAFELevels().end();
	     aLevel++)
	{
		theNames.insert(theNames.end(), (aLevel->first.empty() ? CAFEVar_Name()
								       : CAFEVar_Name() + "_" + aLevel->first));
	}

	return(theNames);
}

string
CAFEState::CAFEField_Name() const
{
	return(CAFELevel_Name().empty() ? CAFEVar_Name()
					: CAFEVar_Name() + "_" + CAFELevel_Name());
}



// ***********************************
// --- Dynamic DataSource Accessor ---
// ***********************************
size_t
CAFEState::DataSources_Size() const
{
	return(GetDataSources().size());
}

CAFEState&
CAFEState::DataSources_Begin()
{
	myDataSourceIter = GetDataSources().begin();
	myCAFEInfo.SetDefaultDataSource(DataSource_Name());
	return(*this);
}

CAFEState&
CAFEState::DataSources_Next()
{
	myDataSourceIter++;
	myCAFEInfo.SetDefaultDataSource(DataSource_Name());
	return(*this);
}

bool
CAFEState::DataSources_HasNext() const
{
	return(GetDataSources().end() != myDataSourceIter);
}

bool
CAFEState::DataSources_JumpTo(const string &sourceName)
{
	const map<string, DataSource>::const_iterator sourceFind = GetDataSources().find(sourceName);

	if (GetDataSources().end() != sourceFind)
	{
		myDataSourceIter = sourceFind;
		myCAFEInfo.SetDefaultDataSource(DataSource_Name());
		return(true);
	}
	else
	{
		return(false);
	}
}

// Temporary...
const DataSource&
CAFEState::DefaultDataSource() const
{
	return(GetDataSource());
}


set<string>
CAFEState::DataSource_Names() const
{
	return(myCAFEInfo.GetDataSourceNames());
}

const string&
CAFEState::DataSource_Name() const
{
	return(myDataSourceIter->first);
}





// ***********************************
// ******* Private Functions *********
// ***********************************
string
CAFEState::GiveTimePeriod(const int &timeOffset) const
{
	return(OffsetToTimePeriod(timeOffset));
}

CAFEState&
CAFEState::ResetEventVars()
{
	if (EventTypes_HasNext())
	{
		myTempEventVarNames = GetEventType().GiveEventVariableNames();
	}
	else
	{
		myTempEventVarNames.clear();
	}

	return(EventVars_Begin());
}

CAFEState&
CAFEState::ResetEventLevels()
{
	if (EventVars_HasNext())
	{
		myTempEventVarLevelNames = GetEventType().GiveEventLevels(EventVar_Name());
	}
	else
	{
		myTempEventVarLevelNames.clear();
	}

	return(EventLevels_Begin());
}



//-----------------------------------------------
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

const vector<string>&
CAFEState::GetEventVars() const
{
	return(myTempEventVarNames);
}

const vector<string>&
CAFEState::GetEventLevels() const
{
	return(myTempEventVarLevelNames);
}

const map<string, size_t>&
CAFEState::GetCAFELevels() const
{
	return(GetCAFEVar().GiveCAFELevels());
}



const DataSource&
CAFEState::GetDataSource() const
{
	return(myDataSourceIter->second);
}

const CAFEVar&
CAFEState::GetCAFEVar() const
{
	return(myCAFEVarIter->second);
}

const EventType&
CAFEState::GetEventType() const
{
	return(myEventTypeIter->second);
}
//----------------------------------------------------

