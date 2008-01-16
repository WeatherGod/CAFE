#ifndef _DATASOURCE_H
#define _DATASOURCE_H

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <cctype>			// for size_t

#include "DataVar.h"

class DataSource
{
	public:
		DataSource();
		DataSource(const DataSource &sourceCopy);
		DataSource(const string &sourceName,
			   const string &projectionName, const string &projectionConf,
			   const map<string, DataVar> dataVars,
			   const time_t &timeStart, const time_t &timeEnd);

		void GetConfigInfo(string &FileLine, fstream &ReadData);

		// right now, they do the same thing, but there should be a difference.
                // ValidConfig() will return whether it was able to load a configuration correctly.
                // IsValid() will return whether the data contained is valid information.
                bool ValidConfig() const;                                       //
                bool IsValid() const;

		string GiveSourceName() const;
		string GiveProjectionName() const;
		string GiveProjectionConfig() const;

		size_t GiveDataVarCount() const;
		const string& GiveDataVarName(const string &CAFEVarName) const;
		
		const string& GiveDataLevel(const string &CAFEVarName, const size_t &CAFELevelIndex) const;
		size_t GiveDataLevelCount(const string &CAFEVarName) const;

		pair<time_t, time_t> GiveTimeRange() const;

		bool AddDataVar(const DataVar &NewDataVar);

	private:
		string mySourceName;
		string myProjectionName;
		string myProjectionConfig;
		map <string, DataVar> myDataVars;
		time_t myLowerTimeRange;
		time_t myUpperTimeRange;

		bool myIsConfigured;
	
		vector<string> InitTagWords() const;

	friend bool operator == (const DataSource &Lefty, const DataSource &Righty);
	friend bool operator != (const DataSource &Lefty, const DataSource &Righty);

	friend bool operator > (const DataSource &TheSource, const string &ASourceName);
	friend bool operator < (const DataSource &TheSource, const string &ASourceName);
	friend bool operator >= (const DataSource &TheSource, const string &ASourceName);
	friend bool operator <= (const DataSource &TheSource, const string &ASourceName);
	friend bool operator == (const DataSource &TheSource, const string &ASourceName);
	friend bool operator != (const DataSource &TheSource, const string &ASourceName);

	friend bool operator > (const string &ASourceName, const DataSource &TheSource);
	friend bool operator < (const string &ASourceName, const DataSource &TheSource);
	friend bool operator >= (const string &ASourceName, const DataSource &TheSource);
	friend bool operator <= (const string &ASourceName, const DataSource &TheSource);
	friend bool operator == (const string &ASourceName, const DataSource &TheSource);
	friend bool operator != (const string &ASourceName, const DataSource &TheSource);
};

class DataSourceID_t
{
	public:
		DataSourceID_t();
		explicit DataSourceID_t(const DataSourceID_t &ACopy);
		DataSourceID_t(const string &DataSourceName);
		DataSourceID_t(const char* DataSourceName);

		string GiveName() const { return myDataSourceName; };

		void operator = (const DataSourceID_t &ACopy);
		void operator = (const string &dataSourceName);

	private:
		string myDataSourceName;

	friend ostream& operator << (ostream& os, const DataSourceID_t &dataSourceID);
	friend bool operator == (const DataSourceID_t &lefty, const DataSourceID_t &righty);
	friend bool operator != (const DataSourceID_t &lefty, const DataSourceID_t &righty);
	friend bool operator < (const DataSourceID_t &lefty, const DataSourceID_t &righty);
};


#endif
