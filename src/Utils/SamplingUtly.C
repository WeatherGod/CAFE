#ifndef _SAMPLINGUTLY_C
#define _SAMPLINGUTLY_C

using namespace std;

#include <fstream>
#include <vector>
#include <algorithm>			// for lower_bound() and binary_search()
#include <string>
#include <ctime>
#include <StrUtly.h>			// for IntToStr()

#include "Utils/LonLatAnom.h"		// for LonLatAnomDate structure
#include "Utils/CAFE_SQLUtly.h"		// for DateTimeToTime_t() and Time_tToDateTime()


string GetCaseFilename(const string &CAFEPath, const string &Database, const string &EventTypeName, const int &CaseNumber)
{
	return(CAFEPath + "/AnalysisInfo/" + Database + '/' + EventTypeName + "_OutOfSample_Fold" + IntToStr(CaseNumber));
}

vector <time_t> LoadCaseTimes(const string &CaseFilename)
{
        ifstream CaseStream( CaseFilename.c_str() );

        if (!CaseStream.is_open())
        {
                throw("Could not open the OutOfSample file: " + CaseFilename + " for reading...");
        }

        vector <time_t> CaseDates(0);
	time_t TimeRead;
	
        CaseStream >> TimeRead;

        while (!CaseStream.eof())
        {
		CaseDates.insert(lower_bound(CaseDates.begin(), CaseDates.end(), TimeRead), TimeRead);
                CaseStream >> TimeRead;
        }

        CaseStream.close();

        return(CaseDates);
}



// TODO: Refactor these so that I use std::set instead.  Would be a whole lot more efficient.
void RemoveCaseDates(vector <double> &Lons, vector <double> &Lats, vector <double> &Anoms, vector <time_t> &DateTimes,
                     const vector <time_t> &CaseDates)
{
        vector<double>::iterator ALon( Lons.begin() ), ALat( Lats.begin() ), AnAnom( Anoms.begin() );
        for (vector<time_t>::iterator AnEventTime = DateTimes.begin(); 
	     AnEventTime != DateTimes.end();
             /* no increments here since I will increment myself */)
        {
                if (binary_search(CaseDates.begin(), CaseDates.end(), *AnEventTime))
                {
                        // Lets take them out!
                        ALon = Lons.erase( ALon );
                        ALat = Lats.erase( ALat );
                        AnAnom = Anoms.erase( AnAnom );
                        AnEventTime = DateTimes.erase( AnEventTime );
                }
                else
                {
                        ALon++;
                        ALat++;
                        AnAnom++;
                        AnEventTime++;
                }
        }
}


void RemoveCaseDates(vector <LonLatAnomDate> &TheMembers, const vector <time_t> &CaseDates)
{
        for (vector<LonLatAnomDate>::iterator AMember( TheMembers.begin() ); AMember != TheMembers.end();)
        /* no increments here since I will increment myself */
        {
                if (binary_search(CaseDates.begin(), CaseDates.end(), AMember->DateInfo))
                {
                        // Lets take out the member!
                        AMember = TheMembers.erase( AMember );
                }
                else
                {
                	AMember++;
                }
        }
}




void RemoveCaseDates(vector <time_t> &DateTimes, const vector <time_t> &CaseDates)
{
        for (vector<time_t>::iterator AnEventTime = DateTimes.begin(); AnEventTime != DateTimes.end(); )
        /* no increments here since I will increment myself */
        {
                if (binary_search(CaseDates.begin(), CaseDates.end(), *AnEventTime))
                {
                        // Lets take them out!
                        AnEventTime = DateTimes.erase( AnEventTime );
                }
                else
                {
                        AnEventTime++;
                }
        }
}


#endif
