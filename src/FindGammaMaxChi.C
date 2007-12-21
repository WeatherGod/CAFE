using namespace std;

#include <iostream>
#include <vector>
#include <string>
#include <cmath>		// for ceil()
#include <algorithm>		// for max()
#include <cctype>		// for size_t
#include <mysql++/mysql++.h>
#include <StrUtly.h>		// for StrToSize_t()


#include "Config/Configuration.h"
#include "Utils/CAFEUtly.h"		// for GetGridInfo()
#include "Utils/CAFE_SQLUtly.h"		// for LoadLonLatAnoms(), SaveGammaChiMaxValues(), EstablishConnection()

#include "Utils/CAFE_CmdLine.h"		// for generic CAFE command line handling...

#include "Utils/CAFEEqtns.h"		// for CalcGamma(), NewCalcChi()

#include <Histogram.h>			// for the Histogram class
#include <CmdLineUtly.h>		// for ProcessFlatCommandLine()


struct LatLon_t
{
	double Lat;
	double Lon;

	LatLon_t()
		:	Lat(0.0),
			Lon(0.0)
	{
	}
};



void PrintSyntax(const CmdOptions &CAFEOptions)
{
	cout << endl;
	cout << "FindGammaMaxChi [--help] [--syntax] --radius=_RADIUS_" << endl;

	CAFEOptions.PrintSyntax(15, 63);

	cout << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
	PrintSyntax(CAFEOptions);
	cout << endl;
	cout << "Calculates the maximum value for Gamma and Chi for each field by brute-force methods." << endl << endl;
	cout << "DESCRIPTION" << endl;
	CAFEOptions.PrintDescription(63);
	cout << endl;
}


vector <LatLon_t> MakeDomainList(const BoardConvertor &ProjectionInfo, const double &Res)
// if Res == 1.0, then the resolution of the domain is the same as the resolution of the original data
// if Res == 2.0, then the resolution is double the resolution of the original data, and so forth.
{
	vector <LatLon_t> TheDomain((size_t) ((ProjectionInfo.Ysize() * Res) * (ProjectionInfo.Xsize() * Res)));
	vector <LatLon_t>::iterator LatLonValue = TheDomain.begin();

	for (double YCoord = 0.0; YCoord < ((double) ProjectionInfo.Ysize()); YCoord += (1.0 / Res))
	{
		for (double XCoord = 0.0; XCoord < ((double) ProjectionInfo.Xsize()); XCoord += (1.0 / Res), LatLonValue++)
		{
			ProjectionInfo.GridToSpherical(XCoord, YCoord, LatLonValue->Lon, LatLonValue->Lat);
		}
	}

	return(TheDomain);
}

double CalcChiField(const vector <LonLatAnom> &TheClustMembers)
{
	vector <double> TheAnoms( TheClustMembers.size() );
	vector<LonLatAnom>::const_iterator AMember( TheClustMembers.begin() );
	for (vector<double>::iterator AnAnom( TheAnoms.begin() ); AnAnom != TheAnoms.end(); AnAnom++, AMember++)
	{
		*AnAnom = AMember->StdAnom;
	}

	const Histogram TheHistogram(TheAnoms);
	double MaxChi = 0.0;
	double TempHold;

	for (size_t BinIndex = 0; BinIndex < TheHistogram.BinCount(); BinIndex++)
	{
		TempHold = CalcChi(TheHistogram, TheHistogram.Center(BinIndex));
//		cerr << "Bin: " << BinIndex << ": " << TempHold << endl;
		if (TempHold > MaxChi)
		{
			MaxChi = TempHold;
		}
	}

	return(MaxChi);
}

double CalcGammaField(const vector <LonLatAnom> &TheClustMembers, const vector <LatLon_t> &TheDomain)
{
	double MaxGamma = 0.0;	// just priming the pump.
	double TempHold;

	const vector <LonLatCount> MemberCounts( GetMemberCounts(TheClustMembers) );
	
	for (vector<LatLon_t>::const_iterator ADomainCoord(TheDomain.begin()); ADomainCoord != TheDomain.end(); ADomainCoord++)
	{
		//TempHold = CalcGamma(TheClustMembers.begin(), TheClustMembers.end(), *ADomainLon, *ADomainLat);
		TempHold = CalcGamma_Opt(MemberCounts, ADomainCoord->Lon, ADomainCoord->Lat);

		if (TempHold > MaxGamma)
		{
			MaxGamma = TempHold;
		}
	}

	return(MaxGamma);
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

	size_t Radius = 0;

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
		else if (CmdArgs[ArgIndex].find("--radius=") == 0)
		{
			Radius = StrToSize_t(CmdArgs[ArgIndex].substr(9));
		}
		else
		{
			cerr << "ERROR: Invalid arguement: " << CmdArgs[ArgIndex] << endl;
			PrintSyntax(CAFEOptions);
			return(8);
		}
	}

	if (Radius == 0)
	{
		cerr << "ERROR: Missing --radius= arguement!\n";
		PrintSyntax(CAFEOptions);
		return(2);
	}
	
	
	Configuration ConfigInfo(CAFEOptions.CAFEPath + '/' + CAFEOptions.ConfigFilename);

	if (!ConfigInfo.IsValid())
	{
		cerr << "Error: Invalid configuration file: " << CAFEOptions.CAFEPath << '/' << CAFEOptions.ConfigFilename << endl;
		return(1);
	}

	if (!CAFEOptions.MergeWithConfiguration(ConfigInfo))
        {
                cerr << "ERROR: Conflicts in the command line..." << endl;
                PrintSyntax(CAFEOptions);
                return(8);
        }

	

	mysqlpp::Connection ServerLink;

        try
        {
		EstablishConnection(ServerLink, CAFEOptions.ServerName, CAFEOptions.LoginUserName);
        }
        catch (const exception& Err)
        {
                cerr << "ERROR: Connection failed: " << Err.what() << endl;
		ServerLink.close();
                return(2);
        }
	catch (const string &ErrStr)
	{
		cerr << "ERROR: Connection failed: " << ErrStr << endl;
		ServerLink.close();
		return(2);
	}
        catch (...)
        {
                cerr << "ERROR: Unknown exception caught..." << endl;
                ServerLink.close();
                return(3);
        }


        try
        {
		const double RESOLUTION = 4.0;	// 1.0 will cause a use of the same resolution as the original data.
						// 2.0 will use double the resolution as the original data.
						// 0.5 will use half the resolution as the original data
						// and so forth...
		BoardConvertor ProjectionInfo;
		if (!GetGridInfo(ConfigInfo, ProjectionInfo, Radius))
		{
			throw((string) "Bad Projection information!");
		}

		const vector <LatLon_t> TheDomain( MakeDomainList(ProjectionInfo, RESOLUTION) );
		

		for (vector<string>::const_iterator TheTimePeriod = CAFEOptions.TimePeriods.begin();
		     TheTimePeriod != CAFEOptions.TimePeriods.end();
		     TheTimePeriod++)
        	{
			const string ClustDatabase = CAFEOptions.GiveClusteredDatabaseName(*TheTimePeriod);
			cout << ClustDatabase << endl;

                	if (!ServerLink.select_db(ClustDatabase))
                	{
	                	throw("Could not select the database: " + ClustDatabase + "\nMySQL message: " + ServerLink.error());
                	}

	                mysqlpp::Query LonLatAnomQuery( MakeLoader_LonLatAnoms(ServerLink) );
			mysqlpp::Query GammaChiMaxQuery( MakeSaver_GammaChiMaxValues(ServerLink) );

			for (vector<string>::const_iterator EventTypeName = CAFEOptions.EventTypes.begin();
			     EventTypeName != CAFEOptions.EventTypes.end();
			     EventTypeName++)
                	{
				cout << '\t' << *EventTypeName << endl;
				LonLatAnomQuery.def["table"] = *EventTypeName;
				GammaChiMaxQuery.def["table"] = *EventTypeName;

                        	const vector <string> VarNames = CAFEOptions.GiveCAFEVarsToDo(ConfigInfo, *EventTypeName);

				for (vector<string>::const_iterator AVarName = VarNames.begin(); AVarName != VarNames.end(); AVarName++)
				{
					const vector <string> CAFELabels = CAFEOptions.GiveLabelsToDo(ConfigInfo, *EventTypeName, *AVarName);

                                	for (vector<string>::const_iterator ALabel = CAFELabels.begin(); ALabel != CAFELabels.end(); ALabel++)
                                	{
						//cerr << *ALabel << endl;
						for (size_t PeakValIndex = 0; PeakValIndex < ConfigInfo.ExtremaCount(); PeakValIndex++)
						{
							const string FieldStem = *ALabel + '_' + ConfigInfo.GiveExtremaName(PeakValIndex);
							// TODO: Wouldn't I rather want to set these to NAN?
							double MaxGamma = 0.0;
							double MaxChi = 0.0;
							const vector <LonLatAnom> TheClustMembers( LoadLonLatAnoms(LonLatAnomQuery, FieldStem) );

							if (TheClustMembers.size() > 1)	// has to be greater than one or else the Variance function in
											// some of the functions will return NaN
							{
								MaxGamma = CalcGammaField(TheClustMembers, TheDomain);
								
//								Histogram StdAnomFreq(ClustAnoms);
								MaxChi = CalcChiField(TheClustMembers);
							}// end if (ClustLons.size() > 1)

							SaveGammaChiMaxValues(GammaChiMaxQuery, FieldStem, MaxGamma, MaxChi);
						}// end extrema loop
					}// end label loop
				}// end CAFEVar name loop
			}// end event type loop
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
