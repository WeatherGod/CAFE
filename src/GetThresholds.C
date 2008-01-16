using namespace std;

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>			// for size_t
#include <cmath>			// for sqrt()

#include "Config/Configuration.h"
#include "Config/CAFEState.h"

#include "Utils/CAFEUtly.h"		// for SaveThresholdVals(), LoadEventScores()
#include <StrUtly.h>			// for TakeDelimitedList(), StrToDouble(), StripWhiteSpace()
#include <StatUtly.h>			// for Variance(), Avg()


#include <CmdLineUtly.h>                // for ProcessFlatCommandLine()
#include "Utils/CAFE_CmdLine.h"                       // for generic CAFE command line option handling
#include <FormatUtly.h>                 // for making things look nice and pretty

#define _PROG_NAME              ((string) "GetThresholds")


void PrintSyntax(const CmdOptions &CAFEOptions)
{
	string TheSpacing((_PROG_NAME).size() + 6, ' ');
        cout << "\n\n";
        cout << Bold("NAME") << endl;
        cout << "     " + _PROG_NAME + " - calculates the threshold values from the hindcasting.\n";
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

        cout << "               This program will calculate the thresholds for each event type from the hindcasting.\n\n";

        cout << Bold("OPTIONS") << endl;

        cout << Bold("\t--help --syntax") << endl << endl;
        
        CAFEOptions.PrintDescription(63);

        cout << "\n\n";
}



int main(int argc, char *argv[])
{
	vector <string> CmdLineArgs = ProcessFlatCommandLine(argc, argv);
        CmdOptions CAFEOptions;

        if (CAFEOptions.ParseCommandArgs(CmdLineArgs) != 0)
        {
                cerr << "ERROR: Invalid syntax..." << endl;
                PrintSyntax(CAFEOptions);
                return(8);
        }


        for (size_t ArgIndex = 0; ArgIndex < CmdLineArgs.size(); ArgIndex++)
        {
                if (CmdLineArgs[ArgIndex] == "--help")
                {
                        PrintHelp(CAFEOptions);
                        return(2);
                }
                else if (CmdLineArgs[ArgIndex] == "--syntax")
                {
                        PrintSyntax(CAFEOptions);
                        return(2);
                }
                else
		{
			cerr << "ERROR: Unknown arguement: " << CmdLineArgs[ArgIndex] << endl;
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



        const string BaseDir = currState.GetCAFEPath() + "/AnalysisInfo/";

        for (currState.TimePeriods_Begin(); currState.TimePeriods_HasNext(); currState.TimePeriods_Next())
        {
		vector<double> ThresholdVals(currState.EventTypes_Size(), 0.0);
		vector<double>::iterator AThreshold = ThresholdVals.begin();

                for (currState.EventTypes_Begin(); currState.EventTypes_HasNext(); currState.EventTypes_Next(), AThreshold++)
        	{
	                const string EventScoreFilename = BaseDir + "CorrelationCalcs/" 
							  + currState.Trained_Name() + '/' + currState.EventType_Name() + "_EventScore.csv";

			vector<double> EventScores(0);
			vector<string> EventDates(0);

			if (!LoadEventScores(EventScores, EventDates, EventScoreFilename, currState.EventType_Name()))
			{
				cerr << "\n\tCould not open event score file: " << EventScoreFilename << endl;
				cerr << "\t\tExiting program...\n";
				return(9);
			}
//			const double Median = InterquartileVal(2, EventScores);
//			const double StdDev = sqrt(Variance(EventScores));

//			ThresholdVals[TableIndex] = Median - StdDev;

			*AThreshold = InterquartileVal(1, EventScores);

			cout << DoubleToStr(*AThreshold, 7) << " ";
		}// end Table for-loop

		const string ThresholdFileName = BaseDir + currState.Trained_Name() + "/ThresholdVals.dat";

		// TODO: Bit of a kludge...
		const set<string> eventNames = currState.EventType_Names();
		if (!SaveThresholdVals(ThresholdFileName, vector<string>(eventNames.begin(), eventNames.end()), ThresholdVals))
		{
			cerr << "ERROR: Could not write the threshold file: " << ThresholdFileName << endl;
			cerr << "\t\tExiting program...\n\n";
			return(10);
		}
	}// end database for-loop

	cout << "\n\tAll done!\n\n";
	return(0);
}
