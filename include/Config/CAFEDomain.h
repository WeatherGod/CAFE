#ifndef _CAFEDOMAIN_H
#define _CAFEDOMAIN_H

#include <fstream>

#include <vector>
#include <string>


class CAFEDomain
{
	public:
		CAFEDomain();
		explicit CAFEDomain(const CAFEDomain &domainCopy);
		CAFEDomain(const vector<float> &boundingBox);
		CAFEDomain(const float &lat1, const float &lon1,
			   const float &lat2, const float &lon2);
		

		// right now, they do the same thing, but there should be a difference.
		// ValidConfig() will return whether it was able to load a configuration correctly.
		// IsValid() will return whether the data contained is valid information.
		bool ValidConfig() const;
		bool IsValid() const;

		void GetConfigInfo(string &FileLine, fstream &ReadData);

		vector <float> GiveBoundingBox() const;
		vector <float> GiveLons() const;
		vector <float> GiveLats() const;
		
	private:
		bool myIsConfigured;
		vector <float> myDomainLons;
		vector <float> myDomainLats;

		vector<string> InitTagWords() const;
};

#endif
