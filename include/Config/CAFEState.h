#ifndef _CAFESTATE_H
#define _CAFESTATE_H

#include <vector>
#include <map>
#include <set>
#include <string>

// I don't need the include headers for these classes,
// so just declare them for now.
class CAFEVar;
class EventType;
class CAFEDomain;
class DataSource;

#include "CAFEParam.h"

class CAFEState
{
	public:
		// Constructors
		CAFEState();
		explicit CAFEState(const CAFEState &stateCopy);
		CAFEState(const CAFEParam& cafeConfig);

		CAFEState& SetParams(const CAFEParam& cafeConfig);
		const CAFEParam& GetParams() const;

		// Value existance checkers
		bool EventType_Check(const string &eventName) const;
		bool TimeOffset_Check(const int offsetHour) const;
		bool CAFEVar_Check(const string &cafeVarName) const;
		bool DataSource_Check(const string &sourceName) const;


		// Getter functions
		const int& GetVerboseLevel() const;
		const string& GetConfigFilename() const;
		const string& GetCAFEPath() const;
		const string& GetLoginUserName() const;
		const string& GetCAFEUserName() const;
		const string& GetServerName() const;
		const string& GetUntrainedNameStem() const;
		const string& GetTrainedNameStem() const;
		const CAFEDomain& GetCAFEDomain() const;

		set<string> GetCAFEFields() const;		// Gets all CAFEFields compiled from known
								// CAFEVars and CAFELevels.
		set<string> GetEventFields() const;		// Gets all CAFEFields for the event types
								// (e.g. - "MSLP", "UWnd_500", "UWnd_850", "VWnd_850", ...)

		

		// ********************************
		// --- Dynamic Parameter Access ---
		// ********************************
		/*    These functions are meant to handle the logical mappings
			of TimePeriods->EventTypes->EventVars->CAFELevels->Extrema.
			In other words, as one accesses a particular event type,
			this class will automatically make the CAFEVariables for
			that event type available along with some helper functions.
		      In addition, various looping functions should help keep the
			for-loops neat and concise.
		*/

		// -------------------
		// --- TimePeriods ---
		//--------------------
		// **** Accessor helpers ****    (Use these in the for-loops)
		size_t TimePeriods_Size() const;	// Number of TimePeriods
		CAFEState& TimePeriods_Begin();		// Resets to the first TimePeriod.
		CAFEState& TimePeriods_Next();		// moves to the next TimePeriod.
		bool TimePeriods_HasNext() const;	// True if there is still another time period to do.
		bool TimePeriods_JumpTo(const int &offset);	// Moves the the indicated time offset.
								// returns false if unable to do so.
								// TODO: Consider throwing exception instead.

		// **** Value Accessors ****
		set<int> TimePeriod_Offsets() const;
		const int& TimePeriod_Offset() const;	// gives the current offset in hours
							// (i.e. - 0 hours)

		set<string> TimePeriod_Names() const;
		string TimePeriod_Name() const;		// gives the current TimePeriod.
							// (i.e. - "Tm00")
		set<string> Untrained_Names() const;
		string Untrained_Name() const;		// gives the current UntrainedName
							// (i.e. - "UnclusteredFields_Tm00")
		set<string> Trained_Names() const;
		string Trained_Name() const;		// gives the current TrainedName
							// (i.e. - "ClusteredFields_Tm00")

		// ------------------
		// --- EventTypes ---
		// ------------------
		// **** Accessor helpers ****    (Use these in the for-loops)
		size_t EventTypes_Size() const;	// Number of EventTypes
		CAFEState& EventTypes_Begin();		// Resets to the first EventType
		CAFEState& EventTypes_Next();		// moves to the next EventType
		bool EventTypes_HasNext() const;	// True if there is another EventType to process
		bool EventTypes_JumpTo(const string &eventName);// Moves to the indicated event type
								// Returns false if unable to do so.
								// TODO: Consider throwing exception.

		// **** Value Accessors ****
		set<string> EventType_Names() const;
		const string& EventType_Name() const;	// gives the current EventType Name
							// (i.e. - "Snow")

		// -----------------
		// --- EventVars ---
		// -----------------
		// **** Accessor helpers ****    (Use these in the for-loops)
		size_t EventVars_Size() const;		// Number of event vars for the current event type
		CAFEState& EventVars_Begin();		// Resets to the first variable for the current event type
		CAFEState& EventVars_Next();		// moves to the next variable for the current event type
		bool EventVars_HasNext() const;		// True if there is another EventVar to process
		bool EventVars_JumpTo(const string &varName);	// Moves to the indicated CAFE variable name
								// Returns false if unable to do so.
								// TODO: Consider throwing exception.

		// **** Value Accessors ****
		set<string> EventVar_Names() const;
		const string& EventVar_Name() const;	// Give the current variable name for the 
							// current event type.
							// (i.e. - "VWind")

		// Gives the CAFEVar/DataVar pairings for the current event type.
		map<string, string> EventVar_DataVar_Names() const;

		set<string> DataVar_Names() const;
		const string& DataVar_Name() const;	// Gives the data source's version of this variable's name
							// (i.e. - "vwnd")

		// -------------------
		// --- EventLevels ---
		// -------------------
		// **** Accessor helpers ****    (Use these in the for-loops)
		size_t EventLevels_Size() const;		// Number of CAFE levels for the current event variable
		CAFEState& EventLevels_Begin();		// Resets to the first level of the current EventVar
		CAFEState& EventLevels_Next();		// Moves to the next level for the current EventVar
		bool EventLevels_HasNext() const;	// True if there is another level to process.
		bool EventLevels_JumpTo(const string &levelName);// Moves to the indicated Level.
								// Returns false if unable to do so.
								// TODO: Consider throwing exception.

		// **** Value Accessors ****
		set<string> EventLevel_Names() const;
		const string& EventLevel_Name() const;	// Gives the current CAFELevel for the current EventVar
							// (i.e. - "500", "" (for no level name))

		// Gives EventLevel/DataLevel pairings for the current EventVar.
		map<string, string> EventLevel_DataLevel_Names() const;

		set<string> DataLevel_Names() const;
		string DataLevel_Name() const;		// Gives the data source's version of the variable's level
							// NOTE: This is JUST the level name, unlike the string 
							// returned by CAFEField_Name()
							// (i.e. - "500 mb")

		set<string> EventField_Names() const;
		string EventField_Name() const;		// Gives the current field name ("%EventVar_%EventLevel" 
							// or "%EventVar" if no levels) for
							// the current CAFE Field
							// (i.e. - "VWind_500", "MSLP")


		// ---------------
		// --- Extrema ---
		// ---------------
		// **** Accessor helpers ****    (Use these in the for-loops)
		size_t Extrema_Size() const;		// Number of extrema for the current Event Level
		CAFEState& Extrema_Begin();		// Resets to the first Extremum
		CAFEState& Extrema_Next();		// Moves to the next extremum for the current Event Level
		bool Extrema_HasNext() const;		// True if there is still more extrema to process
		bool Extrema_JumpTo(const string &extremumName);// Moves to the indicated extremum.
								// Returns false if unable to do so.
								// TODO: Consider throwing exception.

		// **** Value Accessors ****
		// NOTE: vectors are returned here because the order of
		// the extrema may (or may not) be important.  Therefore, I will
		// maintain the order in which they came when SetExtremaNames() was called.
		vector<string> Extremum_Names() const;
		const string& Extremum_Name() const;	// Gives the current extremum name ("%Extremum")
							// for the current EventLevel.
							// (i.e. - "Valley1", "Peak2")
		vector<string> FieldExtremum_Names() const;
		string FieldExtremum_Name() const;	// Gives the current field extremum name
							// for the current CAFEField
							// ("%EventField_%Extremum")
							// (i.e. - "VWind_500_Valley1", "MSLP_Peak2")


		// *********************************
		// --- Dynamic CAFEVar Accessors ---
		// *********************************
		/*    These functions retrieve information about the CAFEVars
			and the data source without refering to any particular event variable.
			Essentially, dealing with the direct logical mappings between
			the CAFEVars and the DataSource, as opposed to using
			the TimePeriods->EventTypes->EventVars->CAFELevels->Extrema controls.
		      These functions operate independently from the above looping functions.
		      The logical controls are CAFEVars->CAFELevels.  From these, equivalent
			data source representations can be retrieved.
		*/

		// ----------------
		// --- CAFEVars ---
		// ----------------
		// **** Accessor Helpers ****
		size_t CAFEVars_Size() const;		// Number of CAFEVars
		CAFEState& CAFEVars_Begin();		// Resets to the first CAFEVar
		CAFEState& CAFEVars_Next();		// Moves to the next CAFEVar
		bool CAFEVars_HasNext() const;		// True if there is another valid CAFEVars_Next() call
		bool CAFEVars_JumpTo(const string &varName);	// Moves to the indicated CAFEVar.
								// Returns false if unable to do so.
								// TODO: Consider throwing exception.

		// **** Value Accessors ****
		set<string> CAFEVar_Names() const;
		const string& CAFEVar_Name() const;	// Gives name of the current CAFEVar
							// (e.g. - "UWnd", "MSLP")

		// Gives all CAFEVar/DataVar pairings
		map<string, string> CAFEVar_CAFEData_Names() const;

		set<string> CAFEData_Names() const;
		const string& CAFEData_Name() const;	// Gives the Data Source's name of the current CAFEVar.


		// -------------------
		// --- CAFE Levels ---
		// -------------------
		// **** Accessor Helpers ****
		size_t CAFELevels_Size() const;		// Number of CAFE Levels for the current CAFEVar
		CAFEState& CAFELevels_Begin();		// Resets to the first CAFE Level
		CAFEState& CAFELevels_Next();		// Moves to the next CAFE Level
		bool CAFELevels_HasNext() const;	// True if there is another valid CAFELevels_Next() call
		bool CAFELevels_JumpTo(const string &levelName);// Moves to the indicated CAFE Level.
								// Returns false if unable to do so.
								// TODO: Consider throwing exception.

		//**** Value Accessors ****
		set<string> CAFELevel_Names() const;
		const string& CAFELevel_Name() const;	// Gives name of the current CAFELevel
							// (e.g. - "850", or "")

		// Gives all CAFELevel/DataLevel pairings
		map<string, string> CAFELevel_CAFEDataLevel_Names() const;

		set<string> CAFEDataLevel_Names() const;
		const string& CAFEDataLevel_Name() const;	// Gives the data source's version of the
								// current CAFELevel.
								// (e.g. - "850 mb", "sfc")

		set<string> CAFEField_Names() const;
		string CAFEField_Name() const;		// Gives the field name of the current CAFELevel
							// (e.g. - "UWnd_850", "MSLP")


		// ***********************************
		// --- Dynamic DataSource Accessor ---
		// ***********************************
		/*   For modifying which data source is
			currently in use.  Using these
			looping controls would modify the
			results of ANY function refering
			data source information.
		     Looping through the DataSources will
			change the DefaultDataSource.
		     Note that SetDefaultDataSource()
			will also effect the looping.
		*/
		// **** Accessor Helpers ****
		size_t DataSources_Size() const;	// Number of Data Sources.
		CAFEState& DataSources_Begin();		// Resets to the first data source
		CAFEState& DataSources_Next();		// Moves to the next data source
		bool DataSources_HasNext() const;	// True if there is another valid call to DataSources_Next()
		bool DataSources_JumpTo(const string &sourceName);
							// Moves to the indicated DataSource.
							// Returns false if unable to do so.
							// Acts a lot like SetDefaultDataSource()
							// TODO: Consider throwing exception instead.

		// **** Value Accessors ****
		const DataSource& DefaultDataSource() const;	// Temporary...
		set<string> DataSource_Names() const;
		const string& DataSource_Name() const;	// Gives the name of the current data source.

			

	private:
		CAFEParam myCAFEInfo;

		set<int>::const_iterator myTimeOffsetIter;
		map<string, EventType>::const_iterator myEventTypeIter;
		map<string, DataSource>::const_iterator myDataSourceIter;
		vector<string>::const_iterator myExtremumIter;
		map<string, CAFEVar>::const_iterator myCAFEVarIter;

		vector<string> myTempEventVarNames;
		vector<string>::const_iterator myEventVarIter;

		vector<string> myTempEventVarLevelNames;
		vector<string>::const_iterator myEventVarLevelIter;

		map<string, size_t>::const_iterator myCAFELevelIter;

		string GiveTimePeriod(const int &timeOffset) const;
		CAFEState& ResetEventVars();
		CAFEState& ResetEventLevels();

		const set<int>& GetTimeOffsets() const;
		const map<string, DataSource>& GetDataSources() const;
		const map<string, CAFEVar>& GetCAFEVars() const;
		const map<string, size_t>& GetCAFELevels() const;
		const map<string, EventType>& GetEventTypes() const;
		const vector<string>& GetEventVars() const;
		const vector<string>& GetEventLevels() const;
		const vector<string>& GetExtremumNames() const;


		const DataSource& GetDataSource() const;
		const CAFEVar& GetCAFEVar() const;
		const EventType& GetEventType() const;
};


#endif
