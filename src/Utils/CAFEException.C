using namespace std;

#include <exception>
#include <string>

#include <StrUtly.h>		// for IntToStr(), Size_tToStr()

#include "Utils/CAFEException.h"


CAFEException::CAFEException() throw()
	:	myFunctionName(""),
		myErrorMesg("Unknown CAFE Error has occured!")
{
}

CAFEException::CAFEException(const string &funcName, const string &errMesg) throw()
	:	myFunctionName(funcName),
		myErrorMesg(errMesg)
{
}


CAFEException::~CAFEException() throw()
{
}


const char*
CAFEException::what() const throw()
{
	if (myFunctionName.empty())
	{
		return(myErrorMesg.c_str());
	}
	else
	{
		return((myFunctionName + " - " + myErrorMesg).c_str());
	}
}

void
CAFEException::AddFunctionName(const string &funcName) throw()
// Adds another function name to the error message.
// Good for trace-back.
{
	if (myFunctionName.empty())
	{
		myFunctionName = funcName;
	}
	else
	{
		myFunctionName = funcName + " -\n" + myFunctionName;
	}
}


// -------------------------
// --- CAFEVar_Not_Found ---
// -------------------------
CAFEVar_Not_Found::CAFEVar_Not_Found() throw()
	:	CAFEException("", "CAFEVar could not be found."),
		myCAFEVarName("UNKNOWN")
{
}

CAFEVar_Not_Found::CAFEVar_Not_Found(const string &funcName, const string &cafeVarName) throw()
	:	CAFEException(funcName, "CAFEVar could not be found."),
		myCAFEVarName(cafeVarName)
{
}


CAFEVar_Not_Found::~CAFEVar_Not_Found() throw()
{
}


const char*
CAFEVar_Not_Found::what() const throw()
{
	return(((string) CAFEException::what() + " CAFEVar: '" + myCAFEVarName + "'").c_str());
}


// ---------------------------
// --- CAFELevel_Not_Found ---
// ---------------------------
CAFELevel_Not_Found::CAFELevel_Not_Found() throw()
	:	CAFEException("", "CAFELevel could not be found."),
		myCAFEVarName("UNKNOWN"),
		myCAFELevelName("UNKOWN")
{
}

CAFELevel_Not_Found::CAFELevel_Not_Found(const string &funcName,
					 const string &cafeLevelName, const string &cafeVarName) throw()
	:	CAFEException(funcName, "CAFELevel could not be found."),
		myCAFEVarName(cafeVarName),
		myCAFELevelName(cafeLevelName)
{
}

CAFELevel_Not_Found::~CAFELevel_Not_Found() throw()
{       
}

const char*
CAFELevel_Not_Found::what() const throw()
{
	return(((string) CAFEException::what() + " CAFEVar: '" + myCAFEVarName 
					       + "'  CAFELevel: '" + myCAFELevelName + "'").c_str());
}


// ---------------------------
// --- EventType_Not_Found ---
// ---------------------------
EventType_Not_Found::EventType_Not_Found() throw()
	:	CAFEException("", "EventType could not be found."),
		myEventName("UNKNOWN")
{
}

EventType_Not_Found::EventType_Not_Found(const string &funcName, const string &eventName) throw()
	:	CAFEException(funcName, "EventType could not be found."),
		myEventName(eventName)
{
}

EventType_Not_Found::~EventType_Not_Found() throw()
{       
}

const char*
EventType_Not_Found::what() const throw()
{
	return(((string) CAFEException::what() + " EventType: '" + myEventName + "'").c_str());
}



// --------------------------
// --- EventVar_Not_Found ---
// --------------------------
EventVar_Not_Found::EventVar_Not_Found() throw()
	:	CAFEException("", "EventVar could not be found."),
		myEventName("UNKNOWN"),
		myEventVarName("UNKNOWN")
{
}

EventVar_Not_Found::EventVar_Not_Found(const string &funcName, 
				       const string &eventVarName, const string &eventName) throw()
	:	CAFEException(funcName, "EventVar could not be found."),
		myEventName(eventName),
		myEventVarName(eventVarName)
{
}

EventVar_Not_Found::~EventVar_Not_Found() throw()
{       
}

const char*
EventVar_Not_Found::what() const throw()
{
	return(((string) CAFEException::what() + " EventType: '" + myEventName
					       + "'  EventVar: '" + myEventVarName + "'").c_str());
}


// ----------------------------
// --- EventLevel_Not_Found ---
// ----------------------------
EventLevel_Not_Found::EventLevel_Not_Found() throw()
	:	CAFEException("", "EventLevel could not be found."),
		myEventName("UNKNOWN"),
		myEventVarName("UNKNOWN"),
		myEventLevelName("UNKNOWN")
{
}

EventLevel_Not_Found::EventLevel_Not_Found(const string &funcName, const string &eventLevelName,
					   const string &eventVarName, const string &eventName) throw()
	:	CAFEException(funcName, "EventLevel could not be found."),
		myEventName(eventName),
		myEventVarName(eventVarName),
		myEventLevelName(eventLevelName)
{
}

EventLevel_Not_Found::~EventLevel_Not_Found() throw()
{       
}

void
EventLevel_Not_Found::SetEventName(const string &eventTypeName) throw()
{
	myEventName = eventTypeName;
}

const char*
EventLevel_Not_Found::what() const throw()
{
	return(((string) CAFEException::what() + " EventType: '" + myEventName
					       + "'  EventVar: '" + myEventVarName
					       + "'  EventLevel: '" + myEventLevelName + "'").c_str());
}


// ----------------------------
// --- DataSource_Not_Found ---
// ----------------------------
DataSource_Not_Found::DataSource_Not_Found() throw()
	:	CAFEException("", "DataSource could not be found."),
		mySourceName("UNKNOWN")
{
}

DataSource_Not_Found::DataSource_Not_Found(const string &funcName, const string &sourceName) throw()
	:	CAFEException(funcName, "DataSource could not be found."),
		mySourceName(sourceName)
{
}

DataSource_Not_Found::~DataSource_Not_Found() throw()
{       
}

const char*
DataSource_Not_Found::what() const throw()
{
	return(((string) CAFEException::what() + " DataSource: '" + mySourceName + "'").c_str());
}



// -------------------------
// --- DataVar_Not_Found ---
// -------------------------
DataVar_Not_Found::DataVar_Not_Found() throw()
	:	CAFEException("", "DataVar could not be found."),
		mySourceName("UNKNOWN"),
		myCAFEVarName("UNKNOWN")
{
}

DataVar_Not_Found::DataVar_Not_Found(const string &funcName, 
				     const string &cafeVarName, const string &sourceName) throw()
	:	CAFEException(funcName, "DataVar could not be found."),
		mySourceName(sourceName),
		myCAFEVarName(cafeVarName)
{
}

DataVar_Not_Found::~DataVar_Not_Found() throw()
{       
}

const char*
DataVar_Not_Found::what() const throw()
{
	return(((string) CAFEException::what() + " DataSource: '" + mySourceName
					       + "'  CAFEVar: '" + myCAFEVarName + "'").c_str());
}



// ---------------------------
// --- DataLevel_Not_Found ---
// ---------------------------
DataLevel_Not_Found::DataLevel_Not_Found() throw()
	:	CAFEException("", "DataLevel could not be found."),
		mySourceName("UNKNOWN"),
		myDataVarName("UNKNOWN"),
		myDataLevelIndex(string::npos)
{
}

DataLevel_Not_Found::DataLevel_Not_Found(const string &funcName, const size_t &dataLevelIndex,
					 const string &dataVarName, const string &sourceName) throw()
	:	CAFEException(funcName, "DataLevel could not be found."),
		mySourceName(sourceName),
		myDataVarName(dataVarName),
		myDataLevelIndex(dataLevelIndex)
{
}

DataLevel_Not_Found::~DataLevel_Not_Found() throw()
{       
}

void
DataLevel_Not_Found::SetDataSourceName(const string &sourceName) throw()
{
	mySourceName = sourceName;
}

const char*
DataLevel_Not_Found::what() const throw()
{
	return(((string) CAFEException::what() + " DataSource: '" + mySourceName
					       + "'  DataVar: '" + myDataVarName
					       + "'  DataLevelIndex: '" + Size_tToStr(myDataLevelIndex) + "'").c_str());
}




// ----------------------------
// --- TimePeriod_Not_Found ---
// ----------------------------
TimePeriod_Not_Found::TimePeriod_Not_Found() throw()
	:	CAFEException("", "TimePeriod could not be found."),
		myTimePeriod("UNKNOWN")
{
}

TimePeriod_Not_Found::TimePeriod_Not_Found(const string &funcName, const string &timePeriodName) throw()
	:	CAFEException(funcName, "TimePeriod could not be found."),
		myTimePeriod(timePeriodName)
{
}

TimePeriod_Not_Found::TimePeriod_Not_Found(const string &funcName, const int &timeOffset) throw()
	:	CAFEException(funcName, "TimePeriod could not be found (using TimeOffset...)."),
		myTimePeriod(IntToStr(timeOffset))
{
}

TimePeriod_Not_Found::~TimePeriod_Not_Found() throw()
{       
}

const char*
TimePeriod_Not_Found::what() const throw()
{
	return(((string) CAFEException::what() + " TimePeriod: '" + myTimePeriod + "'").c_str());
}



// --------------------------
// --- Extremum_Not_Found ---
// --------------------------
Extremum_Not_Found::Extremum_Not_Found() throw()
	:	CAFEException("", "Extremum could not be found."),
		myExtremumName("UNKNOWN")
{
}

Extremum_Not_Found::Extremum_Not_Found(const string &funcName, const string &extremumName) throw()
	:	CAFEException(funcName, "Extremum could not be found."),
		myExtremumName(extremumName)
{
}

Extremum_Not_Found::~Extremum_Not_Found() throw()
{       
}

const char*
Extremum_Not_Found::what() const throw()
{
	return(((string) CAFEException::what() + " Extremum: '" + myExtremumName + "'").c_str());
}
