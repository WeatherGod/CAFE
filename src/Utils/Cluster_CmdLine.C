#ifndef _CLUSTER_CMDLINE_C
#define _CLUSTER_CMDLINE_C

using namespace std;

#include <iostream>
#include <vector>
#include <string>

#include <StrUtly.h>		// for TakeDelimitedList(), StrToInt(), StrToDouble(), StripWhiteSpace()

#include "Utils/Cluster_CmdLine.h"


Cluster_CmdOptions::Cluster_CmdOptions()
	:       StdDeviationValue(1.85),
		LowStdDeviationValue(0.25),
		SeekingRadius(1),
		TouchingParameter(2),
		DoPause(false),
		ShowBestCluster(true),
		ShowAllClusters(false),
		ShowMap(false),
		ShowInfo(true),
		SaveBestCluster(false),
		OptionSwitchers(0),
		ExpectedValueCounts(0)
{
	OptionSwitchers.push_back("--stddev");
	ExpectedValueCounts.push_back(1);
	OptionSwitchers.push_back("--lowstddev");
	ExpectedValueCounts.push_back(1);
	OptionSwitchers.push_back("--radius");
	ExpectedValueCounts.push_back(1);
	OptionSwitchers.push_back("--touch");
	ExpectedValueCounts.push_back(1);
	OptionSwitchers.push_back("--nopause");
	ExpectedValueCounts.push_back(0);
	OptionSwitchers.push_back("--pause");
	ExpectedValueCounts.push_back(0);
	OptionSwitchers.push_back("--showbestclust");
	ExpectedValueCounts.push_back(0);
	OptionSwitchers.push_back("--noshowbestclust");
	ExpectedValueCounts.push_back(0);
	OptionSwitchers.push_back("--showallclust");
	ExpectedValueCounts.push_back(0);
	OptionSwitchers.push_back("--noshowallclust");
	ExpectedValueCounts.push_back(0);
	OptionSwitchers.push_back("--showoriggrid");
	ExpectedValueCounts.push_back(0);
	OptionSwitchers.push_back("--noshoworiggrid");
	ExpectedValueCounts.push_back(0);
	OptionSwitchers.push_back("--showinfo");
	ExpectedValueCounts.push_back(0);
	OptionSwitchers.push_back("--noshowinfo");
	ExpectedValueCounts.push_back(0);
	OptionSwitchers.push_back("--save");
	ExpectedValueCounts.push_back(0);
	OptionSwitchers.push_back("--nosave");
	ExpectedValueCounts.push_back(0);
	OptionSwitchers.push_back("--silent");
	ExpectedValueCounts.push_back(0);
	OptionSwitchers.push_back("--test");
	ExpectedValueCounts.push_back(0);
	OptionSwitchers.push_back("--fulltest");
	ExpectedValueCounts.push_back(0);
}

bool Cluster_CmdOptions::IsAnOption(const string &SomeStr)
{
	return((SomeStr.find("--") == 0 && SomeStr.length() > 3) || (SomeStr.find('-') == 0 && SomeStr.length() == 2));
}

int Cluster_CmdOptions::ParseCommandArgs(vector <string> &CmdArgs)
{
	size_t LastOptionIndex = string::npos;
	// string::npos indicates that the last option met did not match any option that was expected
	// or the last option has already been dealt with.

        for (vector<string>::iterator ACmdArg = CmdArgs.begin(); ACmdArg != CmdArgs.end(); )
        {
		if (RipWhiteSpace(*ACmdArg) == "")
                {
		        cerr << "Empty value encountered in the command line args..." << endl;
                        return(2);
                }
			
		switch(LastOptionIndex)
                {
                case 0:
                	if (IsAnOption(*ACmdArg))
			{
				cerr << "Unexpected option: " << *ACmdArg << endl;
				cerr << "Was expecting an upper standard deviation threshold value." << endl;
				return(1);
			}

			StdDeviationValue = StrToDouble(*ACmdArg);
			ACmdArg = CmdArgs.erase(ACmdArg);
			LastOptionIndex = string::npos;
                        break;

                case 1:
			if (IsAnOption(*ACmdArg))
                        {
                	        cerr << "Unexpected option: " << *ACmdArg << endl;
                                cerr << "Was expecting a bottom standard deviation threshold value." << endl;
                                return(1);
                        }

                        LowStdDeviationValue = StrToDouble(*ACmdArg);
                        ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;

		case 2:
			if (IsAnOption(*ACmdArg))
			{
				cerr << "Unexpected option: " << *ACmdArg << endl;
				cerr << "Was expecting a seeking radius value." << endl;
				return(1);
			}


                        SeekingRadius = StrToInt(*ACmdArg);
                        ACmdArg = CmdArgs.erase(ACmdArg);
			LastOptionIndex = string::npos;
                        break;
		case 3:
			if (IsAnOption(*ACmdArg))
                        {
                	        cerr << "Unexpected option: " << *ACmdArg << endl;
                                cerr << "Was expecting the touching parameter value." << endl;
                                return(1);
                        }


                        TouchingParameter = StrToInt(*ACmdArg);
                        ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;
		case 4:
			DoPause = false;
			ACmdArg = CmdArgs.erase(ACmdArg);
			LastOptionIndex = string::npos;
			break;
		case 5:
			DoPause = true;
			ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;
		case 6:
			ShowBestCluster = true;
			ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;
		case 7:
			ShowBestCluster = false;
			ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;
		case 8:
			ShowAllClusters = true;
			ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;
		case 9:
			ShowAllClusters = false;
			ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;
		case 10:
			ShowMap = true;
			ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;
		case 11:
			ShowMap = false;
			ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;
		case 12:
			ShowInfo = true;
			ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;
		case 13:
			ShowInfo = false;
			ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;
		case 14:
			SaveBestCluster = true;
			ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;
		case 15:
			SaveBestCluster = false;
			ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;
		case 16:
			DoPause = false;
			ShowBestCluster = false;
			ShowAllClusters = false;
			ShowMap = false;
			ShowInfo = false;
			SaveBestCluster = true;
			ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;
		case 17:
			DoPause = true;
                        ShowBestCluster = true;
                        ShowAllClusters = false;
                        ShowMap = false;
                        ShowInfo = true;
                        SaveBestCluster = false;
                        ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;
		case 18:
			DoPause = true;
                        ShowBestCluster = true;
                        ShowAllClusters = true;
                        ShowMap = true;
                        ShowInfo = true;
                        SaveBestCluster = false;
                        ACmdArg = CmdArgs.erase(ACmdArg);
                        LastOptionIndex = string::npos;
                        break;


        	default:
                	if (IsAnOption(*ACmdArg))
			{
				// then ACmdArg represents an option switcher
				if (LastOptionIndex != string::npos)
				{
					// this will only happen if there are more values in the OptionSwitchers vector
					// than are accounted for in the switch block.
					cerr << "FATAL ERROR IN CODE!  LastOptionIndex: " << LastOptionIndex << endl;
					cerr << "Need to add more cases to the switch block!" << endl;
					return(3);
				}

				vector<string>::const_iterator ThePos = find(OptionSwitchers.begin(), OptionSwitchers.end(), *ACmdArg);
				
				if (ThePos == OptionSwitchers.end())
				{
					LastOptionIndex = string::npos;
					// the option is not used in this struct, so skip it.
					ACmdArg++;
				}
				else
				{
					LastOptionIndex = ThePos - OptionSwitchers.begin();

					if (ExpectedValueCounts[LastOptionIndex] > 0)
					{
						ACmdArg = CmdArgs.erase(ACmdArg);
					}
					else
					{
						// don't do anything.  The switch statement corresponding to the option will
						// erase it from the vector.
					}
				}
			}
			else
			{
				// then ACmdArg represents a option value for an option not used in this struct.
				// so skip it.
				ACmdArg++;
			}
			break;
		}// end switch statement
        }// end for loop through the CmdArgs

	if (LastOptionIndex != string::npos)
	{
		cerr << "Missing value for option: " << OptionSwitchers.at(LastOptionIndex) << endl;
		return(2);
	}

	return(0);
}// end ParseCommandArgs()

#endif
