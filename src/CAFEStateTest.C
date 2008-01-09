using namespace std;



#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>


#include <Config/EventType.h>
#include <Config/Variable.h>
#include <Config/DataSource.h>
#include <Config/CAFEVar.h>
#include <Config/CAFEDomain.h>
#include <Config/DataVar.h>

#include <Config/CAFEParam.h>
#include <Config/CAFEState.h>




int main()
{
	cerr << "Initializing basicDataLevels...";
	map<size_t, string> basicDataLevels;
	basicDataLevels[0] = "1000 mb";
	basicDataLevels[5] = "925 mb";
	basicDataLevels[1] = "850 mb";
	basicDataLevels[2] = "700 mb";
	basicDataLevels[3] = "500 mb";
	basicDataLevels[4] = "225 mb";
	basicDataLevels[string::npos] = "sfc";
	cerr << "done\n";

	cerr << "Initializing otherDataLevels...";
	map<size_t, string> otherDataLevels;
	basicDataLevels[string::npos] = "atmos col";
	cerr << "done\n";

	cerr << "Initializing basicCAFELevels...";
	map<string, size_t> basicCAFELevels;
	basicCAFELevels["1000"] = 0;
	basicCAFELevels["850"] = 1;
	basicCAFELevels["700"] = 2;
	basicCAFELevels["925"] = 5;
	basicCAFELevels["500"] = 3;
	basicCAFELevels["225"] = 4;
	cerr << "done\n";

	cerr << "Initializing emptyCAFELevels...";
	map<string, size_t> emptyCAFELevels;
	cerr << "done\n";

	cerr << "Initializing scalars...";
	const int verbosity = 5;
	const string configFilename = "newTestConfig";
	const string CAFEPath = "/something/CAFE";
	const string loginUserName = "bvroot";
	const string CAFEUserName = "CAFEUser";
	const string serverName = "narr.met.psu.edu";
	const string untrainedNameStem = "RawFields";
	const string trainedNameStem = "Clustered";
	const string defaultDataSourceName = "TestSource";
	cerr << "done\n";

	cerr << "Initializing time offsets...";
	set<int> newTimeOffsets;
	newTimeOffsets.insert(newTimeOffsets.begin(), 0);
	newTimeOffsets.insert(newTimeOffsets.begin(), 6);
	newTimeOffsets.insert(newTimeOffsets.begin(), -12);
	cerr << "done\n";

	cerr << "Initializing data info...";
	map<string, DataVar> newDataInfo;
	newDataInfo["UWnd"] = DataVar("uwnd", "UWnd", basicDataLevels);
	newDataInfo["VWnd"] = DataVar("vwnd", "VWnd", basicDataLevels);
	newDataInfo["Temp"] = DataVar("air", "Temp", basicDataLevels);
	newDataInfo["SpecHum"] = DataVar("humid", "SpecHum", basicDataLevels);
	newDataInfo["PWAT"] = DataVar("pr_wtr", "PWAT", otherDataLevels);
	cerr << "done\n";

	cerr << "Initializing data source...";
	DataSource newDataSource("TestSource", "", "", newDataInfo, -1, -1);
	cerr << "done\n";

	cerr << "Initializing new data sources...";
	map<string, DataSource> newDataSources;
	newDataSources["TestSource"] = newDataSource;
	cerr << "done\n";

	cerr << "Initializing domain...";
	CAFEDomain newDomain(30.0, -97.5, 43.5, -58.0);
	cerr << "done\n";

	cerr << "Initializing CAFE Vars...";
	map<string, CAFEVar> newCAFEVars;
	newCAFEVars["VWnd"] = CAFEVar("VWnd", basicCAFELevels);
	newCAFEVars["UWnd"] = CAFEVar("UWnd", basicCAFELevels);
	newCAFEVars["PWAT"] = CAFEVar("PWAT", emptyCAFELevels);
	newCAFEVars["SpecHum"] = CAFEVar("SpecHum", basicCAFELevels);
	newCAFEVars["Temp"] = CAFEVar("Temp", basicCAFELevels);
	cerr << "done\n";

	cerr << "Initializing cafe levels 1...";
	vector<string> cafeLevels_1(3);
	cafeLevels_1[0] = "1000";
	cafeLevels_1[1] = "850";
	cafeLevels_1[2] = "500";
	cerr << "done\n";

	cerr << "Initializing cafe levels 2...";
	vector<string> cafeLevels_2(2);
	cafeLevels_2[0] = "925";
	cafeLevels_2[1] = "500";
	cerr << "done\n";

	cerr << "Initializing cafe levels 3...";
	vector<string> cafeLevels_3(0);
	cerr << "done\n";

	cerr << "Initializing event vars 1...";
	map<string, Variable> eventVars_1;
	eventVars_1["PWAT"] = Variable("PWAT", cafeLevels_3);
	eventVars_1["VWnd"] = Variable("VWnd", cafeLevels_1);
	eventVars_1["UWnd"] = Variable("UWnd", cafeLevels_1);
	cerr << "done\n";

	cerr << "Initializing event vars 2...";
	map<string, Variable> eventVars_2;
	eventVars_2["UWnd"] = Variable("UWnd", cafeLevels_2);
	eventVars_2["SpecHum"] = Variable("SpecHum", cafeLevels_3);
	eventVars_2["VWnd"] = Variable("VWnd", cafeLevels_2);
	eventVars_2["Temp"] = Variable("Temp", cafeLevels_1);
	cerr << "done\n";

	cerr << "Initializing event vars 3...";
	map<string, Variable> eventVars_3;
	eventVars_3["PWAT"] = Variable("PWAT", cafeLevels_3);
	eventVars_3["Temp"] = Variable("Temp", cafeLevels_2);
	cerr << "done\n";

	cerr << "Initializing event types...";
	map<string, EventType> newEventTypes;
	newEventTypes["Snow"] = EventType("Snow", eventVars_2);
	newEventTypes["Cold"] = EventType("Cold", eventVars_3);
	newEventTypes["Heat"] = EventType("Heat", eventVars_3);
	newEventTypes["TWind"] = EventType("TWind", eventVars_1);
	cerr << "done\n";

	cerr << "Initializing extremum names...";
	vector<string> extremumNames(4);
	extremumNames[0] = "Peak1";
	extremumNames[1] = "Peak2";
	extremumNames[2] = "Valley1";
	extremumNames[3] = "Valley2";
	cerr << "done\n";



	cerr << "Initializing cafeInfo...";
	CAFEParam cafeInfo;
	cafeInfo.SetVerboseLevel(verbosity)
		.SetConfigFilename(configFilename)
		.SetCAFEPath(CAFEPath)
		.SetLoginUserName(loginUserName)
		.SetCAFEUserName(CAFEUserName)
		.SetServerName(serverName)
		.SetTimeOffsets(newTimeOffsets)
		.SetUntrainedNameStem(untrainedNameStem)
		.SetTrainedNameStem(trainedNameStem)
		.SetDataSources(newDataSources)
		.SetDefaultDataSource(defaultDataSourceName)
		.SetCAFEDomain(newDomain)
		.SetCAFEVars(newCAFEVars)
		.SetEventTypes(newEventTypes)
		.SetExtremumNames(extremumNames);
	cerr << "done\n";

	cerr << "Initializing curState...";
	CAFEState curState(cafeInfo);
	cerr << "done\n";

	cout << "CAFE State\n"
	     << "==========\n"
	     << "Verbose Level: " << curState.GetVerboseLevel() << '\n'
	     << "Conf Filename: " << curState.GetConfigFilename() << '\n'
	     << "CAFE Path    : " << curState.GetCAFEPath() << '\n'
	     << "Login Name   : " << curState.GetLoginUserName() << '\n'
	     << "CAFEUserName : " << curState.GetCAFEUserName() << '\n'
	     << "ServerName   : " << curState.GetServerName() << '\n'
	     << "TimePeriods  : " << curState.TimePeriods_Size() << '\n'
	     << "Offset\t Name\t Untrained\t Trained\n";
	
	for (curState.TimePeriods_Begin(); curState.TimePeriods_HasNext(); curState.TimePeriods_Next())
	{
		cout << curState.TimePeriod_Offset() << "\t\t "
		     << curState.TimePeriod_Name() << "\t "
		     << curState.Untrained_Name() << "\t "
		     << curState.Trained_Name() << '\n';
	}

	cout << "\nEventTypes: " << curState.EventTypes_Size() << '\n';

	for (curState.EventTypes_Begin(); curState.EventTypes_HasNext(); curState.EventTypes_Next())
	{
		cout << '\t' << curState.EventType_Name() << "\t EventVars: " << curState.EventVars_Size() << '\n';

		for (curState.EventVars_Begin(); curState.EventVars_HasNext(); curState.EventVars_Next())
		{
			cout << "\t\t" << curState.EventVar_Name() << "\t " << curState.DataVar_Name() 
			     << "\t Fields: " << curState.CAFELevels_Size() << '\n';

			for (curState.CAFELevels_Begin(); curState.CAFELevels_HasNext(); curState.CAFELevels_Next())
			{
				cout << "\t\t\t" << curState.CAFEField_Name() << "\t " << curState.DataLevel_Name() << '\n';
			}

			cout << '\n';
		}

		cout << '\n';
	}

	return(0);
}	
