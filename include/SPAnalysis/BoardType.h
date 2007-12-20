#ifndef _BOARDTYPE_H
#define _BOARDTYPE_H

#include <vector>
#include <string>

#define MYSQLPP_SSQLS_NO_STATICS       // makes sure that the SSQL structs are only declared, not defined.
#include "../Utils/CAFE_SQLStructs.h"			// for LonLatAnomDate type.

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
