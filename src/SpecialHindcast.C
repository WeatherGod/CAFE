// Still need a lot of work...
using namespace std;

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "Config/Configuration.h"
#include <StrUtly.h>		// for StrToUpper(), TakeDelimitedList(), StripWhiteSpace()

#include <CmdLineUtly.h>                // for ProcessFlatCommandLine()
#include "Utils/CAFE_CmdLine.h"                       // for generic CAFE command line option handling
#include <FormatUtly.h>                 // for making things look nice and pretty

#define _PROG_NAME              ((string) "SpecialHindcast")


void PrintSyntax(const CmdOptions &CAFEOptions)
{
	string TheSpacing((_PROG_NAME).size(), ' ');
        cout << "\n\n";
        cout << Bold("NAME") << endl;
        cout << "     " + _PROG_NAME + " - does hindcast using special times not in the training database.\n";
        cout << Bold("SYNOPSIS") << endl;
        cout << "     ";
        cout << Bold(_PROG_NAME) + Bold(" [--help] [--syntax] [--use_cache[=") + Underline("CACHE_NAME") 
	     << Bold("] | --set_cache[=") + Underline("CACHE_NAME") + Bold("]] ") + Underline("FileName") << "\n";
	cout << TheSpacing << Bold("[--score_run=") + Underline("SCORE_RUN_NAME") + Bold("]") << "\n";
        CAFEOptions.PrintSyntax((_PROG_NAME).size() + 5, 63);
        cout << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
        PrintSyntax(CAFEOptions);

        cout << Bold("DESCRIPTION") << endl;

        cout << Bold("\t--help --syntax") << endl << endl;
	cout << Bold("\t--use_cache") << endl;
        cout << "\t\tTells the hindcaster to make sure that the programs use the forecast cache as named.\n\n";
        cout << Bold("\t--set_cache") << endl;
        cout << "\t\tTells the hindcaster to have the programs cache their forecast information as named.\n";
	cout << "\t\t" << Underline("CACHE_NAME") << " is, by default, 'Default'.\n\n";
        cout << "\t" << Underline("FileName") << endl;
        cout << "\t\tThe date list file name.\n\n";
	cout << Bold("\t--score_run") << endl;
	cout << "\t\tMakes sure that the hindcasting run is saved as named.  By default, it is named 'Default'.\n\n";

        CAFEOptions.PrintDescription(63);

        cout << "\n\n";
}


void MakeTableListFile(const string &BaseDir, const string &TypeOfEvent, const vector <string> &EventTypeNames)
{
        const string TableListname = BaseDir + '/' + TypeOfEvent + "_EventNameList.txt";
        
        ofstream TableStream(TableListname.c_str());
        if (!TableStream.is_open())
        {
                throw("Could not open the table list file: " + TableListname + " for writing.");
        }

        TableStream << GiveDelimitedList(EventTypeNames, ',') << '\n';
        TableStream.close();
}




void WriteErrorToLog(ofstream &LogJam, const string LastKnownDatabase, const string LastKnownTable, const string LastKnownDate, const string ErrMesg)
{
	cerr << "\nWriting to LogFile...\n";

        LogJam << "---^^^---Error: " + ErrMesg << endl;

        string LogWrite = LastKnownDatabase + ',' + LastKnownTable + ',' + LastKnownDate;

        LogJam << LogWrite << endl;
}


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

	bool UseCache = false;
        bool SetCache = false;
	string CacheName = "Default";
	string ScoreRun_Name = "Default";
	string DateListFileName = "";

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
		else
		{
			DateListFileName = CommandArgs[ArgIndex];
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

	if (DateListFileName.empty())
	{
		cerr << "ERROR: No filename given for the date list..." << endl;
		return(8);
	}

	const string LogFileName = CAFEOptions.CAFEPath + "/logs/" + ScoreRun_Name + "_SpecialHindcast.log";

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

				cout << "Recovering from   Database: " << RedoDatabase << "   Table: " << RedoTable << "   Date: " << RedoDate << endl << endl;
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
		cerr << "\nCould not open the log file for writing: " << LogFileName << endl;
		cerr << "Exiting program..." << endl;
		return(11);
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

	vector <string> DatabaseList(0);
	vector <string> TableList(0);
	vector <string> DateList(0);


	ifstream DateFile(DateListFileName.c_str());

	if (!DateFile.is_open())
	{
		cerr << "Couldn't open the date file: " << DateListFileName << endl;
                string LogWrite = "DateFile open fail ErrorNum: 4\n";

                WriteErrorToLog(LogJam, LastKnownDatabase, LastKnownTable, LastKnownDate, LogWrite);
                LogJam.close();
                cerr << "Exiting Hindcast...\n\n";
                return(4);
	}

	string LineRead = "";

	getline(DateFile, LineRead);
		
	while (!DateFile.eof())
	{
		StripWhiteSpace(LineRead);

		vector <string> TempHold = TakeDelimitedList(LineRead, ' ');
					
		if (TempHold.size() != 3)
		{
			cerr << "Error in the date file: " << DateListFileName << endl;
                        cerr << "The Line: " << LineRead << endl;
                        string LogWrite = "DateFile missread ErrorNum: 4\n";

			WriteErrorToLog(LogJam, LastKnownDatabase, LastKnownTable, LastKnownDate, LogWrite);
                        LogJam.close();
                        DateFile.close();
                        cerr << "Exiting Hindcast...\n\n";
                        return(4);
                }

		DatabaseList.push_back(TempHold[0]);
		TableList.push_back(TempHold[1]);
		DateList.push_back(TempHold[2]);
		
		getline(DateFile, LineRead);
	} // end of the while(!DateFile.eof()) block

        DateFile.close();

	for (vector <string>::const_iterator ADatabase( DatabaseList.begin() ), ATable( TableList.begin() ), ADate( DateList.begin() );
	     ADatabase != DatabaseList.end();
	     ADatabase++, ATable++, ADate++)
	{
		if (NeedRecovery)
		{
			if ((*ADatabase == RedoDatabase) && (*ATable == RedoTable) && (*ADate == RedoDate))
			{
				NeedRecovery = false;
			}

			continue;
		}

		int SystemResult;
		string SysCommand;

		if (!UseCache)
		{
			SysCommand = CAFEOptions.CAFEPath + "/bin/TotalRecall " + *ADate;

			if (SetCache)
			{
				SysCommand += " --set_cache=" + CacheName;
			}

			SysCommand += " -e " + GiveDelimitedList(CAFEOptions.EventTypes, ',');

			cout << SysCommand << endl;
			LogJam << SysCommand << endl;
					
			SystemResult = system(SysCommand.c_str());

			if (SystemResult != 0)
			{
				cerr << "\nSomething wrong with TotalRecall.  ErrorNum: " << SystemResult;
                                string LogWrite = "TotalRecall  ErrorNum: " + IntToStr(SystemResult);
                                WriteErrorToLog(LogJam, LastKnownDatabase, LastKnownTable, LastKnownDate, LogWrite);
       	                        LogJam.close();
                       	        cerr << "Exiting Hindcast...\n\n";
                               	return(3);
                        }
		}// end if !UseCache

		SysCommand = CAFEOptions.CAFEPath + "/bin/CalcCorrelation --type=" + *ATable + " --silent --date=" + *ADate 
				+ " --score_run=" + ScoreRun_Name
				+ " --timeperiod=Tm00";	// for now...

		if (UseCache || SetCache)
		{
			SysCommand += " --use_cache=" + CacheName;
		}

		SysCommand += " -e " + GiveDelimitedList(CAFEOptions.EventTypes, ',');

		cout << SysCommand << endl;
		LogJam << SysCommand << endl;

		SystemResult = system(SysCommand.c_str());
		if (SystemResult != 0)
		{
			cerr << "\nSomething wrong with CalcCorrelation.  ErrorNum: " << SystemResult;
			string LogWrite = "CalcCorrelation.  ErrorNum: " + IntToStr(SystemResult);
			WriteErrorToLog(LogJam, LastKnownDatabase, LastKnownTable, LastKnownDate, LogWrite);
			LogJam.close();
			cerr << "   Exiting Special Hindcast...\n\n";
			return(2);
		}
					
		//  This means that this iteration of the loop was successful!
		LastKnownDatabase = *ADatabase;
		LastKnownTable = *ATable;
		LastKnownDate = *ADate;
	}

	if (!NeedRecovery)
	{
		LogJam << "---^^^---Finished" << endl;
	}
	else
	{
		cerr << "ERROR: Recovery mode yielded nothing!\n";
		WriteErrorToLog(LogJam, LastKnownDatabase, LastKnownTable, LastKnownDate, "Recovery mode yielded nothing!");
		LogJam.close();
		cerr << "  Exiting Special hindcast...\n\n";
		return(2);
	}

	LogJam.close();
	cout << "\n\nAll Done!\n\n";
	return(0);
}
