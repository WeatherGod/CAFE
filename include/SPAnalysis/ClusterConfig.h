#ifndef _CLUSTERCONFIG_H
#define _CLUSTERCONFIG_H

class ClusterConfig
{
	public:
		ClusterConfig();
		~ClusterConfig();

		void PrintConfiguration() const;
		void SetIgnoreHowMuchRule(const double IgnoreHowMuch);
		void SetStdDeviationValue(const double StdDevValue);
		void SetLowStdDeviationValue(const double StdDevValue);
		void SetUpperDensityThreshold(const double Thresh);
		void SetGridSize(const int Xsize, const int Ysize);
		void SetRadius(const int TheRadius);
		void SetTouchingRule(const int TouchingParam);

		int NumRows() const;
		int NumCols() const;
		double UpperDensityThreshold() const;
		double IgnoreHowMuchRule() const;
		double StdDeviationValue() const;
		double LowStdDeviationValue() const;
		int Radius() const;
		int TouchingRule() const;

	private:
		int myRowCnt;
		int myColCnt;
		double myIgnoreHowMuch;
		double myStdDevValue;
		double myLowStdDevValue;
		double myUpperDensityThresh;
		int myRadius;
		int myTouchingRule;
};

#endif
