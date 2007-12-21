// This file is an extension of the LonLatAnom.C file.
// When these items are refactored into oblivion, the clean-up process should be much easier.

// Temporary until the LonLatAnomDate structure is eliminated.
#include <ctime>

// Temporary until the LonLatCount structure is eliminated.
#include <vector>
#include <algorithm>	// for lower_bound(), binary_search()
#include <cmath>	// for powl()
#include <cctype>	// for size_t


// TODO: Refactor away.
LonLatAnomDate::LonLatAnomDate()
	:	Lon(NAN),
		Lat(NAN),
		StdAnom(NAN),
		DateInfo(-1)
{
}

// TODO: Refactor away.
LonLatAnomDate::LonLatAnomDate(const float &newLon, const float &newLat, const float &newStdAnom, const time_t &newDateInfo)
	:	Lon(newLon),
		Lat(newLat),
		StdAnom(newStdAnom),
		DateInfo(newDateInfo)
{
}

// TODO: Refactor away.
LonLatCount::LonLatCount()
        :       Lon(NAN),
                Lat(NAN),
                Count(0)
{
}

// TODO: Refactor away.
LonLatCount::LonLatCount(const float &newLon, const float &newLat, const size_t &newCount)
        :       Lon(newLon),
                Lat(newLat),
                Count(newCount)
{
}


// TODO: Refactor away
bool operator == (const LonLatAnom &MemberA, const LonLatCount &MemberB)
{
	return(MemberA.Lon == MemberB.Lon && MemberA.Lat == MemberB.Lat);
}

bool operator == (const LonLatCount &MemberA, const LonLatAnom &MemberB)
{
	return(MemberA.Lon == MemberB.Lon && MemberA.Lat == MemberB.Lat);
}

bool operator == (const LonLatAnomDate &MemberA, const LonLatAnomDate &MemberB)
{
	return(MemberA.Lon == MemberB.Lon && MemberA.Lat == MemberB.Lat
	       && MemberA.StdAnom == MemberB.StdAnom && MemberA.DateInfo == MemberB.DateInfo);
}

bool operator != (const LonLatAnomDate &MemberA, const LonLatAnomDate &MemberB)
{
	return(MemberA.Lon != MemberB.Lon || MemberA.Lat != MemberB.Lat
	       || MemberA.StdAnom != MemberB.StdAnom || MemberA.DateInfo != MemberB.DateInfo);
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
                return(NAN);
        }
}


double Avg(vector<LonLatAnomDate>::const_iterator VectIter, const vector<LonLatAnomDate>::const_iterator &VectEnd)
{
        long double RunSum = 0.0;
        const long double StartSize( (long double) (VectEnd - VectIter) );

        if (VectIter == VectEnd)
        {
                return(NAN);
        }

        for (; VectIter != VectEnd; VectIter++)
        {
                RunSum += (long double) VectIter->StdAnom;
        }

        return(RunSum / StartSize);
}



void SplitIntoVects(const vector <LonLatAnomDate> &TheMembers,
                    vector <double> &Lons, vector <double> &Lats, vector <double> &Anoms, vector <time_t> &DateTimes)
{
        Lons.resize(TheMembers.size());
        Lats.resize(TheMembers.size());
        Anoms.resize(TheMembers.size());
        DateTimes.resize(TheMembers.size());

        vector<double>::iterator ALon( Lons.begin() ), ALat( Lats.begin() ), AnAnom( Anoms.begin() );
        vector<time_t>::iterator ADate( DateTimes.begin() );

        for (vector<LonLatAnomDate>::const_iterator AMember( TheMembers.begin() ); AMember != TheMembers.end();
             AMember++, ALon++, ALat++, AnAnom++, ADate++)
        {
                *ALon = AMember->Lon;
                *ALat = AMember->Lat;
                *AnAnom = AMember->StdAnom;
                *ADate = AMember->DateInfo;
        }
}


void SplitIntoVects(const vector <LonLatAnom> &TheMembers,
                    vector <double> &Lons, vector <double> &Lats, vector <double> &Anoms)
{
        Lons.resize(TheMembers.size());
        Lats.resize(TheMembers.size());
        Anoms.resize(TheMembers.size());

        vector<double>::iterator ALon( Lons.begin() ), ALat( Lats.begin() ), AnAnom( Anoms.begin() );

        for (vector<LonLatAnom>::const_iterator AMember( TheMembers.begin() ); AMember != TheMembers.end();
             AMember++, ALon++, ALat++, AnAnom++)
        {
                *ALon = AMember->Lon;
                *ALat = AMember->Lat;
                *AnAnom = AMember->StdAnom;
        }
}

