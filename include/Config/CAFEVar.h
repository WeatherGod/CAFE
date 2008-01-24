#ifndef _CAFEVAR_H
#define _CAFEVAR_H

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cctype>		// for size_t

#include "Utils/CAFEException.h"	// for CAFELevel_Not_Found

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
                bool ValidConfig() const throw();
                bool IsValid() const throw();

		const map<string, size_t>& GiveCAFELevels() const throw();

		const string& GiveCAFEVarName() const throw();
		size_t GiveCAFELevelCount() const throw();

		// TODO: Turn into sets?
		// Gives the LevelNames in the order of the Names!
		vector<string> GiveCAFELevelNames() const;

		const size_t& GiveCAFELevelIndex(const string &CAFELevelName) const throw(CAFELevel_Not_Found);
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
