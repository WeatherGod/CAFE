#ifndef _BOARDTYPE_C
#define _BOARDTYPE_C

using namespace std;

#include <vector>

#include "Utils/LonLatAnom.h"		//for LonLatAnomDate structure

#include "SPAnalysis/BoardType.h"
#include "SPAnalysis/ClusterConfig.h"		// for ClusterConfig class


BoardType::BoardType()
	:	myMembers(0)
{
}

void BoardType::AddMember(const LonLatAnomDate &AMember)
{
	myMembers.push_back(AMember);
}

bool BoardType::SpecialStrongPointRule(const ClusterConfig &TheConfig) const
// is the point is significant enough to stand on its own?
{
	return(GiveValue() >= TheConfig.UpperDensityThreshold());
}


bool BoardType::ShouldIgnore(const ClusterConfig &TheConfig) const
//  is the point too insignificant to be considered?
{
	return(GiveValue() <= TheConfig.IgnoreHowMuchRule());
}

bool BoardType::ShouldIgnore(const ClusterConfig &TheConfig, const double DistFromCheck) const
// May want to revise how this works.  Soon, I am going to increase the radius, which will result in odd behavior.
{
	return((GiveValue() / DistFromCheck) <= TheConfig.IgnoreHowMuchRule());
}


void BoardType::Reset()
{
	myMembers.clear();
}

int BoardType::MemberCount() const
{
	return(myMembers.size());
}

double BoardType::GiveValue() const
{
	return((double) myMembers.size());
}

LonLatAnomDate BoardType::ReturnMember(const size_t &IndexNum) const
{
	return(myMembers.at(IndexNum));
}

vector <LonLatAnomDate> BoardType::ReturnMembers() const
{
	return(myMembers);
}


void operator += (BoardType &Lefty, const BoardType &Righty)
{
	Lefty.myMembers.insert(Lefty.myMembers.end(), Righty.myMembers.begin(), Righty.myMembers.end());
}

bool operator == (const BoardType &Lefty, const BoardType &Righty)
{
/*
	for (vector<LonLatAnomDate>::const_iterator LeftyMember( Lefty.myMembers.begin() ), RightyMember( Righty.myMembers.begin() );
	     LeftyMember != Lefty.myMembers.end() && RightyMember != Righty.myMembers.end();
	     LeftyMember++, RightyMember++)
	{
		if ((*LeftyMember) != (*RightyMember))
		{
			return(false);
		}
	}
*/

	return(Lefty.myMembers == Righty.myMembers);
}

bool operator != (const BoardType &Lefty, const BoardType &Righty)
{
/*
	for (vector<LonLatAnomDate>::const_iterator LeftyMember( Lefty.myMembers.begin() ), RightyMember( Righty.myMembers.begin() );
             LeftyMember != Lefty.myMembers.end() && RightyMember != Righty.myMembers.end();
             LeftyMember++, RightyMember++)
        {
		if ((*LeftyMember) != (*RightyMember))
		{
			return(true);
		}
	}
*/

	return(Lefty.myMembers != Righty.myMembers);
}



#endif

