#ifndef _FIELDMEASFILE_H
#define _FIELDMEASFILE_H

#include <fstream>

#include "Utils/FieldMeasure.h"		// for the FieldMeasure struct

class FieldMeasFile : public fstream
{
	public:
		FieldMeasFile(const char* filename, const ios::openmode &theOpenMode);
		~FieldMeasFile();

		FieldMeasure RetrieveFieldMeasure();
		bool SaveFieldMeasure(const FieldMeasure &theFieldMeasure);

	private:
		// Placed in private so that it won't be used.
		// This will not be defined.
		FieldMeasFile(const FieldMeasFile &copyFile);
};

#endif
