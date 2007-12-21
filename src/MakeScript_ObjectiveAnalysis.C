using namespace std;

#include <iostream>
#include <string>
#include <vector>
#include <cctype>			// for size_t
#include <fstream>

#include "Config/Configuration.h"
#include <StrUtly.h>			// for StrToUpper()
#include <CmdLineUtly.h>                // for ProcessFlatCommandLine()
#include "Utils/CAFE_CmdLine.h"               // for generic CAFE command line handling...
#include <FormatUtly.h>			// for making things look nice and pretty

#define _PROG_NAME		((string) "MakeScript_ObjectiveAnalysis")


void PrintSyntax(const CmdOptions &CAFEOptions)
{
	cout << "\n\n";
        cout << Bold("NAME") << endl;
        cout << "     " + _PROG_NAME + " - makes the script for executing an objective analysis run.\n";
	cout << Bold("SYNOPSIS") << endl;
        cout << "     ";
        cout << Bold(_PROG_NAME) + Bold(" [--help] [--syntax]") << "\n";
        CAFEOptions.PrintSyntax((_PROG_NAME).size() + 5, 63);
        cout << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
        PrintSyntax(CAFEOptions);

        cout << Bold("DESCRIPTION") << endl;

        cout << "               This program will make the shell script that will execute a\n";
        cout << "          special run of a hindcast for analyzing the CAFE training.\n";
        cout << "          This hindcast will analyze several 'non-events' and 'fresh' events\n";
        cout << "          (those that were not used in the original training) that are given\n";
        cout << "          manually.  This hindcast is not very scientific.\n\n";

        cout << Bold("OPTIONS") << endl;

        cout << Bold("\t--help --syntax") << endl;

        CAFEOptions.PrintDescription(63);

        cout << "\n\n";
}



int main(int argc, char *argv[])
{
        vector <string> CommandArgs = ProcessFlatCommandLine(argc, argv);
        CmdOptions CAFEOptions;

        if (CAFEOptions.ParseCommandArgs(CommandArgs) != 0)
        {
                cerr << "ERROR: Invalid syntax..." << endl;
                PrintSyntax(CAFEOptions);
                return(8);
        }

        for (size_t ArgIndex = 0; ArgIndex < CommandArgs.size(); ArgIndex++)
        {
                if (CommandArgs[ArgIndex] == "--help")
                {
                        PrintHelp(CAFEOptions);
                        return(2);
                }
                else if (CommandArgs[ArgIndex] == "--syntax")
                {
                        PrintSyntax(CAFEOptions);
                        return(2);
                }
                else
                {
                        cerr << "ERROR: Unknown option: " << CommandArgs[ArgIndex] << endl;
                        PrintSyntax(CAFEOptions);
                        return(8);
                }
        }

	Configuration ConfigInfo(CAFEOptions.CAFEPath + '/' + CAFEOptions.ConfigFilename);

        if (!ConfigInfo.IsValid())
        {
                cerr << "ERROR: Something wrong with the config file: " << CAFEOptions.CAFEPath << '/' << CAFEOptions.ConfigFilename << endl;
                return(1);
        }

        if (!CAFEOptions.MergeWithConfiguration(ConfigInfo))
        {
                cerr << "ERROR: Conflicts in the command line..." << endl;
                PrintSyntax(CAFEOptions);
                return(8);
        }


	const string ScriptName = CAFEOptions.CAFEPath + "/Objective_Hindcast.sh";
	ofstream ShScript(ScriptName.c_str());

	if (!ShScript.is_open())
        {
                cerr << "ERROR: Could not open the file for writing! FileName: " << ScriptName << endl << endl;
                return(9);
        }

	for (vector<string>::const_iterator ADatabaseName( CAFEOptions.DatabaseNames.begin()), ATimePeriod( CAFEOptions.TimePeriods.begin() );
             ADatabaseName != CAFEOptions.DatabaseNames.end();
             ADatabaseName++, ATimePeriod++)
        {
		for (vector<string>::const_iterator EventTypeName = CAFEOptions.EventTypes.begin();
                     EventTypeName != CAFEOptions.EventTypes.end();
                     EventTypeName++)
		{
			const string FreshFileName = CAFEOptions.CAFEPath + "/SpecialDateLists/Fresh_" + *ATimePeriod + '_' + *EventTypeName + ".dat";
			ifstream FreshFile(FreshFileName.c_str());
			const string NonFileName = CAFEOptions.CAFEPath + "/SpecialDateLists/Non_" + *ATimePeriod + '_' + *EventTypeName + ".dat";
			ifstream NonFile(NonFileName.c_str());

			if (!FreshFile.is_open())
			{
				cerr << "ERROR: Could not find Date file: " << FreshFileName << endl;
			}

			string LineRead = "";
			getline(FreshFile, LineRead);
			while (!FreshFile.eof())
			{
				ShScript << CAFEOptions.CAFEPath << "/TotalRecall " << LineRead << '\n'
				         << CAFEOptions.CAFEPath << "/CalcCorrelation --type=UNTRAINED." 
					 << *EventTypeName << " --silent --date=" << LineRead
					 << " --database=" << *ADatabaseName << endl;
					
				getline(FreshFile, LineRead);
			}

			FreshFile.close();
			
			if (!NonFile.is_open())
                        {
				cerr << "ERROR: Could not find Date file: " << NonFileName << endl;
			}

                        LineRead = "";
			getline(NonFile, LineRead);
                        while (!NonFile.eof())
                        {
				ShScript << CAFEOptions.CAFEPath << "/TotalRecall " << LineRead << '\n'
                                         << CAFEOptions.CAFEPath << "/CalcCorrelation --type=NON." 
					 << *EventTypeName << " --silent --date=" << LineRead
					 << " --database=" << *ADatabaseName << endl;

                                getline(NonFile, LineRead);
                        }

                        NonFile.close();
                        
		}// end event type loop
	}// end database/timeperiod loop

	ShScript.close();

	cout << "\nAll Done!\n";
	cout << "The script's name is: " << ScriptName << endl << endl;

	return(0);
}
