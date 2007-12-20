#ifndef _CLUSTERBOARD_C
#define _CLUSTERBOARD_C

using namespace std;

#include "SPAnalysis/ClusterBoard.h"

#include <iostream>
#include <vector>
#include "SPAnalysis/ClusterConfig.h"
#include "SPAnalysis/BoardType.h"
#include <StatUtly.h>			// for Avg()
#include <DistUtly.h>			// for Distance()
#include <cmath>			// for powl()

#include "Utils/CAFEEqtns.h"			// for PhiCalc()


ClusterBoard::ClusterBoard()
	:	myBoardConfig(),
		myBoard(0),
		myPointTypes(0)
{
}


ClusterBoard::ClusterBoard(const ClusterConfig &TheConfig)
	:	myBoardConfig(TheConfig),
		myBoard(TheConfig.NumRows(), vector<BoardType>(TheConfig.NumCols())),
		myPointTypes(TheConfig.NumRows(), vector<int>(TheConfig.NumCols(), -1))
{
}

void ClusterBoard::InsertValue(const size_t XLoc, const size_t YLoc, const BoardType &NewValue)
// This function concatenates NewValue to whatever is already at myBoard[YLoc][XLoc]
//   If the gridpoint is still undefined (i.e.: -1) then change it to 0, indicating that data is there, but not yet determined
//   to be a strong point or a weak point.
{
	if (myPointTypes[YLoc][XLoc] == -1)
	{
		myPointTypes[YLoc][XLoc] = 0;	// zero for filled, but unknown point type
	}

	myBoard[YLoc][XLoc] += NewValue;		// use overloaded += to contatenate the value(s) into what already was there
}

void ClusterBoard::AddStrongPoint(const size_t XLoc, const size_t YLoc, const BoardType &NewValue)
{
	myPointTypes[YLoc][XLoc] = 1;		// 1 for strong point type
	myBoard[YLoc][XLoc] += NewValue;
}

void ClusterBoard::AddWeakPoint(const size_t XLoc, const size_t YLoc, const BoardType &NewValue)
{
	myPointTypes[YLoc][XLoc] = 2;		// 2 for weak point type
	myBoard[YLoc][XLoc] += NewValue;
}


void ClusterBoard::AnalyzeBoard()
{

	double MeanDensity = ((double) TotalMemberCount())/((double) GridSize());
	double TheVariance = DensityVariance();
	double UpperDensity = MeanDensity + (myBoardConfig.StdDeviationValue() * sqrt(TheVariance));
	double LowerDensity = max(MeanDensity - (myBoardConfig.LowStdDeviationValue() * sqrt(TheVariance)), 
				  0.0);
//	double LowerDensity = (myBoardConfig.StdDeviationValue() * sqrt(TheVariance));

//	if (LowerDensity > MeanDensity)
//	{
//		LowerDensity = MeanDensity;
//	}

	myBoardConfig.SetUpperDensityThreshold(UpperDensity);		// any point with a density above this amount automatically becomes a strongpoint.
	myBoardConfig.SetIgnoreHowMuchRule(LowerDensity);		// any point with a density below this amount will not become a strongpoint.
										// (might not be entirely correct..... can't remember the exact rule).
}


size_t ClusterBoard::TotalMemberCount() const
{
	size_t RunningSum = 0;

	for (size_t Y = 0; Y < myBoard.size(); Y++)
	{
		for (size_t X = 0; X < myBoard[Y].size(); X++)
		{
			RunningSum += myBoard[Y][X].MemberCount();
		}
	}

	return(RunningSum);
}

size_t ClusterBoard::TotalGridPointsUsed() const
{
	size_t RunningSum = 0;

	for (size_t Y = 0; Y < myBoard.size(); Y++)
	{
		for (size_t X = 0; X < myBoard[Y].size(); X++)
		{
			if (myPointTypes[Y][X] > 0)
			{
				RunningSum++;
			}
		}
	}

	return(RunningSum);
}

size_t ClusterBoard::GridSize() const
{
	if (myBoard.empty())
	{
		return(0);
	}
	else
	{
		return(myBoard.size() * myBoard[0].size());
	}
}

double ClusterBoard::DensityVariance() const
{
	vector <double> DensityVals(GridSize());

	size_t Indexer = 0;
	long double RunningSum = 0.0;
	for (size_t Y = 0; Y < myBoard.size(); Y++)
	{
		for (size_t X = 0; X < myBoard[Y].size(); X++)
		{
			DensityVals[Indexer] = (double) myBoard[Y][X].MemberCount();
			RunningSum += powl((long double) myBoard[Y][X].MemberCount(), 2.0);
			Indexer++;
		}
	}

	// using Gamma Distribution equations here...
	double MeanDensity = Avg(DensityVals);
	double BetaVal = ((RunningSum/((long double) DensityVals.size())) - pow(MeanDensity,2.0))/MeanDensity;
	double AlphaVal = pow(MeanDensity, 2.0)/((RunningSum/((double) DensityVals.size())) - pow(MeanDensity, 2.0));
//	double BetaVal = MeanDensity/AlphaVal;

//	cout << "GammaDistribution -- Alpha: " << AlphaVal << "   Beta: " << BetaVal << endl;

	return(AlphaVal*pow(BetaVal, 2.0));
}


void ClusterBoard::PrintBoard() const
// This only existed for debugging use.  It just so happened that the 
// domain was small enough to represent on the screen.  Don't use for regular use.
{
	if (myBoard.empty())
	{
		cout << "No map to print..." << endl;
		return;
	}

	int ColWidth = 2;
	cout << "\nX:";
	for (size_t X = 0; X < myBoard[0].size(); X++)
	{
		printf("% *u ", ColWidth, X);
	}

	cout << "\n/-";

	cout << string((ColWidth + 1) * myBoard[0].size(), '-');

	cout << "\\Y:\n";

	for (int Y = 0; Y < myBoard.size(); Y++)
	{
		cout << "| ";
		for (size_t X = 0; X < myBoard[Y].size(); X++)
		{
			if (myPointTypes[Y][X] == -1)
			{
				cout << "   ";
			}
			else if(myPointTypes[Y][X] == 0)
			{
				cout << " . ";
			}
			else if (myPointTypes[Y][X] == 1)
			{
				cout << " * ";
			}
			else if (myPointTypes[Y][X] == 2)
			{
				cout << " ~ ";
			}
			else
			{
				cout << " ? ";
			}
		}

		cout << '|' << Y << endl;
	}

	cout << "\\-";
	cout << (string((ColWidth + 1) * myBoard[0].size(), '-'));

	cout << "/\n";
}

void ClusterBoard::ResetBoard()
{
	for (size_t Y = 0; Y < myBoard.size(); Y++)
	{
		for (size_t X = 0; X < myBoard[Y].size(); X++)
		{
			myPointTypes[Y][X] = -1;
			myBoard[Y][X].Reset();
		}
	}
}

bool ClusterBoard::BeenChecked(const size_t XLoc, const size_t YLoc) const
{
	return(myPointTypes[YLoc][XLoc] > 0);
}

bool ClusterBoard::IsStrongPoint(const int Xindex, const int Yindex) const
//  The idea behind this method is that a "strong point" is most likely surrounded with other values
//     So, we look and see if we can "draw" a line through the point at (Xindex, Yindex) without hitting gridpoints of much lessor values
//     If we can draw that line, then that means that the point is most likely at the edge of a cluster or not on a cluster
//     If we can not draw that line, then the point is surrounded with other grid points with decent values, making it a strong point of the cluster

//   I also added a new rule that a point of a certain density, irregardless of surrounding values will test positive.
{
	if (myPointTypes[Yindex][Xindex] == 1)	// 1 is for strong point
	{
		// if it is already known that it is a strong point for whatever reason, then don't bother going through again
		return(true);
	}
	else if (myPointTypes[Yindex][Xindex] == 2 || myPointTypes[Yindex][Xindex] == -1)
	{
		return(false);
	}


	if (myBoard[Yindex][Xindex].ShouldIgnore(myBoardConfig))
	{
		// in other words, the point itself is too insigificant to be considered a strong point
		return(false);
	}

        if (!myBoard[Yindex][Xindex].SpecialStrongPointRule(myBoardConfig))
	//  The if statement asks if the point is strong enough to stand on its own.
	//    if it isn't, then it goes to the following block where it checks the surroundings
        {
		// The objective of the following algorithm is to prove that the point is NOT a strong point.
		// To prove that it is not a strong point, we try to draw a straight line through the point
		// with a radius of myBoardConfig.Radius() without hitting any other points of significance.
		// If such a line can be drawn, and the point isn't strong enough to stand on its own, then it is
		// likely that the point is on the edge of a cluster and shouldn't be considered a strong point.
		// If no such lines can not be drawn without hitting points of significance, then the point is 
		// surrounded by enough significant points to be considered a strong point.
                bool CanMakeLine = false;
                bool StillLooking = true;

                int Rise = 1;
                int Run = 1;

                while (!CanMakeLine && StillLooking)
                {
                        bool NoPoints = true;           // assume that the line exists, then prove that it doesn't

                        for (int Radial = -1; (Radial <= 1 && NoPoints); Radial++)
                        {
                                if (Radial != 0)
				//  when Radial == 0, then I am at the point that I am checking in the first place, so no need to check it
                                {
                                        int XLoc = Xindex + (Run * Radial);
                                        int YLoc = Yindex + (Rise * Radial);

                                        if (YLoc >= 0 && YLoc < myBoard.size() && XLoc >= 0 && XLoc < myBoard[0].size())
					// anything outside the domain is considered as insignificant points.
					// this does decrease the presence of strong points around the edge, but this is minimal as
					// the gridpoints can still be strong if there are significant points entirely on one side
                                        {
                                                if (!myBoard[YLoc][XLoc].ShouldIgnore(myBoardConfig, Distance(XLoc, YLoc, Xindex, Yindex)))
                                                {
							// in other words, a point of significance was found
							// I have also proven that a line can not be drawn in this direction without hitting
							//		significant points besides the center point.
                                                        NoPoints = false;
                                                }
                                        }
                                }
                        }

                        if (NoPoints)
                        {
				//  if NoPoints is true, then that means I was able to draw a line and the point is no longer considered a strong point
				//		I can stop looking for any more lines
                                CanMakeLine = true;
                                StillLooking = false;
                        }
                        else
                        {
				//  if NoPoints is false, then that means a line could be drawn and now I need to check and see if lines in other directions
				// could be drawn;
                                if (Rise == 1 && Run == 1)
                                {
                                        Run = 0;
                                }
                                else if (Rise == 1 && Run == 0)
                                {
                                        Run = -1;
				}
                                else if (Rise == 1 && Run == -1)
                                {
                                        Rise = 0;
                                }
                                else
                                {
					// I have done all the directions and could not make that line
                                        StillLooking = false;
                                }
                        }
                }

                return(!CanMakeLine);	// if no lines can be made, then the point is a strong point
        }
        else
        {
		// The point satisfied the Special Strong Point rule, which overrides the general rule.
                return(true);           // since the point has a sufficiently strong enough value, it is a strong point
        }
}



double ClusterBoard::StrongPointsTouch(const int Xloc, const int Yloc) const
//  This function counts the number of gridpoints immediately surrounding (XLoc, YLoc) that are strong points
//    and it weights the count according to distance from (Xloc, Yloc)
{
	double StrongPointCount = 0.0;
	for (int Yindex = Yloc - 1; Yindex <= Yloc + 1; Yindex++)
	{
                if (Yindex >= 0 && Yindex < myBoard.size())
                {
                        for (int Xindex = Xloc - 1; Xindex <= Xloc + 1; Xindex++)
                        {
                                if (Xindex >= 0 && Xindex < myBoard[Yindex].size())
                                {
                                        if (Xindex != Xloc || Yindex != Yloc)
					//  only count points that are surrounding (XLoc, YLoc), not (XLoc, YLoc) itself...
                                        {
                                                if (myPointTypes[Yindex][Xindex] == 1)	// 1 for strong point
						// notice that I am looking at the cluster's strong points, not the original grid's strong points.
						//   I do not want a neighboring cluster's strong points to contribute to the decision making.
                                                {
							StrongPointCount += 1.0 / Distance(Xloc, Yloc, Xindex, Yindex);
                                                }
                                        }
                                }
                        }
                }
        }

        return(StrongPointCount);
}


bool ClusterBoard::IsWeakPoint(const int XLoc, const int YLoc, const ClusterBoard &OrigBoard) const
{
	if (myPointTypes[YLoc][XLoc] == 2) 	// 2 for weak point type
	{
		// if already known, then don't go through the process again
		return(true);
	}
	else if (myPointTypes[YLoc][XLoc] == 1 || OrigBoard.myPointTypes[YLoc][XLoc] == -1)
	{
		return(false);
	}

	if (!OrigBoard.myBoard[YLoc][XLoc].ShouldIgnore(myBoardConfig))
	// if the point is significant enough
	{
		return(true);
	}

	double StrongSurroundValue = OrigBoard.myBoard[YLoc][XLoc].GiveValue() * StrongPointsTouch(XLoc, YLoc);
	StrongSurroundValue = StrongSurroundValue / ((double) myBoardConfig.TouchingRule());

	return(StrongSurroundValue >= myBoardConfig.IgnoreHowMuchRule());
}
	


void ClusterBoard::AddStrongPoint(const size_t XLoc, const size_t YLoc, const ClusterBoard &FromBoard)
{
	myPointTypes[YLoc][XLoc] = 1;	// one for strong point type
	myBoard[YLoc][XLoc] += FromBoard.myBoard[YLoc][XLoc];
}

void ClusterBoard::AddWeakPoint(const size_t XLoc, const size_t YLoc, const ClusterBoard &FromBoard)
{
	myPointTypes[YLoc][XLoc] = 2;	// 2 for weak point type
	myBoard[YLoc][XLoc] += FromBoard.myBoard[YLoc][XLoc];
}

BoardType ClusterBoard::ReturnMembers(const size_t XLoc, const size_t YLoc) const
{
	return(myBoard[YLoc][XLoc]);
}

double ClusterBoard::CalcPhi(const int &OccurranceCnt) const
{
	vector <LonLatAnomDate> AnomVals(0);

	for (size_t YLoc = 0; YLoc < myBoard.size(); YLoc++)
	{
		for (size_t XLoc = 0; XLoc < myBoard[YLoc].size(); XLoc++)
		{
			const vector <LonLatAnomDate> ShortList = myBoard[YLoc][XLoc].ReturnMembers();
			AnomVals.insert(AnomVals.end(), ShortList.begin(), ShortList.end());
		}
	}

	return(PhiCalc(AnomVals.begin(), AnomVals.end(), OccurranceCnt));
}


bool operator == (const ClusterBoard &Lefty, const ClusterBoard &Righty)
{
	if ((Lefty.myBoard.size() == Righty.myBoard.size()) && (Lefty.myBoard[0].size() == Righty.myBoard[0].size()))
	{
		for (size_t Y = 0; Y < Lefty.myBoard.size(); Y++)
		{
			for (size_t X = 0; X < Lefty.myBoard[Y].size(); X++)
			{
				if (Lefty.myBoard[Y][X] != Righty.myBoard[Y][X])
				{
					return(false);
				}
			}
		}
		
		return(true);
	}
	else
	{
		return(false);
	}
}

bool operator != (const ClusterBoard &Lefty, const ClusterBoard &Righty)
// notice that there is a slight difference between == and != functions.  First, if the sizes do not match, then they are not equal.
//    Second, if any element of the board is not equal to the corresponding element of the other board, then they are not equal.
{
	if ((Lefty.myBoard.size() == Righty.myBoard.size()) && (Lefty.myBoard[0].size() == Righty.myBoard[0].size()))
        {
               	for (size_t Y = 0; Y < Lefty.myBoard.size(); Y++)
               	{
                       	for (size_t X = 0; X < Lefty.myBoard[Y].size(); X++)
                       	{
                               	if (Lefty.myBoard[Y][X] != Righty.myBoard[Y][X])
                               	{
					return(true);
				}
			}
                }
			
		return(false);	// none of the elements were not equal to each other, they were all equal.
        }
}

#endif
