using namespace std;

#include <fstream>
#include <iostream>		// for printing error messages
#include <vector>

#include "Utils/LonLatAnom.h"            // for LonLatAnom structure
#include "Utils/ClusterFile.h"


ClusterFile::ClusterFile(const char* filename, const ios::openmode &theOpenMode)
{
	open(filename, theOpenMode);
}

ClusterFile::~ClusterFile()
{
	close();
}

LonLatAnom
ClusterFile::RetrieveMember()
{
	float tempLon, tempLat, tempStdAnom;
	*this >> tempLon >> tempLat >> tempStdAnom;
	LonLatAnom theMember(tempLon, tempLat, tempStdAnom);

	return(theMember);
}

vector<LonLatAnom>
ClusterFile::RetrieveCluster()
{
	vector<LonLatAnom> clusterMembers(0);

	LonLatAnom aMember = RetrieveMember();

	while (good())
	{
		clusterMembers.push_back(aMember);
		aMember = RetrieveMember();
	}

	if (bad() || !eof())
	{
		cerr << "ERROR: Problem while reading the cluster file.\n";
		clusterMembers.clear();
	}

	return(clusterMembers);
}

bool
ClusterFile::SaveMember(const LonLatAnom &theMember)
{
	// TODO: Set a precision level of 5 for longitude and latitude, and 7 for standard anomaly
	*this << theMember.Lon << ' '
	      << theMember.Lat << ' '
	      << theMember.StdAnom << '\n';

	return(good());
}

bool
ClusterFile::SaveCluster(const vector<LonLatAnom> &clusterMembers)
{
	for (vector<LonLatAnom>::const_iterator aMember = clusterMembers.begin();
	     aMember != clusterMembers.end();
	     aMember++)
	{
		SaveMember(*aMember);
	}

	return(good());
}

