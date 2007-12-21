#ifndef _EVENTTYPE_H
#define _EVENTTYPE_H

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cctype>		// for size_t

#include "Variable.h"

class EventType
{
	public:
		EventType();
//		EventType(const string EventTypeName);		// does NOT make IsValid() true.

		void GetConfigInfo(string &FileLine, fstream &ReadData);

                // right now, they do the same thing, but there should be a difference.
                // ValidConfig() will return whether it was able to load a configuration correctly.
                // IsValid() will return whether the data contained is valid information.
                bool ValidConfig() const;
                bool IsValid() const;

		string GiveEventTypeName() const;
//		const Variable* GiveVariable(const string CAFEVarName) const;

		vector <string> GiveCAFEVariableNames() const;

		size_t GiveLevelCount(const string &CAFEVarName) const;
		vector <string> GiveCAFELevels(const string &CAFEVarName) const;

		size_t GiveVariableCount() const;

		bool AddVariable(const Variable &NewVariable);
		Variable RemoveVariable(const string &CAFEVarName);
		
	private:
		string myEventTypeName;
		map <string, Variable> myVariables;
		Variable myDummyVariable;

		bool myIsConfigured;

		vector <string> myTagWords;
		void InitTagWords();

	friend bool operator == (const EventType &Lefty, const EventType &Righty);
	friend bool operator != (const EventType &Lefty, const EventType &Righty);

	friend bool operator < (const EventType &TheEvent, const string &EventTypeName);
	friend bool operator > (const EventType &TheEvent, const string &EventTypeName);
	friend bool operator <= (const EventType &TheEvent, const string &EventTypeName);
	friend bool operator >= (const EventType &TheEvent, const string &EventTypeName);
	friend bool operator == (const EventType &TheEvent, const string &EventTypeName);
	friend bool operator != (const EventType &TheEvent, const string &EventTypeName);

	friend bool operator < (const string &EventTypeName, const EventType &TheEvent);
	friend bool operator > (const string &EventTypeName, const EventType &TheEvent);
	friend bool operator <= (const string &EventTypeName, const EventType &TheEvent);
	friend bool operator >= (const string &EventTypeName, const EventType &TheEvent);
	friend bool operator == (const string &EventTypeName, const EventType &TheEvent);
	friend bool operator != (const string &EventTypeName, const EventType &TheEvent);
};

class EventTypeID_t
{
	public:
		EventTypeID_t();
		EventTypeID_t(const size_t &EventTypeIndex);
		EventTypeID_t(const string &EventTypeName);

		size_t GiveIndex(const vector <EventType> &EventTypes) const;

	private:
		size_t myEventTypeIndex;
		string myEventTypeName;
};


#endif
