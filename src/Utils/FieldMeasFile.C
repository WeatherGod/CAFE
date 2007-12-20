using namespace std;

#include <fstream>

#include "Utils/FieldMeasure.h"		// for the FieldMeasure structure
#include "Utils/FieldMeasFile.h"

FieldMeasFile::~FieldMeasFile()
{
	close();
}

FieldMeasure
FieldMeasFile::RetrieveFieldMeasure()
{
	FieldMeasure theFieldMeasure;

	*this >> theFieldMeasure.Alpha
	      >> theFieldMeasure.Phi
	      >> theFieldMeasure.GammaMax
	      >> theFieldMeasure.ChiMax;

	return(theFieldMeasure);
}


bool
FieldMeasFile::SaveFieldMeasure(const FieldMeasure &theFieldMeasure)
{
	// TODO: set precision level for the values to 7
	*this << theFieldMeasure.Alpha << ' '
	      << theFieldMeasure.Phi << '\n'
	      << theFieldMeasure.GammaMax << ' '
	      << theFieldMeasure.ChiMax << '\n';

	return(good());
}
