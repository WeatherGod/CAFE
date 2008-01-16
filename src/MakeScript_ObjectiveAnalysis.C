using namespace std;

#include <iostream>
#include <string>
#include <vector>
#include <cctype>			// for size_t
#include <fstream>

#include "Config/Configuration.h"
#include "Config/CAFEState.h"

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

	CAFEState currState( CAFEOptions.ConfigMerge( ConfigInfo.GiveCAFEInfo() ) );

	const string ScriptName = currState.GetCAFEPath() + "/Objective_Hindcast.sh";
	ofstream ShScript(ScriptName.c_str());

	if (!ShScript.is_open())
        {
                cerr << "ERROR: Could not open the file for writing! FileName: " << ScriptName << endl << endl;
                return(9);
        }

	for (currState.TimePeriods_Begin(); currState.TimePeriods_HasNext(); currState.TimePeriods_Next())
        {
		for (currState.EventTypes_Begin(); currState.EventTypes_HasNext(); currState.EventTypes_Next())
		{
			const string FreshFileName = currState.GetCAFEPath() + "/SpecialDateLists/Fresh_" 
						     + currState.TimePeriod_Name() + '_' + currState.EventType_Name() + ".dat";

			const string NonFileName = currState.GetCAFEPath() + "/SpecialDateLists/Non_"
						   + currState.TimePeriod_Name() + '_' + currState.EventType_Name() + ".dat";


			ifstream FreshFile(FreshFileName.c_str());

			if (!FreshFile.is_open())
			{
				cerr << "ERROR: Could not find Date file: " << FreshFileName << endl;
			}

			string LineRead = "";
			getline(FreshFile, LineRead);
			while (!FreshFile.eof())
			{
				ShScript << currState.GetCAFEPath() << "/TotalRecall " << LineRead << '\n'
				         << currState.GetCAFEPath() << "/CalcCorrelation --type=UNTRAINED." 
					 << currState.EventType_Name() << " --silent --date=" << LineRead
					 << " --database=" << currState.Untrained_Name() << endl;
					
				getline(FreshFile, LineRead);
			}

			FreshFile.close();


			ifstream NonFile(NonFileName.c_str());
			
			if (!NonFile.is_open())
                        {
				cerr << "ERROR: Could not find Date file: " << NonFileName << endl;
			}

                        LineRead = "";
			getline(NonFile, LineRead);
                        while (!NonFile.eof())
                        {
				ShScript << currState.GetCAFEPath() << "/TotalRecall " << LineRead << '\n'
                                         << currState.GetCAFEPath() << "/CalcCorrelation --type=NON." 
					 << currState.EventType_Name() << " --silent --date=" << LineRead
					 << " --database=" << currState.Untrained_Name() << endl;

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
