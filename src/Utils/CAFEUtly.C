#ifndef _CAFEUTLY_C
#define _CAFEUTLY_C

using namespace std;

#include <vector>
#include <iostream>
#include <cmath>			// for isnan(), NAN
#include <string>

#include <StrUtly.h>			// for StrToDouble(), DoubleToStr(),
					// TakeDelimitedList(), GiveDelimitedList()

#include "Utils/EventScoreFile.h"
#include "Utils/FieldMeasFile.h"
#include "Utils/ClusterFile.h"


// These might go away soon...
#include "SPAnalysis/ClusterBoard.h"		// for the ClusterBoard class
#include "SPAnalysis/BoardConvertor.h"		// for the BoardConvertor class
#include "Config/Configuration.h"


#include <Projection_t.h>			// for the Projection_t base class

#include "Utils/LonLatAnom.h"		// for LonLatAnomDate, LonLatAnom type
#include "Utils/FieldMeasure.h"		// for FieldMeasure structure

#include "Utils/CAFEUtly.h"


bool GetGridInfo(const Configuration &ConfigInfo, BoardConvertor &ProjectionInfo, const size_t &Radius)
{
        double X1 = 0.0;
        double X2 = 0.0;
        double Y1 = 0.0;
        double Y2 = 0.0;

	// The Configuration class guarrentees to give a non-NULL pointer.
	// TODO: Check best practices for dealing with pointers.
	const Projection_t* TheProjection = ConfigInfo.Give_DataSource_Projection();
        vector <float> BoundingBox = ConfigInfo.GiveCAFEDomainBoundingBox();

	if (!TheProjection->LatLonToXY(BoundingBox[2], BoundingBox[0], X1, Y1))
	{
		cerr << "ERROR: Bad projection attempt!" << endl;
		delete TheProjection;
		return(false);
	}

	if (!TheProjection->LatLonToXY(BoundingBox[3], BoundingBox[1], X2, Y2))
	{
		cerr << "ERROR: Bad projection attempt!" << endl;
		delete TheProjection;
		return(false);
	}

	ProjectionInfo.SetUpBoard(TheProjection, X1, X2, Y1, Y2, Radius, Radius);

	delete TheProjection;

        return(true);
}


bool LoadClusterBoard(ClusterBoard &EmptyBoard, const vector <LonLatAnomDate> &TheMembers,
                      const BoardConvertor &ProjectionTranslator)
// returns false if an improper gridpoint is found
// true if the board is loaded correctly
{
        for (vector<LonLatAnomDate>::const_iterator AMember( TheMembers.begin() ); AMember != TheMembers.end(); AMember++)
	{
		// If I can guarrentee that there will be no NaNs, then I can take this out for a slight performance boost.
		if (isnan(AMember->Lon))
		{
			// no need to load any nans into the board.
			continue;
		}

               	size_t XLoc, YLoc;
		if (!ProjectionTranslator.SphericalToGrid(AMember->Lon, AMember->Lat, XLoc, YLoc))
		{
                        cerr << "Bad Location    XLoc: " << XLoc << "  YLoc: " << YLoc 
			     << "   Lon: " << AMember->Lon << "  Lat: " << AMember->Lat << endl;
			return(false);
               	}

//		cerr << "Adding Value...";
                BoardType TempMember;
       	        TempMember.AddMember(*AMember);
//		cerr << "done\n";

//		cerr << "Inserting value... XLoc: " << XLoc << "  YLoc: " << YLoc << " ...";
               	EmptyBoard.InsertValue(XLoc, YLoc, TempMember);
//		cerr << "done\n";
	}


	return(true);
}


LonLatAnom LoadForecast(const string &FileName, const string &DateOfEvent)
{
        ifstream InputList(FileName.c_str());

        if (!InputList.is_open())
        {
                throw("Could not open the forecast file: " + FileName);
        }

        string LineRead = "";

        getline(InputList, LineRead);

	const size_t DateOfEventLength = DateOfEvent.size();
		
	while (InputList.good())
        {
		// The Date of the event is at the end of the line, so check there.
		if (LineRead.find(DateOfEvent, LineRead.size() - DateOfEventLength) != string::npos)
		{
			const vector <string> TempHold( TakeDelimitedList(LineRead, ' ') );
		        if (TempHold.size() < 4)
        		{
		                throw("Did not finish file properly.  Filename: " + FileName + '\n'
                		      + "\t\tThe last line read: " + LineRead + '\n'
		                      + "\t\tDelimitedList--\n"
                		      + GiveDelimitedList(TempHold, '\n') + '\n');
        		}

		        LonLatAnom ForecastState;
		        ForecastState.Lon = StrToDouble(TempHold[0]);
		        ForecastState.Lat = StrToDouble(TempHold[1]);
		        ForecastState.StdAnom = StrToDouble(TempHold[2]);

			InputList.close();
			return(ForecastState);
		}
			
                getline(InputList, LineRead);
	}// end while(good) loop

	InputList.close();

	cerr << "ERROR: Could not find the forecast!" << endl;
	LonLatAnom BadState;		// Default constructor initializes with NANs

	return(BadState);
}


// Still needed for some backwards-compatibility programs...
bool WriteLonLatAnoms(const vector <LonLatAnom> &TheMembers, const string &OutFileName)
{
	ClusterFile clusterStream(OutFileName.c_str(), ios::out);

        if (!clusterStream.is_open())
        {
		cerr << "ERROR: Could not open file " << OutFileName << " for writing.\n";
		return(false);
	}

	bool saveState = clusterStream.SaveCluster(TheMembers);

	clusterStream.close();

	return(saveState);       	
}


// Still needed for backwards-compatibility programs...
bool WriteFieldMeasure(const double &Alpha, const double &Phi, const double &GammaMaxValue, const double &ChiMaxValue, const string &OutFileName)
{
        FieldMeasFile fieldMeasStream(OutFileName.c_str(), ios::out);

	if (!fieldMeasStream.is_open())
        {
        	cerr << "ERROR: Could not open the field measure file: " << OutFileName << endl;
                return(false);
	}
                
	FieldMeasure theFieldMeasure(Alpha, Phi, GammaMaxValue, ChiMaxValue);
	
	bool saveState = fieldMeasStream.SaveFieldMeasure(theFieldMeasure);

	fieldMeasStream.close();

	return(saveState);
}


bool SaveEventScores(const string &FileName, const vector <double> &EventScores, const string &dateOfEvent)
// NOT THE COMPLEMENT OF LoadEventScores() below...
{
	// I forget the reason why this is the way it is.
	// TODO: Find out why this is done this way.
	if (EventScores.empty())
	{
		return(true);
	}

	// Typically, this file already has data in it and this is just the next set of scores to add to it.
	// kinda like a log file.
	EventScoreFile scoreStream(FileName.c_str(), ios::app);

        if (!scoreStream.is_open())
        {
	        cerr << "ERROR: Could not open EventScore file: " << FileName << " for writing." << endl;
                return(false);
        }

        bool saveState = scoreStream.SaveEventScores(EventScores, dateOfEvent);

        scoreStream.close();

	return(saveState);
}

bool LoadEventScores(vector <double> &EventScores, vector <string> &Dates, const string &Filename, const string &TableName)
// This function is designed to retrieve ONE event type's eventscores.
// And it doesn't even do that well enough.
// I do not like this function very much and plan on changing this around...
{
	EventScoreFile scoreStream(Filename.c_str(), ios::in);

	if (!scoreStream.is_open())
        {
		cerr << "ERROR: Could not open event score file: " << Filename << endl;
                return(false);
	}

        pair< vector<double>, vector<string> > scoreInfo = scoreStream.RetrieveEventScores();

        scoreStream.close();

	bool readState = true;

	if (scoreInfo.first.empty())
	{
		cerr << "ERROR: Could not read the event score file, " << Filename << " properly\n";
		readState = false;
	}

	EventScores.insert(EventScores.end(), scoreInfo.first.begin(), scoreInfo.first.end());
	Dates.insert(Dates.end(), scoreInfo.second.begin(), scoreInfo.second.end());

	return(readState);
}

// TODO: These threshold value things might get refactored into a ThresholdValueFile class interface.
bool SaveThresholdVals(const string &Filename, const vector <string> &TableNames, const vector <double> &ThresholdVals)
{
	ofstream ThreshStream(Filename.c_str());

	if (!ThreshStream.is_open())
	{
		cerr << "ERROR: Couldn't open the threshold file: " << Filename << endl;
		return(false);
	}

	ThreshStream << GiveDelimitedList(TableNames, ' ') << endl;
	ThreshStream << GiveDelimitedList(DoubleToStr(ThresholdVals), ' ') << endl;

	ThreshStream.close();

	return(true);
}

bool LoadThresholdVals(const string &Filename, vector <string> &TableNames, vector <double> &ThresholdVals)
{
        ifstream ThreshStream(Filename.c_str());

        if (!ThreshStream.is_open())
        {
                cerr << "ERROR: Couldn't open the threshold file: " << Filename << endl;
                return(false);
        }

	string LineRead;
	getline(ThreshStream, LineRead);

	TableNames = TakeDelimitedList(LineRead, ' ');

	getline(ThreshStream, LineRead);
	ThresholdVals = StrToDouble(TakeDelimitedList(LineRead, ' '));

        ThreshStream.close();

        return(true);
}

#endif
