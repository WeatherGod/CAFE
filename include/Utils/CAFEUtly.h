#ifndef _CAFEUTLY_H
#define _CAFEUTLY_H


#include <string>
#include <vector>

#include "Utils/LonLatAnom.h"		// for LonLatAnom, LonLatAnomDate structures

// TODO: Refactor this need away.
#include "SPAnalysis/BoardConvertor.h"
#include "SPAnalysis/ClusterBoard.h"
#include "Config/CAFEDomain.h"

#include <Projection_t.h>


// TODO: Consider redesigning...
bool GetGridInfo(const Projection_t* theProjection, const CAFEDomain &theDomain,
		 BoardConvertor &ProjectionInfo, const size_t &Radius);
bool LoadClusterBoard(ClusterBoard &EmptyBoard, const vector <LonLatAnomDate> &TheMembers,
                      const BoardConvertor &ProjectionTranslator);

// TODO: Temporary, I think
string OffsetToTimePeriod(const int &timeOffset);
int TimePeriodToOffset(const string &timePeriod);


LonLatAnom LoadForecast(const string &FileName, const string &DateOfEvent);
bool WriteLonLatAnoms(const vector <LonLatAnom> &TheMembers, const string &OutFileName);

// TODO: Adopt the use of the FieldMeasure structure.
bool WriteFieldMeasure(const double &Alpha, const double &Phi, 
		       const double &GammaMaxValue, const double &ChiMaxValue,
		       const string &OutFileName);

bool SaveEventScores(const string &FileName, const vector <double> &EventScores, const string &DateOfEvent = "");
bool LoadEventScores(vector <double> &EventScores, vector <string> &Dates, const string &Filename, const string &TableName);

bool SaveThresholdVals(const string &Filename, const vector <string> &TableNames, const vector <double> &ThresholdVals);
bool LoadThresholdVals(const string &Filename, vector <string> &TableNames, vector <double> &ThresholdVals);

#endif
