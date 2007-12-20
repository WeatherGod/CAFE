using namespace std;

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "Config/Configuration.h"

#include "Utils/CAFE_SQLUtly.h"	// for GiveTableNames(), CreateTable(), CreateFieldMeasureTable(), EstablishConnection()
#include <mysql++/mysql++.h>

#include "Utils/CAFE_CmdLine.h"	// for generic CAFE command-line support and control
#include <CmdLineUtly.h>	// for ProcessFlatCommandLine()

void DoCreation(mysqlpp::Query &TheQuery, const string &DatabaseName, const Configuration &ConfigInfo, const CmdOptions &CAFEOptions)
{
	vector <string> TableNames = GiveTableNames(TheQuery, DatabaseName);

        for (vector<string>::const_iterator EventTypeName = CAFEOptions.EventTypes.begin();
             EventTypeName != CAFEOptions.EventTypes.end();
             EventTypeName++)
        {
                if (!binary_search(TableNames.begin(), TableNames.end(), *EventTypeName))
                {
       	                // Table doesn't exist, so make it entirely
                        CreateTable(TheQuery, *EventTypeName, ConfigInfo, CAFEOptions);
		}
                else
                {
                        // the table already exists in the database.
                        // Maybe put some algorithm to make sure the fields match what is in the
                        // configuration file
	        }// end if (TableName is in the database)
        }// end of table (event type) loop
}


void PrintSyntax(const CmdOptions &CAFEOptions)
{
        cout << endl;
        cout << "TableMaker [--help] [--syntax]" << endl;

        CAFEOptions.PrintSyntax(17, 63);

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


	mysqlpp::Connection ServerLink;

	try
        {
		EstablishConnection(ServerLink, CAFEOptions.ServerName, CAFEOptions.LoginUserName, "", true);
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
		for (vector<string>::const_iterator DatabaseName = CAFEOptions.DatabaseNames.begin();
	             DatabaseName != CAFEOptions.DatabaseNames.end();
        	     DatabaseName++)
        	{
	                if (!ServerLink.select_db(*DatabaseName))
        	        {
                	        throw("Unable to select database: " + *DatabaseName + "\nMySQL message: " + ServerLink.error());
                	}

	                mysqlpp::Query TheQuery = ServerLink.query();

			DoCreation(TheQuery, *DatabaseName, ConfigInfo, CAFEOptions);
		}


		for (vector<string>::const_iterator DatabaseName = CAFEOptions.ClustDatabaseNames.begin();
                     DatabaseName != CAFEOptions.ClustDatabaseNames.end();
                     DatabaseName++)
                {
                        if (!ServerLink.select_db(*DatabaseName))
                        {
                                throw("Unable to select database: " + *DatabaseName + "\nMySQL message: " + ServerLink.error());
                        }

                        mysqlpp::Query TheQuery = ServerLink.query();

                        DoCreation(TheQuery, *DatabaseName, ConfigInfo, CAFEOptions);

			for (vector<string>::const_iterator EventTypeName( CAFEOptions.EventTypes.begin() );
			     EventTypeName != CAFEOptions.EventTypes.end(); EventTypeName++)
			{
				CreateFieldMeasureTable(TheQuery, *EventTypeName, ConfigInfo, CAFEOptions);
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
