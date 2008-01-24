#ifndef _EVENTTYPE_H
#define _EVENTTYPE_H

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cctype>		// for size_t

#include "Utils/CAFEException.h"	// for EventVar_Not_Found

#include "Variable.h"

class EventType
{
	public:
		EventType();
		EventType(const EventType &eventCopy);
		EventType(const string &eventTypeName,
			  const map<string, Variable> &eventVars);

		void GetConfigInfo(string &FileLine, fstream &ReadData);

                // right now, they do the same thing, but there should be a difference.
                // ValidConfig() will return whether it was able to load a configuration correctly.
                // IsValid() will return whether the data contained is valid information.
                bool ValidConfig() const throw();
                bool IsValid() const throw();

		const map<string, Variable>& GiveEventVariables() const throw();
		const string& GiveEventTypeName() const throw();

		vector<string> GiveEventVariableNames() const;

		size_t GiveLevelCount(const string &EventVarName) const throw(EventVar_Not_Found);
		const vector<string>& GiveEventLevels(const string &EventVarName) const throw(EventVar_Not_Found);

		size_t GiveVariableCount() const throw();

		bool AddVariable(const Variable &NewVariable);
		Variable RemoveVariable(const string &EventVarName) throw(EventVar_Not_Found);
		
	private:
		string myEventTypeName;
		map<string, Variable> myVariables;

		bool myIsConfigured;

		vector<string> InitTagWords() const;

};


#endif
