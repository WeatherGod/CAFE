using namespace std;

#include <iostream>
#include <vector>
#include <set>
#include <cctype>					// for size_t
#include <string>

#include "Config/Configuration.h"
#include "Config/CAFEState.h"
#include "Utils/CAFEUtly.h"					// for WriteLonLatAnoms(), WriteFieldMeasure()
#include "Utils/CAFE_SQLUtly.h"				// for LoadLonLatAnoms(), LoadFieldMeasValues(), EstablishConnection()
#include "Utils/CAFE_CmdLine.h"				// for generic CAFE command-line options support
#include <StrUtly.h>					// for GiveDelimitedList()
#include <CmdLineUtly.h>				// for ProcessFlatCommandLine()
#include <FormatUtly.h>					// for Bold(), Underline()

#include <mysql++/mysql++.h>

void PrintSyntax(const CmdOptions &CAFEOptions)
{
        cout << "\nLonLatAnomExtract " << Bold("[--help] [--syntax]") << endl;
	cout << endl;

        CAFEOptions.PrintSyntax(17, 63);

        cout << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
        PrintSyntax(CAFEOptions);

        cout << "Collects the longitudes, latitudes and standard anomaly values of the extremum and also the field measures," << endl;
	cout << "and puts them into files for some backwards compatibility issues.\n\n";

        cout << endl << Bold("DESCRIPTION") << endl;

        CAFEOptions.PrintDescription(63);

        cout << endl;
}


void DoDatabaseExtraction(mysqlpp::Connection &ServerLink, CAFEState &currState, 
			  const bool &IsProcessedDatabases)
{
	const set<string> DatabaseNames = (IsProcessedDatabases ? currState.Trained_Names()
								: currState.Untrained_Names());
	for (set<string>::const_iterator ADatabaseName = DatabaseNames.begin();
             ADatabaseName != DatabaseNames.end();
             ADatabaseName++)
        {
	        if (!ServerLink.select_db(*ADatabaseName))
                {
                        throw("Could not select the database: " + *ADatabaseName + "\nMySQL message: " + ServerLink.error());
                }

		if (system(("mkdir --parents '" + currState.GetCAFEPath() + "/AnalysisInfo/" + *ADatabaseName + "'").c_str()) != 0)
		{
			cerr << "WARNING: Trouble while trying to make directory " << currState.GetCAFEPath() + "/AnalysisInfo/" + *ADatabaseName << endl
			     << "       : You may have difficulty trying to save the LonLatAnom and FieldMeasure files.\n";
		}

                mysqlpp::Query TheQuery = MakeLoader_LonLatAnoms(ServerLink);
		mysqlpp::Query FieldMeasQuery = MakeLoader_FieldMeasValues(ServerLink);

                for (currState.EventTypes_Begin(); currState.EventTypes_HasNext(); currState.EventTypes_Next())
                {
			TheQuery.def["table"] =
			FieldMeasQuery.def["table"] = currState.EventType_Name();

                        for (currState.EventVars_Begin(); currState.EventVars_HasNext(); currState.EventVars_Next())
                        {
				for (currState.EventLevels_Begin(); currState.EventLevels_HasNext(); currState.EventLevels_Next())
                                {
					for (currState.Extrema_Begin(); currState.Extrema_HasNext(); currState.Extrema_Next())
					{
                                	        const string FieldName = currState.FieldExtremum_Name();

		                                // This is just a kludge for now as I deal with the transition from flat files to mysql.
						if (IsProcessedDatabases)
                		                {
							double AlphaVal, PhiVal, GammaMax, ChiMax;

							LoadFieldMeasValues(FieldMeasQuery, FieldName, currState.EventType_Name(),
									    AlphaVal, PhiVal, GammaMax, ChiMax);

							string FieldMeasFile = currState.GetCAFEPath() + "/AnalysisInfo/" + *ADatabaseName
									       + '/' + currState.EventType_Name() + '_' + FieldName + ".FieldMeasure";

							if (!WriteFieldMeasure(AlphaVal, PhiVal, GammaMax, ChiMax, FieldMeasFile))
							{
								throw("Could not write the field measure file: " + FieldMeasFile);
							}
						}
											


                                                const string QueryOutput = currState.GetCAFEPath() + "/AnalysisInfo/" + *ADatabaseName + '/'
                                                                           + currState.EventType_Name() + '_' + FieldName + ".lonlatanom";

                                                const vector<LonLatAnom> TheMembers( LoadLonLatAnoms(TheQuery, FieldName) );

                                                if (!WriteLonLatAnoms(TheMembers, QueryOutput))
                                                {
	                                               	throw("Could not write the lonlatanom file: " + QueryOutput);
                                                }
                                        }// end extrema loop

                                }// end CAFE label loop
                        }// end CAFEVar names loop
                }// end Table (event type) loop
	}// end database loop
}




int main(int argc, char *argv[])
{
	vector<string> CommandArgs = ProcessFlatCommandLine(argc, argv);
	CmdOptions CAFEOptions;

	if (CAFEOptions.ParseCommandArgs(CommandArgs) != 0)
	{
		cerr << "ERROR: Invalid syntax..." << endl;
		return(8);
	}

	for (size_t ArgIndex = 0; ArgIndex < CommandArgs.size(); ArgIndex++)
	{
		if (CommandArgs[ArgIndex] == "--help")
		{
			PrintHelp(CAFEOptions);
			return(2);
		}
		else if (CommandArgs[ArgIndex] == "--syntax")
		{
			PrintSyntax(CAFEOptions);
			return(2);
		}
		else
		{
			cerr << "ERROR: Unknown arguement: " << CommandArgs[ArgIndex] << endl;
			PrintSyntax(CAFEOptions);
			return(8);
		}
	}

	Configuration ConfigInfo(CAFEOptions.CAFEPath + '/' + CAFEOptions.ConfigFilename);             // loads configurations

	if (!ConfigInfo.IsValid())
	{
		cerr << "Error: The configuration file is not valid: " << CAFEOptions.CAFEPath << '/' << CAFEOptions.ConfigFilename << endl;
		return(1);
	}

	if (!CAFEOptions.MergeWithConfiguration(ConfigInfo))
        {
                cerr << "ERROR: Conflicts in the command line..." << endl;
                return(8);
        }

	CAFEState currState( CAFEOptions.ConfigMerge( ConfigInfo.GiveCAFEInfo() ) );


	mysqlpp::Connection ServerLink;

        try
        {
		EstablishConnection(ServerLink, currState.GetServerName(), currState.GetCAFEUserName(), "", false);
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
		cerr << "Unknown exception caught..." << endl;
		ServerLink.close();
		return(4);
	}


	try
	{
		DoDatabaseExtraction(ServerLink, currState, false);
		DoDatabaseExtraction(ServerLink, currState, true);
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
