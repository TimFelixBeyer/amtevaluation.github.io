//g++ -O2 MusicXMLToQpr_v191003.cpp -o MusicXMLToQpr
#include<iostream>
#include<string>
#include<sstream>
#include<algorithm>
#include<cmath>
#include<vector>
#include<fstream>
#include<cassert>
#include"QuantizedPianoRoll_v191003.hpp"
using namespace std;

int main(int argc, char** argv) {

	vector<int> v(100);
	vector<double> d(100);
	vector<string> s(100);
	stringstream ss;

	if(argc!=4){cout<<"Error in usage! : $./this outType(0:qpr/1:qipr/2:qpr(w/o rests)/3:qipr(w/o rests) in.xml out_qpr.txt"<<endl; return -1;}
	int outType=atoi(argv[1]);
	string inXML=string(argv[2]);
	string outFile=string(argv[3]);

	assert(outType>=0 && outType<=3);

	QuantizedPianoRoll qpr;
	qpr.ReadMusicXMLFile(inXML);

	if(outType>=2){
		for(int n=qpr.evts.size()-1;n>=0;n-=1){
			if(qpr.evts[n].pitch<0){
				qpr.evts.erase(qpr.evts.begin()+n);
			}//endif
		}//endfor n
	}//endif

	qpr.WriteFile(outFile,outType%2);

	return 0;
}//end main
