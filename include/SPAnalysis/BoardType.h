#ifndef _BOARDTYPE_H
#define _BOARDTYPE_H

#include <vector>
#include <string>

#include "Utils/LonLatAnom.h"			// for LonLatAnomDate structure

#include "ClusterConfig.h"			// for ClusterConfig class


class BoardType
{
	public:
		BoardType();

		void AddMember(const LonLatAnomDate &AMember);

		bool SpecialStrongPointRule(const ClusterConfig &TheConfig) const;
		bool ShouldIgnore(const ClusterConfig &TheConfig) const;
		bool ShouldIgnore(const ClusterConfig &TheConfig, const double DistFromCheck) const;

//		string MemberToString(const size_t MemberIndex) const;

		int MemberCount() const;
		vector <LonLatAnomDate> ReturnMembers() const;

		double GiveValue() const;
		LonLatAnomDate ReturnMember(const size_t &MemberIndex) const;
		
		void Reset();

	private:
		vector <LonLatAnomDate> myMembers;

	friend void operator += (BoardType &Lefty, const BoardType &Righty);
	friend bool operator == (const BoardType &Lefty, const BoardType &Righty);
	friend bool operator != (const BoardType &Lefty, const BoardType &Righty);
};

#endif
