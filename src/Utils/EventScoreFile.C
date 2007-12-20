using namespace std;

#include <fstream>
#include <iostream>	// for printing out error messages
#include <cmath>	// for NAN
#include <string>

#include <StrUtly.h>	// for TakeDelimitedList(), StrToUpper(), StripWhiteSpace(), DoubleToStr()
#include <TimeUtly.h>	// for GetTimeUTC(), GiveTimeUTC()
#include "Utils/EventScoreFile.h"

EventScoreFile::~EventScoreFile()
{
	close();
}

pair< vector<double>, vector<string> >
EventScoreFile::RetrieveEventScores()
{
	vector<double> eventScores;
	vector<string> eventDates;

	pair<double, string> scoreInfo = RetrieveEventScore();

	while (good())
	{
		if (isnan(scoreInfo.first))
		{
			break;	// break out of the while loop.
		}

		eventScores.push_back(scoreInfo.first);
		eventDates.push_back(scoreInfo.second);

		scoreInfo = RetrieveEventScore();
	}

	if (bad() || !eof())
	{
		cerr << "ERROR: Problem with reading the event score file.\n";
		eventScores.clear();
		eventDates.clear();
	}

	return( make_pair(eventScores, eventDates) );
}

pair<double, string>
EventScoreFile::RetrieveEventScore()
{
	double eventScore = NAN;
	string theDate = "";

	string lineRead = "";
	std::getline(*this, lineRead);
	StripWhiteSpace(lineRead);

	vector<string> tempHold = TakeDelimitedList(lineRead, ',');
	if (tempHold.size() == 2)
	{
		eventScore = StrToDouble(tempHold[0]);
		theDate = tempHold[1];
	}
	else
	{
		cerr << "ERROR: Could not read properly from the event score file.\n"
		     << "       The offending line: " << lineRead << endl;
	}

	return( make_pair(eventScore, theDate) );
}


bool
EventScoreFile::SaveEventScores(const vector<double> &eventScores, const string &dateOfEvent)
{
	*this << GiveDelimitedList(DoubleToStr(eventScores), ',');

	if (!dateOfEvent.empty())
	{
		*this << ',' << GiveTimeUTC( GetTimeUTC( StrToUpper(dateOfEvent), "%HZ%d%b%Y"), "%Y-%m-%d %H");
	}

	*this << '\n';

	return(good());
}
