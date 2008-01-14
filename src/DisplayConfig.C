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

	CAFEState curState( CAFEOptions.ConfigMerge( configInfo.GiveCAFEInfo() ) );

	cout << "CAFE State\n"
	     << "==========\n"
	     << "Verbose Level: " << curState.GetVerboseLevel() << '\n'
	     << "Conf Filename: " << curState.GetConfigFilename() << '\n'
	     << "CAFE Path    : " << curState.GetCAFEPath() << '\n'
	     << "Login Name   : " << curState.GetLoginUserName() << '\n'
	     << "CAFEUserName : " << curState.GetCAFEUserName() << '\n'
	     << "ServerName   : " << curState.GetServerName() << '\n'
	     << "TimePeriods  : " << curState.TimePeriods_Size() << '\n'
	     << "Offset\t Name\t Untrained\t Trained" << endl;
	
	for (curState.TimePeriods_Begin(); curState.TimePeriods_HasNext(); curState.TimePeriods_Next())
	{
		cout << curState.TimePeriod_Offset() << "\t "
		     << curState.TimePeriod_Name() << "\t "
		     << curState.Untrained_Name() << "\t "
		     << curState.Trained_Name() << '\n';
	}

	cout << "\nEventTypes: " << curState.EventTypes_Size() << endl;

	for (curState.EventTypes_Begin(); curState.EventTypes_HasNext(); curState.EventTypes_Next())
	{
		cout << '\t' << curState.EventType_Name() << "\t EventVars: " << curState.EventVars_Size() << '\n';

		for (curState.EventVars_Begin(); curState.EventVars_HasNext(); curState.EventVars_Next())
		{
			cout << "\t\t" << curState.EventVar_Name() << "\t " << curState.DataVar_Name() 
			     << "\t Fields: " << curState.CAFELevels_Size() << '\n';

			for (curState.CAFELevels_Begin(); curState.CAFELevels_HasNext(); curState.CAFELevels_Next())
			{
				cerr << "\t\t\t";
				cerr << curState.CAFEField_Name() << "\t "; 
				cerr << curState.DataLevel_Name() << '\n';
			}

			cout << '\n';
		}

		cout << '\n';
	}

	return(0);
}	
