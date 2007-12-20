#ifndef _CLUSTERBOARD_H
#define _CLUSTERBOARD_H

#include <vector>
#include "ClusterConfig.h"
#include "BoardType.h"


class ClusterBoard
{
	public:
		ClusterBoard();
		ClusterBoard(const ClusterConfig &TheConfig);

		void InsertValue(const size_t XLoc, const size_t YLoc, const BoardType &NewValue);
		void AddStrongPoint(const size_t XLoc, const size_t YLoc, const BoardType &Value);
		void AddWeakPoint(const size_t XLoc, const size_t YLoc, const BoardType &Value);
		void AddStrongPoint(const size_t XLoc, const size_t YLoc, const ClusterBoard &FromBoard);
		void AddWeakPoint(const size_t XLoc, const size_t YLoc, const ClusterBoard &FromBoard);

		void AnalyzeBoard();
		size_t TotalMemberCount() const;
		size_t TotalGridPointsUsed() const;
		size_t GridSize() const;
		BoardType ReturnMembers(const size_t XLoc, const size_t YLoc) const;


		void PrintBoard() const;

		void ResetBoard();
		
		bool BeenChecked(const size_t XLoc, const size_t YLoc) const;
		bool IsStrongPoint(const int XLoc, const int YLoc) const;
		bool IsWeakPoint(const int XLoc, const int YLoc, const ClusterBoard &OrigBoard) const;
		
		double CalcPhi(const int &OccurranceCnt) const;

		ClusterConfig myBoardConfig;

	private:
		double StrongPointsTouch(const int XLoc, const int YLoc) const;
		double DensityVariance() const;

		vector< vector<BoardType> > myBoard;
		vector< vector<int> > myPointTypes;


	friend bool operator == (const ClusterBoard &Lefty, const ClusterBoard &Righty);
	friend bool operator != (const ClusterBoard &Lefty, const ClusterBoard &Righty);

};

#endif
