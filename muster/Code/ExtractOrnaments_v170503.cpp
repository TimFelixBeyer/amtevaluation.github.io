//g++ -O2 -I/Users/eita/Dropbox/Research/Tool/All/ ExtractOrnaments_v170503.cpp -o ExtractOrnaments
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
	if(argc!=4){cout<<"Error in usage! : $./this in_fmt3x.txt in_match.txt out_ornament.txt"<<endl; return -1;}

	string fmt3xFile=string(argv[1]);
	string matchFile=string(argv[2]);
	string ornFile=string(argv[3]);

	Fmt3x fmt3x;
	fmt3x.ReadFile(fmt3xFile);
	ScorePerfmMatch match;
	match.ReadFile(matchFile);

/*
[fmt1ID] (matchNoteID) (拍打音から当該noteのontimeの時間差 in +-ms) (当該noteのsitch) velosity (duration in ms), ....
繰り返し音のデリミタとしては、カンマ (,) でつなぎ、改行で、次のトリルに関する情報
もし拍打音がなかった場合は、一番目の音符をそれとする。
もし拍打音が複数あった場合は、最初のものを使う。
*/

	ofstream ofs(ornFile.c_str());

	for(int i=0;i<fmt3x.evts.size();i+=1){
		if(fmt3x.evts[i].eventtype!="chord"){continue;}
		for(int j=0;j<fmt3x.evts[i].notetypes.size();j+=1){
			string notetype=fmt3x.evts[i].notetypes[j].substr(0,fmt3x.evts[i].notetypes[j].find("."));
			if(notetype=="N"){continue;}
			if(notetype!="Tr" && notetype!="Im" && notetype!="Mr" && notetype!="Tn" && notetype!="Dt"
			   && notetype!="It" && notetype!="DIt"){continue;}
			vector<int> matchNoteIDs;
			int onBeatID=-1;
			for(int n=0;n<match.evts.size();n+=1){
				if(match.evts[n].fmt1ID==fmt3x.evts[i].fmt1IDs[j]){
					matchNoteIDs.push_back(n);
					if(onBeatID<0 && match.evts[n].matchStatus==0){
						onBeatID=matchNoteIDs.size()-1;
					}//endif
				}//endif
			}//endfor n
			if(onBeatID<0){onBeatID=0;}

			if(matchNoteIDs.size()==0){
ofs<<"["<<fmt3x.evts[i].fmt1IDs[j]<<"]"<<endl;
				continue;
			}//endif

			double refTime=match.evts[matchNoteIDs[onBeatID]].ontime;
ofs<<"["<<fmt3x.evts[i].fmt1IDs[j]<<"]";
			for(int k=0;k<matchNoteIDs.size();k+=1){
ofs<<" "<<match.evts[matchNoteIDs[k]].ID<<": "<<setprecision(8)<<1000.*(match.evts[matchNoteIDs[k]].ontime-refTime);
ofs<<" "<<match.evts[matchNoteIDs[k]].sitch<<" "<<match.evts[matchNoteIDs[k]].onvel;
ofs<<" "<<setprecision(8)<<1000.*(match.evts[matchNoteIDs[k]].offtime-match.evts[matchNoteIDs[k]].ontime);
				if(k!=matchNoteIDs.size()-1){
ofs<<" ,";
				}//endif
			}//endfor k
ofs<<"\n";

		}//endfor j
	}//endfor i

	ofs.close();

	return 0;
}//end main
