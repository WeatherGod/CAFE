#ifndef _DATASOURCE_H
#define _DATASOURCE_H

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <cctype>			// for size_t

#include "DataVar.h"

#include "Utils/CAFEException.h"	// for DataVar_Not_Found and DataLevel_Not_Found

class Projection_t;

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
                bool ValidConfig() const throw();
                bool IsValid() const throw();

		const string& GiveSourceName() const throw();
		const string& GiveProjectionName() const throw();
		const string& GiveProjectionConfig() const throw();

		const map<string, DataVar>& GiveDataVars() const throw();

		size_t GiveDataVarCount() const throw();
		const string& GiveDataVarName(const string &CAFEVarName) const throw(DataVar_Not_Found);
		
		const string& GiveDataLevel(const string &CAFEVarName,
					    const size_t &CAFELevelIndex) const throw(DataVar_Not_Found, DataLevel_Not_Found);
		size_t GiveDataLevelCount(const string &CAFEVarName) const throw(DataVar_Not_Found);

		pair<time_t, time_t> GiveTimeRange() const throw();
		const Projection_t* GiveProjection() const;

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

};


#endif
