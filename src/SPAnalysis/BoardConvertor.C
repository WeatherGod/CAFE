#ifndef _BOARDCONVERTOR_C
#define _BOARDCONVERTOR_C

using namespace std;

#include <iostream>
#include <cmath>		// for ceil(), floor(), min(), fabs()
#include <cctype>		// for size_t
#include <Projection_t.h>
#include "SPAnalysis/BoardConvertor.h"


BoardConvertor::BoardConvertor()
	:	myXsize(0),
		myYsize(0),
		myXOffset(0.0),
		myYOffset(0.0),
		myXRadial(0),
		myYRadial(0),
		myProjection(new Projection_t())
{
}

BoardConvertor::BoardConvertor(const BoardConvertor &AConvertor)
	:	myXsize(AConvertor.myXsize),
		myYsize(AConvertor.myYsize),
		myXOffset(AConvertor.myXOffset),
		myYOffset(AConvertor.myYOffset),
		myXRadial(AConvertor.myXRadial),
		myYRadial(AConvertor.myYRadial),
		myProjection(AConvertor.myProjection->ValueCopy())
{
}

BoardConvertor::~BoardConvertor()
{
	delete myProjection;
}


void BoardConvertor::PrintConfigs() const
{
	cout << "Grid Size:  X: " << myXsize << "   Y: " << myYsize << endl;
	cout << "Offset:     X: " << myXOffset << "    Y: " << myYOffset << endl;
	cout << "Radial:     X: " << myXRadial << "    Y: " << myYRadial << endl;
}

void BoardConvertor::SetUpBoard(const Projection_t* TheProjection,
				double X1, double X2,
				double Y1, double Y2,
				const size_t &XRadial, const size_t &YRadial)
{
	if (X1 <= X2)
        {
                X1 = ceil(X1);
        }
        else
        {
                X1 = floor(X1);
        }

	if (X2 <= X1)
        {
                X2 = ceil(X2);
        }
        else
        {
                X2 = floor(X2);
        }

	if (Y1 <= Y2)
        {
                Y1 = ceil(Y1);
        }
        else
        {
                Y1 = floor(Y1);
        }

        if (Y2 <= Y1)
        {
                Y2 = ceil(Y2);
        }
        else
        {
                Y2 = floor(Y2);
        }

	SetXOffset(min(X1, X2));
	SetYOffset(min(Y1, Y2));

	SetXsize((size_t) (fabs(X2 - X1) / (float) XRadial) + 1);
	SetYsize((size_t) (fabs(Y2 - Y1)  / (float) YRadial) + 1);

	SetXRadial(XRadial);
	SetYRadial(YRadial);

	SetProjection(TheProjection);
}
	

void BoardConvertor::SetXsize(const size_t Xsize)
{
	myXsize = Xsize;
}

void BoardConvertor::SetYsize(const size_t Ysize)
{
        myYsize = Ysize;
}

void BoardConvertor::SetXOffset(const double &XOffset)
{
        myXOffset = XOffset;
}

void BoardConvertor::SetYOffset(const double &YOffset)
{
        myYOffset = YOffset;
}

void BoardConvertor::SetProjection(const Projection_t* TheProjection)
{
	delete myProjection;
	myProjection = TheProjection->ValueCopy();
}

void BoardConvertor::SetXRadial(const size_t &XRadial)
{
	myXRadial = XRadial;
}

void BoardConvertor::SetYRadial(const size_t &YRadial)
{
	myYRadial = YRadial;
}



size_t BoardConvertor::Xsize() const
{
	return(myXsize);
}

size_t BoardConvertor::Ysize() const
{
        return(myYsize);
}

double BoardConvertor::XOffset() const
{
        return(myXOffset);
}

double BoardConvertor::YOffset() const
{
        return(myYOffset);
}

bool BoardConvertor::SphericalToGrid(const double &Lon, const double &Lat, size_t &XLoc, size_t &YLoc) const
{
	double RealXLoc, RealYLoc;
	if (myProjection->LatLonToXY(Lat, Lon, RealXLoc, RealYLoc))
	{
//		cerr << "In BoardConvertor... RealXLoc: " << RealXLoc << "   RealYLoc: " << RealYLoc << endl;

		XLoc = (size_t) (RealXLoc - myXOffset) / myXRadial;
		YLoc = (size_t) (RealYLoc - myYOffset) / myYRadial;

		if (XLoc >= myXsize || YLoc >= myYsize)
		{
			double tempLat, tempLon;
			cerr << "ERROR: Off the board!" << endl;
			cerr << "     : RealXLoc = " << RealXLoc << "  RealYLoc: " << RealYLoc << endl;
			cerr << "     : Domain Corners -\n";
			myProjection->XYToLatLon(myXOffset, myYOffset, tempLat, tempLon);
			cerr << "     : (0, 0) -> " << tempLat << " Lat, " << tempLon << " Lon\n";
			myProjection->XYToLatLon(myXOffset, myYOffset + myYsize - 1, tempLat, tempLon);
			cerr << "     : (0, " << myYsize - 1 << ") -> " << tempLat << " Lat, " << tempLon << " Lon\n";
			myProjection->XYToLatLon(myXOffset + myXsize - 1, myYOffset, tempLat, tempLon);
			cerr << "     : (" << myXsize - 1 << ", 0) -> " << tempLat << " Lat, " << tempLon << " Lon\n";
			myProjection->XYToLatLon(myXOffset + myXsize - 1, myYOffset + myYsize - 1, tempLat, tempLon);
			cerr << "     : (" << myXsize - 1 << ", " << myYsize - 1 << ") -> " << tempLat << " Lat, " << tempLon << " Lon\n";
			
			// off the board!
			return(false);
		}
		else
		{
			return(true);
		}
	}
	else
	{
		cerr << "ERROR: Could not do conversion." << endl;
		return(false);
	}
}

bool BoardConvertor::SphericalToGrid(const double &Lon, const double &Lat, double &XLoc, double &YLoc) const
{
        if (myProjection->LatLonToXY(Lat, Lon, XLoc, YLoc))
        {
		XLoc = (XLoc - myXOffset) / myXRadial;
		YLoc = (YLoc - myYOffset) / myYRadial;

                if (XLoc >= myXsize || YLoc >= myYsize)
                {
			double tempLat, tempLon;
                        cerr << "ERROR: Off the board!" << endl;
			cerr << "     : XLoc = " << XLoc << "   YLoc = " << YLoc << endl;
			cerr << "     : Domain Corners -\n";
			myProjection->XYToLatLon(myXOffset, myYOffset, tempLat, tempLon);
			cerr << "     : (0, 0) -> " << tempLat << " Lat, " << tempLon << " Lon\n";
			myProjection->XYToLatLon(myXOffset, myYOffset + myYsize - 1, tempLat, tempLon);
			cerr << "     : (0, " << myYsize - 1 << ") -> " << tempLat << " Lat, " << tempLon << " Lon\n";
			myProjection->XYToLatLon(myXOffset + myXsize - 1, myYOffset, tempLat, tempLon);
			cerr << "     : (" << myXsize - 1 << ", 0) -> " << tempLat << " Lat, " << tempLon << " Lon\n";
			myProjection->XYToLatLon(myXOffset + myXsize - 1, myYOffset + myYsize - 1, tempLat, tempLon);
			cerr << "     : (" << myXsize - 1 << ", " << myYsize - 1 << ") -> " << tempLat << " Lat, " << tempLon << " Lon\n";

                        // off the board!
                        return(false);
                }
                else
                {
                        return(true);
                }
        }
        else
        {
                cerr << "ERROR: Could not do conversion." << endl;
                return(false);
        }
}


bool BoardConvertor::GridToSpherical(const size_t &XLoc, const size_t &YLoc, double &Lon, double &Lat) const
{
	return(myProjection->XYToLatLon((double) (XLoc * myXRadial) + myXOffset, (double) (YLoc * myYRadial) + myYOffset, Lat, Lon));
}

bool BoardConvertor::GridToSpherical(const double &XLoc, const double &YLoc, double &Lon, double &Lat) const
{
        return(myProjection->XYToLatLon((XLoc * myXRadial) + (double) myXOffset, (YLoc * myYRadial) + (double) myYOffset, Lat, Lon));
}


#endif
