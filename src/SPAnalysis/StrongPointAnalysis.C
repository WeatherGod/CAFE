#ifndef _STRONGPOINTANALYSIS_C
#define _STRONGPOINTANALYSIS_C

using namespace std;

#include <vector>
#include <algorithm>			// for find()

#include "SPAnalysis/StrongPointAnalysis.h"

#include "SPAnalysis/BoardType.h"			// for the BoardType class
#include "SPAnalysis/ClusterBoard.h"		// for the ClusterBoard class
#include "Utils/CAFEEqtns.h"			// for AlphaCalc()



StrongPointAnalysis::StrongPointAnalysis()
	:	myClusters(0),
		myXsize(0),
		myYsize(0)
{
}

void StrongPointAnalysis::ReceiveUnclusteredGrid(const ClusterBoard &OrigGrid)
{
	myOrigGrid = OrigGrid;
	myXsize = OrigGrid.myBoardConfig.NumCols();
	myYsize = OrigGrid.myBoardConfig.NumRows();
}

void StrongPointAnalysis::PrintOrigGrid() const
{
	myOrigGrid.PrintBoard();
}

void StrongPointAnalysis::PrintCluster(const size_t &ClusterIndex) const
{
	myClusters.at(ClusterIndex).PrintBoard();
}

size_t StrongPointAnalysis::NumberOfClusters() const
{
	return(myClusters.size());
}

void StrongPointAnalysis::ResetClusters()
{
	myClusters.resize(0);
}

void StrongPointAnalysis::ResetOrigGrid()
{
	myOrigGrid.ResetBoard();
}

ClusterBoard StrongPointAnalysis::ReturnCluster(const size_t &ClusterIndex) const
{
	return(myClusters.at(ClusterIndex));
}

double StrongPointAnalysis::CalcAlpha(const size_t &ClusterIndex, const int &EventOccurranceCnt) const
{
	const int ClusterMemberCount = myClusters.at(ClusterIndex).TotalMemberCount();
	const int ClusterGridPointCount = myClusters.at(ClusterIndex).TotalGridPointsUsed();
	const int TotalGridPoints = myXsize * myYsize;

	double ClusterDensity = 0.0;
	double AverageDensity = 0.0;

	if (ClusterGridPointCount != 0 && TotalGridPoints != 0)
	{
		ClusterDensity = ((double) ClusterMemberCount)/((double) ClusterGridPointCount);
		AverageDensity = ((double) myOrigGrid.TotalMemberCount())/((double) TotalGridPoints);
	}

	return(AlphaCalc(ClusterDensity, AverageDensity, ClusterMemberCount, EventOccurranceCnt));
}

double StrongPointAnalysis::CalcPhi(const size_t &ClusterIndex, const int &EventOccurranceCnt) const
{
	return(myClusters.at(ClusterIndex).CalcPhi(EventOccurranceCnt));
}

//-----------------------------------------------------------------------------------------------------------
//------------------------- Clustering Algorithm ------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

void StrongPointAnalysis::ClusterTheGrid()
{
	for (int Xindex = 0; Xindex < myXsize; Xindex++)
	{
		for (int Yindex = 0; Yindex < myYsize; Yindex++)
		{
			ClusterBoard TempBoard(myOrigGrid.myBoardConfig);

			NetworkFrom(Xindex, Yindex, TempBoard);

			if (find(myClusters.begin(), myClusters.end(), TempBoard) == myClusters.end())
			{
				// TempBoard has a new and unique cluster...
				myClusters.push_back(TempBoard);
			}
		}
	}
}

void StrongPointAnalysis::NetworkFrom(const int Xindex, const int Yindex, ClusterBoard &NewCluster)
{
	if (myOrigGrid.IsStrongPoint(Xindex, Yindex))
	{
		NewCluster.AddStrongPoint(Xindex, Yindex, myOrigGrid);

		//  now check surrounding points
		for (int XLoc = Xindex - 1; XLoc <= (Xindex + 1); XLoc++)
		{
			if (XLoc >= 0 && XLoc < myXsize)
			{
				for (int YLoc = Yindex - 1; YLoc <= (Yindex + 1); YLoc++)
				{
					if (YLoc >= 0 && YLoc < myYsize)
					{
						if (XLoc != Xindex || YLoc != Yindex)
						{
							// Don't network from the same location as indicated by (Xindex, Yindex) again.
							// And, don't network from a spot that already been networked from.
							if (!NewCluster.BeenChecked(XLoc, YLoc))
							{
								NetworkFrom(XLoc, YLoc, NewCluster);
							}
						}
					}
				}
			}
		}
	}
}

//----------------------------------- This is the  padding portion ------------------------------------------------------------------------
//  The main clustering code merely finds the various skeletons in the data.
//      the purpose of the padding algorithm is to then complete the clustering by padding the cluster networks.
//	essentially, it is adding the meat to the bones.
//      The points that are padded onto the main cluster are called "weak points"

void StrongPointAnalysis::PadCluster(const size_t &ClusterIndex)
{
	for (size_t x = 0; x < myXsize; x++)
	{
		for (size_t y = 0; y < myYsize; y++)
		{
			if (myClusters[ClusterIndex].IsStrongPoint(x, y))
			//  Ah, I found a strong point.  Now I will check the points immediately surrounding it to see if they are weak points
			{
				for (size_t XLoc = x - 1; XLoc <= (x + 1); XLoc++)
				{
					if (XLoc >= 0 && XLoc < myXsize)
					{
						for (size_t YLoc = y - 1; YLoc <= (y + 1); YLoc++)
						{
							if (YLoc >= 0 && YLoc < myYsize)
							{
								if (YLoc != y || XLoc != x)
								// don't check at the center location.
								{
									if (!myClusters[ClusterIndex].BeenChecked(XLoc, YLoc))
									// don't bother with a point that has already been added
									{
										if (myClusters[ClusterIndex].IsWeakPoint(XLoc, YLoc, myOrigGrid))
										{
											myClusters[ClusterIndex].AddWeakPoint(XLoc, YLoc, myOrigGrid);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
	


#endif
