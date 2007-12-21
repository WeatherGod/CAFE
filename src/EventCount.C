using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>			// for size_t
#include <string>

#include "Config/Configuration.h"

#include <mysql++/mysql++.h>

#include "Utils/CAFE_SQLUtly.h"		// for LoadEventCnt(), EstablishConnection()

#include <CmdLineUtly.h>		// for ProcessFlatCommandLine()
#include <StrUtly.h>			// for Size_tToStr()
#include "Utils/CAFE_CmdLine.h"		// for generic CAFE command line control

void PrintSyntax(const CmdOptions &CAFEOptions)
{
        cout << endl;
        cout << "EventCount [--help] [--syntax]" << endl;

        CAFEOptions.PrintSyntax(10, 63);

        cout << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
        PrintSyntax(CAFEOptions);

        cout << endl;
        cout << "Extracts the event counts for the CAFE training..." << endl << endl;

        cout << "DESCRIPTION" << endl;
        CAFEOptions.PrintDescription(63);
        cout << endl;
}



int main(int argc, char *argv[])
{
	vector <string> CmdArgs = ProcessFlatCommandLine(argc, argv);

        CmdOptions CAFEOptions;

        if (CAFEOptions.ParseCommandArgs(CmdArgs) != 0)
        {
                cerr << "ERROR: Invalid syntax..." << endl;
                PrintSyntax(CAFEOptions);
                return(8);
        }

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
                else
                {
                        cerr << "ERROR: unknown arguement: " << CmdArgs[ArgIndex] << endl;
                        PrintSyntax(CAFEOptions);
                        return(8);
                }
        }

	Configuration ConfigInfo(CAFEOptions.CAFEPath + '/' + CAFEOptions.ConfigFilename);             // loads configurations

	if (!ConfigInfo.IsValid())
	{
		cerr << "ERROR: The configuration file is not valid: " << CAFEOptions.CAFEPath << '/' << CAFEOptions.ConfigFilename << endl;
		return(1);
	}

	if (!CAFEOptions.MergeWithConfiguration(ConfigInfo))
        {
                cerr << "ERROR: conflicts in the command line..." << endl;
                PrintSyntax(CAFEOptions);
                return(8);
        }


	mysqlpp::Connection ServerLink;

        try
        {
		EstablishConnection(ServerLink, CAFEOptions.ServerName, CAFEOptions.CAFEUserName, "", false);
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
		cerr << "ERROR: Unknown exception caught..." << endl;
		ServerLink.close();
		return(4);
	}


	try
	{
		for (vector<string>::const_iterator ATimePeriod = CAFEOptions.TimePeriods.begin();
                     ATimePeriod != CAFEOptions.TimePeriods.end();
                     ATimePeriod++)
	        {
        	        const string Database = CAFEOptions.GiveDatabaseName(*ATimePeriod);
		
			if (!ServerLink.select_db(Database))
			{
				throw("Could not select the database: " + Database + "\nMySQL message: " + ServerLink.error());
			}
                
			if (system(("mkdir --parents '" + CAFEOptions.CAFEPath + "/AnalysisInfo/" + Database + "'").c_str()) != 0)
			{
				cerr << "WARNING: Problems while trying to make directory " + CAFEOptions.CAFEPath + "/AnalysisInfo/" + Database << endl;
				cerr << "       : You may have difficulties saving the event counts.\n";
			}

			mysqlpp::Query TheQuery( MakeLoader_EventCnt(ServerLink) );

	                for (vector<string>::const_iterator EventTypeName = CAFEOptions.EventTypes.begin();
	                     EventTypeName != CAFEOptions.EventTypes.end();
        	             EventTypeName++)
        	        {
                	        const string QueryOutput = CAFEOptions.CAFEPath + "/AnalysisInfo/" + Database + '/' + *EventTypeName + ".eventcnt";
				

				const size_t EventCount = LoadEventCnt(TheQuery, *EventTypeName);

				string SysCommand = "echo " + Size_tToStr(EventCount) + " > " + QueryOutput;
				if (system(SysCommand.c_str()) != 0)
				{
					throw("Could not create eventcount file: " + QueryOutput);
				}
			} // end event type loop
		}// end database loop
	}
	catch (const exception &Err)
	{
		cerr << "ERROR: Exception was thrown: " << Err.what() << endl;
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
