using namespace std;

#include <iostream>
#include <string>
#include <vector>
#include <valarray>

#include <ctime>
#include <StrUtly.h>			// for StrToInt(), StrToDouble(), TakeDelimitedList()
#include <CmdLineUtly.h>		//for ProcessFlatCommandLine()
#include <TimeUtly.h>			// for GetTime(), GiveTime()

#include "GribFile.h"
#include <Projection_t.h>

// Right now, this program is going to act as a drop-in replacement for 
// my GrADS program "MasterControl.gs"
// The arguement list will be the same and the output from the program
// will be the same as before.

// I will later merge this program with AnomalyCollection.C to produce a
// program that truely will be the MasterControl for the anomaly collection.

string ExtremumInfo(const vector <float> &StdAnomalies, const vector <size_t> &Corner, const vector <size_t> &Edge)
{
	float PeakVal1 = -999;
	float PeakVal2 = -999;
	float ValleyVal1 = 999;
	float ValleyVal2 = 999;
	int Peak1X = -999;
	int Peak1Y = -999;
	int Peak2X = -999;
	int Peak2Y = -999;
	int Valley1X = -999;
	int Valley1Y = -999;
	int Valley2X = -999;
	int Valley2Y = -999;

	if (Edge[0] < 3 || Edge[1] < 3)
	{
		return("-999 -999 -999 -999 -999 -999 999 -999 -999 999 -999 -999");
	}

	vector<float>::const_iterator UpperLeft, UpperCent, UpperRight, MidRight, MidLeft, LowerLeft, LowerCent, LowerRight, CentVal;

	UpperLeft = StdAnomalies.begin();
	UpperCent = UpperLeft + 1;
	UpperRight = UpperCent + 1;

	MidLeft = UpperLeft + Edge[0];
	CentVal = MidLeft + 1;
	MidRight = CentVal + 1;

	LowerLeft = MidLeft + Edge[0];
	LowerCent = LowerLeft + 1;
	LowerRight = LowerCent + 1;

	int CoordX = 1;
	int CoordY = 1;

	size_t LowerRightPos = 2 * Edge[0] + 2;
//	cerr << "XSize: " << Edge[0] << "   YSize: " << Edge[1] << endl;
//	cerr << "Size: " << StdAnomalies.size() << endl;

//	cerr << "LowerRightPos: " << LowerRight - StdAnomalies.begin() << endl;

	for (; LowerRight != StdAnomalies.end();)
	{
		if ((CoordX + 1) >= Edge[0])
                {
                        // we have reached the edge on the x-axis,
                        // time to increment the Y coordinate.
                        CoordX = 1;
                        CoordY++;

                        UpperLeft += 2;
                        UpperCent += 2;
                        UpperRight += 2;

                        MidLeft += 2;
                        CentVal += 2;
                        MidRight += 2;

                        LowerLeft += 2;
                        LowerCent += 2;
                        LowerRight += 2;
                }


		if (IsDefined(*CentVal))
		{
			if (*CentVal >= *UpperLeft && *CentVal >= *UpperCent && *CentVal >= *UpperRight &&
			    *CentVal >= *MidLeft && *CentVal >= *MidRight &&
			    *CentVal >= *LowerLeft && *CentVal >= *LowerCent && *CentVal >= *LowerRight)
			{
				// Then it is a peak.
				if (*CentVal > PeakVal2)
				{
					if (*CentVal > PeakVal1)
					{
						PeakVal2 = PeakVal1;
						Peak2X = Peak1X;
						Peak2Y = Peak1Y;
						PeakVal1 = *CentVal;
						Peak1X = CoordX;
						Peak1Y = CoordY;
					}
					else
					{
						PeakVal2 = *CentVal;
						Peak2X = CoordX;
						Peak2Y = CoordY;
					}
				}
			}
			else if (*CentVal <= *UpperLeft && *CentVal <= *UpperCent && *CentVal <= *UpperRight &&
                            	 *CentVal <= *MidLeft && *CentVal <= *MidRight &&
                            	 *CentVal <= *LowerLeft && *CentVal <= *LowerCent && *CentVal <= *LowerRight)
			{
				// Then it is a valley
				if (*CentVal < ValleyVal2)
				{
					if (*CentVal < ValleyVal1)
                                        {
                                                ValleyVal2 = ValleyVal1;
                                                Valley2X = Valley1X;
                                                Valley2Y = Valley1Y;
                                                ValleyVal1 = *CentVal;
                                                Valley1X = CoordX;
                                                Valley1Y = CoordY;
                                        }
                                        else
                                        {
                                                ValleyVal2 = *CentVal;
                                                Valley2X = CoordX;
                                                Valley2Y = CoordY;
                                        }
				}
			}
		}// end if (IsDefined())

		CoordX++;
		UpperLeft++;
		UpperCent++;
		UpperRight++;
		MidLeft++;
		CentVal++;
		MidRight++;
		LowerLeft++;
		LowerCent++;
		LowerRight++;
	}// end for-loop

	if (Peak1X != -999)
	{
		Peak1X += Corner[0];
		Peak1Y += Corner[1];
	}

	if (Peak2X != -999)
	{
		Peak2X += Corner[0];
		Peak2Y += Corner[1];
	}

	if (Valley1X != -999)
	{
		Valley1X += Corner[0];
		Valley1Y += Corner[1];
	}

	if (Valley2X != -999)
	{
		Valley2X += Corner[0];
		Valley2Y += Corner[1];
	}

	return(FloatToStr(PeakVal1) + ' ' + IntToStr(Peak1X) + ' ' + IntToStr(Peak1Y) + ' ' +
	       FloatToStr(PeakVal2) + ' ' + IntToStr(Peak2X) + ' ' + IntToStr(Peak2Y) + ' ' +
	       FloatToStr(ValleyVal1) + ' ' + IntToStr(Valley1X) + ' ' + IntToStr(Valley1Y) + ' ' +
	       FloatToStr(ValleyVal2) + ' ' + IntToStr(Valley2X) + ' ' + IntToStr(Valley2Y));
}


				


void PeakValStuff(const vector <float> &StdAnomalies, const Projection_t* TheProjection, const vector <size_t> &Corner, const vector <size_t> &Edge)
{
/*
	cout << "        ";
	double Lat, Lon;
	for (size_t Xindex = 0; Xindex < Edge[0]; Xindex++)
	{
		TheProjection->XYToLatLon(Xindex + Corner[0], Corner[1], Lat, Lon);
		printf("%7.3f ", Lon);
	}

	cout << endl;

	vector<float>::const_iterator AValue = StdAnomalies.begin();
	for (size_t Yindex = 0; Yindex < Edge[1]; Yindex++)
	{
		TheProjection->XYToLatLon(Corner[0], Yindex + Corner[1], Lat, Lon);
		printf("%7.3f ", Lat);

		for (size_t Xindex = 0; Xindex < Edge[0]; Xindex++, AValue++)
		{
			printf("%7.5f ", *AValue);
		}
		cout << endl;
	}
*/

//	cerr << "Extremum";
	string Extremum = ExtremumInfo(StdAnomalies, Corner, Edge);
	vector <string> ExtremumList = TakeDelimitedList(Extremum, ' ');
//	cerr << " captured\n";

	string Peak1 = ExtremumList[0];
	int Peak1X = StrToInt(ExtremumList[1]);
	int Peak1Y = StrToInt(ExtremumList[2]);
	double Peak1Lat = -999;
	double Peak1Lon = -999;
	string Peak2 = ExtremumList[3];
	double Peak2Lat = -999;
	double Peak2Lon = -999;
	int Peak2X = StrToInt(ExtremumList[4]);
	int Peak2Y = StrToInt(ExtremumList[5]);

	string Valley1 = ExtremumList[6];
        int Valley1X = StrToInt(ExtremumList[7]);
        int Valley1Y = StrToInt(ExtremumList[8]);
	double Valley1Lat = -999;
	double Valley1Lon = -999;
        string Valley2 = ExtremumList[9];
        int Valley2X = StrToInt(ExtremumList[10]);
        int Valley2Y = StrToInt(ExtremumList[11]);
	double Valley2Lat = -999;
	double Valley2Lon = -999;

	if (Peak1 == "-999")
	{
		Peak2 = "-999";
	}
	else
	{
		TheProjection->XYToLatLon(Peak1X, Peak1Y, Peak1Lat, Peak1Lon);

		if (Peak2 != "-999")
		{
			TheProjection->XYToLatLon(Peak2X, Peak2Y, Peak2Lat, Peak2Lon);
		}
	}

	if (Valley1 == "999")
        {
                Valley2 = "999";
        }
        else
        {
                TheProjection->XYToLatLon(Valley1X, Valley1Y, Valley1Lat, Valley1Lon);

                if (Valley2 != "999")
                {
                        TheProjection->XYToLatLon(Valley2X, Valley2Y, Valley2Lat, Valley2Lon);
                }
        }

	cout << "---^^^---Start\n"
	     << Peak1 << ' ' << Peak1Lon << ' ' << Peak1Lat << ' ' << Peak1X << ' ' << Peak1Y << '\n'
	     << Peak2 << ' ' << Peak2Lon << ' ' << Peak2Lat << ' ' << Peak2X << ' ' << Peak2Y << '\n'
	     << Valley1 << ' ' << Valley1Lon << ' ' << Valley1Lat << ' ' << Valley1X << ' ' << Valley1Y << '\n'
	     << Valley2 << ' ' << Valley2Lon << ' ' << Valley2Lat << ' ' << Valley2X << ' ' << Valley2Y << '\n'
	     << "---^^^---End" << endl;
}


string FixnDate(const string &DateTimeStr)
{
	string HourStr = DateTimeStr.substr(0,3);
	string DayStr = DateTimeStr.substr(3,2);
	string MonthStr = DateTimeStr.substr(5,3);
	string YearStr = "1979";	// just a dummy year

	if (DayStr == "29" && StrToUpper(MonthStr) == "FEB")
	{
		DayStr = "28";
	}

	return(HourStr + DayStr + MonthStr + YearStr);
}

int main(int argc, char *argv[])
{
	const vector <string> TheArgs = ProcessFlatCommandLine(argc, argv);

//	const vector <string> TheArgs = TakeDelimitedList("06Z23SEP2003,1,28.0,-145.0,45.0,-55.0,TMP,2,500 mb,850 mb", ',');
//	const vector <string> TheArgs = TakeDelimitedList("06Z23SEP2003,1,28.0,-100.0,32.0,-97.0,TMP,2,500 mb,850 mb", ',');

	if (TheArgs.size() < 2)
	{
		cerr << "Syntax error:  I need, at least, the date in GrADS format and the variable count, in that order..." << endl;
		return(8);
	}

	GribFile Climo;
	GribFile TheData;

	try
	{
		vector<string>::const_iterator AnArg = TheArgs.begin();

		const string DateTimeStr = *AnArg++;
		const int VarCount = StrToInt(*AnArg++);

		if (VarCount <= 0)
		{
			throw(VarCount);
		}

		const string ClimoDateStr = FixnDate(DateTimeStr);

		const string ClimoFileName = GiveTimeUTC(GetTimeUTC(ClimoDateStr, "%HZ%d%b%Y"), "narrclimo/%HZ_Smooth/narrclimo_%m%d_%H.grb");
		const string GribFileName = GiveTimeUTC(GetTimeUTC(DateTimeStr, "%HZ%d%b%Y"), "narr/%Y/%m/narr-a_221_%Y%m%d_%H00_000.grb");

		Climo.Open(ClimoFileName, 552);

		if (!Climo.Is_Open())
		{
			throw("Unable to load the climatology file: " + ClimoFileName);
		}

		TheData.Open(GribFileName);

		if (!TheData.Is_Open())
		{
			throw("unable to load the data file: " + GribFileName);
		}

		float Lat1(0.0), Lat2(0.0), Lon1(0.0), Lon2(0.0);

		if (TheArgs.size() < 6)
		{
			throw((string) "Missing bounding box information...");
		}
		
		Lat1 = StrToFloat(*AnArg++);
		Lon1 = StrToFloat(*AnArg++);
		Lat2 = StrToFloat(*AnArg++);
		Lon2 = StrToFloat(*AnArg++);

		int VarIndex = 0;
		while (VarIndex < VarCount && ((AnArg + 1) < TheArgs.end()))
		{
			const string VarName = *AnArg++;
			const int LevelCount = StrToInt(*AnArg++);
		
			if (LevelCount < 0)
			{
				throw((string) "Negative value for a level count...");
			}

			vector <string> TheLevels(max(LevelCount, 1), "");

			if (AnArg + LevelCount > TheArgs.end())
			{
				throw((string) "not enough arguements for this variable: " + VarName);
			}

			TheLevels.assign(AnArg, AnArg + LevelCount);
			AnArg += LevelCount;
			

			if (VarName == "THK")
			{
				for (vector<string>::const_iterator LevelName = TheLevels.begin();
				     (LevelName + 2) < TheLevels.end(); LevelName++)
				{
					const RecordID_t BotLevel("HGT", *LevelName++);
					const RecordID_t TopLevel("HGT", *LevelName++);

					const RecordID_t MeanID("THK_A", *LevelName);
					const RecordID_t StdDevID("THK_S", *LevelName);

					if (!TheData.TakeRecord_BoundingBox(BotLevel, Lat1, Lon1, Lat2, Lon2))
					{
						throw("couldn't set the bounding box for: " + VarName);
					}

					if (!TheData.TakeRecord_BoundingBox(TopLevel, Lat1, Lon1, Lat2, Lon2))
                                        {
                                                throw("couldn't set the bounding box for: " + VarName);
                                        }

					if (!Climo.TakeRecord_BoundingBox(MeanID, Lat1, Lon1, Lat2, Lon2))
                                        {
                                                throw("couldn't set the bounding box for: " + VarName + "_A");
                                        }

					if (!Climo.TakeRecord_BoundingBox(StdDevID, Lat1, Lon1, Lat2, Lon2))
					{
						throw("couldn't set the bounding box for: " + VarName + "_S");
					}

					valarray<float> StdAnomalies = (TheData.GiveRecord_Data_ValArray(TopLevel, false) - 
												TheData.GiveRecord_Data_ValArray(BotLevel, false)
									- Climo.GiveRecord_Data_ValArray(MeanID, false)) 
								       / Climo.GiveRecord_Data_ValArray(StdDevID, false);

					// Dangerous!!!
					vector <float> Holder(&StdAnomalies[0], &StdAnomalies[0] + StdAnomalies.size());

					PeakValStuff(Holder, TheData.GiveRecord_Projection(TopLevel),
						     TheData.GiveRecord_Corner(TopLevel), TheData.GiveRecord_Edge(TopLevel));
				}
			}
			else if (VarName == "SHEAR")
			{
				for (vector<string>::const_iterator LevelName = TheLevels.begin();
                                     (LevelName + 2) < TheLevels.end(); LevelName++)
                                {
                                        const RecordID_t BotLevel("HGT", *LevelName),
							 BotLevUWnd("UGRD", *LevelName),
							 BotLevVWnd("VGRD", *LevelName++);
                                        const RecordID_t TopLevel("HGT", *LevelName),
							 TopLevUWnd("UGRD", *LevelName),
							 TopLevVWnd("VGRD", *LevelName++);

                                        const RecordID_t MeanID("SHEAR_A", *LevelName);
                                        const RecordID_t StdDevID("SHEAR_S", *LevelName);

                                        if (!TheData.TakeRecord_BoundingBox(BotLevel, Lat1, Lon1, Lat2, Lon2))
                                        {
                                                throw("couldn't set the bounding box for HGT for: " + VarName);
                                        }

					if (!TheData.TakeRecord_BoundingBox(BotLevUWnd, Lat1, Lon1, Lat2, Lon2))
                                        {
                                                throw("couldn't set the bounding box for UGRD for: " + VarName);
                                        }

					if (!TheData.TakeRecord_BoundingBox(BotLevVWnd, Lat1, Lon1, Lat2, Lon2))
                                        {
                                                throw("couldn't set the bounding box for VGRD for: " + VarName);
                                        }



                                        if (!TheData.TakeRecord_BoundingBox(TopLevel, Lat1, Lon1, Lat2, Lon2))
                                        {
                                                throw("couldn't set the bounding box for HGT for: " + VarName);
                                        }

					if (!TheData.TakeRecord_BoundingBox(TopLevUWnd, Lat1, Lon1, Lat2, Lon2))
                                        {
                                                throw("couldn't set the bounding box for UGRD for: " + VarName);
                                        }

					if (!TheData.TakeRecord_BoundingBox(TopLevVWnd, Lat1, Lon1, Lat2, Lon2))
                                        {
                                                throw("couldn't set the bounding box for VGRD for: " + VarName);
                                        }



                                        if (!Climo.TakeRecord_BoundingBox(MeanID, Lat1, Lon1, Lat2, Lon2))
                                        {
                                                throw("couldn't set the bounding box for: " + VarName + "_A");
                                        }

                                        if (!Climo.TakeRecord_BoundingBox(StdDevID, Lat1, Lon1, Lat2, Lon2))
                                        {
                                                throw("couldn't set the bounding box for: " + VarName + "_S");
                                        }

                                        valarray<float> StdAnomalies =  (sqrt( pow( TheData.GiveRecord_Data_ValArray(TopLevUWnd, false)
                                                         			    - TheData.GiveRecord_Data_ValArray(BotLevUWnd, false), 
										   2.0f)
                                                    			     + pow( TheData.GiveRecord_Data_ValArray(TopLevVWnd, false)
                                                           			    - TheData.GiveRecord_Data_ValArray(BotLevVWnd, false), 
										   2.0f))
									   / (TheData.GiveRecord_Data_ValArray(TopLevel, false)
									      - TheData.GiveRecord_Data_ValArray(BotLevel, false))
								         - Climo.GiveRecord_Data_ValArray(MeanID, false)
                                                                        )
                                                                        / Climo.GiveRecord_Data_ValArray(StdDevID, false);

                                        // Dangerous!!!
                                        vector <float> Holder(&StdAnomalies[0], &StdAnomalies[0] + StdAnomalies.size());

                                        PeakValStuff(Holder, TheData.GiveRecord_Projection(TopLevel),
                                                     TheData.GiveRecord_Corner(TopLevel), TheData.GiveRecord_Edge(TopLevel));
                                }
			}
			else
			{
				for (vector<string>::const_iterator LevelName = TheLevels.begin();
                                     LevelName < TheLevels.end(); LevelName++)
                                {
					const RecordID_t TheLevel(VarName, *LevelName);
					const RecordID_t MeanID(VarName + "_A", *LevelName);
					const RecordID_t StdDevID(VarName + "_S", *LevelName);

					if (!TheData.TakeRecord_BoundingBox(TheLevel, Lat1, Lon1, Lat2, Lon2))
                                        {
                                                throw("couldn't set the bounding box for: " + VarName + ':' + *LevelName);
                                        }

                                        if (!Climo.TakeRecord_BoundingBox(MeanID, Lat1, Lon1, Lat2, Lon2))
                                        {
                                                throw("couldn't set the bounding box for: " + VarName + "_A:" + *LevelName);
                                        }

                                        if (!Climo.TakeRecord_BoundingBox(StdDevID, Lat1, Lon1, Lat2, Lon2))
                                        {
                                                throw("couldn't set the bounding box for: " + VarName + "_S:" + *LevelName);
                                        }
					
					//cerr << "Calculating...\n";
					valarray<float> StdAnomalies = (TheData.GiveRecord_Data_ValArray(TheLevel, false) - 
													Climo.GiveRecord_Data_ValArray(MeanID, false))
								       / Climo.GiveRecord_Data_ValArray(StdDevID, false);

					// Dangerous!!!
					vector <float> Holder(&StdAnomalies[0], &StdAnomalies[0] + StdAnomalies.size());
					//cerr << "PeakValStuff...\n";	
					PeakValStuff(Holder, TheData.GiveRecord_Projection(TheLevel),
						     TheData.GiveRecord_Corner(TheLevel), TheData.GiveRecord_Edge(TheLevel));
				}
			}// end if (VarName == "THK")
		}// end while loop
	}
	catch (const exception &Err)
        {
                cerr << "ERROR: " << Err.what() << endl;
                cout << "---^^^---Start\n";
                cout << "---^^^---End" << endl;

                TheData.Close();
                Climo.Close();

                return(5);
        }
	catch (const string &ErrStr)
	{
		cerr << "ERROR: " << ErrStr << endl;
		cout << "---^^^---Start\n";
                cout << "---^^^---End" << endl;

                TheData.Close();
                Climo.Close();

		return(5);
	}
	catch (const int VarCnt)
	{
		if (VarCnt == 0)
		{
			cerr << "No variables to do..." << endl;
		}
		else
		{
			cerr << "ERROR: Invalid Variable Count: " << VarCnt << endl;
		}

		cout << "---^^^---Start\n";
		cout << "---^^^---End" << endl;

		TheData.Close();
		Climo.Close();

		if (VarCnt < 0)
		{
			return(6);
		}
		else
		{
			return(0);
		}
	}
	catch (...)
	{
		cerr << "ERROR: Unknown exception caught..." << endl;
		cout << "---^^^---Start\n";
		cout << "---^^^---End" << endl;
		Climo.Close();
		TheData.Close();
		return(7);
	}


	Climo.Close();
	TheData.Close();
	return(0);
}
	
