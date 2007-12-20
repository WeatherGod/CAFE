#ifndef _PEAKVALLEYFILE_H
#define _PEAKVALLEYFILE_H

#include <fstream>
#include <vector>
#include <cctype>	// for size_t

class PeakValleyFile : public fstream
{
	public:
		vector <double> RetrieveExtrema(const size_t &variableCnt, const size_t &extremumCnt);
		bool SaveExtrema(const vector<double> &extremaInfo, const size_t &variableCnt, const size_t &extremumCnt);
		~PeakValleyFile();
};

#endif
