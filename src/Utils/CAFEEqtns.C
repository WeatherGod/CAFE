#ifndef _CAFEEQTNS_C
#define _CAFEEQTNS_C

using namespace std;

#include <vector>
#include <algorithm>			// for binary_search(), lower_bound()
#include <cmath>                        // for pow(), exp(), sqrt(), M_2_PI
#include <DistUtly.h>                   // for GreatCircleDist()
#include <StatUtly.h>                   // for Avg(), Variance()
#include <Histogram.h>			// for the Histogram class

#define MYSQLPP_SSQLS_NO_STATICS       // makes sure that the SSQL structs are only declared, not defined.
#include "Utils/CAFE_SQLStructs.h"		// for LonLatAnom and LonLatAnomDate and LonLatCount
					// and associated helper functions.

#include "Utils/CAFEEqtns.h"



// Deprecated...but STILL IN USE!!!!
// When I redo the clustering algorithm, this will finally go away...
double PhiCalc(const vector <double> &StdAnoms, const int &TotalEventCount)
{
        if (StdAnoms.size() > 1)
	// I know that TotalEventCount > 0 since StdAnoms.size() > 1
        {
//		const double AnomStrength = pow(abs(1.5 - abs(Avg(StdAnoms))) / (1.0 + sqrt(Variance(StdAnoms))), 2.0);
        	const double AnomStrength = (1.0-sqrt(Variance(StdAnoms))) * abs(1.5 - abs(Avg(StdAnoms)));
//		const double AnomStrength = abs(1.5 - abs(Avg(StdAnoms))) / (1.0 + sqrt(Variance(StdAnoms)));

		if (AnomStrength < 0.0)
		{
			return(0.0);
		}
		else
		{
	                return(AnomStrength*(((double) StdAnoms.size())/((double) TotalEventCount)));
		}
        }
        else
        {
                return(0.0);
        }
}

double PhiCalc(const vector<LonLatAnomDate>::const_iterator &MembersBegin, const vector<LonLatAnomDate>::const_iterator &MembersEnd, 
	       const int &TotalEventCount)
{
	if ((MembersEnd - MembersBegin) > 1)
	{
//		const double AnomStrength = pow(abs(1.5 - abs(Avg(MembersBegin, MembersEnd))) / (1.0 + sqrt(Variance(MembersBegin, MembersEnd))), 2.0);
        	const double AnomStrength = (1.0-sqrt(Variance(MembersBegin, MembersEnd))) * abs(1.5 - abs(Avg(MembersBegin, MembersEnd)));
//		const double AnomStrength = abs(1.5 - abs(Avg(MembersBegin, MembersEnd))) / (1.0 + sqrt(Variance(MembersBegin, MembersEnd)));

	        if (AnomStrength < 0.0)
		{
			return(0.0);
		}
		else
		{
	        	// I know that TotalEventCount > 0 since StdAnoms.size() > 1
	                return(AnomStrength*(((double) (MembersEnd - MembersBegin))/((double) TotalEventCount)));
		}
        }
        else
        {
                return(0.0);
        }
}


double CalcChi(const Histogram &Frequency, const double &PredictedAnom)
// Assumes that the Histogram is a valid histogram.  If it isn't, problems will occur.
{
//	if (isnan(PredictedAnom))
//	{
//		return(0.0);
//	}

        const int BelongsIn = Frequency.WhichBin(PredictedAnom);

        vector <double> AveragingVals(3);
        vector <double> Weights(3);
        Weights[0] = 0.25;
        Weights[1] = 0.50;
        Weights[2] = 0.25;

        for (int AvgIndex( (BelongsIn - 1) ), Counter(0); Counter < 3; AvgIndex++, Counter++)
        {
                AveragingVals[Counter] = (double) Frequency.Freq(AvgIndex);
        }

        return(WeightedAvg(AveragingVals, Weights));
}

double CalcUpsilon(const double &Gamma, const double &Chi, const double &MaxGamma, const double &MaxChi, const double &Alpha, const double &Phi)
{
	return(  ((MaxGamma == 0.0) ? 0.0 : ((Gamma/MaxGamma)*Alpha)) 
		 + 
		 ((MaxChi == 0.0) ? 0.0 : ((Chi/MaxChi)*Phi)));
}

// Deprecated...
/*
double CalcGamma(const vector <double> &ClustLon, const vector <double> &ClustLat, const double &PredictedLon, const double &PredictedLat)
{
//	if (isnan(PredictedLon) || isnan(PredictedLat))
//	{
//		return(0.0);
//	}

        double RunningSum = 0.0;
        for (vector<double>::const_iterator ALon( ClustLon.begin() ), ALat( ClustLat.begin() ); ALon != ClustLon.end(); ALon++, ALat++)
        {
                RunningSum += exp(-pow(GreatCircleDist(*ALon, *ALat, PredictedLon, PredictedLat), 2.0)/(4.0*14792.0));
        }


        return(RunningSum);
}
*/

double CalcGamma(vector<LonLatAnom>::const_iterator MemberIter, const vector<LonLatAnom>::const_iterator &MemberEnd, 
		 const double &PredictedLon, const double &PredictedLat)
{
//	if (isnan(PredictedLon) || isnan(PredictedLat))
//	{
//		return(0.0);
//	}

        double RunningSum = 0.0;
        for (; MemberIter != MemberEnd; MemberIter++)
        {
//              RunningSum += (1.0/((GreatCircleDist(ClustLon[index], ClustLat[index], PredictedLon, PredictedLat)/150.0) + 1.0));
                RunningSum += exp(-pow(GreatCircleDist(MemberIter->Lon, MemberIter->Lat, PredictedLon, PredictedLat), 2.0)/(4.0*14792.0));
        }


        return(RunningSum);
}

// Use in conjunction with GetMemberCounts()
// This optimized version works well when you already have the vector of LonLatCounts and are using it
// Repeatedly.  The penalty of generating the member counts will wash out the gain of using the optimized CalcGamma once.
double CalcGamma_Opt(const vector <LonLatCount> &MemberCounts, const double &PredictedLon, const double &PredictedLat)
{
	double RunningSum = 0.0;
	for (vector<LonLatCount>::const_iterator AMemberCnt( MemberCounts.begin() ); AMemberCnt != MemberCounts.end(); AMemberCnt++)
	{
		RunningSum += exp(-pow(GreatCircleDist(AMemberCnt->Lon, AMemberCnt->Lat, PredictedLon, PredictedLat), 2.0)/(4.0*14792.0))
				* ((double) AMemberCnt->Count);
	}

	return(RunningSum);
}


double CalcEventScore(const vector <double> &Upsilons)
{
        return(Sum(Upsilons));
}


double AlphaCalc(const double &ClusterDensity, const double &AverageDensity, const int &ClusterMemberCount, const int &EventOccurranceCount)
{
        if (ClusterDensity != 0.0 && EventOccurranceCount != 0)
        {
                return(max(((ClusterDensity - AverageDensity)/(ClusterDensity + AverageDensity)) * 
//		return(max(((ClusterDensity - AverageDensity)/(ClusterDensity)) *
			                                                   (((double) ClusterMemberCount)/((double) EventOccurranceCount)),
                           0.0));
        }
        else
        {
                return(0.0);
        }
}


#endif
