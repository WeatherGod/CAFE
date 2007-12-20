#ifndef _STRONGPOINTANALYSIS_H
#define _STRONGPOINTANALYSIS_H

#include <vector>
#include "BoardType.h"		// for the BoardType class
#include "ClusterBoard.h"	// for the ClusterBoard class


class StrongPointAnalysis
{
	public:
		StrongPointAnalysis();
		void ReceiveUnclusteredGrid(const ClusterBoard &OrigGrid);
		

		void ClusterTheGrid();

		void PrintOrigGrid() const;
		void PrintCluster(const size_t &ClusterIndex) const;
		size_t NumberOfClusters() const;

		void ResetClusters();
		void ResetOrigGrid();

		void PadCluster(const size_t &ClusterIndex);
		ClusterBoard ReturnCluster(const size_t &ClusterIndex) const;
		double CalcAlpha(const size_t &ClusterIndex, const int &OrigGridMemberCount) const;
		double CalcPhi(const size_t &ClusterIndex, const int &OrigGridMemberCount) const;

	private:
		void NetworkFrom(const int Xindex, const int Yindex, ClusterBoard &NewCluster);
		
		size_t myXsize;
		size_t myYsize;

		ClusterBoard myOrigGrid;
		vector <ClusterBoard> myClusters;
};

#endif
