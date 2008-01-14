#ifndef _CAFE_CMDLINE_C
#define _CAFE_CMDLINE_C

using namespace std;

#include <iostream>
#include <vector>
#include <cctype>		// for size_t
#include <string>
#include <algorithm>		// for set_intersection(), set_difference(), sort(), find()

#include <StrUtly.h>		// for TakeDelimitedList(), StrToInt()

#include "Config/Configuration.h"

#include <FormatUtly.h>		// for Bold(), Underline()

// Temporary
#include "Utils/CAFEUtly.h"	// for OffsetToTimePeriod()

#include "Utils/CAFE_CmdLine.h"


CmdOptions::CmdOptions()
	:       VerboseLevel(3),
                ConfigFilename("TestTableConfig"),
                EventTypes(0),
		TimePeriods(0),
		DatabaseNames(0),
		ClustDatabaseNames(0),
		CAFEVarNames(0),
		CAFEFields(0),
		LoginUserName("bvroot"),
		CAFEUserName("CAFEUser"),
		CAFEPath("./"),
		ServerName("narr.met.psu.edu"),
		myOptionSwitchers(0),
		myExpectedValueCounts(0)
{
	SetUpSwitchers();
}

void CmdOptions::PrintSyntax(const unsigned short int &SpaceOver, const unsigned short int &ColumnWidth) const
{
	const string SyntaxInfo = GiveSyntaxInfo();
	const vector <string> TheOptions = TakeDelimitedList(SyntaxInfo, '\n');
	const string ColumnSpacing(SpaceOver, ' ');

	string ScreenBuf = "";

	for (vector<string>::const_iterator AnOption = TheOptions.begin(); AnOption != TheOptions.end(); AnOption++)
	{
		if ((ScreenBuf.length() + (*AnOption).length()) > ColumnWidth)
		{
			cout << ColumnSpacing << ScreenBuf << endl;
			ScreenBuf = " " + *AnOption;			// space used to help line up output.
		}
		else
		{
			ScreenBuf += " " + *AnOption;
		}
	}

	cout << ColumnSpacing << ScreenBuf << endl;
}

void CmdOptions::PrintDescription(const unsigned short int &ColumnWidth) const
{
	const string DescriptInfo = GiveDescription();
	const vector <string> Descriptions = TakeDelimitedList(DescriptInfo, '\n');
	
	if (!Descriptions.empty())
	{
		for (size_t Index = 0; Index < (Descriptions.size() - 1); Index += 2)
		{
			cout << '\t' << Descriptions[Index] << '\n';
			const vector <string> DescriptWords = TakeDelimitedList(Descriptions[Index + 1], ' ');
			string ScreenBuf = "";
			for (vector<string>::const_iterator AWord = DescriptWords.begin(); AWord != DescriptWords.end(); AWord++)
			{
				if ((ScreenBuf.length() + AWord->length()) > ColumnWidth)
				{
					cout << "\t\t" << ScreenBuf << endl;
					ScreenBuf = " " + *AWord;
				}
				else
				{
					ScreenBuf += " " + *AWord;
				}
			}

			cout << "\t\t" << ScreenBuf << endl;
			cout << endl;
		}
	}
}
		
string CmdOptions::GiveSyntaxInfo() const
{
	string SyntaxStr = "";

	SyntaxStr += Bold("[-c | --config ") + Underline("config_file") + Bold("]\n");
	SyntaxStr += Bold("[-v | --verbose ") + Underline("level") + Bold("]\n");
	SyntaxStr += Bold("[-e | --eventtypes ") + Underline("type_1,type_2,...") + Bold("]\n");
	SyntaxStr += Bold("[-t | --timeperiods ") + Underline("period_1,period_2,...") + Bold("]\n");
	SyntaxStr += Bold("[-x | --variables ") + Underline("name_1,name_2,...") + Bold("]\n");
	SyntaxStr += Bold("[-f | --fields ") + Underline("label_1,label_2,...") + Bold("]\n");
	SyntaxStr += Bold("[-l | --login ") + Underline("login_name") + Bold("]\n");
	SyntaxStr += Bold("[-u | --user ") + Underline("CAFE_username") + Bold("]\n");
	SyntaxStr += Bold("[-p | --cafepath ") + Underline("CAFE_Path") + Bold("]\n");
	SyntaxStr += Bold("[-h | --host ") + Underline("host_name") + Bold("]\n");

	return(SyntaxStr);
}




string CmdOptions::GiveDescription() const
{
	string Description = "";

	Description += Bold("-c, --config config_file") + "\n";
	Description += "Specify the name of the configuration file relative to the " + Underline("CAFE_Path") + "\n";

        Description += Bold("-v, --verbose level") + "\n";
	Description += "Set the verbosity level for the program standard output. Does not control the error output. ";
	Description += "Use 0 for complete silence. 3 for typical basic information. 5 for basic debugging info.\n";

        Description += Bold("-e, --eventtypes type_1,type_2,...") + "\n";
	Description += "Specify the event types to process. Default is all the event types in the configuration file. ";
	Description += "Must be a comma-delimited list with no spaces!\n";

        Description += Bold("-t, --timeperiods period_1,period_2,...") + "\n";
	Description += "Specify the time periods to process.  Default is all the time periods in the configuration file. ";
	Description += "Must be a comma-delimited list with no spaces!\n";

        Description += Bold("-x, --variables name_1,name_2,...") + "\n";
	Description += "Specify the CAFEVariables that are allowed to be processed. Default is all the CAFEVars in the configuration file. ";
	Description += "Must be a comma-delimited list with no spaces!\n";

        Description += Bold("-f, --fields label_1,label_2,...") + "\n";
	Description += "Specify the fields that are allowed to be processed. This is more specific than the " + Bold("-x") + " option. ";
	Description += "Levels can be specified along with the variable names. Using this option overrides any generality ";
	Description += "of the " + Bold("-x") + " option. Must be a comma-delimited list with no spaces!\n";

        Description += Bold("-l, --login login_name") + "\n";
	Description += "Specify the MySQL login name for writing to the databases. ";
	Description += "This login name will need a password to access the databases.\n";

        Description += Bold("-u, --user CAFE_username") + "\n";
	Description += "Specify the MySQL login name for reading from the databases. ";
	Description += "This login name must not require a password to access the databases.\n";

        Description += Bold("-p, --cafepath CAFE_path") + "\n";
	Description += "Specify the path location of the CAFE installation. The default location is ./ for the programs.\n";

        Description += Bold("-h, --host host_name") + "\n";
	Description += "Specify the host name for MySQL login.";

	return(Description);
}

string CmdOptions::OptionMerge(vector <string> &GivenValues, const vector <string> &DefaultValues, const string &ItemDescription)
{
	string ErrorMesg = "";

	if (GivenValues.empty())
        {
                GivenValues = DefaultValues;
        }
        else
        {
                sort(GivenValues.begin(), GivenValues.end());
		GivenValues.end() = unique(GivenValues.begin(), GivenValues.end());        // eliminates any possible repeated stuff

                // now need to verify that these values are even valid values.
                if (!includes(DefaultValues.begin(), DefaultValues.end(), GivenValues.begin(), GivenValues.end()))
                {
                        // !! there are elements in GivenValues that are not in DefaultValues!!
                        vector <string> UnknownElements(GivenValues.size(), "");
                        vector<string>::iterator EndPos = set_difference(GivenValues.begin(), GivenValues.end(),
                                                                         DefaultValues.begin(), DefaultValues.end(),
                                                                         UnknownElements.begin());

                        ErrorMesg = "Unknown " + ItemDescription + " given in the command-line arguements:";
                        for (vector<string>::iterator Item = UnknownElements.begin(); Item != EndPos; Item++)
                        {
                                ErrorMesg += "\n" + *Item;
                        }
                        ErrorMesg += "\n";
                }
        }

	return(ErrorMesg);
}

bool CmdOptions::MergeWithConfiguration(const Configuration &ConfigInfo)
{
//	cout << "Merging..." << endl;
	string ErrorMesg = "";

	ErrorMesg += OptionMerge(EventTypes, ConfigInfo.GiveEventTypeNames(), (string) "event type(s)");
	ErrorMesg += OptionMerge(TimePeriods, ConfigInfo.GiveTimePeriods(), (string) "time period(s)");
	ErrorMesg += OptionMerge(CAFEVarNames, ConfigInfo.GiveCAFEVarNames(), (string) "CAFE Variable(s)");

//	cout << '^' << GiveDelimitedList(CAFEVarNames, ' ') << '^' << endl;

	DatabaseNames.resize(TimePeriods.size(), "");
	ClustDatabaseNames.resize(TimePeriods.size(), "");

	for (vector<string>::iterator ATimePeriod( TimePeriods.begin() ),
	                              ADatabaseName( DatabaseNames.begin() ), 
				      AClustName( ClustDatabaseNames.begin() );
	     ATimePeriod != TimePeriods.end(); 
	     ATimePeriod++, ADatabaseName++, AClustName++)
	{
		*ADatabaseName = ConfigInfo.GiveDatabaseName(*ATimePeriod);
		*AClustName = ConfigInfo.GiveClusteredDatabaseName(*ATimePeriod);
	}

	vector <string> AllCAFELabels(0);
	for (vector<string>::const_iterator AVarName = CAFEVarNames.begin(); 
	     AVarName != CAFEVarNames.end(); 
	     AVarName++)
	{
		const size_t OldSize = AllCAFELabels.size();
		const vector <string> SomeCAFELabels = ConfigInfo.Give_CAFEVar_CAFEVarLabels(*AVarName);
		AllCAFELabels.insert(AllCAFELabels.end(), SomeCAFELabels.begin(), SomeCAFELabels.end());

//		cout << '$' << GiveDelimitedList(ConfigInfo.Give_CAFEVar_CAFEVarLabels(*AVarName), ' ') << '$' << endl;
		inplace_merge(AllCAFELabels.begin(), AllCAFELabels.begin() + OldSize, AllCAFELabels.end());
	}

//	cout << '#' << GiveDelimitedList(AllCAFELabels, ' ') << '#' << endl;

	ErrorMesg += OptionMerge(CAFEFields, AllCAFELabels, "CAFE Field Label(s)");

	if (!ErrorMesg.empty())
	{
		cerr << "ERROR: " << ErrorMesg << endl;
		return(false);
	}

	return(true);
}

CAFEParam
CmdOptions::ConfigMerge(const CAFEParam& configInfo)
/* This is to be called AFTER MergeWithConfiguration()
   It assumes that this CmdOptions object has been merged
   with the configuration file.
*/
{
	CAFEParam mergedInfo;

	mergedInfo.SetVerboseLevel(VerboseLevel)
		  .SetConfigFilename(ConfigFilename)
		  .SetCAFEPath(CAFEPath)
		  .SetLoginUserName(LoginUserName)
		  .SetCAFEUserName(CAFEUserName)
		  .SetServerName(ServerName)
		  .SetUntrainedNameStem(configInfo.GetUntrainedNameStem())
		  .SetTrainedNameStem(configInfo.GetTrainedNameStem())
		  .SetCAFEDomain(configInfo.GetCAFEDomain())
		  .SetExtremumNames(configInfo.GetExtremumNames())
		  .SetDataSources(configInfo.GetDataSources())
		  .SetDefaultDataSource(configInfo.GetDefaultDataSource());

	for (vector<string>::const_iterator anOffset = TimePeriods.begin();
	     anOffset != TimePeriods.end();
	     anOffset++)
	{
		mergedInfo.AddTimeOffset(TimePeriodToOffset(*anOffset));
	}


	for (vector<string>::const_iterator aVar = CAFEVarNames.begin();
	     aVar != CAFEVarNames.end();
	     aVar++)
	{
		const map<string, CAFEVar>::const_iterator varFind = configInfo.GetCAFEVars().find(*aVar);

		if (varFind == configInfo.GetCAFEVars().end())
		{
			// TODO: throw exception
			cerr << "ERROR: Should throw exception here... CAFEVar name: " << *aVar << endl;
			continue;
		}

		const map<string, size_t>  varCAFELevels = varFind->second.GiveCAFELevels();
		map<string, size_t> mergedLevels;

		for (map<string, size_t>::const_iterator aCAFELevel = varCAFELevels.begin();
		     aCAFELevel != varCAFELevels.end();
		     aCAFELevel++)
		{
			if (binary_search(CAFEFields.begin(), CAFEFields.end(),
					  (aCAFELevel->first.empty() ? varFind->first
								     : varFind->first + "_" + aCAFELevel->first)))
			{
				mergedLevels.insert( mergedLevels.end(), *aCAFELevel );
			}
		}

		if (!mergedLevels.empty())
		{
			// So, only add a CAFEVar if there are fields found.
			mergedInfo.AddCAFEVar(*aVar, CAFEVar(*aVar, mergedLevels));
		}
	}



	for (vector<string>::const_iterator anEvent = EventTypes.begin();
	     anEvent != EventTypes.end();
	     anEvent++)
	{
		const map<string, EventType>::const_iterator eventFind = configInfo.GetEventTypes().find(*anEvent);

		if (eventFind == configInfo.GetEventTypes().end())
		{
			// TODO: throw exception
			cerr << "ERROR: Should throw exception here... event type name: " << *anEvent << endl;
			continue;
		}

		map<string, Variable> mergedVars;

		for (map<string, Variable>::const_iterator anEventVar = eventFind->second.GiveEventVariables().begin();
		     anEventVar != eventFind->second.GiveEventVariables().end();
		     anEventVar++)
		{
			set<string> CAFELevels;
			
			for (vector<string>::const_iterator aLevel = anEventVar->second.GiveCAFELevels().begin();
			     aLevel != anEventVar->second.GiveCAFELevels().end();
			     aLevel++)
			{
				if (binary_search(CAFEFields.begin(), CAFEFields.end(),
						  (aLevel->empty() ? anEventVar->first 
								   : anEventVar->first + "_" + *aLevel)))
				{
					CAFELevels.insert( CAFELevels.end(), *aLevel );
				}
			}

			if (!CAFELevels.empty())
			{
				/* So, only add a variable to the event type if there
				   were any fields found.
				*/
				mergedVars.insert(mergedVars.end(), make_pair(anEventVar->first, 
									      Variable(anEventVar->first, CAFELevels)));
			}
		}

		if (!mergedVars.empty())
		{
			mergedInfo.AddEventType(*anEvent, EventType(*anEvent, mergedVars));
		}
	}

	return(mergedInfo);
}

string CmdOptions::GiveDatabaseName(const string &TheTimePeriod) const
{
	if (binary_search(TimePeriods.begin(), TimePeriods.end(), TheTimePeriod))
	{
		const size_t TimePeriodIndex = lower_bound(TimePeriods.begin(), TimePeriods.end(), TheTimePeriod) - TimePeriods.begin();
		return(DatabaseNames[TimePeriodIndex]);
	}
	
	return("");
}

string CmdOptions::GiveClusteredDatabaseName(const string &TheTimePeriod) const
{
        if (binary_search(TimePeriods.begin(), TimePeriods.end(), TheTimePeriod))
        {
                const size_t TimePeriodIndex = lower_bound(TimePeriods.begin(), TimePeriods.end(), TheTimePeriod) - TimePeriods.begin();
                return(ClustDatabaseNames[TimePeriodIndex]);
        }

        return("");
}


// Deprecated
string CmdOptions::GiveTimePeriod(const int &HourOffset) const
// Will only work properly if done AFTER the MergeWithConfiguration()
// Might work before-hand, but not really guaranteed.
{
	string TheTimePeriod = OffsetToTimePeriod(HourOffset);

	if (!binary_search(TimePeriods.begin(), TimePeriods.end(), TheTimePeriod))
	{
		return("");
	}
	else
	{
		return(TheTimePeriod);
	}
}


void CmdOptions::SetUpSwitchers()
{
        myOptionSwitchers.push_back("--config");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("-c");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("--verbose");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("-v");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("--eventtypes");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("-e");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("--timeperiods");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("-t");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("--variables");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("-x");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("--fields");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("-f");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("--login");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("-l");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("--user");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("-u");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("--cafepath");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("-p");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("--host");
        myExpectedValueCounts.push_back(1);
        myOptionSwitchers.push_back("-h");
        myExpectedValueCounts.push_back(1);
}

bool CmdOptions::IsAnOption(const string &SomeStr) const
{
	return((SomeStr.find("--") == 0 && SomeStr.length() > 3) || (SomeStr.find('-') == 0 && SomeStr.length() == 2));
}

void CmdOptions::DoDecrement(vector <string> &CmdArgs, vector<string>::iterator &ACmdArg, size_t &ValuesToDo) const
{
	ValuesToDo--;
	ACmdArg = CmdArgs.erase(ACmdArg);
}

vector <string> CmdOptions::GiveLabelsToDo(const Configuration &ConfigInfo, const string &EventTypeName, const string &CAFEVarName) const
// it is assumed that CAFELabelNames is in lexiconal order!!
{
	const vector <string> CAFELabelNames = ConfigInfo.Give_EventType_CAFEVarLabels(EventTypeName, CAFEVarName);
//	cout << '+' << GiveDelimitedList(CAFELabelNames, ' ') << '+' << endl;
//	cout << '-' << GiveDelimitedList(CAFEFields, ' ') << '-' << endl;
	vector <string> TheLabelsToDo(CAFELabelNames.size(), "");
	vector<string>::iterator LabelEnd = set_intersection(CAFELabelNames.begin(), CAFELabelNames.end(), 
							     CAFEFields.begin(), CAFEFields.end(),
							     TheLabelsToDo.begin());

	TheLabelsToDo.resize(LabelEnd - TheLabelsToDo.begin());
//	cout << '-' << GiveDelimitedList(TheLabelsToDo, ' ') << '-' << endl;
	return(TheLabelsToDo);
}

vector <string> CmdOptions::GiveCAFEVarsToDo(const Configuration &ConfigInfo, const string &EventTypeName) const
{
	const vector <string> SomeCAFEVarNames = ConfigInfo.Give_EventType_CAFEVarNames(EventTypeName);
	vector <string> CAFEVarsToDo(SomeCAFEVarNames.size(), "");
	vector<string>::iterator VarEnd = set_intersection(SomeCAFEVarNames.begin(), SomeCAFEVarNames.end(),
							   CAFEVarNames.begin(), CAFEVarNames.end(),
							   CAFEVarsToDo.begin());

	CAFEVarsToDo.resize(VarEnd - CAFEVarsToDo.begin());
	return(CAFEVarsToDo);
}


int CmdOptions::ParseCommandArgs(vector <string> &CmdArgs)
{
	size_t LastOptionIndex = string::npos;
	// string::npos indicates that the last option met did not match any option that was expected
	// or the last option has already been dealt with.

	size_t ValuesToDo = string::npos;

       	for (vector<string>::iterator ACmdArg = CmdArgs.begin(); ACmdArg != CmdArgs.end(); )
       	{
		if (RipWhiteSpace(*ACmdArg) == "")
                {
                        cerr << "Empty value encountered in the command line arguements..." << endl;
                        return(2);
                }

		if (LastOptionIndex != string::npos)
		{
			// if the above is true, then this iteration of the loop is expecting a value for a particular option
			// so, if an option is found instead of a value, then there was a mistake in the command line.
			if (IsAnOption(*ACmdArg))
			{
				cerr << "Unexpected option: " << *ACmdArg << endl;
				cerr << "Was expecting " << myExpectedValueCounts.at(LastOptionIndex);
				cerr << " value(s) for the option: " << myOptionSwitchers.at(LastOptionIndex) << endl;
				return(1);
			}
		}
		else if (IsAnOption(*ACmdArg))
		{
			// if LastOptionIndex == string::npos, then the iteration is ready to look for its next switch
			// so, if it this iteration is a switch, then set the LastOptionIndex.
			vector<string>::const_iterator ThePos = find(myOptionSwitchers.begin(), myOptionSwitchers.end(), *ACmdArg);
			if (ThePos != myOptionSwitchers.end())
			{
				LastOptionIndex = ThePos - myOptionSwitchers.begin();
				// for switches with multiple values, this can be used as a countdown.
				// in the case statement, an if statement would determin which value goes
				// to which internal variables.  Only when ValuesToDo reaches 0 should
				// LastOptionIndex be set to string::npos, indicating that we are done with
				// the switch.
				ValuesToDo = myExpectedValueCounts[LastOptionIndex];
			}
			else
			{
				LastOptionIndex = string::npos;
			}
		}
			
		// At this point, LastOptionIndex == string::npos only if there is an unrecognized switch or
		//   there is a value that is not associated with any recognized switch.
		// LastOptionIndex != string::npos only if there is a value associated with a recognized switch or
		//   there is a recognized switch.
		switch(LastOptionIndex)
                {
                       	case 0:
                       	case 1:
				if (ValuesToDo == 1)
				{
					DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
				}
				else
				{
					ConfigFilename = *ACmdArg;
					DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
					LastOptionIndex = string::npos;
				}
                                break;

			case 2:
                        case 3:
				if (ValuesToDo == 1)
                                {
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                }
                                else
                                {
                                        VerboseLevel = StrToInt(*ACmdArg);
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                        LastOptionIndex = string::npos;
                                }

                               	break;
			case 4:
			case 5:
				if (ValuesToDo == 1)
                                {
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                }
                                else if (ValuesToDo == 0)
                                {
                                        EventTypes = TakeDelimitedList(*ACmdArg, ',');
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                        LastOptionIndex = string::npos;
                                }

                                break;
			case 6:
			case 7:
				if (ValuesToDo == 1)
                                {
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                }
                                else if (ValuesToDo == 0)
                                {
                                        TimePeriods = TakeDelimitedList(*ACmdArg, ',');
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                        LastOptionIndex = string::npos;
                        	}

                                break;
			case 8:
			case 9:
				if (ValuesToDo == 1)
                                {
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                }
                                else if (ValuesToDo == 0)
                                {
                                        CAFEVarNames = TakeDelimitedList(*ACmdArg, ',');
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                        LastOptionIndex = string::npos;
                                }
				break;
			case 10:
			case 11:
                                if (ValuesToDo == 1)
                                {
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                }
                                else if (ValuesToDo == 0)
                                {
                                        CAFEFields = TakeDelimitedList(*ACmdArg, ',');
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                        LastOptionIndex = string::npos;
                                }

                                break;
			case 12:
			case 13:
				if (ValuesToDo == 1)
                                {
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                }
                                else if (ValuesToDo == 0)
                                {
                                        LoginUserName = *ACmdArg;
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                        LastOptionIndex = string::npos;
                                }

				break;
			case 14:
			case 15:
				if (ValuesToDo == 1)
                                {
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                }
                                else if (ValuesToDo == 0)
                                {
                                        CAFEUserName = *ACmdArg;
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                        LastOptionIndex = string::npos;
                                }

                                break;
			case 16:
			case 17:
				if (ValuesToDo == 1)
                                {
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                }
                                else if (ValuesToDo == 0)
                                {
                                        CAFEPath = *ACmdArg;
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                        LastOptionIndex = string::npos;
                                }

				break;
			case 18:
			case 19:
				if (ValuesToDo == 1)
                                {
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                }
                                else if (ValuesToDo == 0)
                                {
                                        ServerName = *ACmdArg;
                                        DoDecrement(CmdArgs, ACmdArg, ValuesToDo);
                                        LastOptionIndex = string::npos;
                                }

				break;

			case (string::npos):
				// we have either a value associated with an unrecognized option,
				// or an unrecognized option.  Either case, just move onto the next
				// iteration.
				ACmdArg++;
				break;

       	                default:
               			cerr << "FATAL ERROR IN CODE!  LastOptionIndex: " << LastOptionIndex << endl;
				cerr << "Need to add more cases to the switch block!" << endl;
				return(3);
				break;	// not really needed, but, oh well.
		}// end switch statement
	}// end for loop through the CmdArgs

	if (LastOptionIndex != string::npos)
	{
		cerr << "Missing value for option: " << myOptionSwitchers.at(LastOptionIndex) << endl;
		return(2);
	}

	return(0);
}// end ParseCommandArgs()

#endif
