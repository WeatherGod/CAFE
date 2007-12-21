using namespace std;

#include <iostream>
#include <vector>
#include <cctype>					// for size_t
#include <string>

#include "Config/Configuration.h"
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
	cout << "\t\t\tUSE THIS PROGRAM TO HELP DEAL WITH BACKWARDS COMPATIBILITY ISSUES!" << endl;
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


void DoDatabaseExtraction(mysqlpp::Connection &ServerLink, const Configuration &ConfigInfo, const CmdOptions &CAFEOptions, 
			  const vector <string> &DatabaseNames, const bool &IsProcessedDatabases)
{
	for (vector<string>::const_iterator ADatabaseName = DatabaseNames.begin();
             ADatabaseName != DatabaseNames.end();
             ADatabaseName++)
        {
	        if (!ServerLink.select_db(*ADatabaseName))
                {
                        throw("Could not select the database: " + *ADatabaseName + "\nMySQL message: " + ServerLink.error());
                }

		if (system(("mkdir --parents '" + CAFEOptions.CAFEPath + "/AnalysisInfo/" + *ADatabaseName + "'").c_str()) != 0)
		{
			cerr << "WARNING: Trouble while trying to make directory " << CAFEOptions.CAFEPath + "/AnalysisInfo/" + *ADatabaseName << endl
			     << "       : You may have difficulty trying to save the LonLatAnom and FieldMeasure files.\n";
		}

                mysqlpp::Query TheQuery = MakeLoader_LonLatAnoms(ServerLink);
		mysqlpp::Query FieldMeasQuery = MakeLoader_FieldMeasValues(ServerLink);

                for (vector<string>::const_iterator EventTypeName = CAFEOptions.EventTypes.begin();
                     EventTypeName != CAFEOptions.EventTypes.end();
                     EventTypeName++)
                {
			TheQuery.def["table"] = *EventTypeName;
			FieldMeasQuery.def["table"] = *EventTypeName;

        	        const vector <string> VarNames = CAFEOptions.GiveCAFEVarsToDo(ConfigInfo, *EventTypeName);
                        for (vector<string>::const_iterator AVarName = VarNames.begin(); AVarName != VarNames.end(); AVarName++)
                        {
                	        const vector <string> CAFELabels = CAFEOptions.GiveLabelsToDo(ConfigInfo, *EventTypeName, *AVarName);

                                for (vector<string>::const_iterator ALabel = CAFELabels.begin(); ALabel != CAFELabels.end(); ALabel++)
                                {
                        	        for (size_t PeakValIndex = 0; PeakValIndex < ConfigInfo.ExtremaCount(); PeakValIndex++)
                                        {
                                	        const string FieldName = *ALabel + '_' + ConfigInfo.GiveExtremaName(PeakValIndex);

		                                // This is just a kludge for now as I deal with the transition from flat files to mysql.
						if (IsProcessedDatabases)
                		                {
							double AlphaVal, PhiVal, GammaMax, ChiMax;

							LoadFieldMeasValues(FieldMeasQuery, FieldName, *EventTypeName,
										AlphaVal, PhiVal, GammaMax, ChiMax);

							string FieldMeasFile = CAFEOptions.CAFEPath + "/AnalysisInfo/" + *ADatabaseName
									       + '/' + *EventTypeName + '_' + FieldName + ".FieldMeasure";

							if (!WriteFieldMeasure(AlphaVal, PhiVal, GammaMax, ChiMax, FieldMeasFile))
							{
								throw("Could not write the field measure file: " + FieldMeasFile);
							}
						}
											


                                                const string QueryOutput = CAFEOptions.CAFEPath + "/AnalysisInfo/" + *ADatabaseName + '/'
                                                                            + *EventTypeName + '_' + FieldName + ".lonlatanom";

                                                const vector <LonLatAnom> TheMembers( LoadLonLatAnoms(TheQuery, FieldName) );

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
	vector <string> CommandArgs = ProcessFlatCommandLine(argc, argv);
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
		DoDatabaseExtraction(ServerLink, ConfigInfo, CAFEOptions, CAFEOptions.DatabaseNames, false);
		DoDatabaseExtraction(ServerLink, ConfigInfo, CAFEOptions, CAFEOptions.ClustDatabaseNames, true);
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
