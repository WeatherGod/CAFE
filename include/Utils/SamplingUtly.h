#ifndef _SAMPLINGUTLY_H
#define _SAMPLINGUTLY_H

#include <mysql++/mysql++.h>		// for mysqlpp::DateTime
#include <vector>
#include <string>

#define MYSQLPP_SSQLS_NO_STATICS       // makes sure that the SSQL structs are only declared, not defined.
#include "CAFE_SQLStructs.h"	// for the LonLatAnomDate type


string GetCaseFilename(const string &CAFEPath, const string &Database, const string &EventTypeName, const int &CaseNumber);
vector <mysqlpp::DateTime> LoadCaseTimes(const string &CaseFilename);

void RemoveCaseDates(vector <double> &Lons, vector <double> &Lats, vector <double> &Anoms, vector <time_t> &DateTimes,
                     const vector <time_t> &CaseDates);
void RemoveCaseDates(vector <LonLatAnomDate> &TheMembers, const vector <mysqlpp::DateTime> &CaseDates);
void RemoveCaseDates(vector <time_t> &DateTimes, const vector <time_t> &CaseDates);


#endif
