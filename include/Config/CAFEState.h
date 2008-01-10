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

		CAFEState& SetDefaultDataSource(const string &sourceName);

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
		const string& GetDefaultDataSource() const;

		const set<int>& GetTimeOffsets() const;
		set<string> GetTimePeriod_Names() const;
		set<string> GetUntrained_Names() const;
		set<string> GetTrained_Names() const;

		const map<string, DataSource>& GetDataSources() const;
		set<string> GetDataSource_Names() const;

		const map<string, CAFEVar>& GetCAFEVars() const;
		set<string> GetCAFEVar_Names() const;

		const map<string, EventType>& GetEventTypes() const;
		set<string> GetEventType_Names() const;

		const vector<string>& GetExtremumNames() const;


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
		int TimePeriod_Offset() const;		// gives the current offset in hours
							// (i.e. - 0 hours)
		string TimePeriod_Name() const;		// gives the current TimePeriod.
							// (i.e. - "Tm00")
		string Untrained_Name() const;		// gives the current UntrainedName
							// (i.e. - "UnclusteredFields_Tm00")
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
		string EventType_Name() const;		// gives the current EventType Name
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
		/* TODO: Reconsider the set<> return type for these...
		   the set returned by EventVar_Names() is not going
		   to be parallel to the set returned by DataVar_Names().
		   Nor, is it guarrenteed to be in the same order as processed
		   through the iterators.
		   However, the set would be extremely useful to make sure
		   that there are unique names and that they are sorted.
		   In addition, it is also extremely easy to do merges.
		*/
		set<string> EventVar_Names() const;
		string EventVar_Name() const;		// Give the current variable name for the 
							// current event type.
							// (i.e. - "VWind")
		set<string> DataVar_Names() const;
		string DataVar_Name() const;		// Gives the data source's version of this variable's name
							// (i.e. - "vwnd")

		// ------------------
		// --- CAFELevels ---
		// ------------------
		// **** Accessor helpers ****    (Use these in the for-loops)
		size_t CAFELevels_Size() const;		// Number of CAFE levels for the current event variable
		CAFEState& CAFELevels_Begin();		// Resets to the first level of the current EventVar
		CAFEState& CAFELevels_Next();		// Moves to the next level for the current EventVar
		bool CAFELevels_HasNext() const;	// True if there is another level to process.
		bool CAFELevels_JumpTo(const string &levelName);// Moves to the indicated CAFE Level.
								// Returns false if unable to do so.
								// TODO: Consider throwing exception.

		// **** Value Accessors ****
		/* TODO: Reconsider the set<> return type for these...
                   the set returned by CAFELevel_Names() is not going
                   to be parallel to the set returned by DataLevel_Names().
                   Nor, is it guarrenteed to be in the same order as processed
                   through the iterators.
                   However, the set would be extremely useful to make sure
                   that there are unique names and that they are sorted.
                   In addition, it is also extremely easy to do merges.
                */	
		set<string> CAFELevel_Names() const;
		string CAFELevel_Name() const;		// Gives the current CAFELevel for the current EventVar
							// (i.e. - "500", "" (for no level name))
		set<string> DataLevel_Names() const;
		string DataLevel_Name() const;		// Gives the data source's version of the variable's level
							// NOTE: This is JUST the level name, unlike the string 
							// returned by CAFEField_Name()
							// (i.e. - "500 mb")
		set<string> CAFEField_Names() const;
		string CAFEField_Name() const;		// Gives the current field name ("%EventVar_%CAFELevel" 
							// or "%EventVar" if no levels) for
							// the current CAFE Field
							// (i.e. - "VWind_500", "MSLP")


		// ---------------
		// --- Extrema ---
		// ---------------
		// **** Accessor helpers ****    (Use these in the for-loops)
		size_t Extrema_Size() const;		// Number of extrema for the current CAFEField
		CAFEState& Extrema_Begin();		// Resets to the first Extremum
		CAFEState& Extrema_Next();		// Moves to the next extremum for the current CAFEField
		bool Extrema_HasNext() const;		// True if there is still more extrema to process
		bool Extrema_JumpTo(const string &extremumName);// Moves to the indicated extremum.
								// Returns false if unable to do so.
								// TODO: Consider throwing exception.

		// **** Value Accessors ****
		// NOTE: vectors are returned here because the order of
		// the extrema may (or may not) be important.  Therefore, I will
		// maintain the order in which they came when SetExtremaNames() was called.
		string Extremum_Name() const;		// Gives the current extremum name ("%Extremum")
							// for the current CAFEField.
							// (i.e. - "Valley1", "Peak2")
		vector<string> FieldExtremum_Names() const;
		string FieldExtremum_Name() const;	// Gives the current field extremum name
							// for the current CAFEField
							// ("%CAFEField_%Extremum")
							// (i.e. - "VWind_500_Valley1", "MSLP_Peak2")
	private:
		CAFEParam myCAFEInfo;

		set<int>::const_iterator myTimeOffsetIter;
		map<string, EventType>::const_iterator myEventTypeIter;
		map<string, DataSource>::const_iterator myDataSourceIter;
		vector<string>::const_iterator myExtremumIter;

		vector<string> myTempEventVarNames;
		vector<string>::const_iterator myEventVarIter;

		vector<string> myTempEventVarLevelNames;
		vector<string>::const_iterator myEventVarLevelIter;

		string GiveTimePeriod(const int &timeOffset) const;
		CAFEState& ResetTempEventVars();
		CAFEState& ResetTempEventVarLevels();
};


#endif
