using namespace std;

#include <fstream>
#include <iostream>	// for printing error messages
#include <vector>
#include <cctype>	// for size_t
#include <cmath>	// for NAN
#include <string>

#include <StrUtly.h>	// TakeDelimitedList(), StrToDouble()

#include "Utils/LonLatAnom.h"		// for LonLatAnom structure
#include "Utils/PeakValleyFile.h"


PeakValleyFile::PeakValleyFile(const char* filename, ios::openmode theOpenmode)
	:	fstream(filename, theOpenmode)
{
}

PeakValleyFile::~PeakValleyFile()
{
	close();
}

vector<LonLatAnom>
PeakValleyFile::RetrieveExtrema(const size_t &variableCnt, const size_t &extremumCnt)
{
	string lineRead = "";
	vector<LonLatAnom> extremaInfo(0);
	extremaInfo.reserve(variableCnt * extremumCnt);

	for (size_t variableIndex = 0; variableIndex < variableCnt && good(); variableIndex++)
	{
		bool foundStart = false;

		std::getline(*this, lineRead);
		while (!foundStart && good())
		{
			if (lineRead.find("---^^^---Start") == 0)
			{
				foundStart = true;
			}

			std::getline(*this, lineRead);
		}

		if (!foundStart)
		{
			// This will eventually become an exception throwing
			cerr << "ERROR: Could not find data in the program output file.\n";
			extremaInfo.clear();
			return(extremaInfo);
		}

		// At this point, the read head for the file will be
		// on the line after the "---^^^---Start" line.

		for (size_t extremumIndex = 0; extremumIndex < extremumCnt && good(); extremumIndex++)
		{
			if (lineRead.find("---^^^---End") == string::npos)
			{
				vector<string> lineList = TakeDelimitedList(lineRead, ' ');

				if (lineList.size() < 3)
				{
					cerr << "ERROR: Problem reading the peakvalley file.\n";
					cerr << "       The line does not have the correct number of data fields:\n";
					cerr << lineRead << endl;
					extremaInfo.clear();
					return(extremaInfo);
				}

				// Some programs don't support NANs, so I can accept
				// a magic value of 999 or -999 for the standard anomaly data field
				if (lineList[0] == "999" || lineList[0] == "-999")
				{
					lineList[0] = "NAN";
					lineList[1] = "NAN";
					lineList[2] = "NAN";
				}

				extremaInfo.push_back(LonLatAnom(StrToDouble(lineList[1]),
								 StrToDouble(lineList[2]),
								 StrToDouble(lineList[0])));

/*
				// Standard Anomaly
				extremaInfo[posOffset].StdAnom = StrToDouble(lineList[0]);
				// Longitude
				extremaInfo[posOffset].Lon = StrToDouble(lineList[1]);
				// Latitude
				extremaInfo[posOffset].Lat = StrToDouble(lineList[2]);
*/
			}
			else
			{
				cerr << "ERROR: Problem reading the peakvalley file.\n";
				cerr << "       The end came sooner than expected.  The next line may have additional info.\n";
				cerr << lineRead << endl;
				extremaInfo.clear();
				return(extremaInfo);
			}

			std::getline(*this, lineRead);
		}// end extremum loop
	} // end of variable loop


	if (bad() || !eof())
	{
		cerr << "ERROR: Problem with reading peakvalley file.\n";
		extremaInfo.clear();
	}

	return(extremaInfo);
}


bool
PeakValleyFile::SaveExtrema(const vector<LonLatAnom> &extremaInfo, const size_t &variableCnt, const size_t &extremumCnt)
// This function is likely to become refactored.  So don't rely on its existance.
{
	if (extremaInfo.size() != (variableCnt * extremumCnt))
	{
		cerr << "ERROR: Size mismatch!  The number of extrema information (" << extremaInfo.size() << ") does not\n"
		     << "       match that which is expected (" << variableCnt * extremumCnt << ").\n";
		return(false);
	}

	size_t posOffset = 0;
	for (size_t variableIndex = 0; variableIndex < variableCnt && !fail(); variableIndex++)
	{
		*this << "---^^^---Start\n";

		for (size_t extremumIndex = 0; extremumIndex < extremumCnt; extremumIndex++, posOffset++)
		{
			*this << extremaInfo[posOffset].StdAnom << ' '
			      << extremaInfo[posOffset].Lon << ' '
			      << extremaInfo[posOffset].Lat << '\n';
		}

		*this << "---^^^---End\n";
	}

	return(good());
}
