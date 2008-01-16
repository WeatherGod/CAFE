using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>			// for rand()
#include <cctype>			// for size_t

#include <mysql++/mysql++.h>

#include "Config/Configuration.h"
#include "Config/CAFEState.h"

#include <StrUtly.h>			// for TakeDelimitedList()
#include <TimeUtly.h>			// for RoundHour(), GiveRandomDate(), GiveTimeUTC(), GetTimeUTC()
#include <CmdLineUtly.h>		// for ProcessFlatCommandLine()
#include "Utils/CAFE_CmdLine.h"		// for generic CAFE command line control

#include "Utils/CAFE_SQLUtly.h"       // for LoadEventDateTimes(), EstablishConnection()

bool IsNonEvent(const time_t &TheTime, const vector<time_t> &EventTimes, const double &MinTimeFromEvent)
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
        cout << "MakeFile_RandomNonEventDates [--help] [--syntax] --count=_DATECOUNT_" << endl;

        CAFEOptions.PrintSyntax(28, 63);

        cout << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
        PrintSyntax(CAFEOptions);

        cout << endl;
        cout << "Generates random dates that are useful for statistical analysis..." << endl << endl;

        cout << "DESCRIPTION" << endl;
	cout << "\t--count" << endl;
	cout << "\t\tThe number of dates you want to generate.\n";
        CAFEOptions.PrintDescription(63);
        cout << endl;
}

int main(int argc, char *argv[])
{
	// Working on eliminating this need from the code
	setenv("TZ", "UTC UTC", 1);
	srand(time(NULL));		// seeds the random number generator

	vector <string> CmdArgs = ProcessFlatCommandLine(argc, argv);
	
	CmdOptions CAFEOptions;

	if (CAFEOptions.ParseCommandArgs(CmdArgs) != 0)
        {
                cerr << "ERROR: Invalid syntax..." << endl;
                PrintSyntax(CAFEOptions);
                return(8);
        }

	int RandomDateCount = -1;

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
		else if (CmdArgs[ArgIndex].find("--count=") == 0)
		{
			RandomDateCount = StrToInt(CmdArgs[ArgIndex].substr(8));
		}
                else
                {
                        cerr << "ERROR: unknown arguement: " << CmdArgs[ArgIndex] << endl;
                        PrintSyntax(CAFEOptions);
                        return(8);
                }
        }

	
	if (RandomDateCount < 0)
	{
		cerr << "ERROR: date count not given..." << endl;
		return(8);
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



	
	const double MinTimeFromEvent = 60 * 60 * 56;		// 56 hours is the closest a random non-event can be from an event date

	const pair<time_t, time_t> timeRange = currState.DefaultDataSource().GiveTimeRange();
	const time_t MinDate = timeRange.first;
	const time_t MaxDate = timeRange.second;


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
                ServerLink.close();
                cerr << "ERROR: Connection failed: " << ErrStr << endl;
                return(3);
        }
        catch (...)
        {
                ServerLink.close();
                cerr << "ERROR: Unknown exception caught while trying to link to the server." << endl;
                return(4);
        }

	system(("mkdir '" + currState.GetCAFEPath() + "/SpecialDateLists'").c_str());

        try
        {
                for (currState.TimePeriods_Begin(); currState.TimePeriods_HasNext(); currState.TimePeriods_Next())
                {
/*
                        if (!ServerLink.select_db(currState.Untrained_Name()))
                        {
                                throw("Could not select the database: " + currState.Untrained_Name() + "\nMySQL message: " + ServerLink.error());
                        }

                        mysqlpp::Query TheQuery( MakeLoader_EventDateTimes(ServerLink) );

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
*/
			vector<time_t> RandomDates(0);
			RandomDates.reserve(RandomDateCount);
			for (int DateCounter = 0; DateCounter < RandomDateCount;)
			{
				time_t RandomTime = GiveRandomDate(MinDate, MaxDate);      // range here is MinDate <= RandomTime <= MaxDate
//				cout << "Before rounding: " << RandomTime;
				RoundHour(RandomTime, 6);
//				cout << "    After rounding: " << RandomTime << endl;
		
//				while (!IsNonEvent(RandomTime, MasterList_Dates, MinTimeFromEvent))
//				{
//					RandomTime = GiveRandomDate(MinDate, MaxDate);
//					RoundHour(RandomTime, 6);
//				}

				if (!binary_search(RandomDates.begin(), RandomDates.end(), RandomTime))
				{
					DateCounter++;
					RandomDates.insert(lower_bound(RandomDates.begin(), RandomDates.end(), RandomTime), RandomTime);
				}
			}

			string RandomDateFile = currState.GetCAFEPath() + "/SpecialDateLists/" + "Non_Event_" + currState.TimePeriod_Name() + ".dat";

	
		        ofstream RandomDateStream(RandomDateFile.c_str());

        		if (!RandomDateStream.is_open())
        		{
        			throw("Could not open RandomDateFile: " + RandomDateFile + " for reading.");
                	}

			for (vector<time_t>::const_iterator ARandomDate = RandomDates.begin();
			     ARandomDate != RandomDates.end();
			     ARandomDate++)
			{
				RandomDateStream << currState.Untrained_Name() << " Non_Event " << GiveTimeUTC(*ARandomDate, "%HZ%d%b%Y") << endl;
			}

			RandomDateStream.close();
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
