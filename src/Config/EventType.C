#ifndef _EVENTTYPE_C
#define _EVENTTYPE_C

using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>			// for size_t
#include <map>
#include <string>

#include "Config/EventType.h"
#include "Config/Variable.h"

#include <StrUtly.h>			// for RipWhiteSpace()
#include <ConfigUtly.h>			// for ReadNoComments(), StripTags(), FoundStartTag(), FoundEndTag()

EventTypeID_t::EventTypeID_t()
	:	myEventTypeIndex(string::npos),
		myEventTypeName("")
{
}

EventTypeID_t::EventTypeID_t(const size_t &EventTypeIndex)
        :       myEventTypeIndex(EventTypeIndex),
                myEventTypeName("")
{
}

EventTypeID_t::EventTypeID_t(const string &EventTypeName)
        :       myEventTypeIndex(string::npos),
                myEventTypeName(EventTypeName)
{
}

size_t EventTypeID_t::GiveIndex(const vector <EventType> &EventTypes) const
{
	if (myEventTypeIndex != string::npos)
	{
		return(myEventTypeIndex);
	}

	if (!myEventTypeName.empty())
	{
		if (binary_search(EventTypes.begin(), EventTypes.end(), myEventTypeName))
		{
			return(lower_bound(EventTypes.begin(), EventTypes.end(), myEventTypeName) - EventTypes.begin());
		}
	}

	return(string::npos);
}

string EventTypeID_t::GiveName() const
{
	return(myEventTypeName);
}


EventType::EventType()
	:	myEventTypeName(""),
		myVariables(),
		myIsConfigured(false)
{
}

EventType::EventType(const EventType& eventCopy)
	:	myEventTypeName(eventCopy.myEventTypeName),
		myVariables(eventCopy.myVariables),
		myIsConfigured(eventCopy.myIsConfigured)
{
}

EventType::EventType(const string &eventTypeName, const map<string, Variable> &eventVars)
	:	myEventTypeName(eventTypeName),
		myVariables(eventVars),
		myIsConfigured(true)
{
}

void EventType::GetConfigInfo(string &FileLine, fstream &ReadData)
{
	const vector<string> TagWords = InitTagWords();

	bool BadObject = false;

	while (!FoundEndTag(FileLine, TagWords[0]) && !ReadData.eof())
	{
		if (!BadObject)
		{
			if (FoundStartTag(FileLine, TagWords[1]))		//TypeName
			{
				myEventTypeName = RipWhiteSpace(StripTags(FileLine, TagWords[1]));
			}
			else if (FoundStartTag(FileLine, TagWords[2]))	// Variable
			{
				FileLine = ReadNoComments(ReadData);
				Variable TempVar;
				TempVar.GetConfigInfo(FileLine, ReadData);
				if (!AddVariable(TempVar))
				{
					BadObject = true;
					cerr << "Problem in Variable object in EventType object... Here is the line: " << FileLine << endl;
				}
			}
			else
			{
				BadObject = true;
				cerr << "Problem in EventType object... Here is the line: " << FileLine << endl;
			}
		}// end if !BadObject

		FileLine = ReadNoComments(ReadData);
	}// end while loop

	if (!ReadData.eof() && !myEventTypeName.empty() && !myVariables.empty() && !BadObject)
	{
		myIsConfigured = true;
	}
}

bool EventType::ValidConfig() const
{
	return(myIsConfigured);
}

bool EventType::IsValid() const
{
	// need to get around to modifying this to do a real check
	return(myIsConfigured);
}

string EventType::GiveEventTypeName() const
{
	return(myEventTypeName);
}


size_t EventType::GiveVariableCount() const
{
	return(myVariables.size());
}


vector <string> EventType::GiveCAFEVariableNames() const
// The elements in the CAFEVarNames vector are in alphabetical order.
{
	vector <string> CAFEVarNames(myVariables.size(), "");

	vector<string>::iterator A_CAFEVarName = CAFEVarNames.begin();
	for (map<string, Variable>::const_iterator VarItem = myVariables.begin(); VarItem != myVariables.end(); VarItem++, A_CAFEVarName++)
	{
		(*A_CAFEVarName) = VarItem->first;
	}

	return(CAFEVarNames);
}

size_t EventType::GiveLevelCount(const string &CAFEVarName) const
{
	const map<string, Variable>::const_iterator AVariable = myVariables.find(CAFEVarName);
	if (AVariable == myVariables.end())
	{
		return(string::npos);
	}

	return(AVariable->second.GiveLevelCount());
}

vector <string> EventType::GiveCAFELevels(const string &CAFEVarName) const
// The elements in the CAFELevels vector are in alphabetical order.
{
	const map<string, Variable>::const_iterator AVariable = myVariables.find(CAFEVarName);
        if (AVariable == myVariables.end())
        {
                return(vector<string>(0));
        }

	return(AVariable->second.GiveCAFELevels());
}

bool EventType::AddVariable(const Variable &NewVariable)
// only returns true if the NewVariable is valid.  However, if there is duplication,
// it will try and updating the Variable that NewVariable duplicates by adding levels.
// In such a case, true will still be returned.
{
	if (NewVariable.IsValid())
	{
		const map<string, Variable>::iterator VariableMatch = myVariables.find(NewVariable.GiveCAFEVariableName());
		if (VariableMatch == myVariables.end())
		{
			// the key doesn't exist yet, so I can add in a Variable under that key value
			myVariables[NewVariable.GiveCAFEVariableName()] = NewVariable;
		}
		else
		{
			// This variable already exists, but lets make sure that any information that is in this one
			// is added to any information that the stored variable has...

			const vector <string> TempCAFELevels = NewVariable.GiveCAFELevels();
			for (vector<string>::const_iterator A_CAFELevel = TempCAFELevels.begin(); A_CAFELevel != TempCAFELevels.end(); A_CAFELevel++)
			{
				// it only adds the level if it doesn't exist already
				VariableMatch->second.AddCAFELevel(*A_CAFELevel);
			}
		}

		return(true);
	} // end if NewVariable.IsValid()

	return(false);
}

Variable EventType::RemoveVariable(const string &CAFEVarName)
{
	map <string, Variable>::iterator VariableToRemove = myVariables.find(CAFEVarName);

	if (VariableToRemove == myVariables.end())
	{
		return(Variable());
	}

	Variable VariableCopy = VariableToRemove->second;
	myVariables.erase(VariableToRemove);
	return(VariableCopy);
}


vector<string> EventType::InitTagWords() const
{
	vector<string> TagWords(3);

	TagWords[0] = "EventType";
	TagWords[1] = "TypeName";
	TagWords[2] = "Variable";

	return(TagWords);
}

bool operator == (const EventType &Lefty, const EventType &Righty)
{
	return(Lefty.myEventTypeName == Righty.myEventTypeName);
}

bool operator != (const EventType &Lefty, const EventType &Righty)
{
	return(Lefty.myEventTypeName != Righty.myEventTypeName);
}



bool operator > (const EventType &Lefty, const string &EventTypeName)
{
	return(Lefty.myEventTypeName > EventTypeName);
}

bool operator < (const EventType &Lefty, const string &EventTypeName)
{
        return(Lefty.myEventTypeName < EventTypeName);
}

bool operator >= (const EventType &Lefty, const string &EventTypeName)
{
        return(Lefty.myEventTypeName >= EventTypeName);
}

bool operator <= (const EventType &Lefty, const string &EventTypeName)
{
        return(Lefty.myEventTypeName <= EventTypeName);
}

bool operator == (const EventType &Lefty, const string &EventTypeName)
{
        return(Lefty.myEventTypeName == EventTypeName);
}

bool operator != (const EventType &Lefty, const string &EventTypeName)
{
        return(Lefty.myEventTypeName != EventTypeName);
}


bool operator > (const string &EventTypeName, const EventType &TheEvent)
{
        return(EventTypeName > TheEvent.myEventTypeName);
}

bool operator < (const string &EventTypeName, const EventType &TheEvent)
{
        return(EventTypeName < TheEvent.myEventTypeName);
}

bool operator >= (const string &EventTypeName, const EventType &TheEvent)
{
        return(EventTypeName >= TheEvent.myEventTypeName);
}

bool operator <= (const string &EventTypeName, const EventType &TheEvent)
{
        return(EventTypeName <= TheEvent.myEventTypeName);
}

bool operator == (const string &EventTypeName, const EventType &TheEvent)
{
        return(EventTypeName == TheEvent.myEventTypeName);
}

bool operator != (const string &EventTypeName, const EventType &TheEvent)
{
        return(EventTypeName != TheEvent.myEventTypeName);
}

#endif
