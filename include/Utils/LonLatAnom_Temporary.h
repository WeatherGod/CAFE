// This file is an extension to the LonLatAnom.h file.
// Everything in here is supposed to be refactored into oblivion.
// Putting it in a separate file makes clean-up easier.


// This include is only temporary until the LonLatAnomDate structure is eliminated.
#include <ctime>		// for time_t

// These includes are only temporary until the LonLatCount structure is eliminated.
#include <cctype>		// for size_t
#include <vector>		// for the vector::iterator<>


// This structure is just a temporary.  This will be refactored into oblivion.
// TODO: Refactor away.
struct LonLatAnomDate
{
	LonLatAnomDate();
	LonLatAnomDate(const float &newLon, const float &newLat, const float &newStdAnom, const time_t &newDateInfo);
	
	float Lon;
	float Lat;
	float StdAnom;
	time_t DateInfo;
};

// This structure is just a temporary.  This will be refactored into oblivion.
// TODO: Refactor away.
struct LonLatCount
{
	LonLatCount();
	LonLatCount(const float &newLon, const float &newLat, const size_t &newCount);

	float Lon;
	float Lat;
	size_t Count;
};

// These are also going to be refactored away.
// TODO: Refactor away.
bool operator == (const LonLatAnom &MemberA, const LonLatCount &MemberB);
bool operator == (const LonLatCount &MemberA, const LonLatAnom &MemberB);
bool operator == (const LonLatAnomDate &MemberA, const LonLatAnomDate &MemberB);
bool operator != (const LonLatAnomDate &MemberA, const LonLatAnomDate &MemberB);
bool operator < (const LonLatAnom &MemberA, const LonLatCount &MemberB);
bool operator < (const LonLatCount &MemberA, const LonLatAnom &MemberB);

vector <LonLatCount> GetMemberCounts(const vector <LonLatAnom> &TheMembers);
double Variance(vector<LonLatAnomDate>::const_iterator VectIter,
               const vector<LonLatAnomDate>::const_iterator &VectEnd);
double Avg(vector<LonLatAnomDate>::const_iterator VectIter,
          const vector<LonLatAnomDate>::const_iterator &VectEnd);

void SplitIntoVects(const vector <LonLatAnomDate> &TheMembers,
                    vector <double> &Lons, vector <double> &Lats, vector <double> &Anoms, vector <time_t> &DateTimes);
void SplitIntoVects(const vector <LonLatAnom> &TheMembers,
                    vector <double> &Lons, vector <double> &Lats, vector <double> &Anoms);
