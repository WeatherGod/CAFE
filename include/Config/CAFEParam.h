#ifndef _CAFEPARAM_H
#define _CAFEPARAM_H

#include <vector>
#include <map>
#include <set>
#include <string>

// I don't need the include headers for these classes,
// so just declare them for now.
class CAFEVar;
class EventType;
class CAFEDomain;
class DataSource;

class CAFEParam
{
	public:
		// Constructors
		CAFEParam();

		// *** Setter functions ***
		//   scalars
		CAFEParam& SetVerboseLevel(const int verbosity);
		CAFEParam& SetConfigFilename(const string &filename);
		CAFEParam& SetCAFEPath(const string &pathname);
		CAFEParam& SetLoginUserName(const string &newUserName);
		CAFEParam& SetCAFEUserName(const string &newUserName);
		CAFEParam& SetServerName(const string &newServerName);
		CAFEParam& SetCAFEDomain(const CAFEDomain &newDomain);
		CAFEParam& SetDefaultDataSource(const string &sourceName);
		

		//   Time Periods
		CAFEParam& SetTimeOffsets(const set<int> &newTimeOffsets);
		CAFEParam& AddTimeOffsets(const set<int> &newTimeOffsets);
		CAFEParam& AddTimeOffset(const int newTimeOffset);

		CAFEParam& SetUntrainedNameStem(const string &newNameStem);
		CAFEParam& SetTrainedNameStem(const string &newNameStem);


		//   Data Sources
		CAFEParam& SetDataSources(const map<string, DataSource> &newDataSources);
		CAFEParam& AddDataSources(const map<string, DataSource> &newDataSources);
		CAFEParam& AddDataSource(const string &sourceName, const DataSource &newDataSource);


		//   CAFE Variables
		CAFEParam& SetCAFEVars(const map<string, CAFEVar> &newCAFEVars);
		CAFEParam& AddCAFEVars(const map<string, CAFEVar> &newCAFEVars);
		CAFEParam& AddCAFEVar(const string &varName, const CAFEVar &newCAFEVar);


		//   Event Types
		CAFEParam& SetEventTypes(const map<string, EventType> &newEventTypes);
		CAFEParam& AddEventTypes(const map<string, EventType> &newEventTypes);
		CAFEParam& AddEventType(const string &typeName, const EventType &newEventType);


		//   Extrema
		CAFEParam& SetExtremumNames(const vector<string> &newExtremumNames);
		CAFEParam& AddExtremumNames(const vector<string> &newExtremumNames);
		CAFEParam& AddExtremumName(const string &newExtremumName);



		// *** Getter functions ***
		//   scalars
		const int& GetVerboseLevel() const;
		const string& GetConfigFilename() const;
		const string& GetCAFEPath() const;
		const string& GetLoginUserName() const;
		const string& GetCAFEUserName() const;
		const string& GetServerName() const;
		const CAFEDomain& GetCAFEDomain() const;
		const string& GetDefaultDataSource() const;


		// -------------------------------------
		// --- TimePeriods and dataset names ---
		//--------------------------------------
		const set<int>& GetTimeOffsets() const;
		const string& GetUntrainedNameStem() const;
		const string& GetTrainedNameStem() const;


		// --------------------
		// --- Data Sources ---
		// --------------------
		const map<string, DataSource>& GetDataSources() const;


		// ----------------------
		// --- CAFE Variables ---
		// ----------------------
		const map<string, CAFEVar>& GetCAFEVars() const;

		// ------------------
		// --- EventTypes ---
		// ------------------
		const map<string, EventType>& GetEventTypes() const;
	
	
		// ---------------
		// --- Extrema ---
		// ---------------
		const vector<string>& GetExtremumNames() const;

	private:
		int myVerboseLevel;
		string myUntrainedNameStem;
		string myTrainedNameStem;
		string myDefaultDataSource;

		// CAFE IO access information
		string myConfigFilename;
		string myCAFEPath;
		string myLoginUserName;
		string myCAFEUserName;
		string myServerName;

		// This might become merged with the data source
		CAFEDomain myDomain;

		set<int> myTimeOffsets;
		map<string, EventType> myEventTypes;
		map<string, CAFEVar> myCAFEVars;
		vector<string> myExtremumNames;
		map<string, DataSource> myDataSources;
};


#endif
