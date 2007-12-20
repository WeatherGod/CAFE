#ifndef _EVENTSCOREFILE_H
#define _EVENTSCOREFILE_H

#include <fstream>
#include <vector>
#include <string>

/* 
   This class is currently just a C++ version of the set of C Utilities originally
   developed for CAFE with regards to the event score file.
   The event score file has been the bane of CAFE's existance and is in
   a major need of an overhaul.

   Hopefully, this is a good first step towards that.
*/

class EventScoreFile : public fstream
{
	public:
		pair< vector<double>, vector<string> > RetrieveEventScores();
		pair<double, string> RetrieveEventScore();

		// Saves the event scores from multiple event types for the same
		// moment in time.
		// The date string must be of format %HZ%d%b%Y
		bool SaveEventScores(const vector<double> &eventScores, const string &dateOfEvent);
		
		~EventScoreFile();
};

#endif
