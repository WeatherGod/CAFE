#ifndef _CLUSTER_CMDLINE_H
#define _CLUSTER_CMDLINE_H

#include <vector>
#include <string>

struct Cluster_CmdOptions
{
        double StdDeviationValue;
	double LowStdDeviationValue;
	int SeekingRadius;
	int TouchingParameter;
	bool DoPause;
	bool ShowBestCluster;
	bool ShowAllClusters;
	bool ShowMap;
	bool ShowInfo;
	bool SaveBestCluster;
	vector <string> OptionSwitchers;
	vector <size_t> ExpectedValueCounts;

	Cluster_CmdOptions();
	bool IsAnOption(const string &SomeStr);
	int ParseCommandArgs(vector <string> &CmdArgs);
};


#endif
