using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>					// for size_t
#include <string>
#include <ctime>					// for time_t and time()
#include <cstdlib>					// for srand() and rand()

#include "Config/Configuration.h"
#include "Config/CAFEState.h"

#include "Utils/CAFE_SQLUtly.h"				// for LoadLonLatAnoms(), EstablishConnection()
#include "Utils/CAFE_CmdLine.h"				// for generic CAFE command-line options support
#include <StrUtly.h>					// for GiveDelimitedList()
#include <CmdLineUtly.h>				// for ProcessFlatCommandLine()
#include <FormatUtly.h>					// for Bold(), Underline()

#include "Utils/SamplingUtly.h"				// for GetCaseFilename()

#include <mysql++/mysql++.h>

void PrintSyntax(const CmdOptions &CAFEOptions)
{
        cout << "\nPrepareCrossFold " << Bold("[--help] [--syntax]") << endl;

        CAFEOptions.PrintSyntax(17, 63);

        cout << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
        PrintSyntax(CAFEOptions);

        cout << "Breaks the event lists down into 10 blocks of dates and stores them into files..." << endl;

        cout << endl << Bold("DESCRIPTION") << endl;

        CAFEOptions.PrintDescription(63);

        cout << endl;
}


void DoDatabaseBreakdown(mysqlpp::Connection &ServerLink, CAFEState &currState)
{
	for (currState.TimePeriods_Begin(); currState.TimePeriods_HasNext(); currState.TimePeriods_Next())
        {
		srand( time(NULL) );		// time to seed the randomizer.
	        if (!ServerLink.select_db(currState.Untrained_Name()))
                {
                        throw("Could not select the database: " + currState.Untrained_Name() + "\nMySQL message: " + ServerLink.error());
                }

                mysqlpp::Query TheQuery = ServerLink.query();

                for (currState.EventTypes_Begin(); currState.EventTypes_HasNext(); currState.EventTypes_Next())
                {
			vector<time_t> EventTimes = LoadEventDateTimes(TheQuery, currState.EventType_Name());
			size_t ListSize = EventTimes.size();

			if (ListSize < 10)
			{
				cerr << "Not enough cases for this event type: " << currState.EventType_Name() << endl;
				cerr << "Moving on..." << endl;
				continue;
			}

			double ApproxBlockSize = (double) ListSize / 10.0;
			double NextFoldChange = (double) ListSize - ApproxBlockSize;
			size_t foldIndex = 1;

			string foldFilename = GetCaseFilename(currState.GetCAFEPath(), currState.Untrained_Name(),
							      currState.EventType_Name(), foldIndex);
			ofstream foldStream(foldFilename.c_str());

			if (!foldStream.is_open())
			{
				throw("Could not open the fold file: " + foldFilename + " for writing...");
			}

			while (ListSize > 0)
			{
				// Could possibly improve this to produce better randomness
				size_t RandomIndex = ((size_t) ((((double) rand())/((double) RAND_MAX)) * (ListSize - 1)));

				foldStream << EventTimes.at(RandomIndex) << "\n";
				EventTimes.erase( EventTimes.begin() + RandomIndex );
				ListSize--;

				if ((double) ListSize <= NextFoldChange)
				{
					foldIndex++;
					NextFoldChange -= ApproxBlockSize;
					if (foldIndex <= 10)
					{
						foldStream.close();
						foldFilename = GetCaseFilename(currState.GetCAFEPath(), currState.Untrained_Name(),
									       currState.EventType_Name(), foldIndex);
						foldStream.open( foldFilename.c_str() );

						if (!foldStream.is_open())
			                        {
                        			        throw("Could not open the fold file: " + foldFilename + " for writing...");
                        			}
					}
				}
			}

			foldStream.close();
		}// end of EventType loop
	}// end of database loop
}

int main(int argc, char *argv[])
{
	// Working on eliminationg this need
	setenv("TZ", "UTC UTC", 1);			// to deal with the issue that the dates are in UTC, but I am in another locale.

	vector <string> CommandArgs = ProcessFlatCommandLine(argc, argv);

	CmdOptions CAFEOptions;

	if (CAFEOptions.ParseCommandArgs(CommandArgs) != 0)
	{
		cerr << "ERROR: Invalid syntax..." << endl;
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
			cerr << "ERROR: Unknown arguement: " << CommandArgs[ArgIndex] << endl;
			PrintSyntax(CAFEOptions);
			return(8);
		}
	}

	Configuration ConfigInfo(CAFEOptions.CAFEPath + '/' + CAFEOptions.ConfigFilename);             // loads configurations

	if (!ConfigInfo.IsValid())
	{
		cerr << "Error: The configuration file is not valid: " << CAFEOptions.CAFEPath << '/' << CAFEOptions.ConfigFilename << endl;
		return(1);
	}

	if (!CAFEOptions.MergeWithConfiguration(ConfigInfo))
        {
                cerr << "ERROR: Conflicts in the command line..." << endl;
                return(8);
        }

	CAFEState currState( CAFEOptions.ConfigMerge( ConfigInfo.GiveCAFEInfo() ) );


	mysqlpp::Connection ServerLink;

        try
        {
		EstablishConnection(ServerLink, currState.GetServerName(), currState.GetCAFEUserName(), false);
        }
        catch (const exception& Err)
        {
                cerr << "ERROR: Connection failed: " << Err.what() << endl;
		ServerLink.close();
                return(3);
        }
	catch (const string& ErrStr)
        {
                cerr << "ERROR: Connection failed: " << ErrStr << endl;
                ServerLink.close();
                return(3);
        }
	catch (...)
	{
		cerr << "Unknown exception caught..." << endl;
		ServerLink.close();
		return(4);
	}


	try
	{
		DoDatabaseBreakdown(ServerLink, currState);
	}
	catch (const exception &Err)
	{
		cerr << "ERROR: Something went wrong: " << Err.what() << endl;
		ServerLink.close();
		return(5);
	}
	catch (const string &ErrStr)
	{
		cerr << "ERROR: " << ErrStr << endl;
		ServerLink.close();
		return(6);
	}
	catch (...)
	{
		cerr << "ERROR: Unknown exception caught..." << endl;
		ServerLink.close();
		return(7);
	}

	ServerLink.close();
	return(0);
}
