using namespace std;

#include <iostream>
#include <string>
#include <vector>
#include <cctype>			// for size_t
#include <algorithm>			// for binary_search()
#include <fstream>

#include "Config/Configuration.h"
#include "Config/CAFEState.h"

#include "Utils/CAFE_SQLUtly.h"	// for GiveTableNames(), CreateTable(), CreateFieldMeasureTable(), EstablishConnection()
#include <mysql++/mysql++.h>

#include "Utils/CAFE_CmdLine.h"	// for generic CAFE command-line support and control
#include <CmdLineUtly.h>	// for ProcessFlatCommandLine()

void DoCreation(mysqlpp::Query &TheQuery, const string &DatabaseName, CAFEState &currState)
{
	vector<string> TableNames = GiveTableNames(TheQuery, DatabaseName);

        for (currState.EventTypes_Begin(); currState.EventTypes_HasNext(); currState.EventTypes_Next())
        {
                if (!binary_search(TableNames.begin(), TableNames.end(), currState.EventType_Name()))
                {
       	                // Table doesn't exist, so make it entirely
                        CreateTable(TheQuery, currState.EventType_Name(), currState);
		}
                else
                {
                        // the table already exists in the database.
                        // TODO: Maybe put some algorithm to make sure the fields match what is in the
                        // configuration file
	        }// end if (TableName is in the database)
        }// end of table (event type) loop
}


void PrintSyntax(const CmdOptions &CAFEOptions)
{
        cout << endl;
        cout << "TableMaker [--help] [--syntax]" << endl;

        CAFEOptions.PrintSyntax(11, 63);

        cout << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
        PrintSyntax(CAFEOptions);

        cout << endl;
        cout << "Makes the tables for the current CAFE training..." << endl << endl;

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

	Configuration ConfigInfo(CAFEOptions.CAFEPath + '/' + CAFEOptions.ConfigFilename);

	if (!ConfigInfo.IsValid())
	{
		cerr << "Error: Invalid configuration file: " << CAFEOptions.CAFEPath << '/' << CAFEOptions.ConfigFilename << endl;
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
		EstablishConnection(ServerLink, currState.GetServerName(), currState.GetLoginUserName(), "", true);
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
		cerr << "ERROR: Unknown error..." << endl;
		ServerLink.close();
		return(4);
	}

	try
	{
		for (currState.TimePeriods_Begin(); currState.TimePeriods_HasNext(); currState.TimePeriods_Next())
        	{
	                if (!ServerLink.select_db(currState.Untrained_Name()))
        	        {
                	        throw("Unable to select database: " + currState.Untrained_Name() + "\nMySQL message: " + ServerLink.error());
                	}

	                mysqlpp::Query TheQuery = ServerLink.query();

			DoCreation(TheQuery, currState.Untrained_Name(), currState);
		}


		for (currState.TimePeriods_Begin(); currState.TimePeriods_HasNext(); currState.TimePeriods_Next())
                {
                        if (!ServerLink.select_db(currState.Trained_Name()))
                        {
                                throw("Unable to select database: " + currState.Trained_Name() + "\nMySQL message: " + ServerLink.error());
                        }

                        mysqlpp::Query TheQuery = ServerLink.query();

                        DoCreation(TheQuery, currState.Trained_Name(), currState);

			for (currState.EventTypes_Begin(); currState.EventTypes_HasNext(); currState.EventTypes_Next())
			{
				CreateFieldMeasureTable(TheQuery, currState.EventType_Name(), currState);
			}
                }		
	}
	catch (const exception &Err)
	{
		cerr << "ERROR: Exception caught: " << Err.what() << endl;
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

	cout << "\nAll Done!\n";

	return(0);
}
