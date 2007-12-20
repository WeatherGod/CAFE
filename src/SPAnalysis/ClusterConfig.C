#ifndef _CLUSTERCONFIG_C
#define _CLUSTERCONFIG_C

using namespace std;

#include <iostream>
#include "SPAnalysis/ClusterConfig.h"


ClusterConfig::ClusterConfig()
	:	myRowCnt(0),
		myColCnt(0),
		myIgnoreHowMuch(0.0),
		myStdDevValue(0.0),
		myLowStdDevValue(0.0),
		myUpperDensityThresh(0.0),
		myRadius(0),
		myTouchingRule(9)
{
}

ClusterConfig::~ClusterConfig()
{
	~myRowCnt;
	~myColCnt;
	~myRadius;
	~myTouchingRule;
}


void ClusterConfig::PrintConfiguration() const
{
	cout << "Clustering Configuration:\n";
	cout << "Grid Size:       Cols (X): " << myColCnt << "    Rows (Y): " << myRowCnt << endl;
	cout << "Line Radius:     " << myRadius << "    TouchingRule: " << myTouchingRule << endl;
	cout << "StdDevValue:     " << myStdDevValue << "    LowStdDevValue: " << myLowStdDevValue << endl;
	cout << "UpperDensityThreshold: " << myUpperDensityThresh << "   IgnoreHowMuch:   " << myIgnoreHowMuch << endl;
}

void ClusterConfig::SetIgnoreHowMuchRule(const double IgnoreHowMuch)
{
	myIgnoreHowMuch = IgnoreHowMuch;
}

void ClusterConfig::SetStdDeviationValue(const double StdDevValue)
{
	myStdDevValue = StdDevValue;
}

void ClusterConfig::SetLowStdDeviationValue(const double LowStdDevValue)
{
	myLowStdDevValue = LowStdDevValue;
}

void ClusterConfig::SetUpperDensityThreshold(const double Thresh)
{
	myUpperDensityThresh = Thresh;
}

void ClusterConfig::SetGridSize(const int Xsize, const int Ysize)
{
	myColCnt = Xsize;
	myRowCnt = Ysize;
}

void ClusterConfig::SetRadius(const int TheRadius)
{
	myRadius = TheRadius;
}

void ClusterConfig::SetTouchingRule(const int TouchingParam)
{
	myTouchingRule = TouchingParam;
}


int ClusterConfig::NumRows() const
{
	return(myRowCnt);
}

int ClusterConfig::NumCols() const
{
	return(myColCnt);
}

double ClusterConfig::UpperDensityThreshold() const
{
	return(myUpperDensityThresh);
}

double ClusterConfig::IgnoreHowMuchRule() const
{
	return(myIgnoreHowMuch);
}

double ClusterConfig::StdDeviationValue() const
{
	return(myStdDevValue);
}

double ClusterConfig::LowStdDeviationValue() const
{
	return(myLowStdDevValue);
}

int ClusterConfig::Radius() const
{
	return(myRadius);
}

int ClusterConfig::TouchingRule() const
{
	return(myTouchingRule);
}


#endif
