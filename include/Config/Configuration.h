#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include <vector>
#include <string>
#include <ctime>
#include <cctype>			// for size_t

#include "CAFEDomain.h"
#include "CAFEVar.h"
#include "DataSource.h"
#include "EventType.h"

#include <Projection_t.h>

class Configuration
{
	public:
		Configuration();
		Configuration(const string &ConfigFileName);

		bool ValidConfig() const;	// just returns whether the configuration file itself was a valid file syntaxily.
		bool IsValid() const;		// returns if the information in the configuration file was valid.  Checks to make sure
						// that all references are as expected.  That all variables are represented and all levels
						// are represented.

		bool SetConfigFilename(const string &FileName);
		bool SetDefaultSource(const DataSourceID_t &DataSourceID);



		bool ConfigFromFile(const string &ConfigFileName);
                bool ConfigFromFile();

                bool MakeConfigFile(const string &ConfigFileName);
                bool MakeConfigFile();

		
		size_t DatabaseCount() const;
		size_t CAFEVarCount() const;
		size_t DataSourceCount() const;
                size_t EventTypeCount() const;
		size_t ExtremaCount() const;
		size_t TimePeriodCount() const;

		const Projection_t* Give_DataSource_Projection() const;		// uses the default data source's projection.
		const Projection_t* Give_DataSource_Projection(const DataSourceID_t &DataSourceID) const;
		const CAFEDomain* GiveDomain() const;
		vector <float> GiveCAFEDomainBoundingBox() const;

		pair<time_t, time_t> Give_DataSource_TimeRange() const;		// uses the default data source's time range
		pair<time_t, time_t> Give_DataSource_TimeRange(const DataSourceID_t &DataSourceID) const;

		// Note that the CAFEVars will NEVER be accessible by index numbers publically.
		// So, "GiveCAFEVarName(const size_t CAFEVarIndex)" will NOT be allowed.
		string GiveDatabaseName(const size_t &DatabaseIndex) const;
		string GiveClusteredDatabaseName(const size_t &DatabaseIndex) const;
		string GiveDatabaseName(const string &TimePeriod) const;
		string GiveClusteredDatabaseName(const string &TimePeriod) const;
                string GiveEventTypeName(const size_t &EventTypeIndex) const;
		string GiveExtremaName(const size_t &ExtremaIndex) const;
		string GiveConfigFilename() const;
                string GiveTimePeriod(const size_t &DatabaseIndex) const;
//		string GiveDataSourceName(const size_t &DataSourceIndex) const;
		string GiveDefaultSourceName() const;


		vector <string> GiveDatabaseNames() const;
		vector <string> GiveClusteredDatabaseNames() const;
		vector <string> GiveEventTypeNames() const;
		vector <string> GiveDataSourceNames() const;
		vector <string> GiveCAFEVarNames() const;
		vector <string> GiveExtremaNames() const;
		vector <string> GiveTimePeriods() const;

		// Note that the CAFEVars will NEVER be accessible by index numbers publically.
		// So, "GiveCAFEVarIndex(const string CAFEVarName)" will NOT be allowed.
		size_t GiveDefaultSourceIndex() const;
		size_t GiveEventTypeIndex(const string &EventTypeName) const;
//		size_t GiveDataSourceIndex(const string &DataSourceName) const;
		size_t GiveDatabaseIndex(const string &DatabaseName) const;
		size_t GiveClusteredDatabaseIndex(const string &ClustDatabaseName) const;
		size_t GiveTimePeriodIndex(const string &TimePeriods) const;


		// Container-level counters...
		size_t CAFEVar_LevelCount(const string &CAFEVarName) const;

		size_t DataSource_VarCount(const DataSourceID_t &DataSourceID) const;
		size_t DataSource_DataLevelCount(const DataSourceID_t &DataSourceID, const string &CAFEVarName) const;
		size_t DataSource_VarCount() const;		// uses the default datasource
		size_t DataSource_DataLevelCount(const string &CAFEVarName) const;	// uses the default datasource

		size_t EventType_VarCount(const EventTypeID_t &EventTypeID) const;
		size_t EventType_LevelCount(const EventTypeID_t &EventTypeID, const string &CAFEVarName) const;
		
		
		//Container-level reporters...
		string Give_DataSource_DataLevel(const DataSourceID_t &DataSourceID, const string &CAFEVarName, const string &CAFELevelName) const;
		string Give_DataSource_DataName(const DataSourceID_t &DataSourceID, const string &CAFEVarName) const;
		string Give_DataSource_DataLevel(const string &CAFEVarName, const string &CAFELevelName) const;
		string Give_DataSource_DataName(const string &CAFEVarName) const;

		string Give_CAFEVar_LevelName(const string &CAFEVarName, const string &CAFEVarLabel) const;
		vector <string> Give_CAFEVar_LevelNames(const string &CAFEVarName) const;
		vector <string> Give_CAFEVar_CAFEVarLabels(const string &CAFEVarName) const;

		//vector <string> Give_DataSource_DataLevels(const DataSourceID_t &DataSourceID, const string &CAFEVarName) const;
		vector <string> Give_DataSource_DataNames(const DataSourceID_t &DataSourceID) const;
		//vector <string> Give_DataSource_DataLevels(const string &CAFEVarName) const;
		vector <string> Give_DataSource_DataNames() const;

		vector <string> Give_EventType_CAFEVarNames(const EventTypeID_t &EventTypeID) const;
		vector <string> Give_EventType_LevelNames(const EventTypeID_t &EventTypeID, const string &CAFEVarName) const;
		vector <string> Give_EventType_CAFEVarLabels(const EventTypeID_t &EventTypeID, const string &CAFEVarName) const;


		// Configuration modifiers		
		bool ReplaceDatabaseStems(const string &NewDatabaseStem, const string &NewProcessedStem);
		bool ReplaceDomain(const CAFEDomain &NewDomain);
		bool ReplaceCAFEVar(const CAFEVar &NewVar, const string &OldCAFEVarName);
		bool ReplaceDataSource(const DataSource &NewSource, const DataSourceID_t &OldDataSourceID);
		bool ReplaceDataSource(const DataSource &NewSource);	// changes the default datasource to a new datasource that doesn't exist
		bool ReplaceEventType(const EventType &NewType, const EventTypeID_t &OldEventTypeID);
				
		bool AddTimePeriod(const string &NewTimePeriod);
                bool AddCAFEVar(const CAFEVar &NewVar);
                bool AddDataSource(const DataSource &NewSource);
                bool AddEventType(const EventType &NewType);

		// Removal functions.
		// Note that there is no removal function for the default datasource.
		// You must explicitly remove the default datasource, remembering 
		// that the default datasourceID needs to be reset.
		bool RemoveTimePeriod(const size_t &TimePeriodIndex);
		bool RemoveCAFEVar(const string &CAFEVarName);
		bool RemoveDataSource(const DataSourceID_t &DataSourceID);
		bool RemoveEventType(const EventTypeID_t &EventTypeID);

	private:
		vector <string> myDatabaseNames;
		vector <string> myProcessedNames;
		vector <string> myPeakValleys;
		vector <string> myTimePeriods;

		bool myIsConfiged;

		string myConfigFileName;
		vector <string> myTagWords;

		string myDatabaseStem;
		string myProcessedStem;
		DataSourceID_t myDefaultDataSourceID;

		CAFEDomain myCAFEDomain;
		CAFEDomain myDummyDomain;

		vector <CAFEVar> myCAFEVars;
		CAFEVar myDummyCAFEVar;

		map <DataSourceID_t, DataSource> myDataSources;
		DataSource myDummyDataSource;

		vector <EventType> myEventTypes;
		EventType myDummyEventType;

		void InitTagWords();
		void GetConfigInfo(string &FileLine, fstream &ReadData);
		void CompileDatabaseNames();
		
};


#endif
