#ifndef _PEAKVALLEYFILE_H
#define _PEAKVALLEYFILE_H

#include <fstream>
#include <vector>
#include <cctype>	// for size_t
#include "Utils/LonLatAnom.h"		// for LonLatAnom structure

class PeakValleyFile : public fstream
{
	public:
		PeakValleyFile(const char* filename, const ios::openmode &theOpenMode);
		~PeakValleyFile();

		// This could still be improved further...
		vector <LonLatAnom> RetrieveExtrema(const size_t &variableCnt, const size_t &extremumCnt);
		bool SaveExtrema(const vector<LonLatAnom> &extremaInfo, const size_t &variableCnt, const size_t &extremumCnt);


	private:
		// Placed in private so that it won't be used.
		// This will not be defined.
		PeakValleyFile(const PeakValleyFile& copyFile);
};

#endif
