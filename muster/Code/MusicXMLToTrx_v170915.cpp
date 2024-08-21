//g++ -I/Users/eita/Dropbox/Research/Tool/All/ MusicXMLToTrx_v170915.cpp -o MusicXMLToTrx
#include<fstream>
#include<iostream>
#include<cmath>
#include<string>
#include<sstream>
#include<vector>
#include<algorithm>
#include"stdio.h"
#include"stdlib.h"
#include"Trx_v170203.hpp"
#include"Fmt3x_v170225.hpp"
using namespace std;

int main(int argc, char** argv){
	vector<int> v(100);
	vector<double> d(100);
	vector<string> s(100);
	stringstream ss;
	clock_t start, end;
	start = clock();

	if(argc!=3){
		cout<<"Error in usage: $./this in.xml out_trx.txt"<<endl;
		return -1;
	}//endif

	string xmlFile=string(argv[1]);
	string outTrx=string(argv[2]);

	Fmt1x fmt1;
	Fmt3x fmt3;
	fmt1.ReadMusicXML(xmlFile);
	fmt3.ConvertFromFmt1x(fmt1);

	Trx trx;
	trx.TPQN=fmt3.TPQN;
{
	vector<string> duplicateFmt1IDs;
	for(int i=0;i<fmt3.duplicateOnsets.size();i+=1){
		for(int j=1;j<fmt3.duplicateOnsets[i].fmt1IDs.size();j+=1){
			duplicateFmt1IDs.push_back(fmt3.duplicateOnsets[i].fmt1IDs[j]);
		}//endfor j
	}//endfor i
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
	for(int i=0;i<fmt3.evts.size();i+=1){
		if(fmt3.evts[i].eventtype!="chord"){continue;}
		evt.onstime=fmt3.evts[i].stime;
		evt.offstime=fmt3.evts[i].stime+fmt3.evts[i].dur;
		evt.voice=fmt3.evts[i].voice;
		for(int j=0;j<fmt3.evts[i].sitches.size();j+=1){
			evt.sitch=fmt3.evts[i].sitches[j];
			evt.pitch=SitchToPitch(evt.sitch);
			evt.fmt3xPos1=i;
			evt.fmt3xPos2=j;
			trx.evts.push_back(evt);
		}//endfor j
	}//endfor i
	for(int n=0;n<trx.evts.size();n+=1){
		ss.str(""); ss<<n;
		trx.evts[n].ID=ss.str();
	}//endfor n
}//
	trx.WriteFile(outTrx);


//	end = clock(); cout<<"Elapsed time : "<<((double)(end - start) / CLOCKS_PER_SEC)<<" sec"<<endl; start=end;
	return 0;
}//end main

