using namespace std;

#include <iostream>
#include <set>
#include <string>

#include <mysql++/mysql++.h>

#include "Config/Configuration.h"
#include "Utils/CAFE_SQLUtly.h"		// for EstablishConnection(), GetDatabaseNames()
#include "Utils/CAFE_CmdLine.h"
#include "Config/CAFEParam.h"
#include <CmdLineUtly.h>		// for ProcessFlatCommandLine()


void
CAFEsqlSetup(const CAFEParam &cafeInfo,
             const bool createDatabases, const bool updateCAFEUser, const bool updateLoginUser,
             mysqlpp::Connection &serverLink)
{
	const set<string> untrainedDatabases = cafeInfo.GetUntrainedNames();
	const set<string> trainedDatabases = cafeInfo.GetTrainedNames();

	mysqlpp::Query theQuery = serverLink.query();

	const set<string> sqlDatabases = GetDatabaseNames(theQuery);

	for (set<string>::const_iterator anUntrainedDatabase(untrainedDatabases.begin()),
					 aTrainedDatabase(trainedDatabases.begin());
	     anUntrainedDatabase != untrainedDatabases.end() && aTrainedDatabase != trainedDatabases.end();
	     anUntrainedDatabase++, aTrainedDatabase++)
	{
		if (createDatabases)
		{
			// check current list of databases, see if they exist already.
			// throw an error if one does.
			if (sqlDatabases.find(*anUntrainedDatabase) != sqlDatabases.end())
			{
				throw("Cannot create database, it already exists: " + *anUntrainedDatabase);
			}

			if (sqlDatabases.find(*aTrainedDatabase) != sqlDatabases.end())
			{
				throw("Cannot create database, it already exists: " + *aTrainedDatabase);
			}
		
			// Check first, then create databases.
			if (!serverLink.create_db(*anUntrainedDatabase))
			{
				throw("Could not create database: " + *anUntrainedDatabase + "\nMySQL message: " + serverLink.error());
			}

			cout << *anUntrainedDatabase << " created!" << endl;

			if (!serverLink.create_db(*aTrainedDatabase))
			{
				throw("Could not create database: " + *aTrainedDatabase + "\nMySQL message: " + serverLink.error());
			}

			cout << *aTrainedDatabase << " created!" << endl;
		}
		else
		{
			// Check to see that the databases already exists.
			// If not, throw an error.
			if (sqlDatabases.find(*anUntrainedDatabase) == sqlDatabases.end())
			{
				throw("Missing database: " + *anUntrainedDatabase);
			}

			if (sqlDatabases.find(*aTrainedDatabase) == sqlDatabases.end())
			{
				throw("Missing database: " + *aTrainedDatabase);
			}
		}

		// Now that we know that both *anUntrainedDatabase and *aTrainedDatabase exists,
		// Go ahead and attempt to set privileges, if desired.
		if (updateCAFEUser)
		{
			theQuery << "GRANT SELECT ON '" 
				 << mysqlpp::escape << *anUntrainedDatabase 
				 << "'.* TO '" 
				 << mysqlpp::escape << cafeInfo.GetCAFEUserName() << "'@'%'";

			cout << theQuery.preview() << endl;

			theQuery.execute();

			theQuery << "GRANT SELECT ON '" 
				 << mysqlpp::escape << *aTrainedDatabase
				 << "'.* TO '"
				 << mysqlpp::escape << cafeInfo.GetCAFEUserName() << "'@'%'";

			cout << theQuery.preview() << endl;

			theQuery.execute();
		}

		// Now for the Login user.  He should get ownership of the databases.
		if (updateLoginUser)
		{
			theQuery << "GRANT ALL ON '"
				 << mysqlpp::escape << *anUntrainedDatabase
				 << "'.* TO '"
				 << mysqlpp::escape << cafeInfo.GetLoginUserName() << "'@'%'";

			cout << theQuery.preview() << endl;

			theQuery.execute();

			theQuery << "GRANT ALL ON '"
			         << mysqlpp::escape << *aTrainedDatabase
			         << "'.* TO '"
			         << mysqlpp::escape << cafeInfo.GetLoginUserName() << "'@'%'";

			cout << theQuery.preview() << endl;

			theQuery.execute();
		}
	}
}

void PrintSyntax(const CmdOptions &CAFEOptions)
{
	cout << "\nCAFEsqlSetup [--create_databases] [--update_users]\n"
	     << "               [--update_cafe_user] [--update_login_user]\n"
	     << "               [--help] [--syntax]\n";
	CAFEOptions.PrintSyntax(13, 63);
	cout << "\n\n";
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
	PrintSyntax(CAFEOptions);

	cout << "DESCRIPTION" << endl;

	cout << "\t\t Attempts to connect to the mysql server to create needed databases\n"
	     << "\t\t and updates any user privileges as well.\n\n";

	cout << "\t\t User can specify to create databases (--databases) which would,\n"
	     << "\t\t in turn, update the privileges for the CAFEUser and the login user.\n"
	     << "\t\t User could also just update privileges (--update_users).\n\n";

	cout << "\t\t For just updating the login user, one can use (--update_login_user).\n"
	     << "\t\t For just updating the CAFEUser, one can use (--update_cafe_user).\n"
	     << "\t\t These two switches also effect the behavior --databases switch, similarly.\n\n";
}




int main(int argc, char* argv[])
{
	vector<string> commandArgs = ProcessFlatCommandLine(argc, argv);

	CmdOptions cafeOptions;
	if (cafeOptions.ParseCommandArgs(commandArgs) != 0)
	{
		cerr << "ERROR: Invalid command line arguments...\n\n";
		PrintSyntax(cafeOptions);
		return(EXIT_FAILURE);
	}

	bool createDatabases = false;
	bool updateCAFEUser = false;
	bool updateLoginUser = false;

	for (size_t argIndex = 0; argIndex < commandArgs.size(); argIndex++)
	{
		if (commandArgs[argIndex].find("--help") == 0)
		{
			PrintHelp(cafeOptions);
			return(EXIT_SUCCESS);
		}
		else if (commandArgs[argIndex].find("--syntax") == 0)
		{
			PrintHelp(cafeOptions);
			return(EXIT_SUCCESS);
		}
		else if (commandArgs[argIndex] == "--create_databases")
		{
			createDatabases = true;
		}
		else if (commandArgs[argIndex] == "--update_users")
		{
			updateCAFEUser = true;
			updateLoginUser = true;
		}
		else if (commandArgs[argIndex] == "--update_cafe_user")
		{
			updateCAFEUser = true;
		}
		else if (commandArgs[argIndex] == "--update_login_user")
		{
			updateLoginUser = true;
		}
		else
		{
			cerr << "ERROR: Invalid command line argument: " << commandArgs[argIndex] << "\n\n";
			PrintSyntax(cafeOptions);
			return(EXIT_FAILURE);
		}
	}

	if (createDatabases && !updateCAFEUser && !updateLoginUser)
	{
		// Only --create_databases specified on command-line.
		// So default to updating both CAFEUser and LoginUser.
		updateCAFEUser = true;
		updateLoginUser = true;
	}

	Configuration configInfo(cafeOptions.CAFEPath + '/' + cafeOptions.ConfigFilename);

	if (!configInfo.IsValid())
	{
		cerr << "ERROR: Invalid configuration file: " << cafeOptions.CAFEPath << '/' << cafeOptions.ConfigFilename << endl;
		return(EXIT_FAILURE);
	}

	if (!cafeOptions.MergeWithConfiguration(configInfo))
	{
		cerr << "ERROR: Conflicts in the command line with the configuration file..." << endl;
		return(EXIT_FAILURE);
	}

	const CAFEParam cafeInfo = cafeOptions.ConfigMerge( configInfo.GiveCAFEInfo() );

	string adminUserName = "";		// username to do admin tasks.

	cout << "Login Name: ";
	getline(cin, adminUserName);

	mysqlpp::Connection serverLink;

	try
	{
		EstablishConnection(serverLink, cafeInfo.GetServerName(), adminUserName, "", true);
	}
	catch (exception &Err)
	{
		serverLink.close();
		cerr << "ERROR: Connection failed: " << Err.what() << endl;
		cerr << "No changes were made..." << endl;
		return(EXIT_FAILURE);
	}
	catch (const string &ErrStr)
	{
		serverLink.close();
		cerr << "ERROR: Connection failed: " << ErrStr << endl;
		cerr << "No changes were made..." << endl;
		return(EXIT_FAILURE);
	}
	catch (...)
	{
		serverLink.close();
		cerr << "ERROR: Connection failed due to unknown reason..." << endl;
		cerr << "No changes were made..." << endl;
		return(EXIT_FAILURE);
	}


	try
	{
		CAFEsqlSetup(cafeInfo, 
			     createDatabases, updateCAFEUser, updateLoginUser,
			     serverLink);
	}
	catch (exception &Err)
	{
		cerr << "ERROR: Exception caught! " << Err.what() << endl;
		serverLink.close();
		return(EXIT_FAILURE);
	}
	catch (string &ErrStr)
	{
		cerr << "ERROR: Exception caught! " << ErrStr << endl;
		serverLink.close();
		return(EXIT_FAILURE);
	}
	catch (...)
	{
		cerr << "ERROR: Unknown exception caught! " << endl;
		serverLink.close();
		return(EXIT_FAILURE);
	}


	serverLink.close();
	return(EXIT_SUCCESS);
}
