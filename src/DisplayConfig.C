using namespace std;


#include <iostream>
#include <string>

#include "Config/CAFEParam.h"
#include "Config/CAFEState.h"

#include "Config/Configuration.h"
#include "Utils/CAFE_CmdLine.h"

#include <CmdLineUtly.h>		// for ProcessFlatCommandLine()
#include <FormatUtly.h>			// for Bold()

void PrintSyntax(const CmdOptions &CAFEOptions)
{
	cerr << "\nDisplayConfig " << Bold("[--help] [--syntax]") << endl;

	CAFEOptions.PrintSyntax(13, 63);

	cerr << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
	PrintSyntax(CAFEOptions);

	cerr << "Displays all of the CAFE information as loaded through\n"
	     << "configuration file and the command-line.\n"
	     << "This is really useful as a double-check for CAFE processing.\n";

	CAFEOptions.PrintDescription(63);

	cerr << endl;
}

int main(int argc, char* argv[])
{
	vector<string> commandArgs = ProcessFlatCommandLine(argc, argv);
	CmdOptions CAFEOptions;

	if (CAFEOptions.ParseCommandArgs(commandArgs) != 0)
	{
		cerr << "ERROR: Invalid syntax..." << endl;
		PrintSyntax(CAFEOptions);
		return(0);
	}

	for (vector<string>::const_iterator anArg = commandArgs.begin();
	     anArg != commandArgs.end();
	     anArg++)
	{
		if (*anArg == "--help")
		{
			PrintHelp(CAFEOptions);
			return(2);
		}
		else if (*anArg == "--syntax")
		{
			PrintSyntax(CAFEOptions);
			return(2);
		}
		else
		{
			cerr << "ERROR: Unknown option... '" << *anArg << "'\n";
			PrintSyntax(CAFEOptions);
			return(2);
		}
	}

	cerr << "Loading config file...\n";

	Configuration configInfo(CAFEOptions.CAFEPath + '/' + CAFEOptions.ConfigFilename);

	if (!configInfo.IsValid())
	{
		cerr << "ERROR: Invalid configuration file: " << CAFEOptions.CAFEPath + '/' + CAFEOptions.ConfigFilename << endl;
		return(1);
	}

	cerr << "Is valid\nMerging...\n";

	if (!CAFEOptions.MergeWithConfiguration(configInfo))
	{
		cerr << "ERROR: Conflicts in the command line..." << endl;
		PrintSyntax(CAFEOptions);
		return(1);
	}

	cerr << "Good merge!\n";

	CAFEState currState( CAFEOptions.ConfigMerge( configInfo.GiveCAFEInfo() ) );

	cout << "CAFE State\n"
	     << "==========\n"
	     << "Verbose Level: " << currState.GetVerboseLevel() << '\n'
	     << "Conf Filename: " << currState.GetConfigFilename() << '\n'
	     << "CAFE Path    : " << currState.GetCAFEPath() << '\n'
	     << "Login Name   : " << currState.GetLoginUserName() << '\n'
	     << "CAFEUserName : " << currState.GetCAFEUserName() << '\n'
	     << "ServerName   : " << currState.GetServerName() << '\n'
	     << "TimePeriods  : " << currState.TimePeriods_Size() << '\n'
	     << "DataSource   : " << currState.DataSource_Name() << '\n'
	     << "Offset\t Name\t Untrained\t Trained" << endl;
	
	for (currState.TimePeriods_Begin(); currState.TimePeriods_HasNext(); currState.TimePeriods_Next())
	{
		cout << currState.TimePeriod_Offset() << "\t "
		     << currState.TimePeriod_Name() << "\t "
		     << currState.Untrained_Name() << "\t "
		     << currState.Trained_Name() << '\n';
	}

	cout << "\nEventTypes: " << currState.EventTypes_Size() << endl;

	for (currState.EventTypes_Begin(); currState.EventTypes_HasNext(); currState.EventTypes_Next())
	{
		cout << '\t' << currState.EventType_Name() << "\t EventVars: " << currState.EventVars_Size() << endl;

		for (currState.EventVars_Begin(); currState.EventVars_HasNext(); currState.EventVars_Next())
		{
			cerr << "\t\t";
			cerr << currState.EventVar_Name() << "\t ";
			cerr << currState.DataVar_Name() << "\t Fields: ";
			cerr << currState.EventLevels_Size() << endl;

			for (currState.EventLevels_Begin(); currState.EventLevels_HasNext(); currState.EventLevels_Next())
			{
				cerr << "\t\t\t";
				cerr << currState.EventField_Name() << "\t "; 
				cerr << currState.DataLevel_Name() << '\n';
			}

			cout << '\n';
		}

		cout << '\n';
	}

	return(0);
}
