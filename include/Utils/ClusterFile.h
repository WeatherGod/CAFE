#ifndef _CLUSTERFILE_H
#define _CLUSTERFILE_H

#include <fstream>
#include <vector>

#define MYSQLPP_SSQLS_NO_STATICS       // makes sure that the SSQL structs are only declared, not defined.
#include "CAFE_SQLStructs.h"            // for LonLatAnom

class ClusterFile : public fstream
{
	public:
		vector<LonLatAnom> RetrieveCluster();
		LonLatAnom RetrieveMember();

		bool SaveCluster(const vector<LonLatAnom> &clusterMembers);
		bool SaveMember(const LonLatAnom &theMember);
		~ClusterFile();
};

#endif
