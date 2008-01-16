using namespace std;

#include <iostream>
#include <string>
#include <vector>
#include <cctype>		// for size_t

#include "Config/Configuration.h"
#include "Config/CAFEState.h"

#include <mysql++/mysql++.h>
#include "Utils/CAFE_SQLUtly.h"	// for EstablishConnection()

#include <CmdLineUtly.h>	// for ProcessFlatCommandLine()
#include "Utils/CAFE_CmdLine.h"	// for generic CAFE command line option control

void PrintSyntax(const CmdOptions &CAFEOptions)
{
	cout << endl;
	cout << "DatabaseMaker [--help] [--syntax]" << endl;

	CAFEOptions.PrintSyntax(13, 63);

	cout << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
	PrintSyntax(CAFEOptions);

	cout << endl;
	cout << "Makes the databases for the CAFE training..." << endl << endl;

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
		EstablishConnection(ServerLink, currState.GetServerName(), currState.GetLoginUserName());
	}
	catch (const exception& Err)
	{
		cerr << "ERROR: Connection failed: " << Err.what() << endl;
		ServerLink.close();
		return(3);
	}
	catch (const string &ErrStr)
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
		for (currState.TimePeriods_Begin(); currState.TimePeriods_HasNext(); currState.TimePeriods_Next())
		{
			const string Database = currState.Untrained_Name();
			const string ClustDatabase = currState.Trained_Name();

			mysqlpp::NoExceptions NE(ServerLink);		// if a database doesn't exist, it isn't an error.

			if (!ServerLink.select_db(Database))
			{
				if (!ServerLink.create_db(Database))
				{
					throw("Could not create database: " + Database + "\nMySQL message: " + ServerLink.error());
				}
			}

			if (!ServerLink.select_db(ClustDatabase))
                        {
                                if (!ServerLink.create_db(ClustDatabase))
                                {
                                        throw("Could not create database: " + ClustDatabase + "\nMySQL message: " + ServerLink.error());
                                }
                        }

		}
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
	
	cout << "\nAll Done!" << endl;
	ServerLink.close();
	return(0);
}
