//g++ -I/Users/eita/Dropbox/Research/Tool/All/ ScoreMatchEvaluation_v170814.cpp -o ScoreMatchEvaluation
#include<iostream>
#include<string>
#include<sstream>
#include<algorithm>
#include<cmath>
#include<vector>
#include<fstream>
#include<cassert>
#include"Trx_v170203.hpp"
#include"Fmt3x_v170225.hpp"
#include"ScorePerfmMatch_v170503.hpp"
using namespace std;

class ScoreNote{
public:
	int onstime;
	int offstime;
	string sitch;
	int pitch;
	string fmt1ID;
	string trxID;
	int matchPos;
	int found;

	int lowerMaxPos;
	int higherMinPos;

};//endclass ScoreNote

int main(int argc, char** argv) {

	vector<int> v(100);
	vector<double> d(100);
	vector<string> s(100);
	stringstream ss;

	if(argc!=5){cout<<"Error in usage! : $./this fmt3x.txt MRF_trx.txt match.txt stimeCut(def:-1)"<<endl; return -1;}

	string fmt3xFile=string(argv[1]);
	string trxFile=string(argv[2]);
	string matchFile=string(argv[3]);
	int stimeCut=atoi(argv[4]);

	Fmt3x fmt3x;
	fmt3x.ReadFile(fmt3xFile);
	Trx trx;
	trx.ReadFile(trxFile);
	ScorePerfmMatch match;
	match.ReadFile(matchFile);

	if(stimeCut<0){
		for(int n=0;n<match.evts.size();n+=1){
			if(match.evts[n].errorInd>1){continue;}
			if(match.evts[n].stime>stimeCut){
				stimeCut=match.evts[n].stime;
			}//endif
		}//endfor n
	}//endfor


	/// If still stimeCut<0, no matched notes found.
	if(stimeCut<0){
		double scaleErr=0;
		double pitchER,missRate,extraRate,RCRate,offsetER,meanER,hmeanER;
		pitchER=0;
		missRate=100;
		extraRate=100;
		RCRate=0;
		offsetER=0;
		meanER=(pitchER+missRate+extraRate+RCRate+offsetER)/5.;
		hmeanER=5./(1./pitchER+1./missRate+1./extraRate+1./RCRate+1./offsetER);
cout<<"PitchER,MissRate,ExtraRate,RCRate,OffsetER,MeanER,HMeanER,ScaleErr:\t";
cout<<pitchER<<"\t"<<missRate<<"\t"<<extraRate<<"\t"<<RCRate<<"\t"<<offsetER<<"\t"<<meanER<<"\t"<<hmeanER<<"\t"<<scaleErr<<endl;
		return 0;
	}//endif


	int nTrNote=trx.evts.size();
	int nScoreNote=0;
	int nExtraNote=0;
	int nMissNote=0;
	int nPitchError=0;
	int nCorrectNote=0;

	vector<ScoreNote> matchedNoteTrue;
	vector<ScoreNote> matchedNoteEst;

	vector<string> duplicates;
	for(int i=0;i<fmt3x.duplicateOnsets.size();i+=1){
		for(int j=1;j<fmt3x.duplicateOnsets[i].fmt1IDs.size();j+=1){
			duplicates.push_back(fmt3x.duplicateOnsets[i].fmt1IDs[j]);
		}//endfor j
	}//endfor i

{
	ScoreNote scoreNote;
	for(int n=0;n<fmt3x.evts.size();n+=1){
		if(fmt3x.evts[n].stime>stimeCut){break;}
		if(fmt3x.evts[n].eventtype=="rest"){continue;}
		for(int m=0;m<fmt3x.evts[n].sitches.size();m+=1){
			if(find(duplicates.begin(),duplicates.end(),fmt3x.evts[n].fmt1IDs[m])!=duplicates.end()){continue;}
			scoreNote.onstime=fmt3x.evts[n].stime;
			scoreNote.offstime=fmt3x.evts[n].stime+fmt3x.evts[n].dur;
			scoreNote.sitch=fmt3x.evts[n].sitches[m];
			scoreNote.fmt1ID=fmt3x.evts[n].fmt1IDs[m];
			scoreNote.found=-1;
			matchedNoteTrue.push_back(scoreNote);
		}//endfor m
	}//endfor n
}//
	nScoreNote=matchedNoteTrue.size();


{
	ScoreNote scoreNote;
	for(int n=0;n<trx.evts.size();n+=1){
		scoreNote.onstime=trx.evts[n].onstime;
		scoreNote.offstime=trx.evts[n].offstime;
		scoreNote.sitch=trx.evts[n].sitch;
		scoreNote.fmt1ID="";
		scoreNote.trxID=trx.evts[n].ID;
		scoreNote.found=-1;
		scoreNote.matchPos=-1;
		for(int i=0;i<match.evts.size();i+=1){
			if(match.evts[i].ID==scoreNote.trxID){
				scoreNote.matchPos=i;
				scoreNote.fmt1ID=match.evts[i].fmt1ID;
				break;
			}//endif
		}//endfor i
		assert(scoreNote.matchPos>=0);
		matchedNoteEst.push_back(scoreNote);
	}//endfor n
}//

	// Extract extra notes (including doubly appeared notes)
	for(int i=matchedNoteEst.size()-1;i>=0;i-=1){
		if(match.evts[matchedNoteEst[i].matchPos].errorInd==2 || match.evts[matchedNoteEst[i].matchPos].errorInd==3){
			nExtraNote+=1;
			matchedNoteEst.erase(matchedNoteEst.begin()+i);
		}//endif
	}//endfor i

	for(int i=0;i<matchedNoteEst.size();i+=1){
		for(int n=0;n<matchedNoteTrue.size();n+=1){
			if(matchedNoteTrue[n].fmt1ID==matchedNoteEst[i].fmt1ID){
				if(matchedNoteTrue[n].found<0){
					matchedNoteTrue[n].found=i;
					matchedNoteEst[i].found=n;
				}else{
					matchedNoteEst[i].found=-1;// -> to be an extra note
				}//endif
				break;
			}//endif
		}//endfor n
	}//endfor i

	for(int i=matchedNoteEst.size()-1;i>=0;i-=1){
		if(matchedNoteEst[i].found<0){
			nExtraNote+=1;
			matchedNoteEst.erase(matchedNoteEst.begin()+i);
		}//endif
	}//endfor i

	/// Extract missing notes
	for(int i=matchedNoteTrue.size()-1;i>=0;i-=1){
		if(matchedNoteTrue[i].found<0){
			nMissNote+=1;
			matchedNoteTrue.erase(matchedNoteTrue.begin()+i);
		}//endif
	}//endfor i

	assert(matchedNoteTrue.size()==matchedNoteEst.size());

	/// Extract correct note and pitch error
	for(int i=0;i<matchedNoteEst.size();i+=1){
		if(match.evts[matchedNoteEst[i].matchPos].errorInd==0){
			nCorrectNote+=1;
		}else if(match.evts[matchedNoteEst[i].matchPos].errorInd==1){
			nPitchError+=1;
		}else{//Never reach here
		}//endif
	}//endfor i

	if(nScoreNote!=nCorrectNote+nPitchError+nMissNote){
cout<<"Score,Corr,PitchErr,Miss,Extra: "<<nScoreNote<<"\t"<<nCorrectNote<<"\t"<<nPitchError<<"\t"<<nMissNote<<"\t"<<nExtraNote<<endl;
		assert(nScoreNote==nCorrectNote+nPitchError+nMissNote);
	}//endif


	/// If #Matched notes < 2, rhythm correction cost cannot be calculated
	if(matchedNoteTrue.size()<=1){
		double scaleErr=0;
		double pitchER,missRate,extraRate,RCRate,offsetER,meanER,hmeanER;
		pitchER=double(nPitchError)/double(nScoreNote)*100;
		missRate=double(nMissNote)/double(nScoreNote)*100;
		extraRate=double(nExtraNote)/double(nTrNote)*100;
		RCRate=0;
		offsetER=0;
		meanER=(pitchER+missRate+extraRate+RCRate+offsetER)/5.;
		hmeanER=5./(1./pitchER+1./missRate+1./extraRate+1./RCRate+1./offsetER);
cout<<"PitchER,MissRate,ExtraRate,RCRate,OffsetER,MeanER,HMeanER,ScaleErr:\t";
cout<<pitchER<<"\t"<<missRate<<"\t"<<extraRate<<"\t"<<RCRate<<"\t"<<offsetER<<"\t"<<meanER<<"\t"<<hmeanER<<"\t"<<scaleErr<<endl;
		return 0;
	}//endif

	/// Sort matchedNoteEst
{
	vector<ScoreNote> matchedNoteEst_old;
	matchedNoteEst_old=matchedNoteEst;
	matchedNoteEst.clear();
	for(int i=0;i<matchedNoteTrue.size();i+=1){
		for(int n=0;n<matchedNoteEst_old.size();n+=1){
			if(matchedNoteEst_old[n].fmt1ID==matchedNoteTrue[i].fmt1ID){
				matchedNoteEst.push_back(matchedNoteEst_old[n]);
				matchedNoteEst_old.erase(matchedNoteEst_old.begin()+n);
				break;
			}//endif
		}//endfor n
	}//endfor i
}//

	int lowerMaxPos;
	int higherMinPos;
	for(int i=0;i<matchedNoteEst.size();i+=1){
		matchedNoteEst[i].lowerMaxPos=0;
		matchedNoteEst[i].higherMinPos=-1;
		for(int j=0;j<matchedNoteEst.size();j+=1){
			if(matchedNoteEst[j].onstime<=matchedNoteEst[i].offstime){
				if(matchedNoteEst[j].onstime>matchedNoteEst[matchedNoteEst[i].lowerMaxPos].onstime){
					matchedNoteEst[i].lowerMaxPos=j;
				}else if(matchedNoteEst[j].onstime==matchedNoteEst[matchedNoteEst[i].lowerMaxPos].onstime){
					if(abs(SitchToPitch(matchedNoteEst[j].sitch)-SitchToPitch(matchedNoteEst[i].sitch))
					   < abs(SitchToPitch(matchedNoteEst[matchedNoteEst[i].lowerMaxPos].sitch)-SitchToPitch(matchedNoteEst[i].sitch))){
						matchedNoteEst[i].lowerMaxPos=j;
					}//endif
				}//endif
			}//endif
			if(matchedNoteEst[j].onstime>=matchedNoteEst[i].offstime){
				if(matchedNoteEst[i].higherMinPos<0){
					matchedNoteEst[i].higherMinPos=j;
					continue;
				}//endif
				if(matchedNoteEst[j].onstime<matchedNoteEst[matchedNoteEst[i].higherMinPos].onstime){
					matchedNoteEst[i].higherMinPos=j;
				}else if(matchedNoteEst[j].onstime==matchedNoteEst[matchedNoteEst[i].higherMinPos].onstime){
					if(abs(SitchToPitch(matchedNoteEst[j].sitch)-SitchToPitch(matchedNoteEst[i].sitch))
					   < abs(SitchToPitch(matchedNoteEst[matchedNoteEst[i].higherMinPos].sitch)-SitchToPitch(matchedNoteEst[i].sitch))){
						matchedNoteEst[i].higherMinPos=j;
					}//endif
				}//endif
			}//endif
		}//endfor j
	}//endfor i


	/// Calculate rhythm correction cost for onset times
	int nvAll[]={384,192,96,48,24,12,576,288,144,72,36,18,672,336,168,84,42,21,256,128,64,32,16,8};
	int nNvAll=24;
	vector<IrredFrac> scales;
	for(int i=0;i<nNvAll;i+=1)for(int j=0;j<nNvAll;j+=1){
		IrredFrac irr(nvAll[i],nvAll[j]);
		int found=-1;
		for(int k=0;k<scales.size();k+=1){
			if(scales[k].num==irr.num && scales[k].den==irr.den){
				found=k;
				break;
			}//endif
		}//endfor k
		if(found<0){scales.push_back(irr);}
	}//endfor i,j

	vector<int> optPath(matchedNoteTrue.size()-1);
	vector<double> LP(scales.size());
	vector<vector<int> > amin(matchedNoteTrue.size()-1);
	int nvTrue,nvEst;
	for(int i=0;i<matchedNoteTrue.size()-1;i+=1){amin[i].resize(scales.size());}
	/// Initialize
	LP.assign(scales.size(),1);
	LP[0]=0;
	nvTrue=(matchedNoteTrue[1].onstime-matchedNoteTrue[0].onstime)*trx.TPQN;
	nvEst=(matchedNoteEst[1].onstime-matchedNoteEst[0].onstime)*fmt3x.TPQN;
	for(int k=0;k<scales.size();k+=1){
		LP[k]+=((nvTrue*scales[k].den==nvEst*scales[k].num)? 0:1);
	}//endfor k

	double logP;
	for(int n=1;n<matchedNoteTrue.size()-1;n+=1){
		nvTrue=(matchedNoteTrue[n+1].onstime-matchedNoteTrue[n].onstime)*trx.TPQN;
		nvEst=(matchedNoteEst[n+1].onstime-matchedNoteEst[n].onstime)*fmt3x.TPQN;
		vector<double> preLP(LP);
		for(int k=0;k<scales.size();k+=1){
			LP[k]=preLP[k];
			amin[n][k]=k;
			for(int kp=0;kp<scales.size();kp+=1){
				logP=preLP[kp]+((kp==k)? 0:1);
				if(logP<LP[k]){
					LP[k]=logP;
					amin[n][k]=kp;
				}//endif
			}//endfor kp
			LP[k]+=((nvTrue*scales[k].den==nvEst*scales[k].num)? 0:1);
		}//endfor k
	}//endfor n

	optPath[matchedNoteTrue.size()-2]=0;
	for(int k=0;k<scales.size();k+=1){
		if(LP[k]<LP[optPath[matchedNoteTrue.size()-2]]){
			optPath[matchedNoteTrue.size()-2]=k;
		}//endif
	}//endfor k

	double RCC=LP[optPath[matchedNoteTrue.size()-2]];
	IrredFrac lastScale=scales[optPath[matchedNoteTrue.size()-2]];

	for(int n=matchedNoteTrue.size()-3;n>=0;n-=1){
		optPath[n]=amin[n+1][optPath[n+1]];
	}//endfor n

	/// Correct onset and offset stime
	vector<ScoreNote> matchedNoteEstCorrected;
	matchedNoteEstCorrected=matchedNoteEst;
	for(int n=0;n<matchedNoteTrue.size();n+=1){
		matchedNoteEstCorrected[n].onstime=matchedNoteTrue[n].onstime;

		if(matchedNoteEst[n].higherMinPos<0){//offstime is larger than the last onstime
			matchedNoteEstCorrected[n].offstime=matchedNoteEstCorrected[n].onstime+int(((matchedNoteEst[n].offstime-matchedNoteEst[n].onstime)*fmt3x.TPQN*lastScale.num)/(trx.TPQN*lastScale.den));
			continue;
		}//endif

		if(matchedNoteEst[matchedNoteEst[n].lowerMaxPos].onstime==matchedNoteEst[matchedNoteEst[n].higherMinPos].onstime){
			matchedNoteEstCorrected[n].offstime=matchedNoteTrue[matchedNoteEst[n].lowerMaxPos].onstime;
		}else{
			int durTrue=matchedNoteTrue[matchedNoteEst[n].higherMinPos].onstime-matchedNoteTrue[matchedNoteEst[n].lowerMaxPos].onstime;
			int durEst=matchedNoteEst[matchedNoteEst[n].higherMinPos].onstime-matchedNoteEst[matchedNoteEst[n].lowerMaxPos].onstime;
			if(durTrue==0){
				matchedNoteEstCorrected[n].offstime=matchedNoteTrue[matchedNoteEst[n].higherMinPos].onstime;
			}else{
				if(durEst*durTrue<0){durTrue*=-1;}
				matchedNoteEstCorrected[n].offstime=matchedNoteTrue[matchedNoteEst[n].lowerMaxPos].onstime+(durTrue*(matchedNoteEst[n].offstime-matchedNoteEst[n].onstime))/durEst;
			}//endif
		}//endif

	}//endfor n

	double er=0;
	double countER=0;
	double scaleErr=0;
	double countSE=0;
	for(int n=0;n<matchedNoteTrue.size();n+=1){
		countER+=1;
		er+=((matchedNoteEstCorrected[n].offstime!=matchedNoteTrue[n].offstime)? 1.:0.);

		if(matchedNoteEstCorrected[n].offstime<=matchedNoteEstCorrected[n].onstime || matchedNoteTrue[n].offstime<=matchedNoteTrue[n].onstime){continue;}

		countSE+=1;
		scaleErr+=abs(log(double(matchedNoteEstCorrected[n].offstime-matchedNoteEstCorrected[n].onstime)/double(matchedNoteTrue[n].offstime-matchedNoteTrue[n].onstime)));
	}//endfor n
	er/=countER;
	scaleErr/=countSE;
	scaleErr=exp(scaleErr);

	double pitchER,missRate,extraRate,RCRate,offsetER,meanER,hmeanER;
	pitchER=double(nPitchError)/double(nScoreNote)*100;
	missRate=double(nMissNote)/double(nScoreNote)*100;
	extraRate=double(nExtraNote)/double(nTrNote)*100;
	RCRate=double(RCC)/double(matchedNoteTrue.size())*100;
	offsetER=er*100;
	meanER=(pitchER+missRate+extraRate+RCRate+offsetER)/5.;
	hmeanER=5./(1./pitchER+1./missRate+1./extraRate+1./RCRate+1./offsetER);

cout<<"PitchER,MissRate,ExtraRate,RCRate,OffsetER,MeanER,HMeanER,ScaleErr:\t";
cout<<pitchER<<"\t"<<missRate<<"\t"<<extraRate<<"\t"<<RCRate<<"\t"<<offsetER<<"\t"<<meanER<<"\t"<<hmeanER<<"\t"<<scaleErr<<endl;

	return 0;
}//end main
