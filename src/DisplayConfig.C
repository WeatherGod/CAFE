using namespace std;


#include <iostream>
#include <string>

#include <Config/CAFEParam.h>
#include <Config/CAFEState.h>
#include <Config/Configuration.h>



int main()
{
	const string CAFEPath = "./";
	const string configFilename = CAFEPath + '/' + "TestTableConfig";

	Configuration configInfo(configFilename);

	if (!configInfo.IsValid())
	{
		cerr << "ERROR: Invalid configuration file: " << configFilename << endl;
		return(1);
	}

	CAFEState curState( configInfo.GiveCAFEInfo() );

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
