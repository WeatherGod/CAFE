using namespace std;

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <ctime>
#include <cctype>			// for size_t
#include <algorithm>			// for lower_bound()

#include "Config/Configuration.h"
#include "Config/CAFEState.h"

#include <TimeUtly.h>			// for GetTimeUTC(), GiveTimeUTC()
#include <CmdLineUtly.h>		// for ProcessFlatCommandLine()

#include "Utils/CAFE_CmdLine.h"                       // for generic CAFE command line option handling
#include "Utils/PeakValleyFile.h"
#include "Utils/LonLatAnom.h"		// for LonLatAnom structure

#include <FormatUtly.h>                 // for making things look nice and pretty
#include <StrUtly.h>			// for TakeDelimitedList(), GiveDelimitedList(), StrToDouble(), Size_tToStr(), RipWhiteSpace()

#define _PROG_NAME              ((string) "TotalRecall")


void PrintSyntax(const CmdOptions &CAFEOptions)
{
        cout << "\n\n";
        cout << Bold("NAME") << endl;
        cout << "     " + _PROG_NAME + " - collects information about past events for correlation calculations.\n";
        cout << Bold("SYNOPSIS") << endl;
        cout << "     ";
        cout << Bold(_PROG_NAME) + Bold(" [--help] [--syntax] [--set_cache[=") + Underline("CACHE_NAME") + Bold("]] ") << Underline("DateTimeStr") << "\n";
        CAFEOptions.PrintSyntax((_PROG_NAME).size() + 5, 63);
	cout << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
        PrintSyntax(CAFEOptions);

        cout << Bold("DESCRIPTION") << endl;

        cout << "               This program will analyze the time given and prepare information for CalcCorrelation.\n\n";

        cout << Bold("OPTIONS") << endl;

        cout << Bold("\t--help --syntax") << endl << endl;
	cout << Bold("\t--set_cache") << endl;
	cout << "\t\tSaves forecast information into cachable form under the given name for quicker re-use in the hindcast.\n";
	cout << "\t\tBy default, " << Underline("CACHE_NAME") << " is set to 'Default'.\n\n";
	cout << "\t" << Underline("DateTimeStr") << endl;
	cout << "\t\tThe datetime string must be in PeakValS format. (i.e. - 06Z23Mar1983)\n\n";

        CAFEOptions.PrintDescription(63);

        cout << "\n\n";
}


bool ProcessAndPrint(const string &PeakValoutputName, const set<string> &CAFEVarLabels, const string &ForecastLocation, 
		     CAFEState &currState, const string &EventDate, const bool &SetCache)
{ 
        PeakValleyFile peakValStream(PeakValoutputName.c_str(), ios::in);

        if (!peakValStream.is_open())
        {
                cerr << "ERROR: Could not open the PeakVal output file: " << PeakValoutputName << "  for reading.\n";
                return(false);
        }

        for (set<string>::const_iterator aLabel = CAFEVarLabels.begin();
	     aLabel != CAFEVarLabels.end();
	     aLabel++)
        {
		vector<LonLatAnom> ExtremumInfo = peakValStream.RetrieveExtrema(1, currState.Extrema_Size());

		if (ExtremumInfo.empty())
		{
			cerr << "ERROR: Problem reading the output from peakvalley file: " << PeakValoutputName << endl;
			peakValStream.close();
			return(false);
		}

		size_t extremumIndex = 0;
                for (currState.Extrema_Begin(); currState.Extrema_HasNext() && peakValStream.good(); 
		     currState.Extrema_Next(), extremumIndex++)
		{
			const string ExtremumFilename = ForecastLocation + *aLabel + '_' 
							+ currState.Extremum_Name() + ".field";

			// TODO: Refactor into the ForecastFieldFile class
			ofstream ExtremumStream;

			if (SetCache)
			{
				ExtremumStream.open(ExtremumFilename.c_str(), ios::app);
			}
			else
			{
				ExtremumStream.open(ExtremumFilename.c_str());
			}

                        if (!ExtremumStream.is_open())
                        {
                                cerr << "Could not open the forecast file: " << ExtremumFilename << endl;
                                peakValStream.close();
	                        return(false);
                        }

                        ExtremumStream << DoubleToStr(ExtremumInfo[extremumIndex].Lon) << ' ' 
				       << DoubleToStr(ExtremumInfo[extremumIndex].Lat) << ' '
				       << DoubleToStr(ExtremumInfo[extremumIndex].StdAnom) << ' ' 
				       << EventDate << '\n';

                        ExtremumStream.close();
		}// end extremum loop

        }// end CAFEVarLabel loop

        peakValStream.close();

        return(true);
}


//------------------------------------------------------------------------------------------------------------------------------
// These two functions, MakeCmdLineArgs() and RunPeakValExtractor() will need to be modified for 
// whatever particular extractor program that you have.
string MakeCmdLineArgs(CAFEState &currInfo, const string &varName, const set<string> &loadedLevels)
{
	currInfo.CAFEVars_JumpTo(varName);

	string LevelArgs = "";
	size_t LevelWrdCnt = 0;

        for (set<string>::const_iterator aLevel = loadedLevels.begin();
	     aLevel != loadedLevels.end();
	     aLevel++)
        {
		currInfo.CAFELevels_JumpTo(*aLevel);

                // replacing the commas with spaces.
                const vector<string> theLevWrds = TakeDelimitedList( currInfo.CAFEDataLevel_Name(), ',');
		LevelWrdCnt += theLevWrds.size();
		LevelArgs += "'" + GiveDelimitedList(theLevWrds, "' '") + "' ";
	}

        return("'" + currInfo.CAFEData_Name() + "' " + Size_tToStr(LevelWrdCnt) + ' ' + LevelArgs);
}

bool RunPeakValExtractor(const string &ProgName, const string &CmdLineArg, const size_t VarCount, const CAFEDomain &TheDomain,
			 const string &DateTimeStr, const string &ProgOutputName)
// Could an injection attack happen here?  It would be pretty difficult, but feasible...
{
	const vector<float> TheLons = TheDomain.GiveLons();
	const vector<float> TheLats = TheDomain.GiveLats();

        const string SysCommand = ProgName + ' ' + DateTimeStr + ' ' + Size_tToStr(VarCount) + ' ' 
				  + FloatToStr(TheLats[0]) + ' ' + FloatToStr(TheLons[0]) + ' '
				  + FloatToStr(TheLats[1]) + ' ' + FloatToStr(TheLons[1]) + ' '
				  + CmdLineArg + " >& " + ProgOutputName;

        return(system(SysCommand.c_str()) == 0);
}
//-----------------------------------------------------------------------------------------------------------------------------i


int main(int argc, char *argv[])
{
	// Working on eliminating this need.
	setenv("TZ", "UTC UTC", 1);

	vector <string> CmdLineArgs = ProcessFlatCommandLine(argc, argv);
	CmdOptions CAFEOptions;

        if (CAFEOptions.ParseCommandArgs(CmdLineArgs) != 0)
        {
                cerr << "ERROR: Invalid syntax..." << endl;
                PrintSyntax(CAFEOptions);
                return(8);
        }

	string PeakValDateTimeStr = "";
	string CacheName = "Default";
	bool SetCache = false;

        for (size_t ArgIndex = 0; ArgIndex < CmdLineArgs.size(); ArgIndex++)
        {
                if (CmdLineArgs[ArgIndex] == "--help")
                {
                        PrintHelp(CAFEOptions);
                        return(2);
                }
                else if (CmdLineArgs[ArgIndex] == "--syntax")
                {
                        PrintSyntax(CAFEOptions);
                        return(2);
                }
		else if (CmdLineArgs[ArgIndex].find("--set_cache") == 0)
		{
			SetCache = true;
			if (CmdLineArgs[ArgIndex].find("--set_cache=") == 0)
			{
				CacheName = CmdLineArgs[ArgIndex].substr(12);
			}
		}
                else
                {
                        PeakValDateTimeStr = CmdLineArgs[ArgIndex];
                }
        }

	if (CacheName.empty())
	{
		cerr << "ERROR: Empty string given for the cache name." << endl;
		return(8);
	}

	if (PeakValDateTimeStr.empty())
	{
		cerr << "Invalid syntax..." << endl;
		PrintSyntax(CAFEOptions);
		return(8);
	}

	Configuration ConfigInfo(CAFEOptions.CAFEPath + '/' + CAFEOptions.ConfigFilename);

        if (!ConfigInfo.IsValid())
        {
                cerr << "ERROR: Something wrong with the config file: " << CAFEOptions.CAFEPath << '/' << CAFEOptions.ConfigFilename << endl;
                return(1);
        }

        if (!CAFEOptions.MergeWithConfiguration(ConfigInfo))
        {
                cerr << "ERROR: Conflicts in the command line..." << endl;
                PrintSyntax(CAFEOptions);
                return(8);
        }

	CAFEState currState( CAFEOptions.ConfigMerge( ConfigInfo.GiveCAFEInfo() ) );

	const string ForecastLocation = currState.GetCAFEPath() + "/Forecast/" + CacheName + '/';
	const string PeakValOutputName = currState.GetCAFEPath() + "/scratch/Forecastdata.txt";
	const string PeakValProgName = currState.GetCAFEPath() + "/bin/PeakValleyPicker";

	const time_t PeakValDateTime = GetTimeUTC(PeakValDateTimeStr, "%HZ%d%b%Y");
	const pair<time_t, time_t> timeRange = currState.DefaultDataSource().GiveTimeRange();

	if (PeakValDateTime < timeRange.first || PeakValDateTime >= timeRange.second)
	{
		cerr << "Date out of range.  Pick a date between " << GiveTimeUTC(timeRange.first)
		     << " and " << GiveTimeUTC(timeRange.second) << "." << endl;
		return(3);
	}


	map< string, set<string> > loadedCAFELevels;

	// Getting the list of all CAFELevels that needs to be done, across all event types specified.
	// The map is indexed by CAFEVar names.  The value is a set of the CAFELevel strings.
	for (currState.EventTypes_Begin(); currState.EventTypes_HasNext(); currState.EventTypes_Next())
        {
		for (currState.EventVars_Begin(); currState.EventVars_HasNext(); currState.EventVars_Next())
		{
			// NOTE: insertion of empty set occurs ONLY when the key could not be found.
			//     : In other words, the first time these loops encounter a particular
			//     : EventVar, a new entry will be made in loadedCAFELevels.
			pair< map< string, set<string> >::iterator, bool > eventVar_Levels = 
					loadedCAFELevels.insert( make_pair(currState.EventVar_Name(), set<string>() ) );

			// Appending new information into the stored set, which is stored in loadedCAFELevels
			const set<string> someEventLevels = currState.EventLevel_Names();
			eventVar_Levels.first->second.insert(someEventLevels.begin(), someEventLevels.end());
		}
	}


	// by now, I will have all of the variables and levels that I need to execute the peakvalley extractor.

	string PeakValArgs = "";
	set<string> CAFEVarLabels;
	for (map< string, set<string> >::const_iterator ALoadedCAFEVar = loadedCAFELevels.begin();
	     ALoadedCAFEVar != loadedCAFELevels.end();
	     ALoadedCAFEVar++)
	{
		PeakValArgs += MakeCmdLineArgs(currState, ALoadedCAFEVar->first, ALoadedCAFEVar->second) + ' ';
	
		for (set<string>::const_iterator ALevel = ALoadedCAFEVar->second.begin();
		     ALevel != ALoadedCAFEVar->second.end();
		     ALevel++)
		{
			CAFEVarLabels.insert(CAFEVarLabels.end(), (ALevel->empty() ? ALoadedCAFEVar->first
										   : ALoadedCAFEVar->first + "_" + *ALevel));
		}
	}

	
	if (!RunPeakValExtractor(PeakValProgName, PeakValArgs, loadedCAFELevels.size(), 
				 currState.GetCAFEDomain(), PeakValDateTimeStr, PeakValOutputName))
	{
		cerr << "ERROR: Something went wrong with executing the peak valley extractor..." << endl;
		cerr << "     : Suggest checking the file '" << PeakValOutputName << "' for more info." << endl;
		return(9);
	}


	if (!ProcessAndPrint(PeakValOutputName, CAFEVarLabels, ForecastLocation, currState, PeakValDateTimeStr, SetCache))
	{
		cerr << "\n\tSomething went wrong in processing the peakvalley picker's output..." << endl;
		return(10);
	}
	        	
	return(0);
}
