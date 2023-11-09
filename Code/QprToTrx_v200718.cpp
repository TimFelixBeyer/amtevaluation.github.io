//g++ -O2 -I/Users/eita/Dropbox/Research/Tool/All/ QprToTrx_v200718.cpp -o QprToTrx
#include<iostream>
#include<string>
#include<sstream>
#include<algorithm>
#include<cmath>
#include<vector>
#include<fstream>
#include<cassert>
#include"QuantizedPianoRoll_v191003.hpp"
#include"Trx_v170203.hpp"
using namespace std;

int main(int argc, char** argv) {

	vector<int> v(100);
	vector<double> d(100);
	vector<string> s(100);
	stringstream ss;

	if(argc!=3){cout<<"Error in usage! : $./this in_qpr.txt out_trx.txt"<<endl; return -1;}
	string inFile=string(argv[1]);
	string outFile=string(argv[2]);

	QuantizedPianoRoll qpr(inFile);

	Trx trx;
	trx.TPQN=qpr.TPQN;
{
// 	vector<string> duplicateFmt1IDs;
// 	for(int i=0;i<fmt3.duplicateOnsets.size();i+=1){
// 		for(int j=1;j<fmt3.duplicateOnsets[i].fmt1IDs.size();j+=1){
// 			duplicateFmt1IDs.push_back(fmt3.duplicateOnsets[i].fmt1IDs[j]);
// 		}//endfor j
// 	}//endfor i
	TrxEvt evt;
	evt.ontime=-1;
	evt.offtime=-1;
	evt.endtime=-1;
	evt.onvel=80;
	evt.offvel=80;
	evt.channel=0;
	evt.secPerQN=-1;
	evt.fmt3xPos1=-1;
	evt.fmt3xPos2=-1;
	for(int i=0;i<qpr.evts.size();i+=1){
		if(qpr.evts[i].pitch<0){continue;}
		evt.onstime=qpr.evts[i].onstime;
		evt.offstime=qpr.evts[i].offstime;
		evt.voice=qpr.evts[i].subvoice;//????qpr.evts[i].channel????
		evt.sitch=qpr.evts[i].sitch;
		evt.pitch=qpr.evts[i].pitch;
		trx.evts.push_back(evt);
	}//endfor i
	for(int n=0;n<trx.evts.size();n+=1){
		ss.str(""); ss<<n;
		trx.evts[n].ID=ss.str();
	}//endfor n
}//
	trx.WriteFile(outFile);

	return 0;
}//end main
