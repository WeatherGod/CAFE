#ifndef _CAFEDOMAIN_C
#define _CAFEDOMAIN_C

using namespace std;

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

#include "Config/CAFEDomain.h"

#include <StrUtly.h>			// for TakeDelimitedList(), StrToFloat()
#include <ConfigUtly.h>			// for ReadNoComments(), StripTags(), FoundStartTag(), FoundEndTag()

CAFEDomain::CAFEDomain()
	:	myDomainLons(0),
		myDomainLats(0),
		myIsConfigured(false)
{
}

CAFEDomain::CAFEDomain(const CAFEDomain &domainCopy)
	:	myDomainLons(domainCopy.myDomainLons),
		myDomainLats(domainCopy.myDomainLats),
		myIsConfigured(domainCopy.myIsConfigured)
{
}

CAFEDomain::CAFEDomain(const vector<float> &boundingBox)
	:	myDomainLons(0),
		myDomainLats(0),
		myIsConfigured(false)
{
	if (boundingBox.size() == 4)
	{
		myDomainLons.assign(boundingBox.begin(), boundingBox.begin() + 2);
		myDomainLats.assign(boundingBox.begin() + 2, boundingBox.end());
		myIsConfigured = true;
	}
}

CAFEDomain::CAFEDomain(const float &lat1, const float &lon1,
		       const float &lat2, const float &lon2)
	:	myDomainLons(2),
		myDomainLats(2),
		myIsConfigured(true)
{
	myDomainLons[0] = lon1;
	myDomainLons[1] = lon2;

	myDomainLats[0] = lat1;
	myDomainLats[1] = lat2;
}

bool CAFEDomain::ValidConfig() const
{
	return(myIsConfigured);
}

bool CAFEDomain::IsValid() const
{
	return(myIsConfigured);
}

void CAFEDomain::GetConfigInfo(string &FileLine, fstream &ReadData)
{
	const vector<string> TagWords = InitTagWords();

	bool BadObject = false;

	while (!FoundEndTag(FileLine, TagWords[0]) && !ReadData.eof())
	{
		if (!BadObject)
		{
			if (FoundStartTag(FileLine, TagWords[1]))	// Longitudes
			{
				vector <string> Tempy = TakeDelimitedList(StripTags(FileLine, TagWords[1]), ',');
				if (Tempy.size() == 2)
				{
					myDomainLons = StrToFloat(Tempy);

					if (isnan(myDomainLons[0]) || isnan(myDomainLons[1]))
					{
						BadObject = true;
						cerr << "ERROR -- Invalid values for coordinates.  Must be numbers..." << endl;
						cerr << "The line: " << FileLine << endl;
					}
				}
				else
        	                {
					BadObject = true;
                	                cerr << "ERROR -- Incorrect number of coordinates: " << Tempy.size() << endl;
                        	        cerr << "   The Line: " << FileLine << endl;
                        	}

			}
			else if (FoundStartTag(FileLine, TagWords[2])) 	//latitudes
			{
				vector <string> Tempy = TakeDelimitedList(StripTags(FileLine, TagWords[2]), ',');
				if (Tempy.size() == 2)
				{
					myDomainLats = StrToFloat(Tempy);
	                                
					if (isnan(myDomainLats[0]) || isnan(myDomainLats[1]))
        	                        {
						BadObject = true;
                	                        cerr << "ERROR -- Invalid values for coordinates.  Must be numbers..." << endl;
                        	                cerr << "The line: " << FileLine << endl;
                                	}
				}
				else
				{
					BadObject = true;
					cerr << "ERROR -- Incorrect number of coordinates: " << Tempy.size() << endl;
					cerr << "The Line: " << FileLine << endl;
				}
			}
			else
			{
				BadObject = true;
				cerr << "ERROR -- Problem in parent Domain... couldn't recognize anything.\nHere is the line: " << FileLine << endl;
			}
		}// end if !BadObject

		FileLine = ReadNoComments(ReadData);
	}// end while loop

	if (myDomainLats.size() == 2 && myDomainLons.size() == 2 && !ReadData.eof() && !BadObject)
	{
		myIsConfigured = true;
	}
}// end GetConfigInfo()

vector<string> CAFEDomain::InitTagWords() const
{
	vector<string> TagWords(3);
	TagWords[0] = "Domain";
	TagWords[1] = "Lon";
	TagWords[2] = "Lat";

	return(TagWords);
}

vector <float> CAFEDomain::GiveBoundingBox() const
{
	vector <float> BoundingBox(4, NAN);

	if (myIsConfigured)
	{
		BoundingBox[0] = myDomainLons[0];
		BoundingBox[1] = myDomainLons[1];
		BoundingBox[2] = myDomainLats[0];
		BoundingBox[3] = myDomainLats[1];
	}

	return(BoundingBox);
}

vector <float> CAFEDomain::GiveLons() const
{
	if (myIsConfigured)
	{
		return(myDomainLons);
	}
	else
	{
		return(vector<float>(2, NAN));
	}
}

vector <float> CAFEDomain::GiveLats() const
{
        if (myIsConfigured)
        {
                return(myDomainLats);
        }
        else
        {
                return(vector<float>(2, NAN));
        }
}

#endif
