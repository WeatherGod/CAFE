#ifndef _EVENTTYPE_C
#define _EVENTTYPE_C

using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>			// for size_t
#include <map>
#include <string>

#include "Utils/CAFEException.h"	// for EventVar_Not_Found

#include "Config/EventType.h"
#include "Config/Variable.h"

#include <StrUtly.h>			// for RipWhiteSpace()
#include <ConfigUtly.h>			// for ReadNoComments(), StripTags(), FoundStartTag(), FoundEndTag()



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

				if (TempVar.GiveLevelCount() == 0)
				{
					// Supply a default CAFELevel if none
					// is given.
					TempVar.AddCAFELevel("");
				}

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

bool EventType::ValidConfig() const throw()
{
	return(myIsConfigured);
}

bool EventType::IsValid() const throw()
{
	// need to get around to modifying this to do a real check
	return(myIsConfigured);
}

const string&
EventType::GiveEventTypeName() const throw()
{
	return(myEventTypeName);
}

const map<string, Variable>&
EventType::GiveEventVariables() const throw()
{
	return(myVariables);
}

size_t
EventType::GiveVariableCount() const throw()
{
	return(myVariables.size());
}


vector<string>
EventType::GiveEventVariableNames() const
// The elements in the eventVarNames vector are in alphabetical order.
{
	vector<string> eventVarNames(myVariables.size(), "");

	vector<string>::iterator eventVarIter = eventVarNames.begin();
	for (map<string, Variable>::const_iterator varItem = myVariables.begin(); 
	     varItem != myVariables.end(); 
	     varItem++, eventVarIter++)
	{
		*eventVarIter = varItem->first;
	}

	return(eventVarNames);
}

size_t
EventType::GiveLevelCount(const string &eventVarName) const throw(EventVar_Not_Found)
{
	const map<string, Variable>::const_iterator varFind = myVariables.find(eventVarName);
	if (varFind == myVariables.end())
	{
		throw EventVar_Not_Found("EventType::GiveLevelCount()",
					 eventVarName, myEventTypeName);
	}
	else
	{
		return(varFind->second.GiveLevelCount());
	}
}

const vector<string>&
EventType::GiveEventLevels(const string &eventVarName) const throw(EventVar_Not_Found)
// The elements in the CAFELevels vector are in alphabetical order.
{
	const map<string, Variable>::const_iterator varFind = myVariables.find(eventVarName);
        if (varFind == myVariables.end())
        {
                throw EventVar_Not_Found("EventType::GiveEventLevels()",
					 eventVarName, myEventTypeName);
        }
	else
	{
		return(varFind->second.GiveCAFELevels());
	}
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

			const vector<string> tempCAFELevels = NewVariable.GiveCAFELevels();
			for (vector<string>::const_iterator A_CAFELevel = tempCAFELevels.begin(); 
			     A_CAFELevel != tempCAFELevels.end(); 
			     A_CAFELevel++)
			{
				// it only adds the level if it doesn't exist already
				VariableMatch->second.AddCAFELevel(*A_CAFELevel);
			}
		}

		return(true);
	} // end if NewVariable.IsValid()

	return(false);
}

Variable EventType::RemoveVariable(const string &eventVarName) throw(EventVar_Not_Found)
{
	map<string, Variable>::iterator VariableToRemove = myVariables.find(eventVarName);

	if (VariableToRemove == myVariables.end())
	{
		throw EventVar_Not_Found("EventType::RemoveVariable()",
					 eventVarName, myEventTypeName);
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


#endif
