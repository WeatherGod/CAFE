using namespace std;

#include <map>
#include <set>
#include <vector>
#include <algorithm>		// for find()
#include <string>
#include <cmath>		// for abs()
#include <cstdio>		// for snprintf()

// Temporary
#include <Utils/CAFEUtly.h>	// for OffsetToTimePeriod()

#include <Config/CAFEVar.h>
#include <Config/EventType.h>
#include <Config/CAFEDomain.h>
#include <Config/DataSource.h>

#include <Config/CAFEParam.h>


CAFEParam::CAFEParam()
	:	myVerboseLevel(0),
		myUntrainedNameStem(""),
		myTrainedNameStem(""),
		myDefaultDataSource(""),
		myConfigFilename(""),
		myCAFEPath("."),
		myLoginUserName(""),
		myCAFEUserName(""),
		myServerName(""),
		myDomain(),
		myTimeOffsets(),
		myEventTypes(),
		myCAFEVars(),
		myExtremumNames(0),
		myDataSources()
{
}


// ---------------
// --- Scalars ---
// ---------------
CAFEParam&
CAFEParam::SetVerboseLevel(const int verbosity)
{
	myVerboseLevel = verbosity;
	return(*this);
}

const int&
CAFEParam::GetVerboseLevel() const
{
        return(myVerboseLevel);
}


CAFEParam&
CAFEParam::SetConfigFilename(const string &filename)
{
	if (!filename.empty())
	{
		myConfigFilename = filename;
	}

	return(*this);
}

const string&
CAFEParam::GetConfigFilename() const
{
        return(myConfigFilename);
}


CAFEParam&
CAFEParam::SetCAFEPath(const string &pathname)
{
	if (!pathname.empty())
	{
		myCAFEPath = pathname;
	}
	return(*this);
}

const string&
CAFEParam::GetCAFEPath() const
{
        return(myCAFEPath);
}


CAFEParam&
CAFEParam::SetLoginUserName(const string &newUserName)
{
	if (!newUserName.empty())
	{
		myLoginUserName = newUserName;
	}
	return(*this);
}

const string&
CAFEParam::GetLoginUserName() const
{
        return(myLoginUserName);
}


CAFEParam&
CAFEParam::SetCAFEUserName(const string &newUserName)
{
	if (!newUserName.empty())
	{
		myCAFEUserName = newUserName;
	}
	return(*this);
}

const string&
CAFEParam::GetCAFEUserName() const
{
        return(myCAFEUserName);
}


CAFEParam&
CAFEParam::SetServerName(const string &newServerName)
{
	if (!newServerName.empty())
	{
		myServerName = newServerName;
	}
	return(*this);
}

const string&
CAFEParam::GetServerName() const
{
        return(myServerName);
}


CAFEParam&
CAFEParam::SetCAFEDomain(const CAFEDomain &newDomain)
{
	if (newDomain.IsValid())
	{
		myDomain = newDomain;
	}
	return(*this);
}

const CAFEDomain&
CAFEParam::GetCAFEDomain() const
{
	return(myDomain);
}


CAFEParam&
CAFEParam::SetDefaultDataSource(const string &sourceName)
{
	if (!sourceName.empty())
	{
		myDefaultDataSource = sourceName;
	}

	return(*this);
}

const string&
CAFEParam::GetDefaultDataSource() const
{
	return(myDefaultDataSource);
}



// --------------------
// --- Time Offsets ---
// --------------------
CAFEParam&
CAFEParam::SetTimeOffsets(const set<int> &newTimeOffsets)
{
	myTimeOffsets = newTimeOffsets;
	return(*this);
}

const set<int>&
CAFEParam::GetTimeOffsets() const
{
	return(myTimeOffsets);
}

set<string>
CAFEParam::GetTimePeriods() const
{
	set<string> timePeriods;
	for (set<int>::const_iterator anOffset = myTimeOffsets.begin();
	     anOffset != myTimeOffsets.end();
	     anOffset++)
	{
		timePeriods.insert(timePeriods.end(), OffsetToTimePeriod(*anOffset));
	}

	return(timePeriods);
}

CAFEParam&
CAFEParam::SetUntrainedNameStem(const string &newNameStem)
{
	if (!newNameStem.empty())
	{
		myUntrainedNameStem = newNameStem;
	}
	return(*this);
}

const string&
CAFEParam::GetUntrainedNameStem() const
{
	return(myUntrainedNameStem);
}

set<string>
CAFEParam::GetUntrainedNames() const
{
	set<string> theNames;
	for (set<int>::const_iterator anOffset = myTimeOffsets.begin();
	     anOffset != myTimeOffsets.end();
	     anOffset++)
	{
		theNames.insert(theNames.end(), myUntrainedNameStem + "_" + OffsetToTimePeriod(*anOffset));
	}

	return(theNames);
}

CAFEParam&
CAFEParam::SetTrainedNameStem(const string &newNameStem)
{
	if (!newNameStem.empty())
	{
		myTrainedNameStem = newNameStem;
	}
	return(*this);
}

const string&
CAFEParam::GetTrainedNameStem() const
{
	return(myTrainedNameStem);
}

set<string>
CAFEParam::GetTrainedNames() const
{
	set<string> theNames;
	for (set<int>::const_iterator anOffset = myTimeOffsets.begin();
	     anOffset != myTimeOffsets.end();
	     anOffset++)
	{
		theNames.insert(theNames.end(), myTrainedNameStem + "_" + OffsetToTimePeriod(*anOffset));
	}

	return(theNames);
}

CAFEParam&
CAFEParam::AddTimeOffsets(const set<int> &newTimeOffsets)
{
	myTimeOffsets.insert(newTimeOffsets.begin(), newTimeOffsets.end());
	return(*this);
}

CAFEParam&
CAFEParam::AddTimeOffset(const int newTimeOffset)
{
	myTimeOffsets.insert(myTimeOffsets.end(), newTimeOffset);
	return(*this);
}



// --------------------
// --- Data Sources ---
// --------------------
CAFEParam&
CAFEParam::SetDataSources(const map<string, DataSource> &newDataSources)
{
	/* TODO: Yes, this does kinda assume that all of the entries are
           valid.  Need to think about that for a while...
        */

	myDataSources = newDataSources;
	return(*this);
}

const map<string, DataSource>&
CAFEParam::GetDataSources() const
{
	return(myDataSources);
}

set<string>
CAFEParam::GetDataSourceNames() const
{
	set<string> theNames;
	for (map<string, DataSource>::const_iterator aSource = myDataSources.begin();
	     aSource != myDataSources.end();
	     aSource++)
	{
		theNames.insert(theNames.end(), aSource->first);
	}

	return(theNames);
}

CAFEParam&
CAFEParam::AddDataSources(const map<string, DataSource> &newDataSources)
{
	for (map<string, DataSource>::const_iterator aSource = newDataSources.begin();
	     aSource != newDataSources.end();
	     aSource++)
	{
		AddDataSource(aSource->first, aSource->second);
	}

	return(*this);
}

CAFEParam&
CAFEParam::AddDataSource(const string &sourceName, const DataSource &newDataSource)
{
	if (!newDataSource.IsValid())
	{
		return(*this);
	}

	if (sourceName != newDataSource.GiveSourceName())
	{
		return(*this);
	}

	const map<string, DataSource>::iterator sourceFind = myDataSources.find(sourceName);

	if (sourceFind == myDataSources.end())
	{
		myDataSources.insert(myDataSources.end(), make_pair(sourceName, newDataSource));
	}
	else
	{
		// TODO: Update the data source with new information.
	}

	return(*this);
}



// ----------------------
// --- CAFE Variables ---
// ----------------------
CAFEParam&
CAFEParam::SetCAFEVars(const map<string, CAFEVar> &newCAFEVars)
{
	/* TODO: Yes, this does kinda assume that all of the entries are
	   valid.  Need to think about that for a while...
	*/
	myCAFEVars = newCAFEVars;
	return(*this);
}

const map<string, CAFEVar>&
CAFEParam::GetCAFEVars() const
{
	return(myCAFEVars);
}

set<string>
CAFEParam::GetCAFEVarNames() const
{
	set<string> theNames;
	for (map<string, CAFEVar>::const_iterator aVar = myCAFEVars.begin();
	     aVar != myCAFEVars.end();
	     aVar++)
	{
		theNames.insert(theNames.end(), aVar->first);
	}

	return(theNames);
}

CAFEParam&
CAFEParam::AddCAFEVars(const map<string, CAFEVar> &newCAFEVars)
{
	for (map<string, CAFEVar>::const_iterator aVar = newCAFEVars.begin();
	     aVar != newCAFEVars.end();
	     aVar++)
	{
		AddCAFEVar(aVar->first, aVar->second);
	}

	return(*this);
}

CAFEParam&
CAFEParam::AddCAFEVar(const string &varName, const CAFEVar& newCAFEVar)
{
	if (!newCAFEVar.IsValid())
	{
		return(*this);
	}

	if (varName != newCAFEVar.GiveCAFEVarName())
	{
		return(*this);
	}

	const map<string, CAFEVar>::iterator varFind = myCAFEVars.find(varName);

	if (varFind == myCAFEVars.end())
	{
		myCAFEVars.insert(myCAFEVars.end(), make_pair(varName, newCAFEVar));
	}
	else
	{
		// TODO: Update this element... maybe...
	}

	return(*this);
}



// ------------------
// --- Event Type ---
// ------------------
CAFEParam&
CAFEParam::SetEventTypes(const map<string, EventType> &newEventTypes)
{
	/* TODO: Yes, this does kinda assume that all of the entries are
           valid.  Need to think about that for a while...
        */

	myEventTypes = newEventTypes;
	return(*this);
}

const map<string, EventType>&
CAFEParam::GetEventTypes() const
{
	return(myEventTypes);
}

set<string>
CAFEParam::GetEventTypeNames() const
{
	set<string> theNames;
	for (map<string, EventType>::const_iterator anEvent = myEventTypes.begin();
	     anEvent != myEventTypes.end();
	     anEvent++)
	{
		theNames.insert(theNames.end(), anEvent->first);
	}

	return(theNames);
}

CAFEParam&
CAFEParam::AddEventTypes(const map<string, EventType> &newEvents)
{
	for (map<string, EventType>::const_iterator anEvent = newEvents.begin();
	     anEvent != newEvents.end();
	     anEvent++)
	{
		AddEventType(anEvent->first, anEvent->second);
	}

        return(*this);
}

CAFEParam&
CAFEParam::AddEventType(const string &eventName, const EventType& newEvent)
{
	if (!newEvent.IsValid())
	{
		return(*this);
	}

	if (eventName != newEvent.GiveEventTypeName())
	{
		return(*this);
	}

	const map<string, EventType>::iterator eventFind = myEventTypes.find(eventName);
	if (eventFind == myEventTypes.end())
	{
		myEventTypes.insert(myEventTypes.end(), make_pair(eventName, newEvent));
	}
	else
	{
		// TODO: Update the event type with new information
	}

        return(*this);
}



// ---------------
// --- Extrema ---
// ---------------
CAFEParam&
CAFEParam::SetExtremumNames(const vector<string> &newExtremumNames)
{
	/* TODO: Yes, this does kinda assume that all of the entries are
           valid.  Need to think about that for a while...
        */

	myExtremumNames = newExtremumNames;
	return(*this);
}

const vector<string>&
CAFEParam::GetExtremumNames() const
{
	return(myExtremumNames);
}

CAFEParam&
CAFEParam::AddExtremumNames(const vector<string> &newExtremumNames)
{
	for (vector<string>::const_iterator anExtremum = newExtremumNames.begin();
	     anExtremum != newExtremumNames.end();
	     anExtremum++)
	{
		AddExtremumName(*anExtremum);
	}

	return(*this);
}

CAFEParam&
CAFEParam::AddExtremumName(const string &newExtremumName)
{
	if (!newExtremumName.empty() && myExtremumNames.end() == find(myExtremumNames.begin(),
								      myExtremumNames.end(),
								      newExtremumName))
	{
		myExtremumNames.push_back(newExtremumName);
	}

	return(*this);
}



// ************************************
// --- Basic Interpretive Functions ---
// ************************************
set<string>
CAFEParam::GetCAFEFields() const
{
	set<string> fieldNames;
	for (map<string, CAFEVar>::const_iterator aVar = GetCAFEVars().begin();
	     aVar != GetCAFEVars().end();
	     aVar++)
	{
		const vector<string> levelNames = aVar->second.GiveCAFELevelNames();
		for (vector<string>::const_iterator aLevel = levelNames.begin();
		     aLevel != levelNames.end();
		     aLevel++)
		{
			fieldNames.insert(fieldNames.end(), (aLevel->empty() ? aVar->first
									     : aVar->first + "_" + *aLevel));
		}
	}

	return(fieldNames);
}

set<string>
CAFEParam::GetEventFields() const
{
	set<string> fieldNames;
	for (map<string, EventType>::const_iterator anEvent = GetEventTypes().begin();
	     anEvent != GetEventTypes().end();
	     anEvent++)
	{
		const vector<string> eventVarNames = anEvent->second.GiveEventVariableNames();
		for (vector<string>::const_iterator aVar = eventVarNames.begin();
		     aVar != eventVarNames.end();
		     aVar++)
		{
			const vector<string> levelNames = anEvent->second.GiveEventLevels(*aVar);
			for (vector<string>::const_iterator aLevel = levelNames.begin();
			     aLevel != levelNames.end();
			     aLevel++)
			{
				fieldNames.insert(fieldNames.end(), (aLevel->empty() ? *aVar
										     : *aVar + "_" + *aLevel));
			}
		}
	}

	return(fieldNames);
}


