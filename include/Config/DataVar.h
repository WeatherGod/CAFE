#ifndef _DATAVAR_H
#define _DATAVAR_H

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cctype>			// for size_t

#include "Utils/CAFEException.h"	// for DataLevel_Not_Found

class DataVar
{
	public:
		DataVar();
		DataVar(const DataVar &varCopy);
		DataVar(const string &dataName, const string &CAFEName,
			const map<size_t, string> &dataLevels);

		void GetConfigInfo(string &FileLine, fstream &ReadData);

                // right now, they do the same thing, but there should be a difference.
                // ValidConfig() will return whether it was able to load a configuration correctly.
                // IsValid() will return whether the data contained is valid information.
                bool ValidConfig() const throw();
                bool IsValid() const throw();

		const map<size_t, string>& GiveDataLevels() const throw();

		const string& GiveDataVarName() const throw();
		const string& GiveCAFEVarName() const throw();
		const string& GiveDataLevel(const size_t &CAFELevelIndex) const throw(DataLevel_Not_Found);

		size_t GiveDataLevelCount() const throw();

		bool AddDataLevel(const string &NewDataLevel, const size_t &CAFELevelIndex);
		

	private:
		string myDataVarName;
		string myCAFEVarName;
		map<size_t, string> myDataLevels;
		
		bool myIsConfigured;

		vector<string> InitTagWords() const;
};


#endif
