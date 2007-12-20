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
		myIsConfigured(false),
		myTagWords(0)
{
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
	InitTagWords();

	bool BadObject = false;

	while (!FoundEndTag(FileLine, myTagWords[0]) && !ReadData.eof())
	{
		if (!BadObject)
		{
			if (FoundStartTag(FileLine, myTagWords[1]))	// Longitudes
			{
				vector <string> Tempy = TakeDelimitedList(StripTags(FileLine, myTagWords[1]), ',');
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
			else if (FoundStartTag(FileLine, myTagWords[2])) 	//latitudes
			{
				vector <string> Tempy = TakeDelimitedList(StripTags(FileLine, myTagWords[2]), ',');
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

	myTagWords.resize(0);
}// end GetConfigInfo()

void CAFEDomain::InitTagWords()
{
	if (myTagWords.empty())
	{
		myTagWords.push_back("Domain");
		myTagWords.push_back("Lon");
		myTagWords.push_back("Lat");
	}
}

vector <float> CAFEDomain::GiveBoundingBox() const
{
	vector <float> BoundingBox(4, nanf("nan"));

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
		vector <float> Tempy(2, nanf("nan"));
		return(Tempy);
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
                vector <float> Tempy(2, nanf("nan"));
                return(Tempy);
        }
}

#endif
