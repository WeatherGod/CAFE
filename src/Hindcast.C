using namespace std;

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <mysql++/mysql++.h>

#include "Config/Configuration.h"
#include <StrUtly.h>		// for StrToUpper(), TakeDelimitedList(), StripWhiteSpace()
#include <TimeUtly.h>		// for GiveTime()

#include "Utils/CAFE_SQLUtly.h"	// for LoadEventDateTimes(), EstablishConnection()

#include <CmdLineUtly.h>        // for ProcessFlatCommandLine()
#include "Utils/CAFE_CmdLine.h"       // for handling generic commandline options available to most programs in the CAFE system.

#include <FormatUtly.h>		// for Bold(), Underline()

#define _PROG_NAME ((string) "Hindcast")

void PrintSyntax(const CmdOptions &CAFEOptions)
{
	string TheSpacing((_PROG_NAME).size() + 6, ' ');
        cout << "\n";
        cout << "     " << _PROG_NAME << " " << Bold("[--help] [--syntax] [--use_cache[=") + Underline("CACHE_NAME")
	     << Bold("] | --set_cache[=") + Underline("CACHE_NAME") + Bold("]]") << "\n";
	cout << TheSpacing << Bold("[--score_run=") + Underline("SCORE_RUN_NAME") + Bold("]") << "\n";
#ifdef _SAMPLINGRUN_
        cout << TheSpacing << Bold("--fold=") + Underline("FOLD_NUM") << "\n";
#endif


        CAFEOptions.PrintSyntax(11, 63);

        cout << "        This program will control the hindcast process for the events in the training set.\n";
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
        PrintSyntax(CAFEOptions);

        cout << "DESCRIPTION" << endl;
	cout << Bold("\t--use_cache") << endl;
	cout << "\t\tTells the hindcaster to make sure that the programs use the named forecast cache instead\n";
	cout << "\t\tof re-finding the peaks and valleys.\n\n";
	cout << Bold("\t--set_cache") << endl;
	cout << "\t\tTells the hindcaster to have the programs cache their forecasting results as named for quick re-hindcasting\n";
	cout << "\t\tBy default, " + Underline("CACHE_NAME") + " is 'Default'.\n\n";
	cout << Bold("\t--score_run") << endl;
	cout << "\t\tTells the hindcaster to save this hindcasting run under the given name.\n";
	cout << "\t\tBy default, the hindcasting run is saved as 'Default'.\n\n";

#ifdef _SAMPLINGRUN_
        cout << Bold("--fold") << "\n";
	cout << "\t\tIndicates which sample fold is being dealt with.\n";
#endif
	
        cout << endl;
        CAFEOptions.PrintDescription(63);
        cout << endl;
}


void MakeTableListFile(const string &BaseDir, const string &TypeOfEvent, const string &EventTypeName)
{
	const string TableListname = BaseDir + '/' + TypeOfEvent + "_EventNameList.txt";

        ofstream TableStream(TableListname.c_str());
        if (!TableStream.is_open())
        {
        	throw("Could not open the table list file: " + TableListname + " for writing.");
        }

        TableStream << EventTypeName << '\n';
        TableStream.close();
}


/* WARNING -- This function has an inherent logic flaw to it.
   Because it calls other programs, those programs do not inherit
   this program's configuration options.  Therefore, those
   programs will utilize the default options.
*/

void DoThisHindcast(const vector <string> &DateStrs, 
		    const string &Database, const string &TimePeriod, const string &EventType, const string &CAFEPath,
		    const bool &UseCache, const bool &SetCache, const string &CacheName, 
		    const string &ScoreRun_Name,
		    ofstream &LogJam, string &LastKnownDatabase, string &LastKnownTable, string &LastKnownDate,
		    const vector <string> &EventTypeNames, const int &FoldNumber = -1)
{
	for (vector<string>::const_iterator ADateStr = DateStrs.begin(); ADateStr != DateStrs.end(); ADateStr++)
        {
	        string SysCommand = "";
                int SystemResult = 0;

                if (!UseCache)
                {
        	        SysCommand = CAFEPath + "/bin/TotalRecall " + *ADateStr;

                        if (SetCache)
                        {
                	        SysCommand += " --set_cache=" + CacheName;
                        }

			SysCommand += " -e " + GiveDelimitedList(EventTypeNames, ',');

                        cout << SysCommand << endl;
			LogJam << SysCommand << endl;

                        SystemResult = system(SysCommand.c_str());

                        if (SystemResult != 0)
                        {
                        	throw("TotalRecall  ErrorNum: " + IntToStr(SystemResult));
                        }
		}

		SysCommand = CAFEPath + "/bin/CalcCorrelation --type=" + EventType + " --silent --date=" + *ADateStr
                             + " --timeperiod=" + TimePeriod + " --score_run=" + ScoreRun_Name;

		if (UseCache || SetCache)
                {
                	SysCommand += " --use_cache=" + CacheName;
                }

		if (FoldNumber > 0)
		{
			SysCommand += " --fold=" + IntToStr(FoldNumber);
		}

		SysCommand += " -e " + GiveDelimitedList(EventTypeNames, ',');

                cout << SysCommand << endl;
		LogJam << SysCommand << endl;

                
                SystemResult = system(SysCommand.c_str());
                if (SystemResult != 0)
                {
                	throw("CalcCorrelation.  ErrorNum: " + IntToStr(SystemResult));
                }

                LastKnownDatabase = Database;
		LastKnownTable = EventType;	
                LastKnownDate = *ADateStr;
	}
}



#ifdef _SAMPLINGRUN_
#include "Utils/SamplingUtly.h"               // for LoadCaseTimes(), RemoveCaseDates()
#endif


int main(int argc, char *argv[])
{
	setenv("TZ", "UTC UTC", 1);

	vector <string> CommandArgs = ProcessFlatCommandLine(argc, argv);

        CmdOptions CAFEOptions;
        if (CAFEOptions.ParseCommandArgs(CommandArgs) != 0)
        {
                cerr << "ERROR: Invalid Command Line arguements...\n\n";
                PrintSyntax(CAFEOptions);
                return(8);
        }

#ifdef _SAMPLINGRUN_
        int FoldNumber = -1;
#endif

	bool UseCache = false;
	bool SetCache = false;
	string CacheName = "Default";
	string ScoreRun_Name = "Default";

        for (size_t ArgIndex = 0; ArgIndex < CommandArgs.size(); ArgIndex++)
        {
                if (CommandArgs[ArgIndex].find("--help") == 0)
                {
                        PrintHelp(CAFEOptions);
                        return(2);
                }
                else if (CommandArgs[ArgIndex].find("--syntax") == 0)
                {
                        PrintSyntax(CAFEOptions);
                        return(2);
                }
		else if (CommandArgs[ArgIndex].find("--use_cache") == 0)
		{
			UseCache = true;
			if (CommandArgs[ArgIndex].find("--use_cache=") == 0)
			{
				CacheName = CommandArgs[ArgIndex].substr(12);
			}
		}
		else if (CommandArgs[ArgIndex].find("--set_cache") == 0)
		{
			SetCache = true;
			if (CommandArgs[ArgIndex].find("--set_cache=") == 0)
			{
				CacheName = CommandArgs[ArgIndex].substr(12);
			}
		}
		else if (CommandArgs[ArgIndex].find("--score_run=") == 0)
		{
			ScoreRun_Name = CommandArgs[ArgIndex].substr(12);
		}		
#ifdef _SAMPLINGRUN_
                else if (CommandArgs[ArgIndex].find("--fold=") == 0)
                {
                        FoldNumber = StrToInt( CommandArgs[ArgIndex].substr(7) );
                }
#endif
                else
                {
                        cerr << "ERROR: Invalid Command Line arguement: " << CommandArgs[ArgIndex] << "\n\n";
                        PrintSyntax(CAFEOptions);
                        return(8);
                }
        }

	if (CacheName.empty())
	{
		cerr << "ERROR: Empty string given for the Cache name." << endl;
		return(8);
	}

	if (ScoreRun_Name.empty())
	{
		cerr << "ERROR: Empty string given for the scoring run name." << endl;
		return(8);
	}

#ifdef _SAMPLINGRUN_
	if (FoldNumber <= 0 || FoldNumber > 10)
	{
		cerr << "ERROR: Invalid sample fold number.  This is what was interpreated: " << FoldNumber << endl;
		cerr << "Note: An invalid translation from string to integer would result in a zero." << endl;
		return(8);
	}
#endif


        Configuration ConfigInfo(CAFEOptions.CAFEPath + '/' + CAFEOptions.ConfigFilename);             // loads configurations

        if (!ConfigInfo.IsValid())
        {
                cerr << "ERROR: Invalid configuration file: " << CAFEOptions.CAFEPath << '/' << CAFEOptions.ConfigFilename << endl;
                return(1);
        }

        if (!CAFEOptions.MergeWithConfiguration(ConfigInfo))
        {
                cerr << "ERROR: Conflicts in the command line..." << endl;
                return(8);
        }

	const string LogFileName = CAFEOptions.CAFEPath + "/logs/" + ScoreRun_Name + "_Hindcast.log";

	ifstream LogStream(LogFileName.c_str());

	bool NeedRecovery = false;
	string RedoDate = "";
	string RedoTable = "";
	string RedoDatabase = "";

	if (LogStream.is_open())
	{
		string LogRead = "";
		getline(LogStream, LogRead);

		while (!LogStream.eof() && !NeedRecovery)
		{
			if (LogRead.find("---^^^---Error:") == 0)
			{
				cout << "\n\nLast hindcast didn't finish...\n";

				NeedRecovery = true;
				string ErrorMesg = LogRead.substr(15, LogRead.length() - 15);
				cout << "ErrorMessage: " << ErrorMesg << endl;

				getline(LogStream, LogRead);

				vector <string> RedoList = TakeDelimitedList(LogRead, ',');
				StripWhiteSpace(RedoList);
				if (RedoList.size() != 3)
				{
					cerr << "\n\tCould not recover last hindcast in the log: " << LogRead << endl;
					cerr << "\tExiting program...\n\n";
					return(10);
				}
				RedoDatabase = RedoList[0];
				RedoTable = RedoList[1];
				RedoDate = RedoList[2];

				cout << "Recovering from   Database: " << RedoDatabase << "   Table: " << RedoTable << "   Date: " << RedoDate;
				cout << endl << endl;
			}
			else
			{
				getline(LogStream, LogRead);
			}

		}

		LogStream.close();
	}
	

	ofstream LogJam(LogFileName.c_str());

	if (!LogJam.is_open())
	{
		cerr << "\nCould not open the log file: " << LogFileName << endl;
		cerr << "Exiting program..." << endl;
		return(10);
	}


	string LastKnownDatabase = "";
	string LastKnownTable = "";
	string LastKnownDate = "";

	if (NeedRecovery)
	{
		LastKnownDatabase = RedoDatabase;
		LastKnownTable = RedoTable;
		LastKnownDate = RedoDate;
	}

	if (RedoDatabase.empty() && RedoTable.empty() && RedoDate.empty())
	{
		// Sometimes, the Hindcast will fail before the first date is complete, so just redo that date.
		NeedRecovery = false;
	}

	mysqlpp::Connection ServerLink;

        try
        {
		EstablishConnection(ServerLink, CAFEOptions.ServerName, CAFEOptions.CAFEUserName, "", false);
        }
        catch (const exception& Err)
        {
                ServerLink.close();
                cerr << "ERROR: Connection failed: " << Err.what() << endl;
                cerr << "No changes were made..." << endl;
		cerr << "\nWriting to LogFile...\n";
                LogJam << "---^^^---Error: " + (string) Err.what() << endl;
                LogJam << LastKnownDatabase + ',' + LastKnownTable + ',' + LastKnownDate << endl;
		LogJam.close();
                return(3);
        }
	catch (const string& ErrStr)
        {
                ServerLink.close();
                cerr << "ERROR: Connection failed: " << ErrStr << endl;
                cerr << "No changes were made..." << endl;
                cerr << "\nWriting to LogFile...\n";
                LogJam << "---^^^---Error: " + ErrStr << endl;
                LogJam << LastKnownDatabase + ',' + LastKnownTable + ',' + LastKnownDate << endl;
                LogJam.close();
                return(3);
        }
        catch (...)
        {
                ServerLink.close();
                cerr << "ERROR: Unknown exception caught while trying to link to the server." << endl;
                cerr << "No changes were made..." << endl;
		cerr << "\nWriting to LogFile...\n";
                LogJam << "---^^^---Error: Unknown exception error." << endl;
                LogJam << LastKnownDatabase + ',' + LastKnownTable + ',' + LastKnownDate << endl;
		LogJam.close();
                return(4);
        }


	try
	{
		for (vector<string>::const_iterator ADatabase( CAFEOptions.ClustDatabaseNames.begin() ), 
						    ARawDatabase( CAFEOptions.DatabaseNames.begin() ),
						    ATimePeriod( CAFEOptions.TimePeriods.begin() ); 
		     ADatabase != CAFEOptions.ClustDatabaseNames.end(); ADatabase++, ARawDatabase++, ATimePeriod++)
		{

			if (!ServerLink.select_db(*ADatabase))
			{
				throw("Could not select the database: " + *ADatabase + "\nMySQL message: " + ServerLink.error());
			}

			if (system(("mkdir --parents '" + CAFEOptions.CAFEPath 
				   + "/CorrelationCalcs/" + ScoreRun_Name + '/' + *ADatabase + "'").c_str()) != 0)
			{
				cerr << "WARNING: Trouble trying to create directory " 
				     << CAFEOptions.CAFEPath + "/CorrelationCalcs/" + ScoreRun_Name + '/' + *ADatabase << "\n"
				     << "       : You may have issues saving the event scores...\n";
			}

			mysqlpp::Query TheQuery = ServerLink.query();

                        for (vector<string>::const_iterator EventTypeName = CAFEOptions.EventTypes.begin();
                             EventTypeName != CAFEOptions.EventTypes.end();
                             EventTypeName++)
                        {
				vector <time_t> EventDates = LoadEventDateTimes(TheQuery, *EventTypeName);
//----------------------------------------------------------------------------------------------------------------------------
//****************************************************************************************************************************
#ifdef _SAMPLINGRUN_
                                if (EventDates.size() < 10)
                                {
                                        cerr << "Not enough events for this eventtype: " << *EventTypeName << "  database: " << *ARawDatabase << endl;
                                        cerr << "Moving on to the next event type..." << endl;
                                        continue;
                                }

                                string FoldName = GetCaseFilename(CAFEOptions.CAFEPath, *ARawDatabase, *EventTypeName, FoldNumber);
                                vector <time_t> FoldDates = LoadCaseTimes(FoldName);
                                RemoveCaseDates(EventDates, FoldDates);
#endif
//******************************************************************************************************************************
//------------------------------------------------------------------------------------------------------------------------------


				if (!NeedRecovery)
				{
					MakeTableListFile(CAFEOptions.CAFEPath + "/CorrelationCalcs/" + ScoreRun_Name,
							  *EventTypeName, *EventTypeName);
				}

				vector <string> DateStrs(0);

				for (vector<time_t>::const_iterator ADate = EventDates.begin(); ADate != EventDates.end(); ADate++)
				{
					if (!NeedRecovery)
					{
						DateStrs.push_back(GiveTime(*ADate, "%HZ%d%b%Y"));
					}

                                        if (NeedRecovery && (*ADatabase == RedoDatabase) 
					    && (*EventTypeName == RedoTable) && (GiveTime(*ADate, "%HZ%d%b%Y") == RedoDate))
                                        {
                                                NeedRecovery = false;
                                        }
                                }

				DoThisHindcast(DateStrs, 
					       *ADatabase, *ATimePeriod, *EventTypeName, CAFEOptions.CAFEPath,
					       UseCache, SetCache, CacheName, ScoreRun_Name,
					       LogJam, LastKnownDatabase, LastKnownTable, LastKnownDate, 
					       TakeDelimitedList(*EventTypeName, ' ')
#ifdef _SAMPLINGRUN_
										     , FoldNumber
#endif
												 );


//-----------------------------------------------------------------------------------------------------------------------------------
//***********************************************************************************************************************************
#ifdef _SAMPLINGRUN_
				if (!NeedRecovery)
                                {
					MakeTableListFile(CAFEOptions.CAFEPath + "/CorrelationCalcs/" + ScoreRun_Name + '/', 
							  "UNTRAINED." + *EventTypeName, *EventTypeName);
                                }


				vector <string> FoldDateStrs(0);
				for (vector<time_t>::const_iterator ADate = FoldDates.begin(); ADate != FoldDates.end(); ADate++)
				{
					if (!NeedRecovery)
					{
						FoldDateStrs.push_back(GiveTime(*ADate, "%HZ%d%b%Y"));
					}

					if (NeedRecovery && (*ADatabase == RedoDatabase)
					    && (("UNTRAINED." + *EventTypeName) == RedoTable) && (GiveTime(*ADate, "%HZ%d%b%Y") == RedoDate))
					{
						NeedRecovery = false;
					}
				}

				DoThisHindcast(FoldDateStrs, 
					       *ADatabase, *ATimePeriod, "UNTRAINED." + *EventTypeName, CAFEOptions.CAFEPath,
					       UseCache, SetCache, CacheName, ScoreRun_Name, 
					       LogJam, LastKnownDatabase, LastKnownTable, LastKnownDate, 
					       TakeDelimitedList(*EventTypeName, ' '), FoldNumber);
#endif
//************************************************************************************************************************************
//------------------------------------------------------------------------------------------------------------------------------------
			}// end event type for-loop
		} // end of the database for-loop
	}
	catch (const exception &Err)
	{
		cerr << "ERROR: Something went wrong: " << Err.what() << endl;
		cerr << "Potentially more information: " << ServerLink.error() << endl;
                ServerLink.close();
		cerr << "\nWriting to LogFile...\n";
                LogJam << "---^^^---Error: " + (string) Err.what() << endl;
                LogJam << LastKnownDatabase + ',' + LastKnownTable + ',' + LastKnownDate << endl;
                LogJam.close();
		return(5);
	}
	catch (const string &ErrStr)
	{
		cerr << "ERROR: " << ErrStr << endl;
		ServerLink.close();
		cerr << "\nWriting to LogFile...\n";
                LogJam << "---^^^---Error: " + ErrStr << endl;
                LogJam << LastKnownDatabase + ',' + LastKnownTable + ',' + LastKnownDate << endl;
                LogJam.close();
		return(6);
	}
	catch (...)
	{
		cerr << "ERROR: Unknown exception caught..." << endl;
                ServerLink.close();
		cerr << "\nWriting to LogFile...\n";
                LogJam << "---^^^---Error: Unknown exception error." << endl;
                LogJam << LastKnownDatabase + ',' + LastKnownTable + ',' + LastKnownDate << endl;
                LogJam.close();
                return(7);
	}

	LogJam << "---^^^---Finished" << endl;

	ServerLink.close();
	LogJam.close();

	cout << "\n\nAll Done!\n\n";
	return(0);
}
