using namespace std;

#include <cmath>	// for NAN

#include <Utils/LonLatAnom.h>


LonLatAnom::LonLatAnom()
	:	Lon(NAN),
		Lat(NAN),
		StdAnom(NAN)
{
}

LonLatAnom::LonLatAnom(const float &newLon, const float &newLat, const float &newStdAnom)
	:	Lon(newLon),
		Lat(newLat),
		StdAnom(newStdAnom)
{
}


// TODO: Refactor away.
#include "LonLatAnom_Temporary.C"

