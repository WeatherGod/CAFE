#ifndef _FIELDMEASFILE_H
#define _FIELDMEASFILE_H

#include <fstream>

#include "Utils/FieldMeasure.h"		// for the FieldMeasure struct

class FieldMeasFile : public fstream
{
	public:
		FieldMeasure RetrieveFieldMeasure();
		bool SaveFieldMeasure(const FieldMeasure &theFieldMeasure);
		~FieldMeasFile();
};

#endif
