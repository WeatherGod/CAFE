using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <cctype>			// for size_t
#include <cstdlib>			// for rand()

#include <mysql++/mysql++.h>

#include "Config/Configuration.h"
#include "Config/CAFEState.h"

#include <StrUtly.h>			// for TakeDelimitedList()
#include <TimeUtly.h>			// for RoundHour(), GiveRandomDate(), GiveTimeUTC(), GetTimeUTC()
#include <CmdLineUtly.h>		// for ProcessFlatCommandLine()
#include "Utils/CAFE_CmdLine.h"		// for generic CAFE command line control

#include "Utils/CAFE_SQLUtly.h"       // for LoadEventDateTimes(), EstablishConnection()

bool IsNonEvent(const time_t &TheTime, const vector <time_t> &EventTimes, const double &MinTimeFromEvent)
{
	size_t MinPos = 1;
	size_t MaxPos = EventTimes.size();
//	cout << "TheTime: " << TheTime << endl;

	do
	{
		size_t HalfPoint = ((MaxPos - MinPos + 1) / 2) + MinPos;
//		cout << "MinPos: " << MinPos << "   MaxPos: " << MaxPos << "   HalfPoint: " << HalfPoint;
		double TimeDiff = difftime(EventTimes[HalfPoint - 1], TheTime);
//		cout << "    TimeDiff: " << TimeDiff << endl;
		if (labs((long int) TimeDiff) < MinTimeFromEvent)
		{
			// we have found that TheTime is within the time range of an Event
			return(false);
		}

		if (TimeDiff < 0)
		{
			// EventTimes[HalfPoint] < TheTime
			// so I need to search above
			MinPos = HalfPoint + 1;
		}
		else
		{
			// EventTimes[HalfPoint] > TheTime
			// so I need to seach below
			MaxPos = HalfPoint - 1;
		}
	}
	while (MinPos <= MaxPos);

	return(true);
}

void PrintSyntax(const CmdOptions &CAFEOptions)
{
        cout << endl;
        cout << "QuickCompare [--help] [--syntax] [--near=_HOURS_]" << endl;

        CAFEOptions.PrintSyntax(13, 63);

        cout << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
        PrintSyntax(CAFEOptions);

        cout << endl;
        cout << "Checks the non_event date list to see how many dates coincides..." << endl << endl;

        cout << "DESCRIPTION" << endl;
        CAFEOptions.PrintDescription(63);
        cout << endl;
}

int main(int argc, char *argv[])
{
	// Working on removing this need...
	setenv("TZ", "UTC UTC", 1);

	vector <string> CmdArgs = ProcessFlatCommandLine(argc, argv);
	
	CmdOptions CAFEOptions;

	if (CAFEOptions.ParseCommandArgs(CmdArgs) != 0)
        {
                cerr << "ERROR: Invalid syntax..." << endl;
                PrintSyntax(CAFEOptions);
                return(8);
        }

	int NearTo = 0;	// zero hours...

        for (size_t ArgIndex = 0; ArgIndex < CmdArgs.size(); ArgIndex++)
        {
                if (CmdArgs[ArgIndex] == "--help")
                {
                        PrintHelp(CAFEOptions);
                        return(2);
                }
                else if (CmdArgs[ArgIndex] == "--syntax")
                {
                        PrintSyntax(CAFEOptions);
                        return(2);
                }
		else if (CmdArgs[ArgIndex].find("--near=") == 0)
		{
			NearTo = StrToInt(CmdArgs[ArgIndex].substr(7));
		}
                else
                {
                        cerr << "ERROR: unknown arguement: " << CmdArgs[ArgIndex] << endl;
                        PrintSyntax(CAFEOptions);
                        return(8);
                }
        }

	
	Configuration ConfigInfo(CAFEOptions.CAFEPath + '/' + CAFEOptions.ConfigFilename);

	if (!ConfigInfo.IsValid())
	{
		cerr << "ERROR: Invalid configuration file: " << CAFEOptions.CAFEPath << '/' << CAFEOptions.ConfigFilename << endl;
		return(1);
	}

	if (!CAFEOptions.MergeWithConfiguration(ConfigInfo))
        {
                cerr << "ERROR: conflicts in the command line..." << endl;
                PrintSyntax(CAFEOptions);
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
                ServerLink.close();
                cerr << "ERROR: Connection failed: " << Err.what() << endl;
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
                ServerLink.close();
                cerr << "ERROR: Unknown exception caught while trying to link to the server." << endl;
                return(4);
        }



        try
        {
                for (currState.TimePeriods_Begin(); currState.TimePeriods_HasNext(); currState.TimePeriods_Next())
                {
                        if (!ServerLink.select_db(currState.Untrained_Name()))
                        {
                                throw("Could not select the database: " + currState.Untrained_Name() + "\nMySQL message: " + ServerLink.error());
                        }

                        mysqlpp::Query TheQuery = ServerLink.query();

			// TODO: use set<time_t> instead...
			vector<time_t> MasterList_Dates(0);

                        for (currState.EventTypes_Begin(); currState.EventTypes_HasNext(); currState.EventTypes_Next())
                        {
                                vector<time_t> EventDates = LoadEventDateTimes(TheQuery, currState.EventType_Name());

				for (vector<time_t>::const_iterator ADate = EventDates.begin(); ADate != EventDates.end(); ADate++)
				{
					if (!binary_search(MasterList_Dates.begin(), MasterList_Dates.end(), *ADate))
					{
						MasterList_Dates.insert(lower_bound(MasterList_Dates.begin(), MasterList_Dates.end(), *ADate), *ADate);
					}
				}
			}

			cout << "EventDates loaded..." << endl;

			string RandomDateFile = currState.GetCAFEPath() + "/SpecialDateLists/" 
						+ "Non_Event_" + currState.Untrained_Name() + ".dat";
	
		        ifstream RandomDateStream(RandomDateFile.c_str());

        		if (!RandomDateStream.is_open())
        		{
        			throw("Could not open RandomDateFile: " + RandomDateFile + " for reading.");
                	}

			string LineRead = "";
			size_t MatchCount = 0;

			getline(RandomDateStream, LineRead);

			while (!RandomDateStream.eof())
			{
				vector<string> TempHold = TakeDelimitedList(LineRead, ' ');
				time_t TempTime = GetTimeUTC(TempHold.at(2), "%HZ%d%b%Y");

				vector<time_t>::const_iterator ClosestMatch = lower_bound(MasterList_Dates.begin(), MasterList_Dates.end(), TempTime);

				if (ClosestMatch != MasterList_Dates.end())
				{
					// not exactly correct, and subject to slight errors, but this is just a quick checker...
					// TODO: document exactly why this is not exactly correct...
					if ((TempTime + 60*60*NearTo) >= *ClosestMatch)
					{
						MatchCount++;
					}
				}

				if (ClosestMatch != MasterList_Dates.begin())
				{
					if ((TempTime - 60*60*NearTo) <= *(ClosestMatch - 1))
					{
						MatchCount++;
					}
				}

				getline(RandomDateStream, LineRead);
			}

			RandomDateStream.close();
			cout << "Database: " << currState.Untrained_Name() << "  Match Count: " 
			     << MatchCount << "  out of " << MasterList_Dates.size() << " event dates\n";
		}// end database loop
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
