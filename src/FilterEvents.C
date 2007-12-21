// This is just a utility program meant to help separate a date file into two seasons.
// This program isn't that important, so don't worry too much about it.


using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <cctype>			// for size_t

#include <StrUtly.h>			// for TakeDelimitedList(), GiveDelimitedList()


int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		cerr << "Incorrect syntax.  Please include the source file and the destination filestem." << endl;
		return(1);
	}

	ifstream EventListIn(argv[1]);

	if (!EventListIn.is_open())
	{
		cerr << "Could not open the input eventlist: " << argv[1] << endl;
		return(2);
	}

	string SummerFile = (string) argv[2] + "_Summer.dat";
	string WinterFile = (string) argv[2] + "_Winter.dat";
	size_t SummerEventCount = 0;
	size_t WinterEventCount = 0;

	ofstream EventListSummer(SummerFile.c_str());

	if (!EventListSummer.is_open())
	{
		cerr << "Could not open the summer eventlist for writing: " << SummerFile << endl;
		return(3);
	}

	ofstream EventListWinter(WinterFile.c_str());

	if (!EventListWinter.is_open())
	{
		cerr << "Could not open the winter eventlist for writing: " << WinterFile << endl;
		return(3);
	}

	string LineRead = "";
	getline(EventListIn, LineRead);

	while (!EventListIn.eof())
	{
		vector <string> TempHold = TakeDelimitedList(LineRead, ',');
		struct tm TheDateTime;

		strptime(TempHold[0].c_str(), "%Y-%m-%d %H:%M:%S", &TheDateTime);

		if ((TheDateTime.tm_mon > 4 && TheDateTime.tm_mon < 10) ||
		    (TheDateTime.tm_mon == 4 && TheDateTime.tm_mday >= 15) ||
		    (TheDateTime.tm_mon == 10 && TheDateTime.tm_mday < 15))
		{
			SummerEventCount++;
			TempHold[1] += "_Summer";
			EventListSummer << GiveDelimitedList(TempHold, ',') << endl;
		}
		else
		{
			WinterEventCount++;
			TempHold[1] += "_Winter";
			EventListWinter << GiveDelimitedList(TempHold, ',') << endl;
		}

		getline(EventListIn, LineRead);
	}

	EventListIn.close();
	EventListWinter.close();
	EventListSummer.close();

	cout << "Summer Event Count: " << SummerEventCount << "       Winter Event Count: " << WinterEventCount;
	cout << endl;

	return(0);
}
