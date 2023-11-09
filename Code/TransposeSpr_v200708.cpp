//g++ -O2 -I/Users/eita/boost_1_63_0 -I/Users/eita/Dropbox/Research/Tool/All/ TransposeSpr_v200708.cpp -o TransposeSpr
#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<stdio.h>
#include<cmath>
#include<cassert>
#include<algorithm>
#include "PianoRoll_v170503.hpp"
using namespace std;

int main(int argc, char** argv) {
	vector<int> v(100);
	vector<double> d(100);
	vector<string> s(100);
	stringstream ss;

	if(argc!=4){cout<<"Error in usage: ./this in_spr.txt transposition out_spr.txt"<<endl; return -1;}
	string inSpr=string(argv[1]);
	int transposition=atoi(argv[2]);
	string outSpr=string(argv[3]);

	PianoRoll pr;
	pr.ReadFileSpr(inSpr);

	if(transposition!=0){

		for(int l=0;l<pr.evts.size();l+=1){
			pr.evts[l].pitch+=transposition;
			pr.evts[l].sitch=PitchToSitch(pr.evts[l].pitch);
		}//endfor l

	}//endif

	pr.WriteFileSpr(outSpr);

	return 0;
}//end main
