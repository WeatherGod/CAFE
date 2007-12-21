#ifndef _FIELDMEASURE_H
#define _FIELDMEASURE_H

struct FieldMeasure
{
	FieldMeasure();
	FieldMeasure(const double &newAlpha, const double &newPhi,
		     const double &newGammaMax, const double &newChiMax);

	double Alpha;
	double Phi;
	double GammaMax;
	double ChiMax;
};

#endif
