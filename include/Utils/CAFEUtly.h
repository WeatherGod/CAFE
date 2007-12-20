#ifndef _CAFEUTLY_H
#define _CAFEUTLY_H


#include <string>
#include <vector>

#define MYSQLPP_SSQLS_NO_STATICS       // makes sure that the SSQL structs are only declared, not defined.
#include "CAFE_SQLStructs.h"		// for LonLatAnom, LonLatAnomDate

// This might go away soon...
#include "../SPAnalysis/BoardConvertor.h"
#include "../SPAnalysis/ClusterBoard.h"
#include "../Config/Configuration.h"


// Consider redesigning...
bool GetGridInfo(const Configuration &ConfigInfo, BoardConvertor &ProjectionInfo, const size_t &Radius);
bool LoadClusterBoard(ClusterBoard &EmptyBoard, const vector <LonLatAnomDate> &TheMembers,
                      const BoardConvertor &ProjectionTranslator);


LonLatAnom LoadForecast(const string &FileName, const string &DateOfEvent);
bool WriteLonLatAnoms(const vector <LonLatAnom> &TheMembers, const string &OutFileName);
bool WriteFieldMeasure(const double &Alpha, const double &Phi, 
		       const double &GammaMaxValue, const double &ChiMaxValue,
		       const string &OutFileName);

bool SaveEventScores(const string &FileName, const vector <double> &EventScores, const string &DateOfEvent = "");
bool LoadEventScores(vector <double> &EventScores, vector <string> &Dates, const string &Filename, const string &TableName);

bool SaveThresholdVals(const string &Filename, const vector <string> &TableNames, const vector <double> &ThresholdVals);
bool LoadThresholdVals(const string &Filename, vector <string> &TableNames, vector <double> &ThresholdVals);

#endif
