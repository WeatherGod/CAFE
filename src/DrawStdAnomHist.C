using namespace std;

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>			// for size_t

#include "Config/Configuration.h"
#include "Config/CAFEState.h"
#include <CmdLineUtly.h>		// for ProcessFlatCommandLine()
#include "Utils/CAFE_CmdLine.h"		// for generic CAFE command line handling...
#include <StrUtly.h>			// for GiveDelimitedList()

#include <FormatUtly.h>			// for making things look nice and pretty

void PrintSyntax(const CmdOptions &CAFEOptions)
{
	cout << "     ";
        cout << Bold("DrawStdAnomHist [--help] [--syntax]") << "\n";
	CAFEOptions.PrintSyntax(19, 63);
	cout << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
        cout << "\n\n";
	cout << Bold("NAME") << endl;
	cout << "     DrawStdAnomHist - executes the matlab program to produce the anomaly histograms\n";
	cout << "                           for the current clustered and unclustered CAFE fields.\n\n";

	cout << Bold("SYNOPSIS") << endl;
	PrintSyntax(CAFEOptions);
	
	cout << Bold("DESCRIPTION") << endl;
		
        cout << "               This program will execute matlab to create the histogram files\n";
	cout << "          for the training system.\n";
	cout << "          The pictures are saved to the ";
	cout << Bold("$CAFEPATH/AnalysisInfo/") + Underline("DatabaseName");
	cout << " directorys.\n";
	cout << "          Control over individual fields has not been achieved yet, only for event types.\n\n";

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
        
	// TODO: Bit of a kludge...
	const set<string> eventNames = currState.EventType_Names();

	// TODO: Update the matlab command to let the error message show.
	// The current command uses the try-catch in order to ensure that the Matlab
	// program will not just hang, expecting user input.
	
	// Do the unclustered stuff first
	string SysCommand = "matlab -nodisplay -nodesktop -r \"try MakeStdAnomHists('" + currState.GetCAFEPath() 
			    + "', '" + currState.GetConfigFilename() + "', {'" 
			    + GiveDelimitedList(vector<string>(eventNames.begin(), eventNames.end()), "','")
			    + "'},0); catch disp 'An error occured while making standard anomaly histograms for unclustered fingerprints'; end; exit;\"";

	system(SysCommand.c_str());

	// Do the clustered stuff next
	SysCommand = "matlab -nodisplay -nodesktop -r \"try MakeStdAnomHists('" + currState.GetCAFEPath()
                            + "', '" + currState.GetConfigFilename() + "', {'" 
			    + GiveDelimitedList(vector<string>(eventNames.begin(), eventNames.end()), "','")
                            + "'},1); catch disp 'An error occured while making standard anomaly histograms for clustered fingerprints'; end; exit;\"";

        system(SysCommand.c_str());

	return(0);
}
