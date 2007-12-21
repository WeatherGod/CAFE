#ifndef _VARIABLE_H
#define _VARIABLE_H

#include <fstream>
#include <vector>
#include <string>
#include <cctype>		// for size_t

class Variable
{
	public:
		Variable();

		void GetConfigInfo(string &FileLine, fstream &ReadData);

                // right now, they do the same thing, but there should be a difference.
                // ValidConfig() will return whether it was able to load a configuration correctly.
                // IsValid() will return whether the data contained is valid information.
                bool ValidConfig() const;                                       //
                bool IsValid() const;

		string GiveCAFEVariableName() const;

		vector <string> GiveCAFELevels() const;
		string GiveCAFELevel(const size_t &LevelIndex) const;
		size_t GiveLevelCount() const;

		bool AddCAFELevel(const string &CAFELevelName);

	private:
		string myCAFEVariableName;
		vector <string> myCAFELevels;

		bool myIsConfigured;

		vector <string> myTagWords;
		void InitTagWords();
/*
	friend bool operator > (const Variable &TheVar, const string VariableName);
	friend bool operator < (const Variable &TheVar, const string VariableName);
	friend bool operator >= (const Variable &TheVar, const string VariableName);
	friend bool operator <= (const Variable &TheVar, const string VariableName);
	friend bool operator == (const Variable &TheVar, const string VariableName);
	friend bool operator != (const Variable &TheVar, const string VariableName);

	friend bool operator > (const string VariableName, const Variable &TheVar);
        friend bool operator < (const string VariableName, const Variable &TheVar);
        friend bool operator >= (const string VariableName, const Variable &TheVar);
        friend bool operator <= (const string VariableName, const Variable &TheVar);
        friend bool operator == (const string VariableName, const Variable &TheVar);
        friend bool operator != (const string VariableName, const Variable &TheVar);
*/
};

#endif
