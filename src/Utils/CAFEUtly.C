#ifndef _CAFEUTLY_C
#define _CAFEUTLY_C

using namespace std;

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>	// for isnan(), nan()
#include <string>

#include <StrUtly.h>			// for StrToDouble(), DoubleToStr(), StripWhiteSpace(),
					// TakeDelimitedList(), GiveDelimitedList()


// These might go away soon...
#include "SPAnalysis/ClusterBoard.h"		// for the ClusterBoard class
#include "SPAnalysis/BoardConvertor.h"		// for the BoardConvertor class
#include "Config/Configuration.h"


#include <Projection_t.h>			// for the Projection_t base class
#include <TimeUtly.h>				// for GetTime(), GiveTime()

#include <unistd.h>				// for access()

#define MYSQLPP_SSQLS_NO_STATICS        // makes sure that the SSQL structs are only declared, not defined.
#include "Utils/CAFE_SQLStructs.h"		// for LonLatAnomDate, LonLatAnom type

#include "Utils/CAFEUtly.h"


bool GetGridInfo(const Configuration &ConfigInfo, BoardConvertor &ProjectionInfo, const size_t &Radius)
{
        double X1 = 0.0;
        double X2 = 0.0;
        double Y1 = 0.0;
        double Y2 = 0.0;

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

	const size_t DateOfEventLength( DateOfEvent.size() );
		
	while (InputList.good())
        {
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
	LonLatAnom BadState;
	BadState.Lon = BadState.Lat = BadState.StdAnom = nan("nan");

	return(BadState);
}


// Still needed for some backwards-compatibility programs...
bool WriteLonLatAnoms(const vector <LonLatAnom> &TheMembers, const string &OutFileName)
{
	ofstream NewFile(OutFileName.c_str());

        if (!NewFile.is_open())
        {
		cerr << "ERROR: Could not open file " << OutFileName << " for writing.\n";
		return(false);
	}


       	for (vector<LonLatAnom>::const_iterator AMember( TheMembers.begin() ); AMember != TheMembers.end(); AMember++)
	{
		NewFile << DoubleToStr(AMember->Lon, 5) << ' '
			<< DoubleToStr(AMember->Lat, 5) << ' '
			<< DoubleToStr(AMember->StdAnom, 7) << '\n';
	}

	NewFile.close();
}


// Still needed for backwards-compatibility programs...
bool WriteFieldMeasure(const double &Alpha, const double &Phi, const double &GammaMaxValue, const double &ChiMaxValue, const string &OutFileName)
{
        ofstream OutStream(OutFileName.c_str());

	if (!OutStream.is_open())
        {
        	cerr << "ERROR: Could not open the MaxGammaChi file: " << OutFileName << endl;
                return(false);
	}
                
	OutStream << DoubleToStr(Alpha, 7) << ' '
		  << DoubleToStr(Phi, 7) << '\n'
		  << DoubleToStr(GammaMaxValue, 7) << ' '
		  << DoubleToStr(ChiMaxValue, 7) << '\n';


	OutStream.close();

	return(true);
}


bool SaveEventScores(const string &FileName, const vector <double> &EventScores, const string &DateOfEvent)
// NOT THE COMPLEMENT OF LoadEventScores() below...
{
	if (EventScores.empty())
	{
		return(true);
	}

	ofstream EventScoreFile(FileName.c_str(), ios::app);

        if (!EventScoreFile.is_open())
        {
	        cerr << "ERROR: Could not open EventScore file: " << FileName << " for writing." << endl;
                return(false);
        }

        EventScoreFile << GiveDelimitedList(DoubleToStr(EventScores), ',');

        if (!DateOfEvent.empty())
        {
        	EventScoreFile << ',' << GiveTime( GetTime( StrToUpper(DateOfEvent), "%HZ%d%b%Y"), "%Y-%m-%d %H");
        }

        EventScoreFile << '\n';
        EventScoreFile.close();

	return(true);
}

// Not the best way of doing things, but, oh well...
bool LoadEventScores(vector <double> &EventScores, vector <string> &Dates, const string &Filename, const string &TableName)
// This function is designed to retrieve ONE event type's eventscores.
// I do not like this function very much and plan on changing this around...
{
	size_t colIndex = 0;

	ifstream EventScoreStream(Filename.c_str());

	const size_t OrigSize = EventScores.size();
	const size_t OrigDateSize = Dates.size();

	if (!EventScoreStream.is_open())
        {
		cerr << "\n\tCould not open event score file: " << Filename << endl;
                return(false);
	}

        string ReadLine = "";
        
        getline(EventScoreStream, ReadLine);

        while (!EventScoreStream.eof())
        {
               	StripWhiteSpace(ReadLine);
                vector <string> TempHold = TakeDelimitedList(ReadLine, ',');

                if (TempHold.size() == 2)			// at least one column of event scores and one for the date
                {
                       	EventScores.push_back(StrToDouble(TempHold[colIndex]));
			Dates.push_back(TempHold.back());
                }
                else
                {
                       	cerr << "\n\tCould not read properly from the event score file: " << Filename << endl;
                        cerr << "\t\tThe line: " << ReadLine << endl;
			EventScores.resize(OrigSize);
			Dates.resize(OrigDateSize);
                        EventScoreStream.close();
                        return(false);
                }

                getline(EventScoreStream, ReadLine);
	}

        EventScoreStream.close();

	return(true);
}

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
