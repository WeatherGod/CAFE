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

#include <Config/CAFEState.h>


CAFEState::CAFEState()
	:	myVerboseLevel(0),
		myTimeOffsets(),
		myUntrainedNameStem(""),
		myTrainedNameStem(""),
		myEventTypes(),
		myCAFEVars(),
		myExtremumNames(0),
		myDataSource(),
		myDomain(),
		myCAFEPath(""),
		myConfigFilename(""),
		myLoginUserName(""),
		myCAFEUserName(""),
		myServerName(""),
		myTimeOffsetIter(myTimeOffsets.end()),
		myEventTypeIter(myEventTypes.end()),
		myExtremumIter(myExtremumNames.end()),
		myTempEventVarNames(0),
		myEventVarIter(myTempEventVarNames.end()),
		myTempEventVarLevelNames(0),
		myEventVarLevelIter(myTempEventVarLevelNames.end())
{
}

CAFEState::CAFEState(const CAFEState &stateCopy)
	:	myVerboseLevel(stateCopy.myVerboseLevel),
		myTimeOffsets(stateCopy.myTimeOffsets),
                myEventTypes(stateCopy.myEventTypes),
                myCAFEVars(stateCopy.myCAFEVars),
		myExtremumNames(0),
                myDataSource(stateCopy.myDataSource),
                myDomain(stateCopy.myDomain),
                myCAFEPath(stateCopy.myCAFEPath),
		myConfigFilename(stateCopy.myConfigFilename),
                myLoginUserName(stateCopy.myLoginUserName),
                myCAFEUserName(stateCopy.myCAFEUserName),
                myServerName(stateCopy.myServerName),
                myTimeOffsetIter(),
                myEventTypeIter(),
		myExtremumIter(),
		myTempEventVarNames(stateCopy.myTempEventVarNames),
                myEventVarIter(),
		myTempEventVarLevelNames(stateCopy.myTempEventVarLevelNames),
                myEventVarLevelIter()
{
	if (stateCopy.myTimeOffsetIter != stateCopy.myTimeOffsets.end())
	{
		myTimeOffsetIter = myTimeOffsets.find(*stateCopy.myTimeOffsetIter);
	}
	else
	{
		myTimeOffsetIter = myTimeOffsets.end();
	}

	if (stateCopy.myEventTypeIter != stateCopy.myEventTypes.end())
	{
		myEventTypeIter = myEventTypes.find(stateCopy.myEventTypeIter->first);
	}
	else
	{
		myEventTypeIter = myEventTypes.end();
	}

	myExtremumIter = myExtremumNames.begin() 
			+ (stateCopy.myExtremumIter - stateCopy.myExtremumNames.begin());

	myEventVarIter = myTempEventVarNames.begin() 
			+ (stateCopy.myEventVarIter - stateCopy.myTempEventVarNames.begin());
	myEventVarLevelIter = myTempEventVarLevelNames.begin() 
			     + (stateCopy.myEventVarLevelIter - stateCopy.myTempEventVarLevelNames.begin());
}

CAFEState::~CAFEState()
{
}


CAFEState&
CAFEState::SetVerboseLevel(const int verbosity)
{
	myVerboseLevel = verbosity;
	return(*this);
}

CAFEState&
CAFEState::SetConfigFilename(const string &filename)
{
	myConfigFilename = filename;
	return(*this);
}

CAFEState&
CAFEState::SetCAFEPath(const string &pathname)
{
	myCAFEPath = pathname;
	return(*this);
}

CAFEState&
CAFEState::SetLoginUserName(const string &newUserName)
{
	myLoginUserName = newUserName;
	return(*this);
}

CAFEState&
CAFEState::SetCAFEUserName(const string &newUserName)
{
	myCAFEUserName = newUserName;
	return(*this);
}

CAFEState&
CAFEState::SetServerName(const string &newServerName)
{
	myServerName = newServerName;
	return(*this);
}

/*
CAFEState&
CAFEState::SetTimePeriods(const vector<string> &newTimePeriods)
{
	set<int> newTimeOffsets;

	for (vector<string>::const_iterator aTimePeriod = newTimePeriods.begin();
	     aTimePeriod != newTimePeriods.end();
	     aTimePeriod++)
	{
		const size_t timePeriodLoc = aTimePeriod->find_last_of("_T");

		if (timePeriodLoc == string::npos)
		{
			continue;
		}

		if ((*aTimePeriod)[timePeriodLoc + 2] == 'm')
		{
			newTimeOffsets.insert(newTimeOffsets.begin(),
					      -1 * atoi(aTimePeriod->substr(timePeriodLoc + 3).c_str()));
		}
		else if ((*aTimePeriod)[timePeriodLoc + 2] == 'p')
		{
			newTimeOffsets.insert(newTimeOffsets.begin(),
					      atoi(aTimePeriod->substr(timePeriodLoc + 3).c_str()));
		}
		else
		{
			continue;
		}
	}

	return(SetTimeOffsets(newTimeOffsets));
}
*/

CAFEState&
CAFEState::SetTimeOffsets(const set<int> &newTimeOffsets)
{
	myTimeOffsets = newTimeOffsets;
	return(TimePeriods_Begin());
}

CAFEState&
CAFEState::SetUntrainedNameStem(const string &newNameStem)
{
	myUntrainedNameStem = newNameStem;
	return(*this);
}

CAFEState&
CAFEState::SetTrainedNameStem(const string &newNameStem)
{
	myTrainedNameStem = newNameStem;
	return(*this);
}


CAFEState&
CAFEState::SetDataSource(const DataSource &theDataSource)
{
	myDataSource = theDataSource;
	return(*this);
}

CAFEState&
CAFEState::SetCAFEDomain(const CAFEDomain &newDomain)
{
	myDomain = newDomain;
	return(*this);
}

CAFEState&
CAFEState::SetCAFEVars(const map<string, CAFEVar> &newCAFEVars)
{
	myCAFEVars = newCAFEVars;
	return(*this);
}

CAFEState&
CAFEState::SetEventTypes(const map<string, EventType> &newEventTypes)
{
	myEventTypes = newEventTypes;
	return(EventTypes_Begin());
}

CAFEState&
CAFEState::SetExtremumNames(const vector<string> &newExtremumNames)
{
	myExtremumNames = newExtremumNames;
	return(Extrema_Begin());
}





int
CAFEState::GetVerboseLevel() const
{
	return(myVerboseLevel);
}

string
CAFEState::GetConfigFilename() const
{
	return(myConfigFilename);
}

string
CAFEState::GetCAFEPath() const
{
	return(myCAFEPath);
}

string
CAFEState::GetLoginUserName() const
{
	return(myLoginUserName);
}

string
CAFEState::GetCAFEUserName() const
{
	return(myCAFEUserName);
}

string
CAFEState::GetServerName() const
{
	return(myServerName);
}


const DataSource&
CAFEState::GetDataSource() const
{
	return(myDataSource);
}

const CAFEDomain&
CAFEState::GetCAFEDomain() const
{
	return(myDomain);
}


// -------------------
// --- TimePeriods ---
// -------------------
size_t
CAFEState::TimePeriods_Size() const
{
	return(myTimeOffsets.size());
}

CAFEState&
CAFEState::TimePeriods_Begin()
{
	myTimeOffsetIter = myTimeOffsets.begin();
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
	return(myTimeOffsetIter != myTimeOffsets.end());
}

bool
CAFEState::TimePeriods_JumpTo(const int &offset)
{
	const set<int>::const_iterator offsetFind = myTimeOffsets.find(offset);

	if (offsetFind != myTimeOffsets.end())
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
	return(myTimeOffsets);
}

int
CAFEState::TimePeriod_Offset() const
{
	return(*myTimeOffsetIter);
}

set<string>
CAFEState::TimePeriod_Names() const
{
	set<string> theNames;
	for (set<int>::const_iterator anOffset = myTimeOffsets.begin();
	     anOffset != myTimeOffsets.end();
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
CAFEState::Untrained_Names() const
{
        set<string> theNames;
        for (set<int>::const_iterator anOffset = myTimeOffsets.begin();
             anOffset != myTimeOffsets.end();
             anOffset++)
        {
                theNames.insert(theNames.end(), myUntrainedNameStem + "_" + GiveTimePeriod(*anOffset));
        }

        return(theNames);
}

string
CAFEState::Untrained_Name() const
{
        return(myUntrainedNameStem + "_" + TimePeriod_Name());
}

set<string>
CAFEState::Trained_Names() const
{
        set<string> theNames;
        for (set<int>::const_iterator anOffset = myTimeOffsets.begin();
             anOffset != myTimeOffsets.end();
             anOffset++)
        {
                theNames.insert(theNames.end(), myTrainedNameStem + "_" + GiveTimePeriod(*anOffset));
        }

        return(theNames);
}

string
CAFEState::Trained_Name() const
{
        return(myTrainedNameStem + "_" + TimePeriod_Name());
}



// ------------------
// --- EventTypes ---
// ------------------
size_t
CAFEState::EventTypes_Size() const
{
	return(myEventTypes.size());
}

CAFEState&
CAFEState::EventTypes_Begin()
{
	myEventTypeIter = myEventTypes.begin();
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
	return(myEventTypeIter != myEventTypes.end());
}

bool
CAFEState::EventTypes_JumpTo(const string &eventName)
{
	const map<string, EventType>::const_iterator eventFind = myEventTypes.find(eventName);

	if (eventFind != myEventTypes.end())
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
CAFEState::EventType_Names() const
{
	set<string> theNames;
	for (map<string, EventType>::const_iterator anEventType = myEventTypes.begin();
	     anEventType != myEventTypes.end();
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
                theNames.insert(theNames.end(), myDataSource.GiveDataVarName(*aName));
        }

        return(theNames);
}

string
CAFEState::DataVar_Name() const
{
	return(myDataSource.GiveDataVarName(*myEventVarIter));
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
				myDataSource.GiveDataLevel(*myEventVarIter,
							   myCAFEVars.find(*myEventVarIter)
							   ->second.GiveCAFELevelIndex(*aName)));
        }

        return(theNames);
}

string
CAFEState::DataLevel_Name() const
{
	return(myDataSource.GiveDataLevel(*myEventVarIter, 
					  myCAFEVars.find(*myEventVarIter)
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
	return(myExtremumNames.size());
}

CAFEState&
CAFEState::Extrema_Begin()
{
	myExtremumIter = myExtremumNames.begin();
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
	return(myExtremumIter != myExtremumNames.end());
}

bool
CAFEState::Extrema_JumpTo(const string &extremumName)
{
	const vector<string>::const_iterator extremumFind = find(myExtremumNames.begin(),
								 myExtremumNames.end(),
								 extremumName);

	if (extremumFind != myExtremumNames.end())
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
	return(myExtremumNames);
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
	theNames.reserve(myExtremumNames.size());

	for (vector<string>::const_iterator aName = myExtremumNames.begin();
	     aName != myExtremumNames.end();
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
	if (myEventTypeIter != myEventTypes.end())
	{
		myTempEventVarNames = myEventTypeIter->second.GiveCAFEVariableNames();
	}
	else
	{
		myTempEventVarNames.clear();
	}

	return(EventVars_Begin());
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

	return(CAFELevels_Begin());
}


