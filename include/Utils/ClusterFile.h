#ifndef _CLUSTERFILE_H
#define _CLUSTERFILE_H

#include <fstream>
#include <vector>

#include "Utils/LonLatAnom.h"		// for LonLatAnom struct

class ClusterFile : public fstream
{
	public:
		ClusterFile(const char* filename, const ios::openmode &theOpenMode);
		~ClusterFile();

		vector<LonLatAnom> RetrieveCluster();
		LonLatAnom RetrieveMember();

		bool SaveCluster(const vector<LonLatAnom> &clusterMembers);
		bool SaveMember(const LonLatAnom &theMember);

	private:
		// Placed in private so that it won't be used.
		// This will not be defined.
		ClusterFile(const ClusterFile &copyFile);
};

#endif
