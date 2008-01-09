using namespace std;

#include <map>
#include <set>
#include <vector>
#include <string>

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
		myCAFEPath(""),
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
	myConfigFilename = filename;
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
	myCAFEPath = pathname;
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
	myLoginUserName = newUserName;
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
	myCAFEUserName = newUserName;
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
	myServerName = newServerName;
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
	myDomain = newDomain;
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
	myDefaultDataSource = sourceName;
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

CAFEParam&
CAFEParam::SetUntrainedNameStem(const string &newNameStem)
{
	myUntrainedNameStem = newNameStem;
	return(*this);
}

const string&
CAFEParam::GetUntrainedNameStem() const
{
	return(myUntrainedNameStem);
}

CAFEParam&
CAFEParam::SetTrainedNameStem(const string &newNameStem)
{
	myTrainedNameStem = newNameStem;
	return(*this);
}

const string&
CAFEParam::GetTrainedNameStem() const
{
	return(myTrainedNameStem);
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
	myDataSources = newDataSources;
	return(*this);
}

const map<string, DataSource>&
CAFEParam::GetDataSources() const
{
	return(myDataSources);
}

CAFEParam&
CAFEParam::AddDataSources(const map<string, DataSource> &newDataSources)
{
	/* TODO: include some algorithm to detect duplicates
		 and see if the current data source can get new
		 information from the new data source.
		 Maybe...
	*/
	myDataSources.insert(newDataSources.begin(), newDataSources.end());
	return(*this);
}

CAFEParam&
CAFEParam::AddDataSource(const string &sourceName, const DataSource &newDataSource)
{
	myDataSources.insert(myDataSources.end(), make_pair(sourceName, newDataSource));
	return(*this);
}



// ----------------------
// --- CAFE Variables ---
// ----------------------
CAFEParam&
CAFEParam::SetCAFEVars(const map<string, CAFEVar> &newCAFEVars)
{
	myCAFEVars = newCAFEVars;
	return(*this);
}

const map<string, CAFEVar>&
CAFEParam::GetCAFEVars() const
{
	return(myCAFEVars);
}

CAFEParam&
CAFEParam::AddCAFEVars(const map<string, CAFEVar> &newCAFEVars)
{
	/* TODO: include some algorithm to detect duplicates
		 and see if the current CAFEVar can get new
		 information from the new CAFEVar.
		 Maybe...
        */
	
	myCAFEVars.insert(newCAFEVars.begin(), newCAFEVars.end());
	return(*this);
}

CAFEParam&
CAFEParam::AddCAFEVar(const string &varName, const CAFEVar& newCAFEVar)
{
	myCAFEVars.insert(myCAFEVars.end(), make_pair(varName, newCAFEVar));
	return(*this);
}



// ------------------
// --- Event Type ---
// ------------------
CAFEParam&
CAFEParam::SetEventTypes(const map<string, EventType> &newEventTypes)
{
	myEventTypes = newEventTypes;
	return(*this);
}

const map<string, EventType>&
CAFEParam::GetEventTypes() const
{
	return(myEventTypes);
}

CAFEParam&
CAFEParam::AddEventTypes(const map<string, EventType> &newEvents)
{
	/* TODO: include some algorithm to detect duplicates
		 and see if the current EventType can get new
		 information from the new EventType.
		 Maybe...
        */
        myEventTypes.insert(newEvents.begin(), newEvents.end());
        return(*this);
}

CAFEParam&
CAFEParam::AddEventType(const string &eventName, const EventType& newEvent)
{
        myEventTypes.insert(myEventTypes.end(), make_pair(eventName, newEvent));
        return(*this);
}



// ---------------
// --- Extrema ---
// ---------------
CAFEParam&
CAFEParam::SetExtremumNames(const vector<string> &newExtremumNames)
{
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
	myExtremumNames.insert(myExtremumNames.end(), newExtremumNames.begin(),
						      newExtremumNames.end());
	return(*this);
}

