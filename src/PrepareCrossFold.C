using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>					// for time_t and time()
#include <cstdlib>					// for srand() and rand()

#include "Config/Configuration.h"
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


void DoDatabaseBreakdown(mysqlpp::Connection &ServerLink, const Configuration &ConfigInfo, const CmdOptions &CAFEOptions)
{
	for (vector<string>::const_iterator ADatabaseName = CAFEOptions.DatabaseNames.begin();
             ADatabaseName != CAFEOptions.DatabaseNames.end();
             ADatabaseName++)
        {
		srand( time(NULL) );		// time to seed the randomizer.
	        if (!ServerLink.select_db(*ADatabaseName))
                {
                        throw("Could not select the database: " + *ADatabaseName + "\nMySQL message: " + ServerLink.error());
                }

                mysqlpp::Query TheQuery = ServerLink.query();

                for (vector<string>::const_iterator EventTypeName = CAFEOptions.EventTypes.begin();
                     EventTypeName != CAFEOptions.EventTypes.end();
                     EventTypeName++)
                {
			vector <time_t> EventTimes( LoadEventDateTimes(TheQuery, *EventTypeName) );

			size_t ListSize = EventTimes.size();

			if (ListSize < 10)
			{
				cerr << "Not enough cases for this event type: " << *EventTypeName << endl;
				cerr << "Moving on..." << endl;
				continue;
			}

			double ApproxBlockSize = (double) ListSize / 10.0;
			double NextCaseChange = (double) ListSize - ApproxBlockSize;
			size_t CaseIndex = 1;

			string Casefilename = GetCaseFilename(CAFEOptions.CAFEPath, *ADatabaseName, *EventTypeName, CaseIndex);
			ofstream CaseStream(Casefilename.c_str());

			if (!CaseStream.is_open())
			{
				throw("Could not open the case file: " + Casefilename + " for writing...");
			}

			while (ListSize > 0)
			{
				// Could possibly improve this to produce better randomness
				size_t RandomIndex = ((size_t) ((((double) rand())/((double) RAND_MAX)) * (ListSize - 1)));

				CaseStream << EventTimes.at(RandomIndex) << "\n";
				EventTimes.erase( EventTimes.begin() + RandomIndex );
				ListSize--;

				if ((double) ListSize <= NextCaseChange)
				{
					CaseIndex++;
					NextCaseChange -= ApproxBlockSize;
					if (CaseIndex <= 10)
					{
						CaseStream.close();
						Casefilename = GetCaseFilename(CAFEOptions.CAFEPath, *ADatabaseName, *EventTypeName, CaseIndex);
						CaseStream.open( Casefilename.c_str() );

						if (!CaseStream.is_open())
			                        {
                        			        throw("Could not open the case file: " + Casefilename + " for writing...");
                        			}
					}
				}
			}

			CaseStream.close();
		}// end of EventType loop
	}// end of database loop
}

int main(int argc, char *argv[])
{
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


	mysqlpp::Connection ServerLink;

        try
        {
		EstablishConnection(ServerLink, CAFEOptions.ServerName, CAFEOptions.CAFEUserName, false);
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
		DoDatabaseBreakdown(ServerLink, ConfigInfo, CAFEOptions);
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
