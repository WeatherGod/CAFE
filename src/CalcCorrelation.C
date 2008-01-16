using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>
#include <mysql++/mysql++.h>

#include <StrUtly.h>		// for DoubleToStr(), GiveDelimitedList()
#include "Config/Configuration.h"
#include "Config/CAFEState.h"
#include "Utils/CAFEEqtns.h"		// for all the main CAFE equations
#include "Utils/CAFEUtly.h"		// for LoadForecast(), SaveEventScores(), TimePeriodToOffset()
#include "Utils/CAFE_SQLUtly.h"	// for EstablishConnection()
#include <Histogram.h>

#include <TimeUtly.h>		// for GetTime(), GiveTime()

#include <CmdLineUtly.h>	// for ProcessFlatCommandLine()
#include "Utils/CAFE_CmdLine.h"	// for generic CAFE command line support
#include <FormatUtly.h>                 // for making things look nice and pretty

#define _PROG_NAME              ((string) "CalcCorrelation")


void Calc_Event_GammaChi(const vector <LonLatAnom> &TheMembers, const LonLatAnom &TheForecastField, const vector <double> &ClustStdAnoms,
			 double &Gamma, double &Chi)
{
	Gamma = CalcGamma_Opt(GetMemberCounts(TheMembers), TheForecastField.Lon, TheForecastField.Lat);
//	Gamma = CalcGamma(TheMembers.begin(), TheMembers.end(), TheForecastField.Lon, TheForecastField.Lat);

	if (ClustStdAnoms.empty())
	{
		// Histogram goes nuts if it tries to contruct a histogram from an empty vector
		// which causes errors for CalcChi.
		// I will have to work on the Histogram code so that an empty vector of values is dealt with better.
		Chi = 0.0;
	}
	else
	{
        	Chi = CalcChi( Histogram(ClustStdAnoms) , TheForecastField.StdAnom);
	}
}

double Calc_Event_Field(const vector <LonLatAnom> &TheMembers, const LonLatAnom &TheForecastField, 
		        const double &Alpha, const double &Phi, const double &MaxGamma, const double &MaxChi,
		        double &Gamma, double &Chi)
{
	if (!TheMembers.empty())
        {
        	// -----------------------------------------------------
                // Temporary thing, for now...
                vector <double> ClustStdAnoms( TheMembers.size() );
                vector<LonLatAnom>::const_iterator AMember( TheMembers.begin() );
                for (vector<double>::iterator AnAnom( ClustStdAnoms.begin() ); AnAnom != ClustStdAnoms.end();
                     AnAnom++, AMember++)
                {
                	*AnAnom = AMember->StdAnom;
                }
                // -----------------------------------------------------------

                if (isnan(TheForecastField.StdAnom))
                {
                	Gamma = Chi = 0.0;
			return(0.0);
                }
                else
                {
                        Calc_Event_GammaChi(TheMembers, TheForecastField, ClustStdAnoms, Gamma, Chi);

//                      if (isnan(Gamma) || isnan(Chi))
//                      {
//                      	cerr << "Warning: nan found for " << FullLabel << ".  Continuing..." << endl;
//                      }

                        return(CalcUpsilon(Gamma, Chi, MaxGamma, MaxChi, Alpha, Phi));
		}
	}
	else
        {
		Gamma = Chi = 0.0;
	        return(0.0);
        }
}

/*
vector <double> Calc_Event_Upsilons(mysqlpp::Query &LonLatAnomQuery, mysqlpp::Query &FieldMeasQuery, const string &EventTypeName, 
				    map <string, LonLatAnom> &TheFields, const string &ForecastDir, const string &DateOfEvent,
				    CAFEState &currState)
{
	LonLatAnomQuery.def["table"] = EventTypeName;
        FieldMeasQuery.def["table"] = EventTypeName;

	vector <double> Upsilons(0);

        for (currState.EventVars_Begin(); currState.EventVars_HasNext(); currState.EventVars_Next())
        {
                for (currState.EventLevels_Begin(); currState.EventLevels_HasNext(); currState.EventLevels_Next())
                {
        	        for (currState.Extrema_Begin(); currState.Extrema_HasNext(); currState.Extrema_Next())
                        {
                	        const string FullLabel = currState.FieldExtremum_Name();
                                const vector <LonLatAnom> TheMembers( LoadLonLatAnoms(LonLatAnomQuery, FullLabel) );

				if (!TheMembers.empty())
				{
                         		double Alpha, Phi, MaxGamma, MaxChi, Gamma, Chi;

                                        map<string, LonLatAnom>::const_iterator FieldMatch( TheFields.find(FullLabel) );
                                        if (FieldMatch == TheFields.end())
                                        {
                                        	const string ForecastFile = ForecastDir + FullLabel + ".field";
                                                //The insert function returns a pair type of an iterator and a bool.
                                                // This next line does the insert and grabs the iterator using .first
                                                FieldMatch = (TheFields.insert(make_pair(FullLabel,
                                                                                         LoadForecast(ForecastDir + FullLabel + ".field",
												      DateOfEvent)
											)
									      )
                                                             ).first;
                                       	}

					LoadFieldMeasValues(FieldMeasQuery, FullLabel, Alpha, Phi, MaxGamma, MaxChi);

					Upsilons.push_back( Calc_Event_Field(TheMembers, FieldMatch->second,
                        						     Alpha, Phi, MaxGamma, MaxChi,
									     Gamma, Chi)  );
				}
                                else
                                {
                                	Upsilons.push_back(0.0);
                                }
                        }// end extrema loop
		}// end label loop
	}// end CAFEVar Name loop

	return(Upsilons);
}
*/

// For comparing between two different types.
// TODO: Probably should just type-cast where I use it.
double GiveMax(const double &Val1, const float &Val2)
{
	if (Val1 > Val2)
	{
		return(Val1);
	}

	return((double) Val2);
}

void OutputResults(const string &TableName, const double &EventScore, const vector <double> &Upsilons,
		   mysqlpp::Query AQuery, const vector <double> &Gammas, const vector <double> &Chis)
{
	AQuery << "SELECT FieldName, Alpha, Phi, Gamma_Max, Chi_Max FROM " + TableName + "_FieldMeas";
	mysqlpp::Result TableResult = AQuery.store();
	cout << TableName << ":  " << EventScore;

	vector <size_t> Sizes(TableResult.size());
	size_t MaxNameSize = 0;
	mysqlpp::Result::iterator ARow( TableResult.begin() );

	for (vector<size_t>::iterator ASize( Sizes.begin() ); ASize != Sizes.end(); ASize++, ARow++)
	{
		*ASize = ((*ARow)["FieldName"].get_string()).length();
		MaxNameSize = (MaxNameSize < *ASize) ? *ASize : MaxNameSize;
	}

	int ValueWidth = 10;
	int ValuePrecision = 4;
	double MaxUpsilon = 0.0;
	double MaxAlpha = 0.0;
	double MaxPhi = 0.0;
	double MaxMaxPossibleGamma = 0.0;
	double MaxMaxPossibleChi = 0.0;

	double Max_of_Gammas = 0.0;
	double Max_of_Chis = 0.0;

	double Max_SpatialComp = 0.0;
	double Max_AnomComp = 0.0;

	double SpatialSum = 0.0;
	double StdAnomSum = 0.0;

	cout << endl;
	cout.width(MaxNameSize + 2);
	cout << "Field Names";
	cout << setw(ValueWidth) << "Alpha";
	cout << setw(ValueWidth) << "Phi";
	cout << setw(ValueWidth) << "MaxGamma";
	cout << setw(ValueWidth) << "MaxChi";
	cout << setw(ValueWidth) << "Gamma";
	cout << setw(ValueWidth) << "Chi";
	cout << setw(ValueWidth) << "Spatial";
	cout << setw(ValueWidth) << "Anom";
	cout << setw(ValueWidth) << "Upsilon" << endl;

	vector<double>::const_iterator AGamma( Gammas.begin() ), AChi( Chis.begin() );

	vector<double>::const_iterator AnUpsilon( Upsilons.begin() );
	for (ARow = TableResult.begin(); ARow != TableResult.end(); ARow++, AnUpsilon++, AGamma++, AChi++)
	{
		cout << setw(MaxNameSize + 2) << (*ARow)["FieldName"].get_string();
		cout << setw(ValueWidth) << setprecision(ValuePrecision) << (*ARow)["Alpha"];
		MaxAlpha = GiveMax(MaxAlpha, (*ARow)["Alpha"]);
		cout << setw(ValueWidth) << setprecision(ValuePrecision) << (*ARow)["Phi"];
		MaxPhi = GiveMax(MaxPhi, (*ARow)["Phi"]);
		cout << setw(ValueWidth) << setprecision(ValuePrecision) << (*ARow)["Gamma_Max"];
		MaxMaxPossibleGamma = GiveMax(MaxMaxPossibleGamma, (*ARow)["Gamma_Max"]);
		cout << setw(ValueWidth) << setprecision(ValuePrecision) << (*ARow)["Chi_Max"];
		MaxMaxPossibleChi = GiveMax(MaxMaxPossibleChi, (*ARow)["Chi_Max"]);

		cout << setw(ValueWidth) << setprecision(ValuePrecision) << *AGamma;
		Max_of_Gammas = GiveMax(Max_of_Gammas, *AGamma);
		cout << setw(ValueWidth) << setprecision(ValuePrecision) << *AChi;
		Max_of_Chis = GiveMax(Max_of_Chis, *AChi);

		cout << setw(ValueWidth) << setprecision(ValuePrecision) << ((*AGamma / (*ARow)["Gamma_Max"]) * (*ARow)["Alpha"]);
		Max_SpatialComp = GiveMax(Max_SpatialComp, ((*AGamma / (*ARow)["Gamma_Max"]) * (*ARow)["Alpha"]));
		cout << setw(ValueWidth) << setprecision(ValuePrecision) << ((*AChi / (*ARow)["Chi_Max"]) * (*ARow)["Phi"]);
		Max_AnomComp = GiveMax(Max_AnomComp, ((*AChi / (*ARow)["Chi_Max"]) * (*ARow)["Phi"]));

		cout << setw(ValueWidth) << setprecision(ValuePrecision) << *AnUpsilon;
		MaxUpsilon = GiveMax(MaxUpsilon, *AnUpsilon);
		cout << endl;

		SpatialSum += ((*AGamma / (*ARow)["Gamma_Max"]) * (*ARow)["Alpha"]);
		StdAnomSum += ((*AChi / (*ARow)["Chi_Max"]) * (*ARow)["Phi"]);
	}

	cout << endl;
	cout.width(MaxNameSize + 2);
	cout << "Maxes:";
	cout << setw(ValueWidth) << setprecision(ValuePrecision) << MaxAlpha;
        cout << setw(ValueWidth) << setprecision(ValuePrecision) << MaxPhi;
        cout << setw(ValueWidth) << setprecision(ValuePrecision) << MaxMaxPossibleGamma;
        cout << setw(ValueWidth) << setprecision(ValuePrecision) << MaxMaxPossibleChi;
	
	cout << setw(ValueWidth) << setprecision(ValuePrecision) << Max_of_Gammas;
	cout << setw(ValueWidth) << setprecision(ValuePrecision) << Max_of_Chis;

	cout << setw(ValueWidth) << setprecision(ValuePrecision) << Max_SpatialComp;
	cout << setw(ValueWidth) << setprecision(ValuePrecision) << Max_AnomComp;

        cout << setw(ValueWidth) << setprecision(ValuePrecision) << MaxUpsilon;
	cout << endl;
	cout << "Spatial Sum: " << SpatialSum << "     StdAnom Sum: " << StdAnomSum << endl;
}


void PrintSyntax(const CmdOptions &CAFEOptions)
{
	string TheSpacing((_PROG_NAME).size() + 6, ' ');
        cout << "\n\n";
        cout << Bold("NAME") << endl;
        cout << "     " + _PROG_NAME + " - does the correlation calculations.\n";
        cout << Bold("SYNOPSIS") << endl;
        cout << "     ";
        cout << Bold(_PROG_NAME) + Bold(" [--help] [--syntax] [--type=") << Underline("TypeOfEvent") << Bold("]") << "\n";
	cout << TheSpacing << Bold("[--silent] [--date=") << Underline("DateString") + Bold("]") << "\n";
	cout << TheSpacing << Bold("[--timeperiod=") << Underline("Time_Period") + Bold("]")
	     << Bold(" [--score_run=") << Underline("Score_Run_Name") + Bold("]") << "\n";
	cout << TheSpacing << Bold("[--use_cache=") + Underline("CacheName") + Bold("] [--fullout]") << "\n";

#ifdef _SAMPLINGRUN_
	cout << TheSpacing << Bold("--fold=") << Underline("FoldNumber") << "\n";
#endif
        CAFEOptions.PrintSyntax((_PROG_NAME).size() + 5, 63);
        cout << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
        PrintSyntax(CAFEOptions);

        cout << Bold("DESCRIPTION") << endl;

        cout << "               Calculates the event scores.\n\n";

        cout << Bold("OPTIONS") << endl;

        cout << Bold("\t--help --syntax") << endl;

	cout << Bold("\t--type") << endl;
	cout << "      indicates the kind of event that is occuring at the given moment in time.\n\n";
	cout << Bold("\t--silent") << endl;
	cout << "      nothing will be printed to the screen (except error messages.\n\n";
	cout << Bold("\t--fullout") << endl;
	cout << "      detailed info regarding the correlation is printed to the screen.  Over-rides --silent\n\n";
	cout << Bold("\t--date") << endl;
	cout << "      the date that is being analyzed.  Useful when one wants the event scores tracked.\n\n";
	cout << Bold("\t--timeperiod") << endl;
	cout << "      the timeperiod for which this event is for.  Useful in tracking event scores.\n\n";
	cout << Bold("\t--score_run") << endl;
	cout << "      the name of the scoring run for which this correlation is being done for.\n";
	cout << "      " << Underline("Score_Run_Name") << " is 'Default' if the name is not given.\n\n";
	cout << Bold("\t--use_cache") << endl;
	cout << "      Tells the program to load the forecast data from the named cache, using the date given.\n";
	cout << "      If no name is given, the default cache is used.\n\n";
#ifdef _SAMPLINGRUN_
	cout << Bold("\t--fold") << endl;
	cout << "      The fold number helps to store the event scores for the sampling fold in its own file.\n\n";
#endif

        CAFEOptions.PrintDescription(63);

        cout << "\n\n";

}

	
int main(int argc, char *argv[])
{
	// Working on removing this necessity.
	setenv("TZ", "UTC UTC", 1);             // temporarially sets the computer's locale to UTC, so all conversions are done there.

	vector <string> CommandArgs = ProcessFlatCommandLine(argc, argv);
	CmdOptions CAFEOptions;

	if (CAFEOptions.ParseCommandArgs(CommandArgs) != 0)
	{
		cerr << "ERROR: Invalid syntax..." << endl;
		PrintSyntax(CAFEOptions);
		return(8);
	}

	string TypeOfEvent = "UnknownType";
	string DateOfEvent = "";
	string CacheName = "Default";
	string ScoringRun_Name = "Default";
	bool FullOutput = false;
	bool OutToFiles = false;
	string TheTimePeriod = "";

#ifdef _SAMPLINGRUN_
	int FoldNumber = -1;
#endif
	

	for (vector<string>::const_iterator AnArg( CommandArgs.begin() ); AnArg != CommandArgs.end(); AnArg++)
	{
		if (AnArg->find("--type=") == 0)	
		{
			TypeOfEvent = AnArg->substr(7);
		}
		else if (*AnArg == "--silent")
		{
			OutToFiles = true;
		}
		else if (*AnArg == "--fullout")
		{
			FullOutput = true;
		}
		else if (AnArg->find("--date=") == 0)
		{
			DateOfEvent = AnArg->substr(7);
		}
		else if (AnArg->find("--timeperiod=") == 0)
		{
			TheTimePeriod = AnArg->substr(13);
		}
		else if (AnArg->find("--use_cache=") == 0)
		{
			CacheName = AnArg->substr(12);
		}
		else if (AnArg->find("--score_run=") == 0)
		{
			ScoringRun_Name = AnArg->substr(12);
		}
		else if (*AnArg == "--help")
		{
			PrintHelp(CAFEOptions);
			return(2);
		}
		else if (*AnArg == "--syntax")
		{
			PrintSyntax(CAFEOptions);
			return(2);
		}
#ifdef _SAMPLINGRUN_
		else if (AnArg->find("--fold=") == 0)
		{
			FoldNumber = StrToInt(AnArg->substr(7));
		}
#endif
		else
		{
			cerr << "ERROR: Unknown Command Line arguement: " << *AnArg << "\n";
			PrintHelp(CAFEOptions);
			return(8);
		}
	}

	if (CacheName.empty())
	{
		cerr << "ERROR: Empty string given for the CacheName..." << endl;
		return(8);
	}

	if (ScoringRun_Name.empty())
	{
		cerr << "ERROR: Empty string given for the Scoring run's name..." << endl;
		return(8);
	}

#ifdef _SAMPLINGRUN_
        if (FoldNumber <= 0 || FoldNumber > 10)
        {
                cerr << "ERROR: Invalid Fold number.  The value was interpreated as: " << FoldNumber << endl;
                cerr << "Note: An invalid conversion from string to int returns a zero." << endl;
                return(8);
        }
#endif


	Configuration ConfigInfo(CAFEOptions.CAFEPath + '/' + CAFEOptions.ConfigFilename);

	if (!ConfigInfo.ValidConfig())		// faster, but no as complete check.
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

	if (!TheTimePeriod.empty())
	{
		if (!currState.TimePeriods_JumpTo(TimePeriodToOffset(TheTimePeriod)))
		{
			cerr << "ERROR: Invalid time period: " << TheTimePeriod << endl;
			return(8);
		}
	}

	const string ClustDatabase = currState.Trained_Name();

	mysqlpp::Connection ServerLink;

	try
	{
		EstablishConnection(ServerLink, currState.GetServerName(), currState.GetCAFEUserName(), ClustDatabase, false);
	}
	catch (const exception &Err)
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
                cerr << "ERROR: Unknown error..." << endl;
                ServerLink.close();
                return(4);
	}


	vector<double> EventScores( currState.EventTypes_Size() );
	vector<double>::iterator AnEventScore( EventScores.begin() );
	const string ForecastDir = currState.GetCAFEPath() + "/Forecast/" + CacheName + '/';

	try
	{
		mysqlpp::Query LonLatAnomQuery( MakeLoader_LonLatAnoms(ServerLink) );
		mysqlpp::Query FieldMeasQuery( MakeLoader_FieldMeasValues(ServerLink) );

		map<string, LonLatAnom> TheFields;

		for (currState.EventTypes_Begin(); currState.EventTypes_HasNext(); currState.EventTypes_Begin(), AnEventScore++)
        	{
			LonLatAnomQuery.def["table"] = 
			FieldMeasQuery.def["table"] = currState.EventType_Name();
	        	
			vector<double> Upsilons(0);
			vector<double> Gammas(0);
			vector<double> Chis(0);
			
			for (currState.EventVars_Begin(); currState.EventVars_HasNext(); currState.EventVars_Next())
			{
        	                for (currState.EventLevels_Begin(); currState.EventLevels_HasNext(); currState.EventLevels_Next())
                	        {
					for (currState.Extrema_Begin(); currState.Extrema_HasNext(); currState.Extrema_Next())
					{
						const string FullLabel = currState.FieldExtremum_Name();
						const vector <LonLatAnom> TheMembers( LoadLonLatAnoms(LonLatAnomQuery, FullLabel)   );

						if (!TheMembers.empty())
						{
							double Alpha, Phi, MaxGamma, MaxChi, Gamma, Chi;

							map<string, LonLatAnom>::const_iterator FieldMatch = TheFields.find(FullLabel);
							if (FieldMatch == TheFields.end())
							{
								//The insert function returns a pair type of an iterator and a bool.
								// This next line does the insert and grabs the iterator using .first
								FieldMatch = (TheFields.insert(make_pair(FullLabel, 
													 LoadForecast(ForecastDir + FullLabel + ".field",
														      DateOfEvent)
													)
											       )
									      ).first;
							}

							LoadFieldMeasValues(FieldMeasQuery, FullLabel, currState.EventType_Name(),
									    Alpha, Phi, MaxGamma, MaxChi);

							Upsilons.push_back( Calc_Event_Field(TheMembers, FieldMatch->second,
                                                                		             Alpha, Phi, MaxGamma, MaxChi,
                                                                            		     Gamma, Chi) );

							if (FullOutput)
							{
								Gammas.push_back(Gamma);
								Chis.push_back(Chi);
							}
						}
						else
						{
							if (FullOutput)
							{
								Gammas.push_back(0.0);
								Chis.push_back(0.0);
							}

							Upsilons.push_back(0.0);
						}
					}// end extrema loop
				}// end label loop
			}// end CAFEVar Name loop

			*AnEventScore = CalcEventScore(Upsilons);

			if (FullOutput)
		        {
                		OutputResults(currState.EventType_Name(), *AnEventScore, Upsilons, ServerLink.query(),
					      Gammas, Chis);
			}
		}// end EventTypeNames loop
	}
	catch (const exception &Err)
        {
                cerr << "ERROR: Exception caught.  Message: " << Err.what() << endl;
		cerr << "Possible additional info: " << ServerLink.error() << endl;
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
                cerr << "ERROR: Unknown exception caught." << endl;
                ServerLink.close();
                return(7);
        }

        ServerLink.close();



	if (!OutToFiles)
	{
		// TODO: Temporary kludge...
		const set<string> eventNames = currState.EventType_Names();
		cout << GiveDelimitedList(vector<string>(eventNames.begin(), eventNames.end()), ' ') << endl;
		cout << GiveDelimitedList(DoubleToStr(EventScores), ' ') << endl;
	}
	else
	{
		const string BaseStem = currState.GetCAFEPath() + "/CorrelationCalcs/" + ScoringRun_Name + '/' + ClustDatabase;

		if (system(("mkdir --parents '" + BaseStem + "'").c_str()) != 0)
		{
			cerr << "WARNING: Problem making directory " << BaseStem << endl;
			cerr << "       : You may have problems saving results...\n";
		}
		
#ifdef _SAMPLINGRUN_
		const string EventScoreFileName = BaseStem + '/' + TypeOfEvent + "_EventScore_Fold_" + IntToStr(FoldNumber) + ".csv";
#else
		const string EventScoreFileName = BaseStem + '/' + TypeOfEvent + "_EventScore.csv";
#endif

		if (!SaveEventScores(EventScoreFileName, EventScores, DateOfEvent))
                {
                        cerr << "ERROR: Could not save event scores to file: " << EventScoreFileName << endl;
                        return(9);
                }
	}

	return(0);
}
