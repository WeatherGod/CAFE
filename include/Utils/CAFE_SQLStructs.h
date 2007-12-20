#ifndef _CAFE_SQLSTRUCTS_H
#define _CAFE_SQLSTRUCTS_H

#include <mysql++/mysql++.h>
#include <mysql++/custom.h>                     // for the mysql++ structs
#include <vector>

sql_create_3(LonLatAnom, 1, 0, float, Lon, float, Lat, float, StdAnom)
sql_create_4(LonLatAnomDate, 1, 0, float, Lon, float, Lat, float, StdAnom, mysqlpp::DateTime, DateInfo)
sql_create_3(LonLatCount, 1, 0, float, Lon, float, Lat, long int, Count)

bool operator == (const LonLatAnom &MemberA, const LonLatCount &MemberB);
bool operator == (const LonLatCount &MemberA, const LonLatAnom &MemberB);
bool operator < (const LonLatAnom &MemberA, const LonLatCount &MemberB);
bool operator < (const LonLatCount &MemberA, const LonLatAnom &MemberB);

vector <LonLatCount> GetMemberCounts(const vector <LonLatAnom> &TheMembers);
double Variance(vector<LonLatAnomDate>::const_iterator VectIter,
		const vector<LonLatAnomDate>::const_iterator &VectEnd);
double Avg(vector<LonLatAnomDate>::const_iterator VectIter,
	   const vector<LonLatAnomDate>::const_iterator &VectEnd);

#endif

