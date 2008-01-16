using namespace std;

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <cctype>				// for size_t
#include <algorithm>				// for max_element(), set_difference()

#include <StrUtly.h>				// for TakeDelimitedList(), StrToDouble(), DoubleToStr(), ValidNumber()
#include "Config/Configuration.h"
#include "Config/CAFEState.h"

#include <mysql++/mysql++.h>

#include "Utils/CAFEUtly.h"			// for GetGridInfo(). LoadClusterBoard()
#include "Utils/CAFE_SQLUtly.h"			// for LoadLonLatAnoms(), SaveLonLatAnoms(), SaveBoardToDatabase()

#include "SPAnalysis/ClusterConfig.h"
#include "SPAnalysis/BoardConvertor.h"		// for BoardConvertor class
#include "SPAnalysis/StrongPointAnalysis.h" 	// for the StrongPointAnalysis class

#include <CmdLineUtly.h>			// for ProcessFlatCommandLine()
#include "Utils/CAFE_CmdLine.h"			// for handling generic commandline options available to most programs in the CAFE system.
#include "Utils/Cluster_CmdLine.h"		// for handling command line options available for the clustering algorithm.

#include <FormatUtly.h>				// for Underline(), Bold()


#define DEBUG_VERBOSE_LEVEL 5


void PrintSyntax(const CmdOptions &CAFEOptions)
{
        cout << "\n";
        cout << "ClusterWork [--silent | --test | --fulltest] [--NOPAUSE | --pause]\n"
	     << "            [--SHOWBESTCLUST | --noshowbestclust] [--NOSHOWALLCLUST | --showallclust]\n"
	     << "            [--NOSHOWORIGGRID | --showoriggrid] [--SHOWINFO | --noshowinfo]\n"
	     << "            [--NOSAVE | --save]\n"
	     << "            [--stddev " << Underline("STDDEVVAL") << "] [--lowstddev " << Underline("LOWSTDDEVVAL") << "]\n"
	     << "            [--radius " << Underline("SEEKRADIUS") << "] [--touch " << Underline("TOUCHINGPARAM") << "]\n";
#ifdef _SAMPLINGRUN_
	cout << "	     --fold= " << Underline("FOLD_NUM") << "\n";
#endif

	CAFEOptions.PrintSyntax(11, 63);

	cout << "            [--help] [--syntax]\n\n";
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
	PrintSyntax(CAFEOptions);

	cout << "DESCRIPTION" << endl;

	cout << "\t\t  Silent mode:   Nothing is displayed to the screen (except error mesgs) and the clusters are saved to files\n";
	cout << "\t\t  Test mode:     Basic information is displayed about each field and the choosen cluster is displayed with information\n";
	cout << "\t\t                 No clusters are saved to files.\n";
	cout << "\t\t  FullTest mode: Lots of information is displayed about each field along with info on each cluster found in the grid\n";
	cout << "\t\t                 No clusters are saved to files.\n\n";
	
	cout << "\t\t  Any property of these different modes can be overridden by using the appropriate arguement AFTER choosing one of the modes\n";
	cout << "\t\t  Mode selection is not mandatory, and the default mode is the Test mode (without pauses).\n";
	cout << "\t\t  Default values are:\n";
	cout << "\t\t           StdDev = 1.85\n";
	cout << "\t\t           LowStdDev = 0.25\n";
	cout << "\t\t           Radius = 1 gridpoint(s)\n";
	cout << "\t\t           Touch = 2 gridpoint(s)\n";

	cout << endl;
	CAFEOptions.PrintDescription(63);
	cout << endl;
}



#ifdef _SAMPLINGRUN_
#include "Utils/SamplingUtly.h"		// for LoadCaseTimes(), RemoveCaseDates()
#endif




int main(int argc, char *argv[])
{
	// Working on eliminating this need.
	setenv("TZ", "UTC UTC", 1);

	vector <string> CommandArgs = ProcessFlatCommandLine(argc, argv);
	
	CmdOptions CAFEOptions;
	if (CAFEOptions.ParseCommandArgs(CommandArgs) != 0)
	{
		cerr << "ERROR: Invalid Command Line arguements...\n\n";
                PrintSyntax(CAFEOptions);
		return(8);
	}

	Cluster_CmdOptions ClusterOptions;
	if (ClusterOptions.ParseCommandArgs(CommandArgs) != 0)
	{
		cerr << "ERROR: Invalid Command Line arguements...\n\n";
		PrintSyntax(CAFEOptions);
		return(8);
	}

	
#ifdef _SAMPLINGRUN_
	int FoldNumber = -1;
#endif

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

	CAFEState currState( CAFEOptions.ConfigMerge( ConfigInfo.GiveCAFEInfo() ) );


	if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "Preparing the board...";}

        BoardConvertor ProjectionTranslator;

	// TODO: Must eliminate the use of ConfigInfo...
	// Must delete the projection when finished.
	const Projection_t* theProjection = ConfigInfo.Give_DataSource_Projection();
        if (!GetGridInfo(theProjection, currState.GetCAFEDomain(), ProjectionTranslator, ClusterOptions.SeekingRadius))
        {
                cerr << "ERROR: Could not obtain grad grid info..." << endl;
		delete theProjection;
                return(9);
	}

	delete theProjection;

	if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "done\n";}

        ClusterConfig MasterConfiguration;

	if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "Setting basic configurations...";}

        MasterConfiguration.SetRadius(ClusterOptions.SeekingRadius);
        MasterConfiguration.SetTouchingRule(ClusterOptions.TouchingParameter);
        MasterConfiguration.SetStdDeviationValue(ClusterOptions.StdDeviationValue);
        MasterConfiguration.SetLowStdDeviationValue(ClusterOptions.LowStdDeviationValue);
        MasterConfiguration.SetGridSize(ProjectionTranslator.Xsize(), ProjectionTranslator.Ysize());

	if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "done\n";}

        if (ClusterOptions.ShowInfo)
        {
		cout << "The projection translator configurations: " << endl;
                ProjectionTranslator.PrintConfigs();
        }


        const size_t GridCount = ProjectionTranslator.Xsize() * ProjectionTranslator.Ysize();


	mysqlpp::Connection ServerLink;
	mysqlpp::Connection ClustLink;

        try
        {
		// if I am not saving any of the clusters, why bother logging in using the password,
		// when I can log in using the read-only account that does not have a password.
		if (ClusterOptions.SaveBestCluster)
		{
			EstablishConnection(ClustLink, currState.GetServerName(), 
				    	    currState.GetLoginUserName(), "",
				    	    true);
		}
		else
		{
			EstablishConnection(ClustLink, currState.GetServerName(),
                                            currState.GetCAFEUserName(), "",
                                            false);
		}

		// Read-only
		EstablishConnection(ServerLink, currState.GetServerName(), currState.GetCAFEUserName(), "", false);
        }
        catch (const exception& Err)
        {
		ClustLink.close();
		ServerLink.close();
                cerr << "ERROR: Connection failed: " << Err.what() << endl;
		cerr << "No changes were made..." << endl;
                return(3);
        }
	catch (const string &ErrStr)
	{
		ClustLink.close();
                ServerLink.close();
                cerr << "ERROR: Connection failed: " << ErrStr << endl;
                cerr << "No changes were made..." << endl;
                return(3);
	}
	catch (...)
	{
		ClustLink.close();
		ServerLink.close();
		cerr << "ERROR: Unknown exception caught while trying to link to the server." << endl;
		cerr << "No changes were made..." << endl;
		return(4);
	}

	try
	{
		if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "Going into the mega for loops..." << endl;}
		for (currState.TimePeriods_Begin(); currState.TimePeriods_HasNext(); currState.TimePeriods_Next())
		{
		
			const string Database = currState.Untrained_Name();
			const string ClustDatabase = currState.Trained_Name();

			if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL - 1) {cout << "\tDatabase: " << Database << endl;}

			if (!ServerLink.select_db(Database))
                        {
                                throw("Could not select the database: " + Database + "\nMySQL message: " + ServerLink.error());
                        }

			if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "UntrainedDatabase selected...\n";}

			if (!ClustLink.select_db(ClustDatabase))
			{
				throw("Could not select the database: " + ClustDatabase + "\nMySQL message: " + ClustLink.error());
			}

			if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "TrainedDatabase selected...\n";}


                        mysqlpp::Query EventCntQuery = MakeLoader_EventCnt(ServerLink);
			mysqlpp::Query LonLatAnomDatesQuery = MakeLoader_LonLatAnomDates(ServerLink);
			mysqlpp::Query ClustLonLatAnomDatesQuery = MakeSaver_LonLatAnomDate(ClustLink);
			mysqlpp::Query AlphaPhiQuery = MakeSaver_AlphaPhiValues(ClustLink);

			if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) cerr << "Queries created...\n";
			

			for (currState.EventTypes_Begin(); currState.EventTypes_HasNext(); currState.EventTypes_Next())
			{
				if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) cerr << "EventType: " << currState.EventType_Name() << endl;

				EventCntQuery.def["table"] 		= 
				LonLatAnomDatesQuery.def["table"] 	= 
				ClustLonLatAnomDatesQuery.def["table"] 	= 
				AlphaPhiQuery.def["table"] 		= currState.EventType_Name();

//------------------------------------------------------------------------------------------------------------------------------------
#ifndef _SAMPLINGRUN_
				const size_t OccurranceCnt = LoadEventCnt(EventCntQuery, currState.EventType_Name());
#else

				size_t OccurranceCnt = LoadEventCnt(EventCntQuery, currState.EventType_Name());

				if (OccurranceCnt < 10)
				{
					cerr << "Not enough events for this eventtype: " << currState.EventType_Name() << "  database: " << Database << endl;
					cerr << "Moving on to the next event type..." << endl;
					continue;
				}

				string FoldName = GetCaseFilename(currState.GetCAFEPath(), Database, currState.EventType_Name(), FoldNumber);
				vector <time_t> FoldDates = LoadCaseTimes(FoldName);
				OccurranceCnt -= FoldDates.size();
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------


				for (currState.EventVars_Begin(); currState.EventVars_HasNext(); currState.EventVars_Next())
				{
					for (currState.EventLevels_Begin(); currState.EventLevels_HasNext(); currState.EventLevels_Next())
					{
						for (currState.Extrema_Begin(); currState.Extrema_HasNext(); currState.Extrema_Next())
						{
							ClusterBoard freshBoard(MasterConfiguration);

							if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "Loading LonLatAnoms data...";}

							vector<LonLatAnomDate> TheMembers = LoadLonLatAnomDates(LonLatAnomDatesQuery, 
														currState.FieldExtremum_Name());

							if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "done\n";}
//---------------------------------------------------------------------------------------------------
#ifdef _SAMPLINGRUN_
							RemoveCaseDates(TheMembers, FoldDates);
#endif
//---------------------------------------------------------------------------------------------------

							if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "Loading cluster board...";}

							if (!LoadClusterBoard(freshBoard, TheMembers, ProjectionTranslator))
							{
								throw("Could not load the Cluster board: " + Database + " EventType: " 
								      + currState.EventType_Name() + "  field: " + currState.FieldExtremum_Name());
							}

							if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "done\n";}
					
							if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "Analyzing board...";}

							freshBoard.AnalyzeBoard();
							
							if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "done\n";}

							if (ClusterOptions.ShowMap)
							{
								freshBoard.PrintBoard();
							}


							if (ClusterOptions.ShowInfo)
							{
								cout << "\nDatabase: " << Database << "  EventType: " << currState.EventType_Name()
								     << "   field: " << currState.FieldExtremum_Name() << endl;
								cout << "OccurranceCount: " << OccurranceCnt << endl;
						
								freshBoard.myBoardConfig.PrintConfiguration();
							}

							StrongPointAnalysis AnalysisStuff;

							if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "Receiving unclustered grid...";}

							AnalysisStuff.ReceiveUnclusteredGrid(freshBoard);

							if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "done.\n";}
							if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "Clustering the grid...";}

							AnalysisStuff.ClusterTheGrid();

							if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "done.\n";}
						
							const size_t ClusterCount = AnalysisStuff.NumberOfClusters();

							if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "Padding clusters...";}

							vector <double> AlphaVals(ClusterCount, 0.0);
							vector <double> PhiVals(ClusterCount, 0.0);
							vector <double> TheSums(ClusterCount, 0.0);
							for (size_t index = 0; index < ClusterCount; index++)
							{
								AnalysisStuff.PadCluster(index);
								AlphaVals[index] = AnalysisStuff.CalcAlpha(index, OccurranceCnt);
								PhiVals[index] = AnalysisStuff.CalcPhi(index, OccurranceCnt);
								TheSums[index] = AlphaVals[index] + PhiVals[index];

								if (ClusterOptions.ShowAllClusters)
								{
									cout << "\nCluster #" << index << "   Alpha: " << AlphaVals[index] 
									     << "   Phi: " << PhiVals[index];
							
									AnalysisStuff.PrintCluster(index);
								}
							}
							if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "done\n";}

							if (ClusterOptions.ShowAllClusters)
							{
								cout << "\n";
							}


							const size_t BestClusterIndex = max_element(TheSums.begin(), TheSums.end()) - TheSums.begin();
							const ClusterBoard TheCluster = AnalysisStuff.ReturnCluster(BestClusterIndex);
					
							if (ClusterOptions.ShowBestCluster)
							{
								cout << "Alpha: " << AlphaVals[BestClusterIndex] << "  Phi: " << PhiVals[BestClusterIndex]
								     << "   Consistency Coefficient: " 
								     << ((double) TheCluster.TotalMemberCount()) / ((double) OccurranceCnt);
								TheCluster.PrintBoard();
								cout << "Cluster Member Count: " << TheCluster.TotalMemberCount() << endl << endl;

								if (ClusterOptions.DoPause)
								{
									string dummy;
									cout << "Enter a character and press enter..." << endl;
									getline(cin, dummy);
								}
							}

												
							if (ClusterOptions.SaveBestCluster)
							{
								if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "Saving cluster...";}

								SaveBoardToDatabase(TheCluster, ClustLonLatAnomDatesQuery, 
										    currState.FieldExtremum_Name(), ProjectionTranslator);

								if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "done\n";}
								if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "Saving AlphaPhi...";}

								SaveAlphaPhiValues(AlphaPhiQuery, currState.FieldExtremum_Name(),
										   AlphaVals[BestClusterIndex], PhiVals[BestClusterIndex]);
								
								if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "done\n";}
								if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "Tidy up database...";}
								// This is very important because it prevents previous runs from
								// contaminating the correlation calculations later.
								// This is done by setting all the information for those unused dates in the cluster
								// database to nulls (or NANs).

								// The dates come in ascending order.
								vector<time_t> ClusteredDates = GiveClusteredDates(TheCluster, 
														   ProjectionTranslator);
								vector<time_t> OrigDates = SplitIntoTime(TheMembers);
								sort(OrigDates.begin(), OrigDates.end());

								vector<time_t> UnusedDates(OrigDates.size() - ClusteredDates.size());

								if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "set difference...";}

								set_difference(OrigDates.begin(), OrigDates.end(),
									       ClusteredDates.begin(), ClusteredDates.end(),
									       UnusedDates.begin());

								
								const vector<double> TempHold(UnusedDates.size(), NAN);

								if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "saving...";}

								SaveLonLatAnoms(TempHold, TempHold, TempHold, UnusedDates,
										ClustLonLatAnomDatesQuery, currState.FieldExtremum_Name());

								if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "done\n";}
//---------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _SAMPLINGRUN_
								// Remember, I removed dates from the original list of members,
								// so those dates wouldn't have been cleaned up.
								if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "Clean up old folds...";}
								const vector<double> TempFoldHold(FoldDates.size(), NAN);
								SaveLonLatAnoms(TempFoldHold, TempFoldHold, TempFoldHold, FoldDates, 
										ClustLonLatAnomDatesQuery, currState.FieldExtremum_Name());
								if (currState.GetVerboseLevel() >= DEBUG_VERBOSE_LEVEL) {cerr << "done\n";}
#endif
//----------------------------------------------------------------------------------------------------------------------------------------------
							}// end if (SaveToFiles)
						}// end extremum loop
					}// end label loop
				}// end CAFEVar loop
			}// end event type loop
		}// end database loop
	}
	catch (const exception &Err)
        {
                cerr << "ERROR: Something went wrong: " << Err.what() << endl;
                ServerLink.close();
		ClustLink.close();
                return(5);
        }
	catch (const string &ErrStr)
	{
		cerr << "ERROR: " << ErrStr << endl;
		ServerLink.close();
		ClustLink.close();
		return(6);
	}
	catch (...)
	{
		cerr << "ERROR: Unknown exception caught..." << endl;
		ServerLink.close();
		ClustLink.close();
		return(7);
	}


        ServerLink.close();
	ClustLink.close();
	cout << "\nAll done!\n\n";
	return(0);
}

