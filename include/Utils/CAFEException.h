#ifndef _CAFEEXCEPTION_H
#define _CAFEEXCEPTION_H


#include <exception>
#include <string>


class CAFEException : public exception
{
	public:
		CAFEException() throw();
		// Adding support for taking std::strings.
		// I am sick of constantly having to use string::c_str()...
		CAFEException(const string &functionName, const string &errMesg = "Unknown CAFE Error has occured!") throw();
		virtual ~CAFEException() throw();

		virtual void AddFunctionName(const string &funcName) throw();
		virtual const char* what() const throw();

	protected:
		string myFunctionName;
		string myErrorMesg;
};



class CAFEVar_Not_Found : public CAFEException
{
	public:
		CAFEVar_Not_Found() throw();
		CAFEVar_Not_Found(const string &funcName,
				  const string &cafeVarName = "UNKNOWN") throw();

		virtual ~CAFEVar_Not_Found() throw();
		virtual const char* what() const throw();

	protected:
		string myCAFEVarName;
};


class CAFELevel_Not_Found : public CAFEException
{
	public:
		CAFELevel_Not_Found() throw();
		CAFELevel_Not_Found(const string &funcName,
				    const string &cafeLevelName = "UNKOWN", const string &cafeVarName = "UNKNOWN") throw();
		virtual ~CAFELevel_Not_Found() throw();
		virtual const char* what() const throw();

	protected:
		string myCAFEVarName;
		string myCAFELevelName;
};




class EventType_Not_Found : public CAFEException
{
	public:
		EventType_Not_Found() throw();
		EventType_Not_Found(const string &funcName,
				    const string &eventName = "UNKNOWN") throw();

		virtual ~EventType_Not_Found() throw();
		virtual const char* what() const throw();

	protected:
		string myEventName;
};


class EventVar_Not_Found : public CAFEException
{
	public:
		EventVar_Not_Found() throw();
		EventVar_Not_Found(const string &funcName,
				   const string &eventVarName = "UNKNOWN", const string &eventName = "UNKNOWN") throw();

		virtual ~EventVar_Not_Found() throw();
		virtual const char* what() const throw();

	protected:
		string myEventName;
		string myEventVarName;
};


class EventLevel_Not_Found : public CAFEException
{
	public:
		EventLevel_Not_Found() throw();
		EventLevel_Not_Found(const string &funcName,
				     const string &eventLevelName = "UNKNOWN", 
				     const string &eventVarName = "UNKNOWN", 
				     const string &eventName = "UNKNOWN") throw();
		virtual void SetEventName(const string &eventTypeName) throw();

		virtual ~EventLevel_Not_Found() throw();
		virtual const char* what() const throw();

	protected:
		string myEventName;
		string myEventVarName;
		string myEventLevelName;
};


class DataSource_Not_Found : public CAFEException
{
	public:
		DataSource_Not_Found() throw();
		DataSource_Not_Found(const string &funcName, const string &sourceName = "UNKNOWN") throw();

		virtual ~DataSource_Not_Found() throw();
		virtual const char* what() const throw();

	protected:
		string mySourceName;
};


class DataVar_Not_Found : public CAFEException
{
	public:
		DataVar_Not_Found() throw();
		DataVar_Not_Found(const string &funcName,
				  const string &cafeVarName = "UNKNOWN", const string &sourceName = "UNKNOWN") throw();

		virtual ~DataVar_Not_Found() throw();
		virtual const char* what() const throw();

	protected:
		string mySourceName;
		string myCAFEVarName;
};

class DataLevel_Not_Found : public CAFEException
{
	public:
		DataLevel_Not_Found() throw();
		DataLevel_Not_Found(const string &funcName,
				    const size_t &dataLevelIndex = string::npos,
				    const string &dataVarName = "UNKNOWN",
				    const string &sourceName = "UNKNOWN") throw();
		virtual void SetDataSourceName(const string &sourceName) throw();

		virtual ~DataLevel_Not_Found() throw();
		virtual const char* what() const throw();

	protected:
		string mySourceName;
		string myDataVarName;
		size_t myDataLevelIndex;
};

class TimePeriod_Not_Found : public CAFEException
{
	public:
		TimePeriod_Not_Found() throw();
		TimePeriod_Not_Found(const string &funcName, const string &timePeriodName = "UNKNOWN") throw();
		TimePeriod_Not_Found(const string &funcName, const int &timeOffset) throw();

		virtual ~TimePeriod_Not_Found() throw();
		virtual const char* what() const throw();

	protected:
		string myTimePeriod;

};

class Extremum_Not_Found : public CAFEException
{
	public:
		Extremum_Not_Found() throw();
		Extremum_Not_Found(const string &funcName, const string &extremumName = "UNKNOWN") throw();

		virtual ~Extremum_Not_Found() throw();
		virtual const char* what() const throw();

	protected:
		string myExtremumName;
};

#endif
