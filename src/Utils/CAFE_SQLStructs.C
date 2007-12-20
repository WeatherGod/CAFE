#ifndef _CAFE_SQLSTRUCTS_C
#define _CAFE_SQLSTRUCTS_C

using namespace std;

#include <vector>
#include <algorithm>		// for lower_bound(), binary_search()
#include <cmath>		// for powl()
#include "Utils/CAFE_SQLStructs.h"	// for LonLatAnom, LonLatCount, LonLatAnomDate

bool operator == (const LonLatAnom &MemberA, const LonLatCount &MemberB)
{
        return(MemberA.Lon == MemberB.Lon && MemberA.Lat == MemberB.Lat);
}

bool operator == (const LonLatCount &MemberA, const LonLatAnom &MemberB)
{
        return(MemberA.Lon == MemberB.Lon && MemberA.Lat == MemberB.Lat);
}

bool operator < (const LonLatAnom &MemberA, const LonLatCount &MemberB)
{
        return(MemberA.Lon < MemberB.Lon || (MemberA.Lon == MemberB.Lon && MemberA.Lat < MemberB.Lat));
}

bool operator < (const LonLatCount &MemberA, const LonLatAnom &MemberB)
{
        return(MemberA.Lon < MemberB.Lon || (MemberA.Lon == MemberB.Lon && MemberA.Lat < MemberB.Lat));
}


vector <LonLatCount> GetMemberCounts(const vector <LonLatAnom> &TheMembers)
{
        vector <LonLatCount> MemberCounts(0);

        for (vector<LonLatAnom>::const_iterator AMember( TheMembers.begin() ); AMember != TheMembers.end(); AMember++)
        {
                if (binary_search(MemberCounts.begin(), MemberCounts.end(), *AMember))
                {
                        (lower_bound(MemberCounts.begin(), MemberCounts.end(), *AMember))->Count++;
                }
                else
                {
                        LonLatCount TempCount;
                        TempCount.Lon = AMember->Lon;
                        TempCount.Lat = AMember->Lat;
                        TempCount.Count = 1;
                        MemberCounts.insert(lower_bound(MemberCounts.begin(), MemberCounts.end(), *AMember), TempCount);
                }
        }

        return(MemberCounts);
}



double Variance(vector<LonLatAnomDate>::const_iterator VectIter, const vector<LonLatAnomDate>::const_iterator &VectEnd)
{
        const size_t StartSize( VectEnd - VectIter );
        if (StartSize > 1)
        {
                long double RunSum1 = 0.0;
                long double RunSum2 = 0.0;

                for (; VectIter != VectEnd; VectIter++)
                {
                        RunSum1 += powl((long double) VectIter->StdAnom, 2.0);
                        RunSum2 += ((long double) VectIter->StdAnom);
                }

                return((RunSum1 - (powl(RunSum2, 2.0)/((long double) StartSize))) / ((long double) (StartSize - 1)));
        }
        else
        {
                return(nan("nan"));
        }
}



double Avg(vector<LonLatAnomDate>::const_iterator VectIter, const vector<LonLatAnomDate>::const_iterator &VectEnd)
{
        long double RunSum = 0.0;
        const long double StartSize( (long double) (VectEnd - VectIter) );

        if (VectIter == VectEnd)
        {
                return(nan("NAN"));
        }

        for (; VectIter != VectEnd; VectIter++)
        {
                RunSum += (long double) VectIter->StdAnom;
        }

        return(RunSum / StartSize);
}

#endif
