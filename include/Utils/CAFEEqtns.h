#ifndef _CAFEEQTNS_H
#define _CAFEEQTNS_H

#include <vector>
#include <Histogram.h>                  // for the Histogram class

#define MYSQLPP_SSQLS_NO_STATICS 	// makes sure that the SSQL structs are only declared, not defined.
#include "CAFE_SQLStructs.h"            // for LonLatAnom and LonLatAnomDate and LonLatCount

double CalcEventScore(const vector <double> &Upsilons);

// Deprecated...but STILL IN USE!!!!
// When I redo the clustering algorithm, this will finally go away...
double PhiCalc(const vector <double> &StdAnoms, const int &TotalEventCount);


double PhiCalc(const vector<LonLatAnomDate>::const_iterator &MembersBegin, const vector<LonLatAnomDate>::const_iterator &MembersEnd,
               const int &TotalEventCount);
double AlphaCalc(const double &ClusterDensity, const double &AverageDensity, const int &ClusterMemberCount, const int &EventOccurranceCount);


// Use in conjunction with GetMemberCounts()
// This optimized version works well when you already have the vector of LonLatCounts and are using it
// Repeatedly.  The penalty of generating the member counts will wash out the gain of using the optimized CalcGamma once.
double CalcGamma_Opt(const vector <LonLatCount> &MemberCounts, const double &PredictedLon, const double &PredictedLat);
double CalcGamma(vector<LonLatAnom>::const_iterator MemberIter, 
		 const vector<LonLatAnom>::const_iterator &MemberEnd,
                 const double &PredictedLon, const double &PredictedLat);

double CalcChi(const Histogram &Frequency, const double &PredictedAnom);


double CalcUpsilon(const double &Gamma, const double &Chi, 
		   const double &MaxGamma, const double &MaxChi, 
		   const double &Alpha, const double &Phi);

#endif
