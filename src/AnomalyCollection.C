using namespace std;

#include <iostream>
#include <vector>
#include <algorithm>				// for binary_search(), lower_bound(), inplace_merge()
#include <string>
#include <fstream>
#include <ctime>
#include <cmath>
#include <errno.h>

#include <CmdLineUtly.h>			// for ProcessFlatCommandLine()
#include <StrUtly.h>				// for TakeDelimitedList(), ToUpper(), StrToDouble(), Size_tToStr()
#include <Utils/CAFEUtly.h>				// for WriteLonLatAnoms()
#include <Utils/CAFE_SQLUtly.h>			// for InsertEvent(), SaveLonLatAnom(), EstablishConnection()
#include <Config/Configuration.h>

#include <Utils/CAFE_CmdLine.h>			// for generic CAFE command line option handling...

#include <TimeUtly.h>				// for RoundHour() and others
#include <FormatUtly.h>				// for Bold(), Underline()

#include <mysql++/mysql++.h>



bool CheckRead(const string &LineRead)
{
	if (LineRead.find("---^^^---End") != string::npos)
	{
		cerr << "\n\tError in reading data from GrAD output.  End of data came earlier than expected\n";
		cerr << "If there is a filename on the next line, then GrAD could not open this file\n";
		cerr << LineRead.substr(12) << endl;
		return(false);
	}

	return(true);
}


void CheckValues(vector <double> &ExtremumInfo, vector <string> TempHold, const size_t &PosOffset)
{
	if (TempHold[0] == "999" || TempHold[0] == "-999")
	{
		TempHold[0] = "\\N";
		TempHold[1] = "\\N";
		TempHold[2] = "\\N";
	}

	// StrToDouble() will return a NAN value if it encounters "\\N"
	// This is standard anomaly
	ExtremumInfo[PosOffset] = StrToDouble(TempHold[0]);
	// This is longitude
	ExtremumInfo[PosOffset + 1] = StrToDouble(TempHold[1]);
	// This is latitude
	ExtremumInfo[PosOffset + 2] = StrToDouble(TempHold[2]);
}


bool ProcessExtrema(vector <double> &ExtremumInfo, const string &LineRead, const size_t &PosOffset)
{
	if (CheckRead(LineRead))
        {
        	vector <string> TempHold = TakeDelimitedList(LineRead, ' ');
                CheckValues(ExtremumInfo, TempHold, PosOffset);
		return(true);
	}
	else
	{
		return(false);
	}
}


bool ProcessAndPrint(const string &ProgOutputName, const vector <string> &AllCAFEVarLabels, const time_t &EventDateTime,
		     const double &EventLon, const double &EventLat, const string &EventType,
		     mysqlpp::Query &TheQuery, const Configuration &ConfigInfo)
// This funcrion will save the data to the mysql database.
{
	string LineRead = "";
	ifstream ProgStream(ProgOutputName.c_str());

	if (!ProgStream.is_open())
	{
		cerr << "\n\tCould not open the program's output file: " << ProgOutputName << "  for reading.\n";
		return(false);
	}

	const size_t DataFieldCount = 3;	// StdAnom, Lon, Lat
	vector <string> ColumnStems(DataFieldCount, "");
	vector <double> ExtremumInfo(DataFieldCount * AllCAFEVarLabels.size() * ConfigInfo.ExtremaCount(), nan("NAN"));
	vector <string> ColumnNames(DataFieldCount * AllCAFEVarLabels.size() * ConfigInfo.ExtremaCount(), "");

	ColumnStems[0] = "StdAnom";
	ColumnStems[1] = "Lon";
	ColumnStems[2] = "Lat";

	size_t PosOffset = 0;
	for (vector<string>::const_iterator CAFELabel = AllCAFEVarLabels.begin(); CAFELabel != AllCAFEVarLabels.end(); CAFELabel++)
	{
		bool FoundStart = false;

		getline(ProgStream, LineRead);
		while (!FoundStart && !ProgStream.fail())
		{
			if (LineRead.find("---^^^---Start") != string::npos)
			{
				FoundStart = true;
			}
			getline(ProgStream, LineRead);
		}

		if (!FoundStart)
		{
			cerr << "ERROR: Could not find data in the program output file.\n";
                        ProgStream.close();
                        return(false);
		}

		for (size_t ExtremumIndex = 0; ExtremumIndex < ConfigInfo.ExtremaCount() && !ProgStream.eof(); ExtremumIndex++)
		{
			if (!ProcessExtrema(ExtremumInfo, LineRead, PosOffset))
			{
				cerr << "ERROR: Problem reading the output from the peakvalley program..." << endl;
				ProgStream.close();
				return(false);
			}

			// PosOffset will be incremented by this for loop.
			for (vector<string>::const_iterator AColumnStem = ColumnStems.begin(); AColumnStem != ColumnStems.end(); 
			     AColumnStem++, PosOffset++)
			{
				ColumnNames[PosOffset] = *CAFELabel + '_' + ConfigInfo.GiveExtremaName(ExtremumIndex) + '_' + *AColumnStem;
			}

			getline(ProgStream, LineRead);
		}

		if (ProgStream.eof())
		{
			cerr << "Error: Reached end of the program output early." << endl;;
			cerr << "\tBe sure to reset this part of the training before re-starting" << endl;;
			
			ProgStream.close();
			return(false);
		}
	}

	ProgStream.close();

	if (!UpdateTable(ExtremumInfo, ColumnNames, EventDateTime, TheQuery, EventType))
        {
	        cerr << "Error: Could not add event information into database for Event Type: " << EventType << endl;
                return(false);
        }

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
//		cout << '+' << LevelName << '+' << endl;

		const vector <string> TheLevWrds = TakeDelimitedList(ConfigInfo.Give_DataSource_DataLevel(CAFEVarName, LevelName), ',');
		LevelWrdCnt += TheLevWrds.size();
                LevelArgs += "'" + GiveDelimitedList(TheLevWrds, "' '") + "' ";
        }

	return("'" + DataVarName + "' " + Size_tToStr(LevelWrdCnt) + ' ' + LevelArgs);
}

bool RunPeakValExtractor(const string &ProgName, const string &CmdLineArg, const size_t VarCount, const CAFEDomain &TheDomain,
			 const string &DateTimeStr, const string &ProgOutputName)
// Could an injection attack happen here?  It would be pretty difficult, but possibly feasible.
// Not going to worry about it since I will not be using GrADs that much longer.
{
	const vector <float> TheLons = TheDomain.GiveLons();
	const vector <float> TheLats = TheDomain.GiveLats();

	const string SysCommand = ProgName + ' ' + DateTimeStr + ' ' + Size_tToStr(VarCount) + ' '
				  + FloatToStr(TheLats[0]) + ' ' + FloatToStr(TheLons[0]) + ' '
				  + FloatToStr(TheLats[1]) + ' ' + FloatToStr(TheLons[1]) + ' '
				  + CmdLineArg + " >& " + ProgOutputName;

//	cerr << SysCommand << endl;
	return(system(SysCommand.c_str()) == 0);
}
//-----------------------------------------------------------------------------------------------------------------------------


void PrintSyntax(const CmdOptions &CAFEOptions)
{
	cout << "\nAnomalyCollection " << Bold("[--offset=") << Underline("Hour_Offset") << Bold("]")
	     << " " << Bold("[--round=") << Underline("Rounding_Value") << Bold("]") << endl;

	CAFEOptions.PrintSyntax(17, 63);

	cout << "                  " << Bold("[--help] [--syntax] ") << Underline("Input_Date_File") << endl;
	cout << endl;
}

void PrintHelp(const CmdOptions &CAFEOptions)
{
	PrintSyntax(CAFEOptions);

	cout << "Collects the peaks and the valleys and stores them away..." << endl;

	cout << endl << Bold("DESCRIPTION") << endl;

	CAFEOptions.PrintDescription(63);

	cout << endl;
}

int main(int argc, char *argv[])
{
	setenv("TZ", "UTC UTC", 1);		// set's the computer's locale to UTC, so all conversions are done there.

	vector <string> CommandArgs = ProcessFlatCommandLine(argc, argv);
	CmdOptions CAFEOptions;

	if (CAFEOptions.ParseCommandArgs(CommandArgs) != 0)
	{
		cerr << "ERROR: Invalid syntax..." << endl;
		PrintSyntax(CAFEOptions);
		return(8);
	}

	string InputFile = "";
	struct TimeIncrement_t TimeOffset;		// by default, it is zero offset.
	int RoundingHourValue = 6;

	for (size_t ArgIndex = 0; ArgIndex < CommandArgs.size(); ArgIndex++)
	{
		if (CommandArgs[ArgIndex].find("--offset=") == 0)
		{
			string TempHold = CommandArgs[ArgIndex].substr(9);

			errno = 0;
			TimeOffset.HourInterval = strtol(TempHold.c_str(), NULL, 10);

			if (errno != 0)
			{
				perror("Syntax Error! (--offset=)");
				return(8);
			}
		}
		else if (CommandArgs[ArgIndex].find("--round=") == 0)
		{
			string TempHold = CommandArgs[ArgIndex].substr(8);
			
			errno = 0;
			RoundingHourValue = strtol(TempHold.c_str(), NULL, 10);

			if (errno != 0)
			{
				perror("Syntax Error! (--round=)");
				return(8);
			}

			if (RoundingHourValue <= 0)
			{
				cerr << "Syntax Error! (--round=): Rounding value cannot be zero or negative.\n";
				return(8);
			}
		}
		else if (CommandArgs[ArgIndex] == "--help")
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
			InputFile = CommandArgs[ArgIndex];
		}
	}

	if (InputFile.empty())
	{
		cerr << "\n\tImproper syntax:  must have the filename of the event list file\n\n";
		PrintSyntax(CAFEOptions);
		return(8);
	}

	const string PeakValoutputName = CAFEOptions.CAFEPath + "/scratch/tempdata.txt";
	Configuration ConfigInfo( CAFEOptions.CAFEPath + '/' + CAFEOptions.ConfigFilename );

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

	if (!ConfigInfo.SetDefaultSource("NARR"))
	{
		cerr << "ERROR: Couldn't set NARR as a default data source\n";
		return(1);
	}

	const string TheTimePeriod = CAFEOptions.GiveTimePeriod(TimeOffset.HourInterval);

	if (TheTimePeriod.empty())
	{
		cerr << "ERROR: Invalid timeperiod: " << TimeOffset.HourInterval << endl;
		return(8);
	}

	const string DatabaseName = CAFEOptions.GiveDatabaseName(TheTimePeriod);
	const string ClustDatabaseName = CAFEOptions.GiveClusteredDatabaseName(TheTimePeriod);
	const string PeakValProgName = CAFEOptions.CAFEPath + "/bin/PeakValleyPicker";

	vector <double> EventLons;
	vector <double> EventLats;
	vector <time_t> EventDates;
	vector <string> EventTypes;
	
	ifstream EventFile(InputFile.c_str());

	if (!EventFile.is_open())
	{
		cerr << "\n\tProblem opening the event file: " << InputFile << endl;
		return(9);
	}

	string LineRead;

	getline(EventFile, LineRead);

	while (!EventFile.eof())
	{
		vector <string> TempHold = TakeDelimitedList(LineRead, ',');

		if (TempHold.size() == 4)	// Date, type, lat, lon
		{
			if (ConfigInfo.GiveEventTypeIndex(TempHold[1]) == string::npos)
        	        {
				// the event type doesn't even exist in the configuration file.
                        	cerr << "ERROR: Invalid event type: " << TempHold[1] << endl;
	                        EventFile.close();
        	                return(9);
                	}
			else if (binary_search(CAFEOptions.EventTypes.begin(), CAFEOptions.EventTypes.end(), TempHold[1]))
			{
				// the event type is an event type indicated by given command 
				// line options or by the defaults (config file) if no options are given
				time_t TheDateTime = GetTime(TempHold[0]);
				TheDateTime = IncrementDateTime(TheDateTime, TimeOffset);	// by default, no change occurs
				RoundHour(TheDateTime, RoundingHourValue);			// round to the nearest RoundingHourValue hours, starting at 00Z.
				EventDates.push_back(TheDateTime);
				EventTypes.push_back(TempHold[1]);
				EventLats.push_back(StrToDouble(TempHold[2]));
				EventLons.push_back(StrToDouble(TempHold[3]));
			}
		}
		else
		{
			cerr << "ERROR: Invalid line in the event file: " << LineRead << endl
			     << "       Must be a comma delimited list of date, event type, lat, and lon.\n"
			     << "       Skipping...\n";
		}

		
		getline(EventFile, LineRead);
	}

	EventFile.close();

	cout << "Number of Events: " << EventDates.size() << endl;

	if (EventDates.empty())
	{
		cout << "No events to do!" << endl;
		return(0);
	}

	mysqlpp::Connection ServerLink;

        try
        {
		EstablishConnection(ServerLink, CAFEOptions.ServerName, CAFEOptions.LoginUserName, DatabaseName);
	}
	catch (const string &ErrStr)
	{
		cerr << "ERROR: Connection failed: " << ErrStr << endl;
		ServerLink.close();
		return(3);
	}
        catch (const exception& Err)
        {
                cerr << "ERROR: Connection failed: " << Err.what() << endl;
		ServerLink.close();
                return(3);
        }
        catch (...)
        {
                cerr << "ERROR: Unknown error..." << endl;
		ServerLink.close();
                return(4);
        }

	try
	{
		mysqlpp::Query TheQuery = ServerLink.query();

		vector <string> UniqueTypesList(0);

		for (size_t DateIndex = 0; DateIndex < EventDates.size(); DateIndex++)
		{
			if (!binary_search(UniqueTypesList.begin(), UniqueTypesList.end(), EventTypes[DateIndex]))
			{
				UniqueTypesList.insert(lower_bound(UniqueTypesList.begin(), UniqueTypesList.end(), EventTypes[DateIndex]), 
						       EventTypes[DateIndex]);
			}

			char GradDateTimeStr[13];
			memset(GradDateTimeStr, '\0', 13);
			char DateTimeStr[20];
			memset(DateTimeStr, '\0', 20);

			struct tm DateTime;
		        DateTime.tm_sec = 0;
 	        	DateTime.tm_min = 0;
        	
			localtime_r(&EventDates[DateIndex], &DateTime);
			strftime(GradDateTimeStr, 13, "%HZ%d%b%Y", &DateTime);
			strftime(DateTimeStr, 20, "%Y-%m-%d %H:%M:%S", &DateTime);

			cout << "EventType: " << EventTypes[DateIndex] << " for index: " << DateIndex << "  Date: " << DateTimeStr << endl;
			
			InsertEvent(EventDates[DateIndex], EventLons[DateIndex], EventLats[DateIndex], TheQuery, EventTypes[DateIndex]);
			
			const vector <string> CAFEVarNames = CAFEOptions.GiveCAFEVarsToDo(ConfigInfo, EventTypes[DateIndex]);
			vector <string> AllCAFEVarLabels(0);

			string CmdLineArg = "";
			
			for (vector<string>::const_iterator AVarName = CAFEVarNames.begin(); AVarName != CAFEVarNames.end(); AVarName++)
			{
				const size_t OldSize = AllCAFEVarLabels.size();
				const vector <string> CAFELabels = CAFEOptions.GiveLabelsToDo(ConfigInfo, EventTypes[DateIndex], *AVarName);
				AllCAFEVarLabels.insert(AllCAFEVarLabels.end(), CAFELabels.begin(), CAFELabels.end());

				inplace_merge(AllCAFEVarLabels.begin(), AllCAFEVarLabels.begin() + OldSize, AllCAFEVarLabels.end());
				// this is ok since CAFELabels is sorted, and are
				// unique from other calls to GiveLabelsToDo()

				// assumes that the AVarName and CAFELabels are all valid.  If not, hilarity ensues...
				CmdLineArg += MakeCmdLineArgs(*AVarName, CAFELabels, ConfigInfo) + ' ';
			}// end CAFEVar loop
			
			if (!RunPeakValExtractor(PeakValProgName, CmdLineArg, CAFEVarNames.size(), *ConfigInfo.GiveDomain(),
			                         GradDateTimeStr, PeakValoutputName))
                        {
                                throw("Unable to execute the PeakValley collection. EventIndex: " + Size_tToStr(DateIndex));
                        }

			if (!ProcessAndPrint(PeakValoutputName, AllCAFEVarLabels, EventDates[DateIndex], EventLons[DateIndex], EventLats[DateIndex],
					     EventTypes[DateIndex], TheQuery, ConfigInfo))
                        {
                                throw("Unable to ProcessAndPrint(). EventIndex: " + Size_tToStr(DateIndex));
                        }
		}// end date index loop





		ServerLink.select_db(ClustDatabaseName);
		for (vector<string>::const_iterator AnEventType = UniqueTypesList.begin(); 
		     AnEventType != UniqueTypesList.end(); 
		     AnEventType++)
		{
			TheQuery << "INSERT IGNORE " << *AnEventType << " (DateInfo, Event_Lon, Event_Lat) "
				 << "SELECT DateInfo, Event_Lon, Event_Lat FROM " << DatabaseName << '.' << *AnEventType;

			TheQuery.execute();

			if (!TheQuery.success())
			{
				throw("Unable to insert event information to the clustered database: " + ClustDatabaseName
				      + " for event type: " + *AnEventType);
			}
		}
	}
	catch (const exception &Err)
	{
		cerr << "ERROR: Exception caught.  Message: " << Err.what() << endl;
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
	return(0);
}

