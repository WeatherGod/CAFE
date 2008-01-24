#ifndef _CAFE_SQLUTLY_C
#define _CAFE_SQLUTLY_C

using namespace std;

#include <vector>
#include <set>
#include <string>
#include <mysql++/mysql++.h>

#include "Utils/LonLatAnom.h"		// for LonLatAnom, LonLatAnomDate, and LonLatCount structure

#define MYSQLPP_SSQLS_NO_STATICS       // makes sure that the SSQL structs are only declared, not defined.
#include "Utils/CAFE_SQLStructs.h"			// for LonLatAnom_sql and LonLatAnomDate_sql and LonLatCount_sql

#include <cmath>
#include <ctime>
#include <unistd.h>				// for getpass()

#include <StrUtly.h>
#include <TimeUtly.h>				// for GiveTimeUTC()

#include "Config/CAFEState.h"

#include "SPAnalysis/ClusterBoard.h"
#include "SPAnalysis/BoardConvertor.h"
#include "SPAnalysis/BoardType.h"

#include "Utils/CAFE_SQLUtly.h"



void EstablishConnection(mysqlpp::Connection &ConnectLink, const string &HostName, 
			 const string &UserName, const string &DatabaseName, const bool NeedPass)
{
	if (NeedPass)
	{
		char* ThePass = getpass(("Enter MySQL password for user " + UserName + " on host " + HostName + ": ").c_str());

		if (ThePass == NULL)
        	{
                	throw((string) "PASSWORD ERROR!");
	        }

		try
        	{
                	ConnectLink.connect(DatabaseName.c_str(), HostName.c_str(), UserName.c_str(), ThePass);
	        }
        	catch (...)
	        {
        	        while (*ThePass != '\0')
                	{
                        	*ThePass++ = '\0';
	                }

        	        throw;
        	}

	        while (*ThePass != '\0')
        	{
                	*ThePass++ = '\0';
        	}
	}
	else
	{
		ConnectLink.connect(DatabaseName.c_str(), HostName.c_str(), UserName.c_str(), "");
	}
}


//-------------------------------------------------------------------------------------------------------------------------------------
//*************************************************************************************************************************************
//-------------------------------------------------------------------------------------------------------------------------------------

time_t DateTimeToTime_t(const mysqlpp::DateTime &SQLTime)
// warning, it will assume that the time being given is the local time.
{
	struct tm ThisTime;
        ThisTime.tm_sec = SQLTime.second;
        ThisTime.tm_min = SQLTime.minute;
        ThisTime.tm_hour = SQLTime.hour;
        ThisTime.tm_mday = SQLTime.day;
        ThisTime.tm_mon = ((int) SQLTime.month - 1);
        ThisTime.tm_year = (SQLTime.year - 1900);


	return(mktime(&ThisTime));
}

mysqlpp::DateTime Time_tToDateTime(const time_t &TheTime)
// warning, this uses localtime_r(), so be careful!
// TODO: Double-check documentation to see if it still does that in the DateTime constructor
{
	mysqlpp::DateTime SQLTime(TheTime);
/*
	struct tm ThisTime;
	localtime_r(&TheTime, &ThisTime);

	SQLTime.second = ThisTime.tm_sec;
	SQLTime.minute = ThisTime.tm_min;
	SQLTime.hour = ThisTime.tm_hour;
	SQLTime.day = ThisTime.tm_mday;
	SQLTime.month = ThisTime.tm_mon + 1;
	SQLTime.year = ThisTime.tm_year + 1900;
*/
	return(SQLTime);
}

string DateTimeToStr(const mysqlpp::DateTime &SQLTime)
{
	char DateStr[20];
	memset(DateStr, '\0', 20);
	if (snprintf(DateStr, 20, "%0.4i-%0.2i-%0.2i %0.2i:%0.2i:%0.2i",
		    SQLTime.year,
		    (int) SQLTime.month,
		    (int) SQLTime.day,
		    (int) SQLTime.hour,
		    (int) SQLTime.minute,
		    (int) SQLTime.second) < 0)
	{
		cerr << "WARNING: Bad conversion of time: " << SQLTime << endl;
		return("0000-00-00 00:00:00");
	}

	return((string) DateStr);
}

// Temporary code...
vector <time_t> SplitIntoTime(const vector <LonLatAnomDate> &TheMembers)
{
	vector <time_t> TheTimes(TheMembers.size());

	vector<LonLatAnomDate>::const_iterator AMember( TheMembers.begin() );
	for (vector<time_t>::iterator ATime( TheTimes.begin() ); ATime != TheTimes.end(); ATime++, AMember++)
	{
		*ATime = AMember->DateInfo;
	}

	return(TheTimes);
}



mysqlpp::Query MakeLoader_LonLatAnoms(mysqlpp::Connection &TheConnection)
{
	mysqlpp::Query TheQuery = TheConnection.query();

	try
	{
		TheQuery << "SELECT %0_Lon AS Lon, %0_Lat AS Lat, %0_StdAnom AS StdAnom FROM %1:table WHERE %0_Lon IS NOT NULL";
		TheQuery.parse();
	}
	catch (mysqlpp::Exception &Err)
	{
		cerr << "ERROR: Exception caught: " << Err.what() << endl;
		cerr << "query: " << TheQuery.preview() << endl;
		throw;
	}
	catch (...)
	{
		cerr << "ERROR: Unknown exception caught." << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
	}

	return(TheQuery);
}


LonLatAnom SQLtoCAFE(const LonLatAnom_sql &sqlValue)
{
	return(LonLatAnom(sqlValue.Lon, sqlValue.Lat, sqlValue.StdAnom));
}

LonLatAnomDate SQLtoCAFE(const LonLatAnomDate_sql &sqlValue)
{
        return(LonLatAnomDate(sqlValue.Lon, sqlValue.Lat, sqlValue.StdAnom, DateTimeToTime_t(sqlValue.DateInfo)));
}

LonLatCount SQLtoCAFE(const LonLatCount_sql &sqlValue)
{
        return(LonLatCount(sqlValue.Lon, sqlValue.Lat, sqlValue.Count));
}

LonLatAnom_sql CAFEtoSQL(const LonLatAnom &cafeValue)
{
        return((LonLatAnom_sql)(cafeValue.Lon, cafeValue.Lat, cafeValue.StdAnom));
}

LonLatAnomDate_sql CAFEtoSQL(const LonLatAnomDate &cafeValue)
{
        return((LonLatAnomDate_sql)(cafeValue.Lon, cafeValue.Lat, cafeValue.StdAnom, Time_tToDateTime(cafeValue.DateInfo)));
}

LonLatCount_sql CAFEtoSQL(const LonLatCount &cafeValue)
{
        return((LonLatCount_sql)(cafeValue.Lon, cafeValue.Lat, cafeValue.Count));
}


vector<LonLatAnom> SQLtoCAFE(const vector<LonLatAnom_sql> &sqlVals)
{
	vector<LonLatAnom> cafeVals(sqlVals.size());
	for (size_t index = 0; index < sqlVals.size(); index++)
	{
		cafeVals[index] = SQLtoCAFE(sqlVals[index]);
	}

	return(cafeVals);
}

vector<LonLatAnomDate> SQLtoCAFE(const vector<LonLatAnomDate_sql> &sqlVals)
{
        vector<LonLatAnomDate> cafeVals(sqlVals.size());
        for (size_t index = 0; index < sqlVals.size(); index++)
        {
                cafeVals[index] = SQLtoCAFE(sqlVals[index]);
        }

        return(cafeVals);
}

vector<LonLatCount> SQLtoCAFE(const vector<LonLatCount_sql> &sqlVals)
{
        vector<LonLatCount> cafeVals(sqlVals.size());
        for (size_t index = 0; index < sqlVals.size(); index++)
        {
                cafeVals[index] = SQLtoCAFE(sqlVals[index]);
        }

        return(cafeVals);
}

vector<LonLatAnom_sql> CAFEtoSQL(const vector<LonLatAnom> &cafeVals)
{
        vector<LonLatAnom_sql> sqlVals(cafeVals.size());
        for (size_t index = 0; index < cafeVals.size(); index++)
        {
                sqlVals[index] = CAFEtoSQL(cafeVals[index]);
        }

        return(sqlVals);
}

vector<LonLatAnomDate_sql> CAFEtoSQL(const vector<LonLatAnomDate> &cafeVals)
{
        vector<LonLatAnomDate_sql> sqlVals(cafeVals.size());
        for (size_t index = 0; index < cafeVals.size(); index++)
        {
                sqlVals[index] = CAFEtoSQL(cafeVals[index]);
        }

        return(sqlVals);
}

vector<LonLatCount_sql> CAFEtoSQL(const vector<LonLatCount> &cafeVals)
{
        vector<LonLatCount_sql> sqlVals(cafeVals.size());
        for (size_t index = 0; index < cafeVals.size(); index++)
        {
                sqlVals[index] = CAFEtoSQL(cafeVals[index]);
        }

        return(sqlVals);
}



vector <LonLatAnom> LoadLonLatAnoms(mysqlpp::Query &TheQuery, const string &FieldStem)
{
//        TheQuery << "select " << FieldStem << "_Lon as Lon, " << FieldStem << "_Lat as Lat, " << FieldStem << "_StdAnom as StdAnom"
//                 << " from " << EventTypeName << " where " << FieldStem << "_Lon IS NOT NULL";
        vector <LonLatAnom_sql> TheResults;

	try
	{

        TheQuery.storein(TheResults, FieldStem);

	if (!TheQuery.success())
	{
		throw("Problem loading lonlats for field " + FieldStem + " and event type " + TheQuery.def["table"] 
		      + "\nMySQL message: " + TheQuery.error());
	}

	}
	catch (...)
	{
		cerr << "ERROR: Problem loading lonlats for field " + FieldStem + " and event type " + TheQuery.def["table"]
			+ "\nTheQuery: " << TheQuery.preview(FieldStem) << endl;
		throw;
	}

	return(SQLtoCAFE(TheResults));
}


mysqlpp::Query MakeLoader_LonLatAnomDates(mysqlpp::Connection &TheConnection)
{

	mysqlpp::Query TheQuery = TheConnection.query();

	try
	{
        	TheQuery << "SELECT %0_Lon AS Lon, %0_Lat AS Lat, %0_StdAnom AS StdAnom, DateInfo FROM %1:table WHERE %0_Lon IS NOT NULL";
	        TheQuery.parse();
	}
	catch (mysqlpp::Exception &Err)
        {
                cerr << "ERROR: Exception caught: " << Err.what() << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }
        catch (...)
        {
                cerr << "ERROR: Unknown exception caught." << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }

	return(TheQuery);
}


vector <LonLatAnomDate> LoadLonLatAnomDates(mysqlpp::Query &TheQuery, const string &FieldStem)
{
//        TheQuery << "select " << FieldStem << "_Lon as Lon, " << FieldStem << "_Lat as Lat, " << FieldStem << "_StdAnom as StdAnom, "
//		 << "DateInfo from " << EventTypeName << " where " << FieldStem << "_Lon IS NOT NULL";

        vector <LonLatAnomDate_sql> TheResults;

        TheQuery.storein(TheResults, FieldStem);

	if (!TheQuery.success())
	{
		throw("Problem loading lonlats for field " + FieldStem + " and event type " + TheQuery.def["table"] + "\nMySQL message: " + TheQuery.error());
	}

        return(SQLtoCAFE(TheResults));
}

mysqlpp::Query MakeLoader_MemberCnt(mysqlpp::Connection &TheConnection)
{
	
        mysqlpp::Query TheQuery = TheConnection.query();

	try
	{
	        TheQuery << "SELECT COUNT(DateInfo) AS MemberCount FROM %1:table WHERE %0_Lon IS NOT NULL";
        	TheQuery.parse();
	}
	catch (mysqlpp::Exception &Err)
        {
                cerr << "ERROR: Exception caught: " << Err.what() << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }
        catch (...)
        {
                cerr << "ERROR: Unknown exception caught." << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }

	return(TheQuery);
}


size_t LoadMemberCnt(mysqlpp::Query &TheQuery, const string &FieldStem, const string &TableName)
// FieldStem refers to the stem with the extremum name.
{
//        TheQuery << "SELECT COUNT(DateInfo) AS MemberCount FROM " << TableName
//		 << " WHERE " << FieldStem << "_Lon IS NOT NULL";
//	TheQuery.reset();
	try
	{
//        mysqlpp::Result TheResult( TheQuery.store(FieldStem) );
//	TheQuery << "SELECT COUNT(DateInfo) AS MemberCount FROM " << TableName
//		 << " WHERE " << FieldStem << "_Lon IS NOT NULL";

	mysqlpp::Result TheResult = TheQuery.store(FieldStem, TableName);

        if (!TheQuery.success())
        {
                throw("Problem getting member count for field " + FieldStem + " and event type " + TheQuery.def["table"] + "\nMySQL message: " + TheQuery.error());
        }

        if (TheResult)
        {
                mysqlpp::Result::iterator ARow( TheResult.begin() );
                return((*ARow)["MemberCount"]);
        }

	}
	catch(...)
	{
		cerr << "ERROR: Problem with query: " << TheQuery.preview(FieldStem, TableName) << endl;
		throw;
	}

	return(0);
}

mysqlpp::Query MakeLoader_EventCnt(mysqlpp::Connection &TheConnection)
{
	
        mysqlpp::Query TheQuery = TheConnection.query();

	try
	{
	        TheQuery << "SELECT COUNT(DateInfo) AS EventCount FROM %0:table";
        	TheQuery.parse();
	}
	catch (mysqlpp::Exception &Err)
        {
                cerr << "ERROR: Exception caught: " << Err.what() << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }
        catch (...)
        {
                cerr << "ERROR: Unknown exception caught." << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }

	return(TheQuery);

}


size_t LoadEventCnt(mysqlpp::Query &TheQuery, const string &TableName)
{
	TheQuery.reset();
	TheQuery << "SELECT COUNT(DateInfo) AS EventCount FROM " << mysqlpp::escape << TableName;
        mysqlpp::Result TheResult = TheQuery.store();

	if (!TheQuery.success())
	{
		throw("Problem getting event count for event type " + TableName + "\nMySQL message: " + TheQuery.error());
	}

        if (TheResult)
        {
	        mysqlpp::Result::iterator ARow( TheResult.begin() );
                return((*ARow)["EventCount"]);
        }

	return(0);
}

/*
mysqlpp::Query MakeLoader_EventDateTimes(mysqlpp::Connection &TheConnection)
{

	mysqlpp::Query TheQuery = TheConnection.query();

	try
	{
        	TheQuery << "SELECT DateInfo FROM %0:table";
	        TheQuery.parse();
	}
	catch (mysqlpp::Exception &Err)
        {
                cerr << "ERROR: Exception caught: " << Err.what() << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }
        catch (...)
        {
                cerr << "ERROR: Unknown exception caught." << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }


        return(TheQuery);
}
*/

vector <time_t> LoadEventDateTimes(mysqlpp::Query &TheQuery, const string &TableName)
{
        TheQuery << "SELECT DateInfo FROM " << mysqlpp::escape << TableName;

//	mysqlpp::Result TheResult( TheQuery.store(TableName) );

	mysqlpp::Result TheResult = TheQuery.store();

        if (!TheQuery.success())
        {
		throw("Problem extracting event date times for event type " + TableName + "\nMySQL message: " + TheQuery.error());
        }

	if (TheResult)
        {
		vector <time_t> EventTimes( TheResult.size() );
		vector<time_t>::iterator ATime = EventTimes.begin();
                for (mysqlpp::Result::iterator ARow( TheResult.begin() ); ARow != TheResult.end(); ARow++, ATime++)
		{
			*ATime = DateTimeToTime_t( (*ARow)["DateInfo"] );
		}
		
		return(EventTimes);
        }
	else
	{
		return(vector<time_t>(0));
	}
}






void InsertEvent(const time_t &EventDateTime, const double &EventLon, const double &EventLat, const vector <double> &TheValues,
		 mysqlpp::Query &TheQuery, const string &TableName)
// assumes that the values in TheValues correspond to the table's column in the SAME ORDER as they were created.
{
	TheQuery << "INSERT IGNORE INTO " << TableName << " VALUES('" << Time_tToDateTime(EventDateTime) << "'," << EventLon << ',' << EventLat
		 << ',' << GiveDelimitedList(DoubleToStr(TheValues), ',') << ')';

	TheQuery.execute();

	if (!TheQuery.success())
	{
		throw("Problem doing event insertion into the database for event type " + TableName + "\nMySQL message: " + TheQuery.error());
	}

	TheQuery.reset();
}

void InsertEvent(const time_t &EventDateTime, const double &EventLon, const double &EventLat, mysqlpp::Query &TheQuery, const string &TableName)
{
	TheQuery << "INSERT IGNORE INTO " << TableName << " (DateInfo, Event_Lon, Event_Lat) "
		 << " VALUES('" << Time_tToDateTime(EventDateTime) << "'," << EventLon << ',' << EventLat << ')';

	TheQuery.execute();

	if (!TheQuery.success())
	{
		throw("Problem doing event insertion into the database for event type " + TableName + "\nMySQL message: " + TheQuery.error());
	}

	TheQuery.reset();
}



mysqlpp::Query MakeSaver_LonLatAnomDate(mysqlpp::Connection &TheConnection)
{
	
        mysqlpp::Query TheQuery = TheConnection.query();
	try
	{
	        TheQuery << "UPDATE %5:table SET %0_Lon=%1, %0_Lat=%2, %0_StdAnom=%3 WHERE DateInfo = %4q LIMIT 1";
		TheQuery.parse();
	}
	catch (mysqlpp::Exception &Err)
        {
                cerr << "ERROR: Exception caught: " << Err.what() << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }
        catch (...)
        {
                cerr << "ERROR: Unknown exception caught." << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }

	return(TheQuery);
}


void SaveLonLatAnom(const double &Lon, const double &Lat, const double &Anom, const time_t ADateTime,
                     mysqlpp::Query &TheQuery, const string &FieldStem)
// DoubleToStr() used for now so that NaNs are converted to '\N' for mysql
{
//	TheQuery << "UPDATE " << EventTypeName << " SET "
//                 << FieldStem << "_Lon=" << DoubleToStr(Lon) << ", "
//                 << FieldStem << "_Lat=" << DoubleToStr(Lat) << ", "
//                 << FieldStem << "_StdAnom=" << DoubleToStr(Anom)
//                 << " WHERE DateInfo = '" << Time_tToDateTime(ADateTime) << "' LIMIT 1";

	TheQuery.execute(FieldStem, DoubleToStr(Lon), DoubleToStr(Lat), DoubleToStr(Anom), GiveTime(ADateTime));

        if (!TheQuery.success())
        {
                throw("Could not save the lon/lat/anom value to the database for date: " + GiveTime(ADateTime) + " for field " + FieldStem
		      + " for event type " + TheQuery.def["table"] + "\nMySQL message: " + TheQuery.error());
        }
}


void SaveLonLatAnoms(const vector <double> &Lons, const vector <double> &Lats, const vector <double> &Anoms, const vector <time_t> &DateTimes, 
		     mysqlpp::Query &TheQuery, const string &FieldStem)
{
	if (Lons.size() != Lats.size() || Lons.size() != Anoms.size() || Lons.size() != DateTimes.size())
	{
		throw((string) "SaveLonLatAnoms(): These are not parallel vectors...");
	}

	vector<time_t>::const_iterator ATime( DateTimes.begin() );
	for (vector<double>::const_iterator ALon( Lons.begin() ), ALat( Lats.begin() ), AnAnom( Anoms.begin() ); ALon != Lons.end();
	     ALon++, ALat++, AnAnom++, ATime++)
	{
		TheQuery.execute(FieldStem, DoubleToStr(*ALon), DoubleToStr(*ALat), DoubleToStr(*AnAnom), DateTimeToStr(Time_tToDateTime(*ATime)));
	}

	if (!TheQuery.success())
	{
		throw("Problem saving the lon/lat/anoms for field " + FieldStem + " and for event type " + TheQuery.def["table"]
		      + "\nMySQL message: " + TheQuery.error());
	}
}

void SaveBoardToDatabase(const ClusterBoard &TheBoard, mysqlpp::Query &TheQuery, const string &FieldStem,
		         const BoardConvertor &ProjectionInfo)
// Note, you can't tell right now if the table was actually updated or not.
// This should probably be fixed, somehow.
{
//        TheQuery << "UPDATE " << TableName << " SET "
//                 << FieldStem << "_Lon=%0,"
//                 << FieldStem << "_Lat=%1,"
//                 << FieldStem << "_StdAnom=%2"
//                 << " WHERE DateInfo = %3q LIMIT 1";
//        TheQuery.parse();
	try
	{
        	for (size_t XLoc = 0; XLoc < ProjectionInfo.Xsize(); XLoc++)
        	{
                	for (size_t YLoc = 0; YLoc < ProjectionInfo.Ysize(); YLoc++)
	                {
        	 //		double SphericalLon, SphericalLat;
                //        	ProjectionInfo.GridToSpherical(XLoc, YLoc, SphericalLon, SphericalLat);
				const vector <LonLatAnomDate> TheMembers = TheBoard.ReturnMembers(XLoc, YLoc).ReturnMembers();
	                        for (vector<LonLatAnomDate>::const_iterator AMember = TheMembers.begin(); 
				     AMember != TheMembers.end();
				     AMember++)
        	                {
					TheQuery.execute(FieldStem, AMember->Lon, AMember->Lat, AMember->StdAnom, DateTimeToStr(Time_tToDateTime(AMember->DateInfo)));
//					cout << SphericalLon << ", " << SphericalLat << ", " << TempGridPoint.GiveMemberValue(MemberIndex)
//					     << ", " << GiveTimeUTC(TempGridPoint.GiveMemberDate(MemberIndex)) << "\n";
				}
			}
		}
	}
	catch (...)
	{
		cerr << "ERROR: Problem saving the lon/lat/anoms for field " << FieldStem << ", event type " << TheQuery.def["table"] << endl;
		throw;
	}

	if (!TheQuery.success())
	{
		throw("Problem saving the lon/lat/anoms for field " + FieldStem + " and for event type " + TheQuery.def["table"]
		      + "\nMySQL message: " + TheQuery.error());
        }
}

vector <time_t> GiveClusteredDates(const ClusterBoard &TheBoard, const BoardConvertor &ProjectionInfo)
// Temporary until I fix and reorganize the clustering algorithm.
// The dates will be in ascending order.
{
	vector <time_t> TheDates(0);
        for (size_t XLoc = 0; XLoc < ProjectionInfo.Xsize(); XLoc++)
        {
                for (size_t YLoc = 0; YLoc < ProjectionInfo.Ysize(); YLoc++)
                {
                        const vector<LonLatAnomDate> TheMembers = TheBoard.ReturnMembers(XLoc, YLoc).ReturnMembers();

			for (size_t MemberIndex = 0; MemberIndex < TheMembers.size(); MemberIndex++)
                        {
				TheDates.insert(lower_bound(TheDates.begin(), TheDates.end(), TheMembers[MemberIndex].DateInfo), 
						TheMembers[MemberIndex].DateInfo);
			}
		}
	}

	return(TheDates);
}


set<string> GetDatabaseNames(mysqlpp::Query &theQuery)
// Returns the names of the databases available to the user in sorted order.
{
	theQuery << "SHOW DATABASES";
	mysqlpp::Result databaseResult = theQuery.store();

	set<string> databaseNames;

	for (mysqlpp::Result::iterator aRow = databaseResult.begin(); aRow != databaseResult.end(); aRow++)
	{
		databaseNames.insert(databaseNames.end(), (*aRow)["Database"].get_string());
	}

	return(databaseNames);
}

vector <string> GiveTableNames(mysqlpp::Query &TheQuery, const string &Database)
// Returns the names of the Tables available in the database in alphabetical order.
// Returns an empty array if there are no databases selected.
// Errors will be thrown if the database is not the same database that the query is connected to.
{
	TheQuery << "SHOW TABLES";
        mysqlpp::Result TableResult = TheQuery.store();

        vector <string> TableNames(0);

        for (mysqlpp::Result::iterator ARow = TableResult.begin(); ARow != TableResult.end(); ARow++)
        {
	        string EventName( (*ARow)[("Tables_in_" + Database).c_str()].get_string() );
                TableNames.insert(lower_bound(TableNames.begin(), TableNames.end(), EventName), EventName);
        }

	return(TableNames);
}

bool DropTables(mysqlpp::Query &TheQuery, const set<string> &TableNames)
{
	if (TableNames.empty())
        {
                return(true);
        }

	set<string>::const_iterator ATable = TableNames.begin();
	string tableList = *ATable;
	string fieldMeasList = *ATable + "_FieldMeas";
	ATable++;
	for (; ATable != TableNames.end(); ATable++)
	{
		tableList += ", " + *ATable;
		fieldMeasList += ", " + *ATable + "_FieldMeas";
	}

	if (!TheQuery.exec("DROP TABLE IF EXISTS " + tableList))
	{
		return(false);
	}

	if (!TheQuery.exec("DROP TABLE IF EXISTS " + fieldMeasList))
	{
		return(false);
	}

	return(true);
}

bool ClearTable(mysqlpp::Query &TheQuery, const string &TableName)
{
	return(TheQuery.exec("DELETE FROM " + TableName));
}

bool UpdateTable(const vector <LonLatAnom> &TheValues, const vector <string> &ColumnNames, const time_t &ADateTime, 
		 mysqlpp::Query &TheQuery, const string &TableName)
{
	if (TheValues.empty())
	{
		return(true);
	}

	// TODO: Sanitize input.
	TheQuery << "UPDATE " << TableName << " SET " << ColumnNames[0] << "_StdAnom" << '=' << DoubleToStr(TheValues[0].StdAnom) << ", "
						      << ColumnNames[0] << "_Lon" << '=' << DoubleToStr(TheValues[0].Lon) << ", "
						      << ColumnNames[0] << "_Lat" << '=' << DoubleToStr(TheValues[0].Lat);
	for (size_t Index = 1; Index < TheValues.size(); Index++)
	{
		TheQuery << ", " << ColumnNames[Index] << "_StdAnom" << '=' << DoubleToStr(TheValues[Index].StdAnom) << ", "
				 << ColumnNames[Index] << "_Lon" << '=' << DoubleToStr(TheValues[Index].Lon) << ", "
				 << ColumnNames[Index] << "_Lat" << '=' << DoubleToStr(TheValues[Index].Lat);
	}

	TheQuery << " WHERE DateInfo = '" << Time_tToDateTime(ADateTime) << "'";

	TheQuery.execute();
	return(TheQuery.success());
}


void CreateFieldMeasureTable(mysqlpp::Query &TheQuery, const string &EventTypeName, CAFEState &currState)
{
	size_t MaxSize = 0;
	vector <string> FieldNames(0);
	
	for (currState.EventVars_Begin(); currState.EventVars_HasNext(); currState.EventVars_Next())
	{
		for (currState.EventLevels_Begin(); currState.EventLevels_HasNext(); currState.EventLevels_Next())
                {
                        for (currState.Extrema_Begin(); currState.Extrema_HasNext(); currState.Extrema_Next())
                        {
				FieldNames.push_back(currState.FieldExtremum_Name());
				if (FieldNames.back().size() > MaxSize)
				{
					MaxSize = FieldNames.back().size();
				}
			}
		}
	}
			


	TheQuery << "create table IF NOT EXISTS " << mysqlpp::escape << EventTypeName << "_FieldMeas (FieldName VARCHAR(" 
		 << MaxSize << ") NOT NULL PRIMARY KEY,"
		 << " Alpha float(8, 5), Phi float(8, 5), Gamma_Max float(8, 5), Chi_Max float(8, 5))";

	TheQuery.execute();
	if (!TheQuery.success())
	{
		throw("Couldn't create the field measure table for event type " + EventTypeName + "\nMySQL message: " + TheQuery.error());
	}

	TheQuery << "INSERT IGNORE INTO " << mysqlpp::escape << EventTypeName << "_FieldMeas (FieldName) VALUES('" 
		 << GiveDelimitedList(FieldNames, "'),('") << "')";
	TheQuery.execute();

	if (!TheQuery.success())
	{
		throw("Couldn't insert the field names into the field measure table for event type " + EventTypeName 
		      + "\nMySQL message: " + TheQuery.error());
	}
}


mysqlpp::Query MakeSaver_AlphaPhiValues(mysqlpp::Connection &TheConnection)
{
	mysqlpp::Query TheQuery = TheConnection.query();

	try
	{
	        TheQuery << "UPDATE %3:table:_FieldMeas SET Alpha=%1, Phi=%2 WHERE FieldName = %0q LIMIT 1";
        	TheQuery.parse();

	}
	catch (mysqlpp::Exception &Err)
        {
                cerr << "ERROR: Exception caught: " << Err.what() << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }
        catch (...)
        {
                cerr << "ERROR: Unknown exception caught." << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }

	
	return(TheQuery);
}


void SaveAlphaPhiValues(mysqlpp::Query &TheQuery, const string &FieldName,
			const double &AlphaVal, const double &PhiVal)
{
//	TheQuery << "UPDATE " << EventTypeName << "_FieldMeas SET Alpha=" << DoubleToStr(AlphaVal) << ", Phi=" << DoubleToStr(PhiVal)
//		 << " WHERE FieldName = '" << FieldName << "' LIMIT 1";
	try
	{
		TheQuery.execute(FieldName, AlphaVal, PhiVal);
	}
	catch (...)
	{
		cerr << "ERROR: Problem saving alpha/phi values for field " << FieldName << ", event type " << TheQuery.def["table"] << endl;
		throw;
	}

	if(!TheQuery.success())
	{
		throw("Could not save alpha/phi values for field " + FieldName + " for event type " + TheQuery.def["table"] 
		      + "\nMySQL message: " + TheQuery.error());
	}

//	TheQuery.reset();
}


mysqlpp::Query MakeLoader_AlphaPhiValues(mysqlpp::Connection &TheConnection)
{
        mysqlpp::Query TheQuery = TheConnection.query();

	try
	{
	        TheQuery << "SELECT Alpha, Phi FROM %1:table:_FieldMeas WHERE FieldName = %0q LIMIT 1";
        	TheQuery.parse();
	}
	catch (mysqlpp::Exception &Err)
        {
                cerr << "ERROR: Exception caught: " << Err.what() << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }
        catch (...)
        {
                cerr << "ERROR: Unknown exception caught." << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }

	return(TheQuery);
}


void LoadAlphaPhiValues(mysqlpp::Query &TheQuery, const string &FieldName,
                        double &AlphaVal, double &PhiVal)
{
//        TheQuery << "SELECT Alpha, Phi FROM " << EventTypeName << "_FieldMeas"
//                 << " WHERE FieldName = '" << FieldName << "' LIMIT 1";
        mysqlpp::Result TheResult( TheQuery.store(FieldName) );

        if (!TheQuery.success())
        {
                throw("Problem extracting Alpha/Phi values for field " + FieldName + " for event type " + TheQuery.def["table"]
		      + "\nMySQL message: " + TheQuery.error());
        }

        if (TheResult)
        {
                mysqlpp::Result::iterator ARow( TheResult.begin() );
                AlphaVal = (*ARow)["Alpha"];
		PhiVal = (*ARow)["Phi"];
        }
	else
	{
		AlphaVal = PhiVal = NAN;
	}

//	TheQuery.reset();
}


mysqlpp::Query MakeLoader_GammaChiMaxValues(mysqlpp::Connection &TheConnection)
{
        mysqlpp::Query TheQuery = TheConnection.query();

	try
	{
        	TheQuery << "SELECT Gamma_Max, Chi_Max FROM %1:table:_FieldMeas WHERE FieldName = %0q LIMIT 1";
	        TheQuery.parse();
	}
	catch (mysqlpp::Exception &Err)
        {
                cerr << "ERROR: Exception caught: " << Err.what() << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }
        catch (...)
        {
                cerr << "ERROR: Unknown exception caught." << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }

        return(TheQuery);
}


void LoadGammaChiMaxValues(mysqlpp::Query &TheQuery, const string &FieldName,
			   double &GammaMax, double &ChiMax)
{
//        TheQuery << "SELECT Gamma_Max, Chi_Max FROM " << EventTypeName << "_FieldMeas"
//                 << " WHERE FieldName = '" << FieldName << "' LIMIT 1";
        mysqlpp::Result TheResult( TheQuery.store(FieldName) );

        if (!TheQuery.success())
        {
	        throw("Problem extracting gamma/chi max values for field " + FieldName + " for event type " + TheQuery.def["table"]
                      + "\nMySQL message: " + TheQuery.error());
        }

        if (TheResult)
        {
                mysqlpp::Result::iterator ARow( TheResult.begin() );
                GammaMax = (*ARow)["Gamma_Max"];
                ChiMax = (*ARow)["Chi_Max"];
        }
        else
        {
                GammaMax = ChiMax = NAN;
        }
}


mysqlpp::Query MakeLoader_FieldMeasValues(mysqlpp::Connection &TheConnection)
{
        mysqlpp::Query TheQuery = TheConnection.query();

	try
	{
	        TheQuery << "SELECT Alpha, Phi, Gamma_Max, Chi_Max FROM %1:table:_FieldMeas WHERE FieldName = %0q LIMIT 1";
        	TheQuery.parse();
	}
	catch (mysqlpp::Exception &Err)
        {
                cerr << "ERROR: Exception caught: " << Err.what() << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }
        catch (...)
        {
                cerr << "ERROR: Unknown exception caught." << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }

        return(TheQuery);
}



void LoadFieldMeasValues(mysqlpp::Query &TheQuery, const string &FieldName, const string &TableName,
                         double &AlphaVal, double &PhiVal, double &GammaMax, double &ChiMax)
{
//        TheQuery << "SELECT Alpha, Phi, Gamma_Max, Chi_Max FROM " << EventTypeName << "_FieldMeas"
//                 << " WHERE FieldName = '" << FieldName << "' LIMIT 1";

	try
	{
        mysqlpp::Result TheResult( TheQuery.store(FieldName, TableName) );

        if (!TheQuery.success())
        {
                throw("Problem extracting field measure values for field " + FieldName + " for event type " + TheQuery.def["table"]
                      + "\nMySQL message: " + TheQuery.error());
        }

        if (TheResult)
        {
                mysqlpp::Result::iterator ARow( TheResult.begin() );
		AlphaVal = ((*ARow)["Alpha"].get_string() == "NULL" ? NAN : (*ARow)["Alpha"]);
		PhiVal = ((*ARow)["Phi"].get_string() == "NULL" ? NAN : (*ARow)["Phi"]);
                GammaMax = ((*ARow)["Gamma_Max"].get_string() == "NULL" ? NAN : (*ARow)["Gamma_Max"]);
                ChiMax = ((*ARow)["Chi_Max"].get_string() == "NULL" ? NAN : (*ARow)["Chi_Max"]);
        }
        else
        {
		AlphaVal = PhiVal = GammaMax = ChiMax = NAN;
        }
	}
	catch (...)
	{
		cerr << "ERROR: Problem extracting field measure values for field " + FieldName + " for event type " + TheQuery.def["table"]
		     << "\nMySQL query: " << TheQuery.preview(FieldName) << endl;
		throw;
	}

//	TheQuery.reset();
}




mysqlpp::Query MakeSaver_GammaChiMaxValues(mysqlpp::Connection &TheConnection)
{
	mysqlpp::Query TheQuery = TheConnection.query();

	try
	{
	        TheQuery << "UPDATE %3:table:_FieldMeas SET Gamma_Max=%1, Chi_Max=%2 WHERE FieldName = %0q LIMIT 1";
        	TheQuery.parse();
	}
	catch (mysqlpp::Exception &Err)
        {
                cerr << "ERROR: Exception caught: " << Err.what() << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }
        catch (...)
        {
                cerr << "ERROR: Unknown exception caught." << endl;
                cerr << "query: " << TheQuery.preview() << endl;
                throw;
        }

        return(TheQuery);
}

void SaveGammaChiMaxValues(mysqlpp::Query &TheQuery, const string &FieldName,
                        const double &GammaMax, const double &ChiMax)
{
//        TheQuery << "UPDATE " << EventTypeName << "_FieldMeas SET Gamma_Max=" << DoubleToStr(GammaMax) << ", Chi_Max=" << DoubleToStr(ChiMax)
//                 << " WHERE FieldName = '" << FieldName << "' LIMIT 1";

	TheQuery.execute(FieldName, GammaMax, ChiMax);

	if (!TheQuery.success())
	{
		throw("Couldn't save the gamma/chi max values for field " + FieldName + " for event type " + TheQuery.def["table"]
		      + "\nMySQL message: " + TheQuery.error());
	}
}


void CreateTable(mysqlpp::Query &TheQuery, const string &EventTypeName, CAFEState &currState)
{
	vector <string> DataNames(3);
        DataNames[0] = "StdAnom float(8, 6)";
        DataNames[1] = "Lon float(7, 3)";
        DataNames[2] = "Lat float(7, 3)";

	TheQuery << "CREATE TABLE " << EventTypeName << "(DateInfo datetime NOT NULL PRIMARY KEY,"
		 << " Event_Lon float(6, 3) NOT NULL, Event_Lat float(6, 3) NOT NULL";

        for (currState.EventVars_Begin(); currState.EventVars_HasNext(); currState.EventVars_Next())
        {
                for (currState.EventLevels_Begin(); currState.EventLevels_HasNext(); currState.EventLevels_Next())
		{
	                for (currState.Extrema_Begin(); currState.Extrema_HasNext(); currState.Extrema_Next())
                        {
	                        const string fieldName = currState.FieldExtremum_Name();
                                TheQuery << ',' << fieldName + '_' + DataNames.at(0);

                                for (size_t DataIndex = 1; DataIndex < DataNames.size(); DataIndex++)
                                {
        	                        TheQuery << ", " << fieldName + '_' + DataNames[DataIndex];
                                }
                        }// end of extremum loop
		}// end of Label loop
	}// end of CAFEVar loop

	TheQuery << ")";
	TheQuery.execute();

        if (!TheQuery.success())
	{
		throw("Couldn't create the table for event type " + EventTypeName + "\nMySQL message: " + TheQuery.error());
	}
}

#endif
