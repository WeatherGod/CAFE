using namespace std;


#include <iostream>
#include <vector>
#include <string>

#include <cstdio>		// for getchar()
#include <cctype>		// for toupper(), size_t

#include "Config/Configuration.h"
#include "Config/CAFEState.h"

#include <mysql++/mysql++.h>

#include "Utils/CAFE_SQLUtly.h"		// for DropTables(), EstablishConnection(), ClearTables()

#include <CmdLineUtly.h>		// for ProcessFlatCommandLine()
#include "Utils/CAFE_CmdLine.h"		// for generic CAFE command line handling...
#include <StrUtly.h>			// for TakeDelimitedList()



char CheckResponse()
/* Returns Y or N by error checking methods
Pre: None
Post: returns value of 'Y' or 'N'               */
{
	char Response = 'D';		//Dummy value
	Response = getchar();
	cin.ignore(100, '\n');

	while ((toupper(Response) != 'Y') && (toupper(Response) != 'N'))
	{
		cout << "\nPlease press 'Y' or 'N': ";
		Response = getchar();
		cin.ignore(100, '\n');
	}

	return (toupper(Response));
}// end CheckResponse()




void PrintSyntax(const CmdOptions &CAFEOptions)
{
	cout << endl;
	cout << "ResetTraining [--drop] [--clustered | --both]" << endl;
	
	CAFEOptions.PrintSyntax(13, 63);

	cout << "              [--help] [--syntax]" << endl;
	cout << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
	PrintSyntax(CAFEOptions);

	cout << endl;
	cout << "Resets the training information in the MySQL databases.  If you want to completely remove the tables, use '--drop'" << endl;
	cout << "By default, the program will clear all the tables for both the unclustered and clustered databases." << endl;
	cout << "Use '--clustered' to only effect the clustered databases." << endl << endl;
	
	cout << "DESCRIPTION" << endl;

	CAFEOptions.PrintDescription(63);

	cout << endl;
}


int main(int argc, char *argv[])
{
	vector <string> ArgList = ProcessFlatCommandLine(argc, argv);
	CmdOptions CAFEOptions;
	if (CAFEOptions.ParseCommandArgs(ArgList) > 0)
	{
		cerr << "ERROR: Invalid syntax..." << endl;
		PrintSyntax(CAFEOptions);
		return(8);
	}

	bool DoDrop = false;
	bool DoBoth = true;
	
	for (size_t ArgIndex = 0; ArgIndex < ArgList.size(); ArgIndex++)
	{
		if (ArgList[ArgIndex] == "--drop")
		{
			DoDrop = true;
		}
		else if (ArgList[ArgIndex] == "--clustered")
		{
			DoBoth = false;
		}
		else if (ArgList[ArgIndex] == "--both")
		{
			DoBoth = true;
		}
		else if (ArgList[ArgIndex] == "--help")
		{
			PrintHelp(CAFEOptions);
			return(2);
		}
		else if (ArgList[ArgIndex] == "--syntax")
		{
			PrintSyntax(CAFEOptions);
			return(2);
		}
		else
		{
			cerr << "ERROR: unknown arguement: " << ArgList[ArgIndex] << endl;
			PrintSyntax(CAFEOptions);
			return(8);
		}
	}// end of ArgList loop

	Configuration ConfigInfo(CAFEOptions.CAFEPath + '/' + CAFEOptions.ConfigFilename);

	if (!ConfigInfo.ValidConfig())
        {
                cerr << "ERROR: Invalid configuration file: " << CAFEOptions.CAFEPath << '/' << CAFEOptions.ConfigFilename << endl;
                return(1);
        }

	if (!CAFEOptions.MergeWithConfiguration(ConfigInfo))
        {
                cerr << "ERROR: Conflicts in the command line..." << endl;
                PrintSyntax(CAFEOptions);
                return(8);
        }

	CAFEState currState( CAFEOptions.ConfigMerge( ConfigInfo.GiveCAFEInfo() ) );

	cout << "Are you sure you want to ";
	if (DoDrop)
	{
		cout << "REMOVE";
	}
	else
	{
		cout << "clear";
	}

	// TODO: Bit of a kludge for now...
	const set<string> eventNames = currState.EventType_Names();
	cout << " these tables (" << GiveDelimitedList(vector<string>(eventNames.begin(), eventNames.end()), ", ") << ")\nfor ";
	
	if (DoBoth)
	{
		cout << "both the clustered and unclustered ";
	}
	else
	{
		cout << "the clustered ";
	}

	// TODO: same thing here...
	const set<string> timePeriods = currState.TimePeriod_Names();
	cout << "databases of timeperiods: " 
	     << GiveDelimitedList(vector<string>(timePeriods.begin(), timePeriods.end()), ", ") << "? (Y/N)" << endl;

	// CheckResponse can only return a 'Y' or 'N'
        if (CheckResponse() == 'N')
        {
                cout << "\nNo changes were made..." << endl;
                return(0);
        }


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
		for (currState.TimePeriods_Begin(); currState.TimePeriods_HasNext(); currState.TimePeriods_Next())
		{
			const string Database = currState.Untrained_Name();
			const string ClustDatabase = currState.Trained_Name();

			if (!ServerLink.select_db(ClustDatabase))
                        {
                                throw("Could not select the database: " + ClustDatabase + "\nMySQL message: " + ServerLink.error());
                        }

			mysqlpp::Query TheQuery = ServerLink.query();

                        if (DoDrop)
                        {
                                if (!DropTables(TheQuery, currState.EventType_Names()))
                                {
                                        throw("Could not drop the tables requested for database: " + ClustDatabase
                                              + "\nMySQL message: " + TheQuery.error());
                                }
                        }
                        else
                        {
                                for (currState.EventTypes_Begin(); currState.EventTypes_HasNext(); currState.EventTypes_Next())
                                {
                                        if (!ClearTable(TheQuery, currState.EventType_Name()))
                                        {
                                                throw("Could not clear the table, " + currState.EventType_Name()
                                                      + ", for database: " + ClustDatabase + "\nMySQL message: " + TheQuery.error());
                                        }

					TheQuery << "UPDATE " << mysqlpp::escape << (currState.EventType_Name() + "_FieldMeas") << 
						    " SET Alpha = NULL, Phi = NULL, Gamma_Max = NULL, Chi_Max = NULL";

					try
					{
						TheQuery.execute();
					}
					catch (...)
					{
						cerr << "MySQL query: " << TheQuery.preview() << endl;
						throw;
					}
                                }
                        }


			if (DoBoth)
			{
				if (!ServerLink.select_db(Database))
                        	{
                                	throw("Could not select the database: " + Database + "\nMySQL message: " + ServerLink.error());
                        	}
			
				if (DoDrop)
				{
					if (!DropTables(TheQuery, currState.EventType_Names()))
                        		{
                                		throw("Could not drop the tables requested for database: " 
						      + Database + "\nMySQL message: " + TheQuery.error());
                        		}
				}
				else
				{
					for (currState.EventTypes_Begin(); currState.EventTypes_HasNext(); currState.EventTypes_Next())
					{
						if (!ClearTable(TheQuery, currState.EventType_Name()))
						{
							throw("Could not clear the table, " + currState.EventType_Name()
						      		+ ", for database: " + Database + "\nMySQL message: " + TheQuery.error());
						}
					}
				}
			}
		}
	}
	catch (const exception &Err)
	{
		cerr << "ERROR: Exception caught: " << Err.what() << endl;
		ServerLink.close();
		return(5);
	}
	catch (const string &ErrorStr)
	{
		cerr << "ERROR: " << ErrorStr << endl;
		ServerLink.close();
		return(6);
	}
	catch (...)
	{
		cerr << "ERROR: Caught unknown exception..." << endl;
		ServerLink.close();
		return(7);
	}

	ServerLink.close();
	return(0);
}
