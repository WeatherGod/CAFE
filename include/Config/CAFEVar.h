#ifndef _CAFEVAR_H
#define _CAFEVAR_H

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cctype>		// for size_t

class CAFEVar
{
	public:
		CAFEVar();
		CAFEVar(const CAFEVar &varCopy);
		CAFEVar(const string &varName, const map<string, size_t> &CAFELevels);

		void GetConfigInfo(string &FileLine, fstream &ReadData);

		// right now, they do the same thing, but there should be a difference.
                // ValidConfig() will return whether it was able to load a configuration correctly.
                // IsValid() will return whether the data contained is valid information.
                bool ValidConfig() const;
                bool IsValid() const;

		string GiveCAFEVarName() const;
		size_t GiveCAFELevelCount() const;

		string GiveCAFELevelName(const size_t &CAFELevelIndex) const;
		// Gives the LevelNames in the order of the Names!
		vector <string> GiveCAFELevelNames() const;

		size_t GiveCAFELevelIndex(const string &CAFELevelName) const;
		// Gives the LevelIndicies in the order of the LevelName, not the indicies!
		vector <size_t> GiveCAFELevelIndicies() const;
		
		bool AddCAFELevel(const string &NewCAFELevelName, const size_t &CAFELevelIndex);
		
	private:
		string myCAFEVarName;

		// Cannot be a map!
		// TODO: Why not?
		vector <size_t> myCAFELevelIndicies;
		vector <string> myCAFELevelNames;

		bool myIsConfigured;

		vector<string> InitTagWords() const;

	friend bool operator == (const CAFEVar &Lefty, const CAFEVar &Righty);
	friend bool operator != (const CAFEVar &Lefty, const CAFEVar &Righty);

	friend bool operator > (const CAFEVar &TheVar, const string &CAFEVarName);
	friend bool operator < (const CAFEVar &TheVar, const string &CAFEVarName);
	friend bool operator >= (const CAFEVar &TheVar, const string &CAFEVarName);
	friend bool operator <= (const CAFEVar &TheVar, const string &CAFEVarName);
	friend bool operator == (const CAFEVar &TheVar, const string &CAFEVarName);
	friend bool operator != (const CAFEVar &TheVar, const string &CAFEVarName);

	friend bool operator > (const string &CAFEVarName, const CAFEVar &TheVar);
	friend bool operator < (const string &CAFEVarName, const CAFEVar &TheVar);
	friend bool operator >= (const string &CAFEVarName, const CAFEVar &TheVar);
	friend bool operator <= (const string &CAFEVarName, const CAFEVar &TheVar);
	friend bool operator == (const string &CAFEVarName, const CAFEVar &TheVar);
	friend bool operator != (const string &CAFEVarName, const CAFEVar &TheVar);
};


class CAFEVarID_t
{
	public:
		CAFEVarID_t();
		CAFEVarID_t(const size_t &CAFEVarIndex);
		CAFEVarID_t(const string &CAFEVarName);

		size_t GiveIndex(const vector <CAFEVar> &TheCAFEVars) const;

	private:
		size_t myCAFEVarIndex;
		string myCAFEVarName;
};

#endif
