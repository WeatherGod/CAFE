#ifndef _CAFE_CMDLINE_H
#define _CAFE_CMDLINE_H

#include <vector>
#include <string>

#include "../Config/Configuration.h"


class CmdOptions
{
        public:
                CmdOptions();

                // The Labels are in alphabetical order
                vector <string> GiveLabelsToDo(const Configuration &ConfigInfo, const string &EventTypeName, const string &CAFEVarName) const;

                // The CAFEVars are in alphabetical order
                vector <string> GiveCAFEVarsToDo(const Configuration &ConfigInfo, const string &EventTypeName) const;

                int ParseCommandArgs(vector <string> &CommandArgs);


                bool MergeWithConfiguration(const Configuration &ConfigInfo);

                string GiveTimePeriod(const int &TimeOffset) const;
                string GiveDatabaseName(const string &TheTimePeriod) const;
                string GiveClusteredDatabaseName(const string &TheTimePeriod) const;

                void PrintSyntax(const unsigned short int &SpaceOver, const unsigned short int &ColumnWidth) const;
                void PrintDescription(const unsigned short int &ColumnWidth) const;
                string GiveSyntaxInfo() const;
                string GiveDescription() const;

                int VerboseLevel;
                string ConfigFilename;
                string CAFEPath;
                vector <string> EventTypes;                     // values stored in alphabetical order
                vector <string> TimePeriods;                    // stored in alphabetical order
                vector <string> DatabaseNames;                  // stored in alphabetical order
                vector <string> ClustDatabaseNames;             // stored in alphabetical order
                vector <string> CAFEVarNames;                   // stored in alphabetical order
                vector <string> CAFEFields;                     // stored in alphabetical order
                string LoginUserName;
                string CAFEUserName;
                string ServerName;

        private:
                vector <string> myOptionSwitchers;
                vector <size_t> myExpectedValueCounts;

                bool IsAnOption(const string &SomeStr) const;
                void SetUpSwitchers();
                void DoDecrement(vector <string> &CmdArgs, vector<string>::iterator &ACmdArg, size_t &ValuesToDo) const;
                string OptionMerge(vector <string> &GivenValues, const vector <string> &DefaultValues, const string &ItemDescription);
};

#endif
