//g++ -I/Users/eita/Dropbox/Research/Tool/All/ TrxToSpr_v170814.cpp -o TrxToSpr
#include<iostream>
#include<string>
#include<sstream>
#include<algorithm>
#include<cmath>
#include<vector>
#include<fstream>
#include<cassert>
#include"Trx_v170203.hpp"
using namespace std;

int main(int argc, char** argv) {

	vector<int> v(100);
	vector<double> d(100);
	vector<string> s(100);
	stringstream ss;

	if(argc!=3){cout<<"Error in usage! : $./this in_trx.txt out_spr.txt"<<endl; return -1;}

	string inTrx=string(argv[1]);
	string outSpr=string(argv[2]);

	Trx trx;
	trx.ReadFile(inTrx);

	PianoRoll pr;
	PianoRollEvt evt;
	for(int n=0;n<trx.evts.size();n+=1){
		evt.ID=trx.evts[n].ID;
		evt.ontime=trx.evts[n].onstime/double(trx.TPQN);
		evt.offtime=trx.evts[n].offstime/double(trx.TPQN);
		evt.sitch=trx.evts[n].sitch;
		evt.pitch=trx.evts[n].pitch;
		evt.onvel=80;
		evt.offvel=80;
		evt.channel=trx.evts[n].voice;
		pr.evts.push_back(evt);
	}//endfor n
	pr.Sort();
	pr.WriteFileSpr(outSpr);


	return 0;
}//end main
