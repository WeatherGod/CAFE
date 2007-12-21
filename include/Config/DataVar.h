#ifndef _DATAVAR_H
#define _DATAVAR_H

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cctype>			// for size_t

class DataVar
{
	public:
		DataVar();

		void GetConfigInfo(string &FileLine, fstream &ReadData);

                // right now, they do the same thing, but there should be a difference.
                // ValidConfig() will return whether it was able to load a configuration correctly.
                // IsValid() will return whether the data contained is valid information.
                bool ValidConfig() const;                                       //
                bool IsValid() const;

		string GiveDataVarName() const;
		string GiveCAFEVarName() const;
		string GiveDataLevel(const size_t &CAFELevelIndex) const;

		size_t GiveDataLevelCount() const;

		bool AddDataLevel(const string &NewDataLevel, const size_t &CAFELevelIndex);
		

	private:
		string myDataVarName;
		string myCAFEVarName;
		map <size_t, string> myDataLevels;
		
		bool myIsConfigured;

		vector <string> myTagWords;
		void InitTagWords();
/*
	friend bool operator > (const DataVar &TheDataVar, const string DataVarName);
	friend bool operator < (const DataVar &TheDataVar, const string DataVarName);
	friend bool operator >= (const DataVar &TheDataVar, const string DataVarName);
	friend bool operator <= (const DataVar &TheDataVar, const string DataVarName);
	friend bool operator != (const DataVar &TheDataVar, const string DataVarName);
	friend bool operator == (const DataVar &TheDataVar, const string DataVarName);

	friend bool operator > (const string DataVarName, const DataVar &TheDataVar);
	friend bool operator < (const string DataVarName, const DataVar &TheDataVar);
	friend bool operator >= (const string DataVarName, const DataVar &TheDataVar);
	friend bool operator <= (const string DataVarName, const DataVar &TheDataVar);
	friend bool operator == (const string DataVarName, const DataVar &TheDataVar);
	friend bool operator != (const string DataVarName, const DataVar &TheDataVar);
*/
};


#endif
