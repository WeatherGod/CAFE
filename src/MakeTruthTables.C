using namespace std;

#include <iostream>
#include <iomanip>			// for printf()
#include <fstream>
#include <string>
#include <vector>
#include <cctype>			// for size_t
#include <cmath>			// for sqrt()

#include "Config/Configuration.h"

#include "Utils/CAFEUtly.h"			// for LoadEventScores()
#include <StrUtly.h>			// for TakeDelimitedList(), StrToDouble(), StripWhiteSpace()

#include <CmdLineUtly.h>                // for ProcessFlatCommandLine()
#include "Utils/CAFE_CmdLine.h"               // for generic CAFE command line handling...
#include <FormatUtly.h>                 // for making things look nice and pretty

#define _PROG_NAME              ((string) "MakeTruthTables")


void PrintSyntax(const CmdOptions &CAFEOptions)
{
        cout << "\n\n";
        cout << Bold("NAME") << endl;
        cout << "     " + _PROG_NAME + " - displays truth tables for the hindcast.\n";
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

        cout << "               This program will analyze the results of the latest hindcast\n";
	cout << "               and display truth tables showing how many events were recognized\n";
	cout << "		as events.\n\n";

        cout << Bold("OPTIONS") << endl;

        cout << Bold("\t--help --syntax") << endl;

        CAFEOptions.PrintDescription(63);

        cout << "\n\n";
}




vector <int> GenerateTruthTable(const vector <double> &EventScores, const vector <double> &NonEventScores, const double &ThresholdVal)
{
	vector <int> TruthTable(4, 0);			// each element counts the number of event scores that fall in to each catagory

	for (size_t i = 0; i < EventScores.size(); i++)
	{
		if (EventScores[i] >= ThresholdVal)
		{
			TruthTable[0]++;		// the event did happen AND the system recognized it		    (good)
		}
		else
		{
			TruthTable[1]++;		// the event did happen AND the system did NOT recognize it         (bad)
		}
	}

        for (size_t i = 0; i < NonEventScores.size(); i++)
        {
                if (NonEventScores[i] >= ThresholdVal)
                {
                        TruthTable[2]++;                // the event did NOT happen AND the system thought it was there      (bad)
                }
                else
                {
                        TruthTable[3]++;                // the event did NOT happen AND the system thought it was NOT there  (good)
                }
        }


	return(TruthTable);

}


void PrintTruthTable(const vector <int> &TruthTable)
{
	const int SumEvents = TruthTable[0] + TruthTable[1] + TruthTable[2] + TruthTable[3];

	
	cout << "                              Recognize                    " << endl;
	cout << "                      True        |       False          " << endl;
	cout << "             /--------------------|--------------------\\" << endl;
	cout << "             |                    |                    |" << endl;
	printf("       True  |       %-6d       |       %-6d       |\n", TruthTable[0], TruthTable[1]);
	cout << "             |                    |                    |" << endl;
	cout << "Event        |--------------------+--------------------|" << endl;
	cout << "             |                    |                    |" << endl;
	printf("       False |       %-6d       |       %-6d       |\n", TruthTable[2], TruthTable[3]);
	cout << "             |                    |                    |" << endl;
	cout << "             \\--------------------|--------------------/" << endl;
	cout << "     Correct: %" << (((double) (TruthTable[0] + TruthTable[3]))/((double) SumEvents)) * 100.0;
	cout << "\n     Incorrect: %" << (((double) (TruthTable[1] + TruthTable[2]))/((double) SumEvents)) * 100.0;
	cout << endl;
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

	const string BaseDir = CAFEOptions.CAFEPath + "/AnalysisInfo/";

	try
	{
        	for (vector<string>::const_iterator ADatabaseName( CAFEOptions.ClustDatabaseNames.begin()), ATimePeriod( CAFEOptions.TimePeriods.begin() );
	             ADatabaseName != CAFEOptions.DatabaseNames.end();
        	     ADatabaseName++, ATimePeriod++)
        	{
			vector <string> TheTableList;
			vector <double> ThresholdVals;

			if (!LoadThresholdVals(BaseDir + *ADatabaseName + "/ThresholdVals.dat",
						TheTableList, ThresholdVals))
			{
				throw("Could not load the threshold values...");
			}

               	        for (vector<string>::const_iterator EventTypeName = CAFEOptions.EventTypes.begin();
                             EventTypeName != CAFEOptions.EventTypes.end();
                             EventTypeName++)
                        {
				if (find(TheTableList.begin(), TheTableList.end(), *EventTypeName) == TheTableList.end())
				{
					cerr << "We do not have the threshold value for this event type: " << *EventTypeName << endl;
					continue;
				}

				size_t ThreshIndex = find(TheTableList.begin(), TheTableList.end(), *EventTypeName) - TheTableList.begin();

				vector <double> EventScores(0);
				vector <double> NonEventScores(0);
				vector <string> EventDates(0);
				vector <string> NonEventDates(0);
				string EventScoreFilename = BaseDir + "CorrelationCalcs/" + *ADatabaseName 
							    + "/" + *EventTypeName + "_EventScore.csv";
				string NonEventScoreFilename = BaseDir + "CorrelationCalcs/" + *ADatabaseName
							    + "/NON_" + *EventTypeName + "_EventScore.csv";

				if (!LoadEventScores(EventScores, EventDates, EventScoreFilename, *EventTypeName))
				{
					throw("Could not read from the event scores file: " + EventScoreFilename);
				}

				if (!LoadEventScores(NonEventScores, NonEventDates, NonEventScoreFilename, *EventTypeName))
				{
					throw("Could not read from the non-event scores file: " + NonEventScoreFilename);
				}

				vector <int> TruthTable = GenerateTruthTable(EventScores, NonEventScores, ThresholdVals[ThreshIndex]);

				cout << "     Time Period: " << *ATimePeriod << "      Table: " << *EventTypeName << endl;
				PrintTruthTable(TruthTable);
				cout << endl;
			}// end Table for-loop
		}// end database for-loop
	}
        catch (const exception &Err)
        {
                cerr << "ERROR: Something went wrong: " << Err.what() << endl;
                return(5);
        }
        catch (const string &ErrStr)
        {
                cerr << "ERROR: " << ErrStr << endl;
                return(6);
        }
        catch (...)
        {
                cerr << "ERROR: Unknown exception caught..." << endl;
                return(7);
        }


	cout << "\n\tAll done!\n\n";
	return(0);
}
