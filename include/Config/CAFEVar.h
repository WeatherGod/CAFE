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

		const map<string, size_t>& GiveCAFELevels() const;

		const string& GiveCAFEVarName() const;
		size_t GiveCAFELevelCount() const;

		// TODO: Turn into sets?
		// Gives the LevelNames in the order of the Names!
		vector<string> GiveCAFELevelNames() const;

		size_t GiveCAFELevelIndex(const string &CAFELevelName) const;
		// Gives the LevelIndicies in the order of the LevelName, not the indicies!
		vector<size_t> GiveCAFELevelIndicies() const;
		
		bool AddCAFELevel(const string &NewCAFELevelName, const size_t &CAFELevelIndex);
		
	private:
		string myCAFEVarName;
		map<string, size_t> myCAFELevels;;

		bool myIsConfigured;

		vector<string> InitTagWords() const;

};


#endif
