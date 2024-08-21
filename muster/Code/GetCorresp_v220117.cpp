//g++ -O2 -I/Users/eita/Dropbox/Research/Tool/All/ GetCorresp_v220117.cpp -o GetCorresp
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
#include"ScorePerfmMatch_v170503.hpp"

using namespace std;

int main(int argc, char** argv) {

	vector<int> v(100);
	vector<double> d(100);
	vector<string> s(100);
	stringstream ss;

	if(argc!=6){cout<<"Error in usage! : $./this in1_qpr.txt in2_qpr.txt in2_match.txt in2_trx.txt out_corresp.txt"<<endl; return -1;}
	string in1Qpr=string(argv[1]);
	string in2Qpr=string(argv[2]);
	string inMatch=string(argv[3]);
	string inTrx=string(argv[4]);
	string outFile=string(argv[5]);

	QuantizedPianoRoll qpr1(in1Qpr),qpr2(in2Qpr);
	ScorePerfmMatch match;
	match.ReadFile(inMatch);
	Trx trx;
	trx.ReadFile(inTrx);

	vector<int> trxToqpr2Idx;
	vector<int> trxToqpr1Idx;
	trxToqpr2Idx.assign(trx.evts.size(),-1);
	trxToqpr1Idx.assign(trx.evts.size(),-1);

	for(int n=0;n<trxToqpr2Idx.size();n++){
		for(int np=0;np<qpr2.evts.size();np++){
			if(qpr2.evts[np].sitch==trx.evts[n].sitch
			    && qpr2.evts[np].onstime==trx.evts[n].onstime){
				trxToqpr2Idx[n]=np;
				break;
			}//endif
		}//endfor np
		if(trxToqpr2Idx[n]<0){cerr<<"trx event "<<n<<" not found in qpr"<<endl;continue;}

		int matchPos=-1;
		for(int np=0;np<match.evts.size();np++){
			if(n==atoi(match.evts[np].ID.c_str())){
				matchPos=np;
				break;
			}//endif
		}//endfor np
		if(matchPos<0){cerr<<"trx event "<<n<<" not found in match"<<endl;continue;}

		if(match.evts[matchPos].matchStatus==0){
			for(int np=0;np<qpr1.evts.size();np++){
				if(qpr1.evts[np].label==match.evts[matchPos].fmt1ID){
					trxToqpr1Idx[n]=np;
					break;
				}//endif
			}//endfor np
		}//endif

	}//endfor n

{
	ofstream ofs(outFile.c_str());
ofs<<"# GT_qpr_ID\tEST_qpr_ID\n";
	for(int n=0;n<trxToqpr2Idx.size();n++){
		if(trxToqpr2Idx[n]<0){continue;}
		if(trxToqpr1Idx[n]<0){continue;}
ofs<<trxToqpr1Idx[n]<<"\t"<<trxToqpr2Idx[n]<<"\n";
	}//endfor n
	ofs.close();
}//

	return 0;
}//end main
