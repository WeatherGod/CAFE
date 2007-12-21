#ifndef _CAFE_SQLSTRUCTS_H
#define _CAFE_SQLSTRUCTS_H

#include <mysql++/mysql++.h>
#include <mysql++/custom.h>                     // for the mysql++ structs

sql_create_3(LonLatAnom_sql, 1, 0, float, Lon, float, Lat, float, StdAnom)
sql_create_4(LonLatAnomDate_sql, 1, 0, float, Lon, float, Lat, float, StdAnom, mysqlpp::DateTime, DateInfo)
sql_create_3(LonLatCount_sql, 1, 0, float, Lon, float, Lat, long int, Count)

#endif

