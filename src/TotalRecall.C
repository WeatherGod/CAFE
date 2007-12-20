using namespace std;

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>			// for lower_bound()

#include "Config/Configuration.h"
#include <TimeUtly.h>			// for GetTime(), GiveTime()
#include <CmdLineUtly.h>		// for ProcessFlatCommandLine()
#include "Utils/CAFE_CmdLine.h"                       // for generic CAFE command line option handling
#include <FormatUtly.h>                 // for making things look nice and pretty
#include <StrUtly.h>			// for TakeDelimitedList(), GiveDelimitedList(), StrToDouble(), Size_tToStr(), RipWhiteSpace()
#include <VectorUtly.h>			// for MergeUnique()

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



bool CheckRead(const string &LineRead)
{
        if (LineRead.find("---^^^---End") != string::npos)
        {
                cerr << "\n\tError in reading data from PeakVal output.  End of data came earlier than expected\n";
                cerr << "If there is a filename on the next line, then PeakVal could not open this file\n";
                cerr << LineRead.substr(12) << endl;
                return(false);
        }

        return(true);
}


void CheckValues(vector <double> &ExtremumInfo, vector <string> &TempHold)
{
        if (TempHold[0] == "999" || TempHold[0] == "-999")
        {
                TempHold[0] = "\\N";
                TempHold[1] = "\\N";
                TempHold[2] = "\\N";
        }

        // StrToDouble() will return a NAN value if it encounters "\\N"
        ExtremumInfo[0] = StrToDouble(TempHold[0]);
        ExtremumInfo[1] = StrToDouble(TempHold[1]);
        ExtremumInfo[2] = StrToDouble(TempHold[2]);
}


bool ProcessData(vector <double> &ExtremumInfo, const string &LineRead)
{
        if (CheckRead(LineRead))
        {
                vector <string> TempHold = TakeDelimitedList(LineRead, ' ');
                CheckValues(ExtremumInfo, TempHold);
                return(true);
        }
        else
        {
                return(false);
        }
}

bool ProcessAndPrint(const string &PeakValoutputName, const vector <string> &CAFEVarLabels, const string &ForecastLocation, 
		     const Configuration &ConfigInfo, const string &EventDate, const bool &SetCache)
{
        string LineRead = "";
        ifstream PeakValStream(PeakValoutputName.c_str());

        if (!PeakValStream.is_open())
        {
                cerr << "\n\tCould not open the PeakVal output file: " << PeakValoutputName << "  for reading.\n";
                return(false);
        }

        for (size_t DataIndex = 0; DataIndex < CAFEVarLabels.size(); DataIndex++)
        {
                bool FoundStart = false;

                getline(PeakValStream, LineRead);
                while (!FoundStart && !PeakValStream.eof())
                {
                        if (LineRead.find("---^^^---Start") != string::npos)
                        {
                                FoundStart = true;
                        }
                        getline(PeakValStream, LineRead);
                }

                if (!FoundStart)
                {
			cerr << "\n\tCould not find data in the PeakVal output file.  Exiting prematurely...\n";
                        cerr << "\tDataIndex: " << DataIndex << "    CAFEVarLabels.size(): " << CAFEVarLabels.size() << endl;

                        PeakValStream.close();

                        return(false);
		}
                
	        for (size_t ExtremumIndex = 0; ExtremumIndex < ConfigInfo.ExtremaCount() && !PeakValStream.eof(); ExtremumIndex++)
		{
			const string ExtremumFilename = ForecastLocation + CAFEVarLabels[DataIndex] + '_' 
							+ ConfigInfo.GiveExtremaName(ExtremumIndex) + ".field";
				
			vector <double> ExtremumInfo(3);
			if (!ProcessData(ExtremumInfo, LineRead))
                        {
				cerr << "Problem reading the output from PeakVals..." << endl;
       	                        PeakValStream.close();
               	                return(false);
                       	}

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
                                PeakValStream.close();
	                        return(false);
                        }

                        ExtremumStream << DoubleToStr(ExtremumInfo[1]) << ' ' << DoubleToStr(ExtremumInfo[2]) 
				       << ' ' << DoubleToStr(ExtremumInfo[0]) << ' ' << EventDate << '\n';

                        ExtremumStream.close();

			getline(PeakValStream, LineRead);
		}// end extremum loop
        }// end CAFEVarLabel loop

        PeakValStream.close();

        return(true);
}


//------------------------------------------------------------------------------------------------------------------------------
// These two functions, MakeCmdLineArgs() and RunPeakValExtractor() will need to be modified for 
// whatever particular extractor program that you have.
string MakeCmdLineArgs(const string &CAFEVarName, const vector <string> &CAFEVarLabels, Configuration &ConfigInfo)
// assume that all the CAFEVarLabels are valid.
{
        const string DataVarName = ConfigInfo.Give_DataSource_DataName(CAFEVarName);

        string LevelArgs = "";
	size_t LevelWrdCnt = 0;
        for (vector<string>::const_iterator A_CAFEVarLabel = CAFEVarLabels.begin(); A_CAFEVarLabel != CAFEVarLabels.end(); A_CAFEVarLabel++)
        {
                const string LevelName = ConfigInfo.Give_CAFEVar_LevelName(CAFEVarName, *A_CAFEVarLabel);
//              cout << '+' << LevelName << '+' << endl;

                // replacing the commas with spaces.
                const vector<string> TheLevWrds = TakeDelimitedList( ConfigInfo.Give_DataSource_DataLevel(CAFEVarName, LevelName),
                                                                   ',');
		LevelWrdCnt += TheLevWrds.size();
		LevelArgs += "'" + GiveDelimitedList(TheLevWrds, "' '") + "' ";
	}

        return("'" + DataVarName + "' " + Size_tToStr(LevelWrdCnt) + ' ' + LevelArgs);
}

bool RunPeakValExtractor(const string &ProgName, const string &CmdLineArg, const size_t VarCount, const CAFEDomain &TheDomain,
			 const string &DateTimeStr, const string &ProgOutputName)
// Could an injection attack happen here?  It would be pretty difficult, but feasible...
{
	const vector <float> TheLons = TheDomain.GiveLons();
	const vector <float> TheLats = TheDomain.GiveLats();

        const string SysCommand = ProgName + ' ' + DateTimeStr + ' ' + Size_tToStr(VarCount) + ' ' 
				  + FloatToStr(TheLats[0]) + ' ' + FloatToStr(TheLons[0]) + ' '
				  + FloatToStr(TheLats[1]) + ' ' + FloatToStr(TheLons[1]) + ' '
				  + CmdLineArg + " >& " + ProgOutputName;

        return(system(SysCommand.c_str()) == 0);
}
//-----------------------------------------------------------------------------------------------------------------------------i


int main(int argc, char *argv[])
{
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

	const string ForecastLocation = CAFEOptions.CAFEPath + "/Forecast/" + CacheName + '/';
	const string PeakValOutputName = CAFEOptions.CAFEPath + "/scratch/Forecastdata.txt";
	const string PeakValProgName = CAFEOptions.CAFEPath + "/bin/PeakValleyPicker";

	time_t PeakValDateTime = GetTimeUTC(PeakValDateTimeStr, "%HZ%d%b%Y");

	pair<time_t, time_t> timeRange = ConfigInfo.Give_DataSource_TimeRange();

	if (PeakValDateTime < timeRange.first || PeakValDateTime >= timeRange.second)
	{
		cerr << "Date out of range.  Pick a date between " << GiveTimeUTC(timeRange.first)
		     << " and " << GiveTimeUTC(timeRange.second) << "." << endl;
		return(3);
	}


	map <string, vector<string> > LoadedCAFELabels;

	// Getting the list of all CAFELabels that needs to be done, across all event types specified.
	// The map is indexed by CAFEVar names
	for (vector<string>::const_iterator EventTypeName = CAFEOptions.EventTypes.begin();
             EventTypeName != CAFEOptions.EventTypes.end();
             EventTypeName++)
        {
		const vector <string> SomeCAFEVars = CAFEOptions.GiveCAFEVarsToDo(ConfigInfo, *EventTypeName);

		for (vector<string>::const_iterator AVarName = SomeCAFEVars.begin();
		     AVarName != SomeCAFEVars.end();
		     AVarName++)
		{
			// Note, insertion of "" occurs ONLY when the key could not be found.
			pair< map<string, vector<string> >::iterator, bool > CAFEVar_Labels = 
					LoadedCAFELabels.insert( make_pair(*AVarName, vector<string>(0) ) );
			
			const vector <string> SomeCAFELabels = CAFEOptions.GiveLabelsToDo(ConfigInfo, *EventTypeName, *AVarName);

			for (vector<string>::const_iterator ALabel = SomeCAFELabels.begin();
			     ALabel != SomeCAFELabels.end();
			     ALabel++)
			{
				if (!binary_search(CAFEVar_Labels.first->second.begin(), CAFEVar_Labels.first->second.end(), *ALabel))
				{
					CAFEVar_Labels.first->second.insert(lower_bound(CAFEVar_Labels.first->second.begin(),
										  CAFEVar_Labels.first->second.end(), *ALabel), *ALabel);
				}
			}
		}
	}


	// by now, I will have all of the variables and labels that I need to execute the peakvalley extractor.

	string PeakValArgs = "";
	vector <string> CAFEVarLabels(0);
	for (map<string, vector<string> >::const_iterator ALoadedCAFEVar(LoadedCAFELabels.begin());
	     ALoadedCAFEVar != LoadedCAFELabels.end();
	     ALoadedCAFEVar++)
	{
		PeakValArgs += MakeCmdLineArgs(ALoadedCAFEVar->first, ALoadedCAFEVar->second, ConfigInfo) + ' ';
	
		for (vector<string>::const_iterator ALabel = ALoadedCAFEVar->second.begin();
		     ALabel != ALoadedCAFEVar->second.end();
		     ALabel++)
		{
			CAFEVarLabels.push_back(*ALabel);
		}
	}

	
	if (!RunPeakValExtractor(PeakValProgName, PeakValArgs, LoadedCAFELabels.size(), *ConfigInfo.GiveDomain(), PeakValDateTimeStr, PeakValOutputName))
	{
		cerr << "ERROR: Something went wrong with executing the peak valley extractor..." << endl;
		return(9);
	}


	if (!ProcessAndPrint(PeakValOutputName, CAFEVarLabels, ForecastLocation, ConfigInfo, PeakValDateTimeStr, SetCache))
	{
		cerr << "\n\tSomething went wrong in processing the peakvalley picker's output..." << endl;
		return(10);
	}
	        	
	return(0);
}
