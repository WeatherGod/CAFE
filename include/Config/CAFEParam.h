#ifndef _CAFEPARAM_H
#define _CAFEPARAM_H

#include <vector>
#include <map>
#include <set>
#include <string>

#include "CAFEVar.h"
#include "EventType.h"
#include "CAFEDomain.h"
#include "DataSource.h"

class CAFEParam
{
	public:
		// Constructors
		CAFEParam();

		// *** Setter functions ***
		//   scalars
		CAFEParam& SetVerboseLevel(const int verbosity) throw();
		CAFEParam& SetConfigFilename(const string &filename) throw(CAFEException);
		CAFEParam& SetCAFEPath(const string &pathname) throw(CAFEException);
		CAFEParam& SetLoginUserName(const string &newUserName) throw(CAFEException);
		CAFEParam& SetCAFEUserName(const string &newUserName) throw(CAFEException);
		CAFEParam& SetServerName(const string &newServerName) throw(CAFEException);
		CAFEParam& SetCAFEDomain(const CAFEDomain &newDomain) throw(CAFEException);
		CAFEParam& SetDefaultDataSource(const string &sourceName) throw(CAFEException);
		

		//   Time Periods
		CAFEParam& SetTimeOffsets(const set<int> &newTimeOffsets);
		CAFEParam& AddTimeOffsets(const set<int> &newTimeOffsets);
		CAFEParam& AddTimeOffset(const int newTimeOffset);

		CAFEParam& SetUntrainedNameStem(const string &newNameStem) throw(CAFEException);
		CAFEParam& SetTrainedNameStem(const string &newNameStem) throw(CAFEException);


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
		const int& GetVerboseLevel() const throw();
		const string& GetConfigFilename() const throw();
		const string& GetCAFEPath() const throw();
		const string& GetLoginUserName() const throw();
		const string& GetCAFEUserName() const throw();
		const string& GetServerName() const throw();
		const CAFEDomain& GetCAFEDomain() const throw();
		const string& GetDefaultDataSource() const throw();


		// -------------------------------------
		// --- TimePeriods and dataset names ---
		//--------------------------------------
		const set<int>& GetTimeOffsets() const throw();
		set<string> GetTimePeriods() const;
		const string& GetUntrainedNameStem() const throw();
		set<string> GetUntrainedNames() const;
		const string& GetTrainedNameStem() const throw();
		set<string> GetTrainedNames() const;


		// --------------------
		// --- Data Sources ---
		// --------------------
		const map<string, DataSource>& GetDataSources() const throw();
		set<string> GetDataSourceNames() const;


		// ----------------------
		// --- CAFE Variables ---
		// ----------------------
		const map<string, CAFEVar>& GetCAFEVars() const throw();
		set<string> GetCAFEVarNames() const;

		// ------------------
		// --- EventTypes ---
		// ------------------
		const map<string, EventType>& GetEventTypes() const throw();
		set<string> GetEventTypeNames() const;
	
	
		// ---------------
		// --- Extrema ---
		// ---------------
		const vector<string>& GetExtremumNames() const throw();


		// ************************************
		// --- Basic Interpretive Functions ---
		// ************************************
		/*   These functions perform basic interpretive
			duties of the CAFE parameters.  For
			example, one function would return all
			of the field names for a particular
			event type.
		*/
		// Returns all field names for the entire CAFE config.
		set<string> GetCAFEFields() const;

		// Returns all field names across the Event Types.
		set<string> GetEventFields() const;

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
