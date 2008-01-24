#ifndef _VARIABLE_H
#define _VARIABLE_H

#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <cctype>		// for size_t

class Variable
{
	public:
		Variable();
		Variable(const Variable &varCopy);
		Variable(const string &CAFEName,
			 const vector<string> &CAFELevels);
		Variable(const string &CAFEName,
			 const set<string> &CAFELevels);

		void GetConfigInfo(string &FileLine, fstream &ReadData);

                // right now, they do the same thing, but there should be a difference.
                // ValidConfig() will return whether it was able to load a configuration correctly.
                // IsValid() will return whether the data contained is valid information.
                bool ValidConfig() const throw();
                bool IsValid() const throw();

		const string& GiveCAFEVariableName() const throw();
		const vector<string>& GiveCAFELevels() const throw();

		size_t GiveLevelCount() const throw();

		bool AddCAFELevel(const string &CAFELevelName);

	private:
		string myCAFEVariableName;
		vector <string> myCAFELevels;

		bool myIsConfigured;

		vector<string> InitTagWords() const;
};

#endif
