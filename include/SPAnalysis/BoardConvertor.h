#ifndef _BOARDCONVERTOR_H
#define _BOARDCONVERTOR_H

#include <Projection_t.h>

class BoardConvertor
{
	public:
		BoardConvertor();
		BoardConvertor(const BoardConvertor &AConvertor);
		~BoardConvertor();

		void PrintConfigs() const;

		void SetUpBoard(const Projection_t *TheProjection,
				double X1, double X2,
				double Y1, double Y2,
				const size_t &XRadial, const size_t &YRadial);

		void SetProjection(const Projection_t *TheProjection);

		void SetXsize(const size_t Xsize);
		void SetYsize(const size_t Ysize);
		
		void SetXOffset(const double &YOffset);
		void SetYOffset(const double &YOffset);

		void SetXRadial(const size_t &XRadial);
		void SetYRadial(const size_t &YRadial);

		size_t Xsize() const;
		size_t Ysize() const;

		double XOffset() const;
		double YOffset() const;

		bool SphericalToGrid(const double &Lon, const double &Lat, size_t &XLoc, size_t &YLoc) const;
		bool SphericalToGrid(const double &Lon, const double &Lat, double &XLoc, double &YLoc) const;
		bool GridToSpherical(const size_t &XLoc, const size_t &YLoc, double &Lon, double &Lat) const;
		bool GridToSpherical(const double &XLoc, const double &YLoc, double &Lon, double &Lat) const;

	private:
		size_t myXsize;
		size_t myYsize;
		double myXOffset;
		double myYOffset;
		size_t myXRadial;
		size_t myYRadial;

		Projection_t* myProjection;
};

#endif
