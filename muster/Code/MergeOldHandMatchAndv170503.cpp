//g++ -O2 -I/Users/eita/Dropbox/Research/Tool/All/ MergeOldHandMatchAndv170503.cpp -o MergeOldHandMatchAndv170503
#include<iostream>
#include<string>
#include<sstream>
#include<cmath>
#include<vector>
#include<fstream>
#include<cassert>
#include<iomanip>
#include "ScorePerfmMatch_v170503.hpp"
#include "Fmt3x_v170225.hpp"
using namespace std;

int main(int argc,char** argv){

	vector<int> v(100);
	vector<double> d(100);
	vector<string> s(100);
	stringstream ss;
	if(argc!=5){cout<<"Error in usage! : $./this in_fmt3x.txt old_hand_match.txt new_auto_match.txt out_new_match.txt"<<endl; return -1;}

	string fmt3xFile=string(argv[1]);
	string handMatchFile=string(argv[2]);
	string autoMatchFile=string(argv[3]);
	string newMatchFile=string(argv[4]);

	Fmt3x fmt3x;
	fmt3x.ReadFile(fmt3xFile);
	ScorePerfmMatch handMatch;
	handMatch.ReadFile(handMatchFile);
	ScorePerfmMatch autoMatch;
	autoMatch.ReadFile(autoMatchFile);

	assert(handMatch.evts.size()==autoMatch.evts.size());

/*
SkipInd = '+' -> replace with '-'
Set matchStatus of extra notes to -1
Set matchStatus of ornament notes
ASSUMPTION: hand match file has fmt1IDs on temporal notes (atemporal notes must be extra notes).
Respect always hand matches with fmt1IDs -> matchStatus=0
Substitute auto match results only if ornaments and extra notes in the hand match result -> matchStatus=1
*/

	///Set matchStatus
	for(int n=0;n<handMatch.evts.size();n+=1){
		if(handMatch.evts[n].errorInd>1){
			handMatch.evts[n].matchStatus=-1;
		}else{
			handMatch.evts[n].matchStatus=0;
		}//endif
		if(handMatch.evts[n].skipInd=="+"){
			handMatch.evts[n].skipInd="-";
		}//endif
	}//endfor n


	for(int i=0;i<fmt3x.evts.size();i+=1){
		if(fmt3x.evts[i].eventtype!="chord"){continue;}
		for(int j=0;j<fmt3x.evts[i].notetypes.size();j+=1){
			string notetype=fmt3x.evts[i].notetypes[j].substr(0,fmt3x.evts[i].notetypes[j].find("."));
			if(notetype=="N"){continue;}
			if(notetype!="Tr" && notetype!="Im" && notetype!="Mr" && notetype!="Tn" && notetype!="Dt"
			   && notetype!="It" && notetype!="DIt"){continue;}
			for(int n=0;n<autoMatch.evts.size();n+=1){
				if(autoMatch.evts[n].fmt1ID==fmt3x.evts[i].fmt1IDs[j]){
					if(handMatch.evts[n].errorInd>1){
						handMatch.evts[n].matchStatus=1;
						handMatch.evts[n].stime=autoMatch.evts[n].stime;
						handMatch.evts[n].fmt1ID=autoMatch.evts[n].fmt1ID;
						handMatch.evts[n].errorInd=autoMatch.evts[n].errorInd;
					}//endif
				}//endif
			}//endfor n
		}//endfor j
	}//endfor i

	handMatch.WriteFile(newMatchFile);

	return 0;
}//end main
