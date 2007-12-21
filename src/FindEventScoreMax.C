using namespace std;

#include <iostream>
#include <string>
#include <vector>
#include <cctype>			// for size_t
#include <iomanip>			// for (TODO: find what I am using this for)

#include "Config/Configuration.h"


#include <CmdLineUtly.h>        // for ProcessFlatCommandLine()
#include "Utils/CAFE_CmdLine.h"       // for handling generic commandline options available to most programs in the CAFE system.
#include <FormatUtly.h>                 // for making things look nice and pretty

#define _PROG_NAME              ((string) "FindEventScoreMax")


void PrintSyntax(const CmdOptions &CAFEOptions)
{
        cout << "\n\n";
        cout << Bold("NAME") << endl;
        cout << "     " + _PROG_NAME + " - collects information from correlation calculations.\n";
        cout << Bold("SYNOPSIS") << endl;
        cout << "     ";
        cout << Bold(_PROG_NAME) + Bold(" [--help] [--syntax] ") << "\n";
        CAFEOptions.PrintSyntax((_PROG_NAME).size() + 5, 63);
        cout << endl;
}



void PrintHelp(const CmdOptions &CAFEOptions)
{
        PrintSyntax(CAFEOptions);

        cout << Bold("DESCRIPTION") << endl;

        cout << "               This program will process information from the correlation calcs.\n\n";

        cout << Bold("OPTIONS") << endl;

        cout << Bold("\t--help --syntax") << endl;

        CAFEOptions.PrintDescription(63);

        cout << "\n\n";
}


//#include <cmath>			// for sqrt()
//#include "VectorUtly.h"			// for FindMax(), FindMin()
//#include "StatUtly.h"			// for Variance(), Avg(), Interquartile()

//void DeterminPassFail(const vector <double> &EventScores, const double &TheThreshold, int &Passes, int &Fails)
//{
//	for (int i = 0; i < EventScores.size(); i++)
//	{
//		if (EventScores[i] >= TheThreshold)
//		{
//			Passes++;
//		}
//		else
//		{
//			Fails++;
//		}
//	}
//}

int main(int argc, char *argv[])
{
	vector <string> CmdLineArgs = ProcessFlatCommandLine(argc, argv);
        CmdOptions CAFEOptions;

        if (CAFEOptions.ParseCommandArgs(CmdLineArgs) != 0)
        {
                cerr << "ERROR: Invalid syntax..." << endl;
                PrintSyntax(CAFEOptions);
                return(8);
        }

	for (size_t ArgIndex = 0; ArgIndex < CmdLineArgs.size(); ArgIndex++)
        {
                if (CmdLineArgs[ArgIndex] == "--help")
                {
                        PrintHelp(CAFEOptions);
                        return(2);
                }
                else if (CmdLineArgs[ArgIndex] == "--syntax")
                {
                        PrintSyntax(CAFEOptions);
                        return(2);
                }
                else
                {
			cerr << "ERROR: Unknown command line option: " << CmdLineArgs[ArgIndex] << endl;
			PrintSyntax(CAFEOptions);
			return(8);
                }
        }

        Configuration ConfigInfo(CAFEOptions.CAFEPath + '/' + CAFEOptions.ConfigFilename);

        if (!ConfigInfo.IsValid())
        {
                cerr << "ERROR: Something wrong with the config file: " << CAFEOptions.CAFEPath << '/' << CAFEOptions.ConfigFilename << endl;
                return(1);
        }

        if (!CAFEOptions.MergeWithConfiguration(ConfigInfo))
        {
                cerr << "ERROR: Conflicts in the command line..." << endl;
                PrintSyntax(CAFEOptions);
                return(8);
        }


	const string BaseDir = CAFEOptions.CAFEPath + "/AnalysisInfo/";

        for (vector<string>::const_iterator ADatabase = CAFEOptions.ClustDatabaseNames.begin();
                     ADatabase != CAFEOptions.DatabaseNames.end(); ADatabase++)
        {
		cout << "    Database: " << *ADatabase << endl;
		cout << "       Table: ";
		
		vector <double> EventScoreMaxes(CAFEOptions.EventTypes.size(), 0.0);
		vector <double> AlphaTotals(CAFEOptions.EventTypes.size(), 0.0);
		vector <double> PhiTotals(CAFEOptions.EventTypes.size(), 0.0);
//		vector <double> MaxHindcastScores(CAFEOptions.EventTypes.size(), 0.0);
//		vector <double> MinHindcastScores(CAFEOptions.EventTypes.size(), 0.0);
//		vector <double> MeanHindcastScores(CAFEOptions.EventTypes.size(), 0.0);
//		vector <double> MedianHindcastScores(CAFEOptions.EventTypes.size(), 0.0);
//		vector <double> ThresholdScores1(CAFEOptions.EventTypes.size(), 0.0);
//		vector <double> ThresholdScores2(CAFEOptions.EventTypes.size(), 0.0);

//		vector <int> PassMeans(CAFEOptions.EventTypes.size(), 0);
//		vector <int> FailMeans(CAFEOptions.EventTypes.size(), 0);
//		vector <int> PassMedians(CAFEOptions.EventTypes.size(), 0);
//		vector <int> FailMedians(CAFEOptions.EventTypes.size(), 0);
//		vector <int> PassThresholds1(CAFEOptions.EventTypes.size(), 0);
//		vector <int> FailThresholds1(CAFEOptions.EventTypes.size(), 0);
//		vector <int> PassThresholds2(CAFEOptions.EventTypes.size(), 0);
//		vector <int> FailThresholds2(CAFEOptions.EventTypes.size(), 0);

		size_t TableIndex = 0;
                for (vector<string>::const_iterator EventTypeName = CAFEOptions.EventTypes.begin();
                     EventTypeName != CAFEOptions.EventTypes.end();
                     EventTypeName++, TableIndex++)
                {
			printf("%-13.13s ", (*EventTypeName).c_str());
//			const string EventScoreFilename = BaseDir + "CorrelationCalcs/" + *ADatabase + '/' + *EventTypeName + "_EventScore.csv";

//			vector <double> EventScores(0);
//			vector <string> DateStrs(0);
//			if (!LoadEventScores(EventScores, DateStrs, EventScoreFilename, *EventTypeName))
//			{
//				cerr << "\n\tCould not load EventScores file: " << EventScoreFilename << endl;
//				return(3);
//			}

//			MaxHindcastScores[TableIndex] = FindMax(EventScores);
//			MinHindcastScores[TableIndex] = FindMin(EventScores);
//			MeanHindcastScores[TableIndex] = Avg(EventScores);
//			MedianHindcastScores[TableIndex] = InterquartileVal(2, EventScores);
//			ThresholdScores1[TableIndex] = MeanHindcastScores[TableIndex] - sqrt(Variance(EventScores));
//			ThresholdScores2[TableIndex] = InterquartileVal(1, EventScores);
//			DeterminPassFail(EventScores, MeanHindcastScores[TableIndex], PassMeans[TableIndex], FailMeans[TableIndex]);
//			DeterminPassFail(EventScores, MedianHindcastScores[TableIndex], PassMedians[TableIndex], FailMedians[TableIndex]);
//			DeterminPassFail(EventScores, ThresholdScores1[TableIndex], PassThresholds1[TableIndex], FailThresholds1[TableIndex]);
//			DeterminPassFail(EventScores, ThresholdScores2[TableIndex], PassThresholds2[TableIndex], FailThresholds2[TableIndex]);

			const vector <string> VarNames = CAFEOptions.GiveCAFEVarsToDo(ConfigInfo, *EventTypeName);

                        for (vector<string>::const_iterator AVarName = VarNames.begin(); AVarName != VarNames.end(); AVarName++)
                        {
				const vector <string> CAFELabels = CAFEOptions.GiveLabelsToDo(ConfigInfo, *EventTypeName, *AVarName);

                                for (vector<string>::const_iterator ALabel = CAFELabels.begin(); ALabel != CAFELabels.end(); ALabel++)
                                {
					for (size_t PeakValIndex = 0; PeakValIndex < ConfigInfo.ExtremaCount(); PeakValIndex++)
					{
						const string FieldName = *EventTypeName + '_' + *ALabel + '_' 
									 + ConfigInfo.GiveExtremaName(PeakValIndex);

						const string FieldMeasureFilename = BaseDir + '/' + *ADatabase + '/' + FieldName + ".FieldMeasure";
						double Alpha = 0.0;
						double Phi = 0.0;

						if (!LoadAlphaPhi(Alpha, Phi, FieldMeasureFilename))
						{
							cerr << "\n\tCould not load FieldMeasure file: " << FieldMeasureFilename << endl;
							return(9);
						}

						AlphaTotals[TableIndex] += Alpha;
						PhiTotals[TableIndex] += Phi;
						EventScoreMaxes[TableIndex] += (Alpha + Phi);
					}// end extrema loop
				}// end label loop
			}// end CAFEVarName loop
		}// end event type loop
		cout << endl << " Event Maxes: ";

		for (size_t TableIndex = 0; TableIndex < CAFEOptions.EventTypes.size(); TableIndex++)
		{
			printf("%-13.7g ", EventScoreMaxes[TableIndex]);
		}

/*		cout << endl << "     Means: ";
		for (size_t TableIndex = 0; TableIndex < CAFEOptions.EventTypes.size(); TableIndex++)
                {
                        printf("%-11.5g ", MeanHindcastScores[TableIndex]);
                }
*/
		cout << endl << "TotAlpha/Phi: ";
		for (size_t TableIndex = 0; TableIndex < CAFEOptions.EventTypes.size(); TableIndex++)
                {
                        printf("%6.4g/%-6.4g ", AlphaTotals[TableIndex], PhiTotals[TableIndex]);
                }
/*

		cout << endl << "   Medians: ";
                for (size_t TableIndex = 0; TableIndex < CAFEOptions.EventTypes.size(); TableIndex++)
                {
                        printf("%-11.5g ", MedianHindcastScores[TableIndex]);
                }

		cout << endl << " Pass/Fail: ";
                for (size_t TableIndex = 0; TableIndex < CAFEOptions.EventTypes.size(); TableIndex++)
                {
                        printf("%-5i %-5i ", PassMedians[TableIndex], FailMedians[TableIndex]);
                }


		cout << endl << "Threshold1: ";
                for (size_t TableIndex = 0; TableIndex < CAFEOptions.EventTypes.size(); TableIndex++)
                {
                        printf("%-11.5g ", ThresholdScores1[TableIndex]);
                }

		cout << endl << " Pass/Fail: ";
                for (size_t TableIndex = 0; TableIndex < CAFEOptions.EventTypes.size(); TableIndex++)
                {
                        printf("%-5i %-5i ", PassThresholds1[TableIndex], FailThresholds1[TableIndex]);
                }


		cout << endl << "Threshold2: ";
                for (size_t TableIndex = 0; TableIndex < CAFEOptions.EventTypes.size(); TableIndex++)
                {
                        printf("%-11.5g ", ThresholdScores2[TableIndex]);
                }

		cout << endl << " Pass/Fail: ";
                for (size_t TableIndex = 0; TableIndex < CAFEOptions.EventTypes.size(); TableIndex++)
                {
                        printf("%-5i %-5i ", PassThresholds2[TableIndex], FailThresholds2[TableIndex]);
                }


		cout << endl << "  HindMax: ";
                for (size_t TableIndex = 0; TableIndex < CAFEOptions.EventTypes.size(); TableIndex++)
                {
                        printf("%-11.5g ", MaxHindcastScores[TableIndex]);
                }

		cout << endl << "  HindMin: ";
                for (size_t TableIndex = 0; TableIndex < CAFEOptions.EventTypes.size(); TableIndex++)
                {
                        printf("%-11.5g ", MinHindcastScores[TableIndex]);
                }

*/
		cout << endl << endl;
	}// end databse loop
	
	return(0);
}
