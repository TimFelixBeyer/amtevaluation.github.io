//g++ Fmt3xToSpr_v220118.cpp -o Fmt3xToSpr
#include<iostream>
#include<string>
#include<sstream>
#include<algorithm>
#include<cmath>
#include<vector>
#include<fstream>
#include<cassert>
#include"Fmt3x_v170225.hpp"
#include"PianoRoll_v170503.hpp"

using namespace std;

int main(int argc, char** argv) {

	vector<int> v(100);
	vector<double> d(100);
	vector<string> s(100);
	stringstream ss;

	if(argc!=3){cout<<"Error in usage! : $./this in_fmt3x.txt out_spr.txt"<<endl; return -1;}

	string inFmt3x=string(argv[1]);
	string outSpr=string(argv[2]);

	Fmt3x fmt3;
	fmt3.ReadFile(inFmt3x);

// 	Trx trx;
// 	trx.ReadFile(inTrx);

	PianoRoll pr;
	PianoRollEvt evt;
	for(int n=0;n<fmt3.evts.size();n++){
		if(fmt3.evts[n].eventtype=="rest"){continue;}
		for(int j=0;j<fmt3.evts[n].notetypes.size();j++){
			evt.ID=fmt3.evts[n].fmt1IDs[j];
			evt.ontime=fmt3.evts[n].stime/double(fmt3.TPQN);
			evt.offtime=(fmt3.evts[n].stime+fmt3.evts[n].dur)/double(fmt3.TPQN);
			evt.sitch=fmt3.evts[n].sitches[j];
			evt.pitch=SitchToPitch(evt.sitch);
			evt.onvel=80;
			evt.offvel=80;
			evt.channel=fmt3.evts[n].voice;
			pr.evts.push_back(evt);
		}//endfor j
	}//endfor n

	pr.Sort();
	pr.WriteFileSpr(outSpr);

	return 0;
}//end main
