#ifndef _SAMPLINGUTLY_H
#define _SAMPLINGUTLY_H

#include <ctime>
#include <vector>
#include <string>

#include "Utils/LonLatAnom.h"		// for the LonLatAnomDate structure


string GetCaseFilename(const string &CAFEPath, const string &Database, const string &EventTypeName, const int &CaseNumber);
vector <time_t> LoadCaseTimes(const string &CaseFilename);

void RemoveCaseDates(vector <double> &Lons, vector <double> &Lats, vector <double> &Anoms, vector <time_t> &DateTimes,
                     const vector <time_t> &CaseDates);
void RemoveCaseDates(vector <LonLatAnomDate> &TheMembers, const vector <time_t> &CaseDates);
void RemoveCaseDates(vector <time_t> &DateTimes, const vector <time_t> &CaseDates);


#endif
