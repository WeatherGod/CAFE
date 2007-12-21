#ifndef _CAFE_SQLUTLY_H
#define _CAFE_SQLUTLY_H

#include <vector>
#include <string>
#include <mysql++/mysql++.h>
#include <ctime>

#include "Utils/LonLatAnom.h"		// for LonLatAnom and LonLatAnomDate structure (the LonLatAnomDate is only temporary)
#include "Utils/FieldMeasure.h"		// for the FieldMeasure structure

#include "SPAnalysis/BoardConvertor.h"
#include "SPAnalysis/ClusterBoard.h"

#include "Config/Configuration.h"
#include "CAFE_CmdLine.h"


void EstablishConnection(mysqlpp::Connection &ConnectLink, const string &HostName,
                         const string &UserName, const string &DatabaseName = "", const bool NeedPass = true);


time_t DateTimeToTime_t(const mysqlpp::DateTime &SQLTime);
mysqlpp::DateTime Time_tToDateTime(const time_t &TheTime);
string DateTimeToStr(const mysqlpp::DateTime &SQLTime);

//------------------------------------------------------------------------------------------------------
// SQLstructs utilities.  Some may soon be deprecated...
vector <mysqlpp::DateTime> SplitIntoTime(const vector <LonLatAnomDate> &TheMembers);
vector <mysqlpp::DateTime> GiveClusteredDates(const ClusterBoard &TheBoard, const BoardConvertor &ProjectionInfo);
//------------------------------------------------------------------------------------------------------


// Retrieve LonLatAnoms
mysqlpp::Query MakeLoader_LonLatAnoms(mysqlpp::Connection &TheConnection);
vector <LonLatAnom> LoadLonLatAnoms(mysqlpp::Query &TheQuery, const string &FieldStem);

// Retrieve LonLatAnomDate
mysqlpp::Query MakeLoader_LonLatAnomDates(mysqlpp::Connection &TheConnection);
vector <LonLatAnomDate> LoadLonLatAnomDates(mysqlpp::Query &TheQuery, const string &FieldStem);

// Retrieve Member Counts
mysqlpp::Query MakeLoader_MemberCnt(mysqlpp::Connection &TheConnection);
size_t LoadMemberCnt(mysqlpp::Query &TheQuery, const string &FieldStem, const string &TableName);

// Retrieve Event Counts
mysqlpp::Query MakeLoader_EventCnt(mysqlpp::Connection &TheConnection);
size_t LoadEventCnt(mysqlpp::Query &TheQuery, const string &TableName);

// Retrieve Event DateTimes from CAFE database (not the events database!)
mysqlpp::Query MakeLoader_EventDateTimes(mysqlpp::Connection &TheConnection);
vector <time_t> LoadEventDateTimes(mysqlpp::Query &TheQuery, const string &TableName);


void InsertEvent(const time_t &EventDateTime, const double &EventLon, const double &EventLat, mysqlpp::Query &TheQuery, const string &TableName);
void InsertEvent(const time_t &EventDateTime, const double &EventLon, const double &EventLat, const vector <double> &TheValues,
                 mysqlpp::Query &TheQuery, const string &TableName);

// Stores LonLatAnomDate
mysqlpp::Query MakeSaver_LonLatAnomDate(mysqlpp::Connection &TheConnection);
void SaveLonLatAnom(const double &Lon, const double &Lat, const double &Anom, const time_t ADateTime,
                     mysqlpp::Query &TheQuery, const string &FieldStem);
void SaveLonLatAnoms(const vector <double> &Lons, const vector <double> &Lats,
                     const vector <double> &Anoms, const vector <mysqlpp::DateTime> &DateTimes,
                     mysqlpp::Query &TheQuery, const string &FieldStem);
// Consider Redesigning...
void SaveBoardToDatabase(const ClusterBoard &TheBoard, mysqlpp::Query &TheQuery, const string &FieldStem,
                         const BoardConvertor &ProjectionInfo);



vector <string> GiveTableNames(mysqlpp::Query &TheQuery, const string &Database);
bool DropTables(mysqlpp::Query &TheQuery, const vector <string> &TableNames);
bool ClearTable(mysqlpp::Query &TheQuery, const string &TableName);
bool UpdateTable(const vector <LonLatAnom> &TheValues, const vector <string> &ColumnNames, const time_t &ADateTime,
                 mysqlpp::Query &TheQuery, const string &TableName);


void CreateTable(mysqlpp::Query &TheQuery, const string &EventTypeName,
		 const Configuration &ConfigInfo, const CmdOptions &CAFEOptions);
void CreateFieldMeasureTable(mysqlpp::Query &TheQuery, const string &EventTypeName, 
			     const Configuration &ConfigInfo, const CmdOptions &CAFEOptions);

// Stores AlphaPhi values
mysqlpp::Query MakeSaver_AlphaPhiValues(mysqlpp::Connection &TheConnection);
void SaveAlphaPhiValues(mysqlpp::Query &TheQuery, const string &FieldName,
                        const double &AlphaVal, const double &PhiVal);

// Stores GammaChi values
mysqlpp::Query MakeSaver_GammaChiMaxValues(mysqlpp::Connection &TheConnection);
void SaveGammaChiMaxValues(mysqlpp::Query &TheQuery, const string &FieldName,
                        const double &GammaMax, const double &ChiMax);


// Retrieves AlphaPhi values
mysqlpp::Query MakeLoader_AlphaPhiValues(mysqlpp::Connection &TheConnection);
void LoadAlphaPhiValues(mysqlpp::Query &TheQuery, const string &FieldName,
                        double &AlphaVal, double &PhiVal);

// Retrieves GammaChi values
mysqlpp::Query MakeLoader_GammaChiMaxValues(mysqlpp::Connection &TheConnection);
void LoadGammaChiMaxValues(mysqlpp::Query &TheQuery, const string &FieldName,
                           double &GammaMax, double &ChiMax);

// Retrieves FieldMeas values
mysqlpp::Query MakeLoader_FieldMeasValues(mysqlpp::Connection &TheConnection);
void LoadFieldMeasValues(mysqlpp::Query &TheQuery, const string &FieldName, const string &TableName,
                         double &AlphaVal, double &PhiVal, double &GammaMax, double &ChiMax);

#endif
