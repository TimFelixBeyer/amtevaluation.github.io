//g++ -I/Users/eita/Dropbox/Research/Tool/All/ TakeStatistics.cpp -o TakeStatistics
#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<stdio.h>
#include<stdlib.h>
#include<cmath>
#include<cassert>
#include"BasicCalculation_v170122.hpp"
using namespace std;

int main(int argc, char** argv) {
	vector<int> v(100);
	vector<double> d(100);
	vector<string> s(100);
	string str;
	stringstream ss;

	if(argc!=2){cout<<"Error in usage! : $./this res.txt"<<endl; return -1;}

	string resFile=string(argv[1]);

	vector<double> pitchErrRate;
	vector<double> missRate;
	vector<double> extraRate;
	vector<double> rhythmCorrectionRate;
	vector<double> offsetErrRate;
	vector<double> meanErrRate;
	vector<double> hmeanErrRate;
	vector<double> offsetScaleErr;

	ifstream ifs(resFile.c_str());
	while(ifs>>s[1]){
		if(s[1]=="File"){
			getline(ifs,s[99]);
			continue;
		}//endif
		assert(s[1].find(":")!=string::npos);
//		for(int i=0;i<7;i+=1){ifs>>s[0];}
		ifs>>d[1]>>d[2]>>d[3]>>d[4]>>d[5]>>d[6]>>d[7]>>d[8];
		pitchErrRate.push_back(d[1]);
		missRate.push_back(d[2]);
		extraRate.push_back(d[3]);
		rhythmCorrectionRate.push_back(d[4]);
		offsetErrRate.push_back(d[5]);
		meanErrRate.push_back(d[6]);
		hmeanErrRate.push_back(d[7]);
		offsetScaleErr.push_back(d[8]);
		getline(ifs,s[99]);
	}//endwhile
	ifs.close();

//cout<<"nPiece,pitchErrRate,missRate,extraRate,rhythmCorrectionRate,offsetErrRate,offsetScaleErr: "<<pitchErrRate.size()<<"\t";
cout<<"nPiece:               "<<pitchErrRate.size()<<endl;
cout<<"pitchErrRate:         "<<Average(pitchErrRate)<<"\t"<<StDev(pitchErrRate)/pow(pitchErrRate.size(),0.5)<<"\t"<<StDev(pitchErrRate)<<endl;
cout<<"missRate:             "<<Average(missRate)<<"\t"<<StDev(missRate)/pow(missRate.size(),0.5)<<"\t"<<StDev(missRate)<<endl;
cout<<"extraRate:            "<<Average(extraRate)<<"\t"<<StDev(extraRate)/pow(extraRate.size(),0.5)<<"\t"<<StDev(extraRate)<<endl;
cout<<"rhythmCorrectionRate: "<<Average(rhythmCorrectionRate)<<"\t"<<StDev(rhythmCorrectionRate)/pow(rhythmCorrectionRate.size(),0.5)<<"\t"<<StDev(rhythmCorrectionRate)<<endl;
cout<<"offsetErrRate:        "<<Average(offsetErrRate)<<"\t"<<StDev(offsetErrRate)/pow(offsetErrRate.size(),0.5)<<"\t"<<StDev(offsetErrRate)<<endl;
cout<<"meanErrRate:          "<<Average(meanErrRate)<<"\t"<<StDev(meanErrRate)/pow(meanErrRate.size(),0.5)<<"\t"<<StDev(meanErrRate)<<endl;
cout<<"hmeanErrRate:         "<<Average(hmeanErrRate)<<"\t"<<StDev(hmeanErrRate)/pow(hmeanErrRate.size(),0.5)<<"\t"<<StDev(hmeanErrRate)<<endl;
cout<<"offsetScaleErr:       "<<Average(offsetScaleErr)<<"\t"<<StDev(offsetScaleErr)/pow(offsetScaleErr.size(),0.5)<<"\t"<<StDev(offsetScaleErr)<<endl;

	return 0;
}//end main
