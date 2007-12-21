#ifndef _LONLATANOM_H
#define _LONLATANOM_H


struct LonLatAnom
{
	LonLatAnom();
	LonLatAnom(const float &newLon, const float &newLat, const float &newStdAnom);

	float Lon;
	float Lat;
	float StdAnom;
};

// TODO: Refactor away.
#include "LonLatAnom_Temporary.h"

#endif
