using namespace std;

#include <cmath>		// for NAN

#include "Utils/FieldMeasure.h"


FieldMeasure::FieldMeasure()
	:	Alpha(NAN),
		Phi(NAN),
		GammaMax(NAN),
		ChiMax(NAN)
{
}

FieldMeasure::FieldMeasure(const double &newAlpha, const double &newPhi,
			   const double &newGammaMax, const double &newChiMax)
	:	Alpha(newAlpha),
		Phi(newPhi),
		GammaMax(newGammaMax),
		ChiMax(newChiMax)
{
}

