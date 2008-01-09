#ifndef _CAFESTATE_H
#define _CAFESTATE_H

#include <vector>
#include <map>
#include <set>
#include <string>
#include <cctype>		// for size_t

// I don't need the include headers for these classes,
// so just declare them for now.
class CAFEVar;
class EventType;
class CAFEDomain;
class DataSource;

class CAFEState
{
	public:
		// Constructors
		CAFEState();
		explicit CAFEState(const CAFEState &stateCopy);

		// Destructor
		~CAFEState();

		// Setter functions
		CAFEState& SetVerboseLevel(const int verbosity);
		CAFEState& SetConfigFilename(const string &filename);
		CAFEState& SetCAFEPath(const string &pathname);
		CAFEState& SetLoginUserName(const string &newUserName);
		CAFEState& SetCAFEUserName(const string &newUserName);
		CAFEState& SetServerName(const string &newServerName);

		// Deprecated
//		CAFEState& SetTimePeriods(const vector<string> &newTimePeriods);

		CAFEState& SetTimeOffsets(const set<int> &newTimeOffsets);
		CAFEState& SetUntrainedNameStem(const string &newNameStem);
		CAFEState& SetTrainedNameStem(const string &newNameStem);

		CAFEState& SetDataSource(const DataSource &theDataSource);
		CAFEState& SetCAFEDomain(const CAFEDomain &newDomain);

		CAFEState& SetCAFEVars(const map<string, CAFEVar> &newCAFEVars);
		CAFEState& SetEventTypes(const map<string, EventType> &newEventTypes);

		CAFEState& SetExtremumNames(const vector<string> &newExtremumNames);

		// Getter functions
		int GetVerboseLevel() const;
		string GetConfigFilename() const;
		string GetCAFEPath() const;
		string GetLoginUserName() const;
		string GetCAFEUserName() const;
		string GetServerName() const;
		const DataSource& GetDataSource() const;
		const CAFEDomain& GetCAFEDomain() const;


		// -------------------------------------
		// --- TimePeriods and dataset names ---
		//--------------------------------------
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
		int TimePeriod_Offset() const;		// gives the current offset in hours
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
		size_t EventTypes_Size() const;		// Number of EventTypes
		CAFEState& EventTypes_Begin();		// Resets to the first EventType
		CAFEState& EventTypes_Next();		// moves to the next EventType
		bool EventTypes_HasNext() const;	// True if there is another EventType to process
		bool EventTypes_JumpTo(const string &eventName);// Moves to the indicated event type
								// Returns false if unable to do so.
								// TODO: Consider throwing exception.

		// **** Value Accessors ****
		set<string> EventType_Names() const;
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
		string EventVar_Name() const;		// Give the current variable name for the current event type
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
		vector<string> Extremum_Names() const;
		string Extremum_Name() const;		// Gives the current extremum name ("%Extremum")
							// for the current CAFEField.
							// (i.e. - "Valley1", "Peak2")
		vector<string> FieldExtremum_Names() const;
		string FieldExtremum_Name() const;	// Gives the current field extremum name
							// for the current CAFEField
							// ("%CAFEField_%Extremum")
							// (i.e. - "VWind_500_Valley1", "MSLP_Peak2")

	private:
		int myVerboseLevel;

		set<int> myTimeOffsets;
		string myUntrainedNameStem;
		string myTrainedNameStem;

		// This will contain the needed information for each event type.
		map<string, EventType> myEventTypes;

		// This info should eventually be incorporated into the data source?
		map<string, CAFEVar> myCAFEVars;

		vector<string> myExtremumNames;

		// This will contain the needed information regarding the given data source
		DataSource myDataSource;

		// This might become merged with the data source
		CAFEDomain myDomain;

		// CAFE IO access information
		string myConfigFilename;
		string myCAFEPath;
		string myLoginUserName;
		string myCAFEUserName;
		string myServerName;

		set<int>::const_iterator myTimeOffsetIter;
		map<string, EventType>::const_iterator myEventTypeIter;
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
