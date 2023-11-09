//g++ Code/ScoreMatchEvaluation_VoicePlus_v220118.cpp -o Programs/ScoreMatchEvaluation_VoicePlus
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
	string voiceStr;//partXML-staffXML-voiceXML
	int voice;//0-3(RH), 4-7(LH)

	int lowerMaxPos;
	int higherMinPos;

};//endclass ScoreNote

int main(int argc, char** argv) {

	vector<int> v(100);
	vector<double> d(100);
	vector<string> s(100);
	stringstream ss;

//	if(argc!=6){cout<<"Error in usage! : $./this GT_fmt3x.txt EST_fmt3x.txt MRF_trx.txt match.txt stimeCut(def:-1)"<<endl; return -1;}
	if(argc!=6){cout<<"Error in usage! : $./this GT_fmt3x.txt EST_fmt3x.txt match.txt detail.txt stimeCut(def:-1)"<<endl; return -1;}

	string fmt3xFile=string(argv[1]);//GT
	string fmt3xFileEST=string(argv[2]);
	string matchFile=string(argv[3]);
	string detailFile=string(argv[4]);
	int stimeCut=atoi(argv[5]);

	ofstream ofsDetail(detailFile.c_str());

	Fmt3x fmt3x;
	fmt3x.ReadFile(fmt3xFile);
	Fmt3x fmt3xEST;
	fmt3xEST.ReadFile(fmt3xFileEST);
// 	Trx trx;
// 	trx.ReadFile(trxFile);
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


//	int nTrNote=trx.evts.size();
	int nEstNote=match.evts.size();
	int nScoreNote=0;
	int nExtraNote=0;

	vector<ScoreNote> matchedNoteTrue;
	vector<ScoreNote> matchedNoteEst;

	vector<string> duplicates;
	for(int i=0;i<fmt3x.duplicateOnsets.size();i+=1){
		for(int j=1;j<fmt3x.duplicateOnsets[i].fmt1IDs.size();j+=1){
			duplicates.push_back(fmt3x.duplicateOnsets[i].fmt1IDs[j]);
		}//endfor j
	}//endfor i

	//Store notes in the GT score
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
			scoreNote.voiceStr=fmt3x.evts[n].notetypes[m];
			scoreNote.found=-1;
			matchedNoteTrue.push_back(scoreNote);
		}//endfor m
	}//endfor n
}//
	nScoreNote=matchedNoteTrue.size();

	//Store notes in the EST score
{
	ScoreNote scoreNote;
	for(int n=0;n<match.evts.size();n++){
		scoreNote.fmt1ID=match.evts[n].fmt1ID;
		scoreNote.trxID=match.evts[n].ID;
		scoreNote.found=-1;
		scoreNote.matchPos=n;
		scoreNote.sitch="";
		for(int i=0;i<fmt3xEST.evts.size();i+=1){
			if(fmt3xEST.evts[i].eventtype=="rest"){continue;}
			for(int j=0;j<fmt3xEST.evts[i].fmt1IDs.size();j+=1){
				if(fmt3xEST.evts[i].fmt1IDs[j]==match.evts[n].ID){
					scoreNote.onstime=fmt3xEST.evts[i].stime;
					scoreNote.offstime=fmt3xEST.evts[i].stime+fmt3xEST.evts[i].dur;
					scoreNote.sitch=fmt3xEST.evts[i].sitches[j];
					scoreNote.voiceStr=fmt3xEST.evts[i].notetypes[j];
					break;
				}//endif
			}//endfor j
			if(scoreNote.sitch!=""){break;}
		}//endfor i
		matchedNoteEst.push_back(scoreNote);
	}//endfor n

// 	for(int n=0;n<trx.evts.size();n+=1){
// 		scoreNote.onstime=trx.evts[n].onstime;
// 		scoreNote.offstime=trx.evts[n].offstime;
// 		scoreNote.sitch=trx.evts[n].sitch;
// 		scoreNote.fmt1ID="";
// 		scoreNote.voiceStr=fmt3xEST.evts[trx.evts[n].fmt3xPos1].notetypes[trx.evts[n].fmt3xPos2];
// 		scoreNote.trxID=trx.evts[n].ID;
// 		scoreNote.found=-1;
// 		scoreNote.matchPos=-1;
// 		for(int i=0;i<match.evts.size();i+=1){
// 			if(match.evts[i].ID==scoreNote.trxID){
// 				scoreNote.matchPos=i;
// 				scoreNote.fmt1ID=match.evts[i].fmt1ID;
// 				break;
// 			}//endif
// 		}//endfor i
// 		assert(scoreNote.matchPos>=0);
// 		matchedNoteEst.push_back(scoreNote);
// 	}//endfor n
}//

	// Extract extra notes (including doubly appeared notes)
	for(int i=matchedNoteEst.size()-1;i>=0;i-=1){
		if(match.evts[matchedNoteEst[i].matchPos].errorInd==2 || match.evts[matchedNoteEst[i].matchPos].errorInd==3){
ofsDetail<<"ExtraNote:\tEstID\t"<<matchedNoteEst[i].trxID<<"\tsitch\t"<<matchedNoteEst[i].sitch<<"\tonstime\t"<<matchedNoteEst[i].onstime<<"\n";
			nExtraNote+=1;
			matchedNoteEst.erase(matchedNoteEst.begin()+i);
		}//endif
	}//endfor i

	//Detect non-uniquely-aligned notes
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
ofsDetail<<"ExtraNote:\tEstID\t"<<matchedNoteEst[i].trxID<<"\tsitch\t"<<matchedNoteEst[i].sitch<<"\tonstime\t"<<matchedNoteEst[i].onstime<<"\n";
			nExtraNote+=1;
			matchedNoteEst.erase(matchedNoteEst.begin()+i);
		}//endif
	}//endfor i

	/// Extract missing notes
	int nMissNote=0;
	for(int i=matchedNoteTrue.size()-1;i>=0;i-=1){
		if(matchedNoteTrue[i].found<0){
ofsDetail<<"MissNote:\tGtID\t"<<matchedNoteTrue[i].fmt1ID<<"\tsitch\t"<<matchedNoteTrue[i].sitch<<"\tonstime\t"<<matchedNoteTrue[i].onstime<<"\n";
			nMissNote+=1;
			matchedNoteTrue.erase(matchedNoteTrue.begin()+i);
		}//endif
	}//endfor i

	if(matchedNoteTrue.size()!=matchedNoteEst.size()){
cerr<<"Error: numbers of matched notes are different in GT and EST files"<<endl;
		return -1;
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

	/// Extract correct note and pitch error
	int nPitchError = 0;
	int nCorrectNote = 0;

	for (int i = 0; i < matchedNoteEst.size(); i += 1) {
		if (match.evts[matchedNoteEst[i].matchPos].errorInd == 0) {
			nCorrectNote += 1;
		} else if (match.evts[matchedNoteEst[i].matchPos].errorInd == 1) {
			ofsDetail << "PitchError:\tGtID\t" << match.evts[matchedNoteEst[i].matchPos].fmt1ID << "\tEstID\t";
			ofsDetail << matchedNoteEst[i].trxID << "\tGtSitch\t" << matchedNoteTrue[i].sitch << "\tEstSitch\t";
			ofsDetail << matchedNoteEst[i].sitch << "\tGtOnstime\t" << matchedNoteTrue[i].onstime << "\tEstOnstime\t";
			ofsDetail << matchedNoteEst[i].onstime << "\n";
			nPitchError += 1;
		} else {
			// Never reach here
		}
	}

	if(nScoreNote!=nCorrectNote+nPitchError+nMissNote){
cerr<<"number of notes do not match"<<endl;
cerr<<"Score,Corr,PitchErr,Miss,Extra: "<<nScoreNote<<"\t"<<nCorrectNote<<"\t"<<nPitchError<<"\t"<<nMissNote<<"\t"<<nExtraNote<<endl;
		return -1;
	}//endif

	/// If #Matched notes < 2, rhythm correction cost cannot be calculated
	if(matchedNoteTrue.size()<=1){
		double scaleErr=0;
		double pitchER,missRate,extraRate,RCRate,offsetER,meanER,hmeanER;
		pitchER=double(nPitchError)/double(nScoreNote)*100;
		missRate=double(nMissNote)/double(nScoreNote)*100;
		extraRate=double(nExtraNote)/double(nEstNote)*100;
		RCRate=0;
		offsetER=0;
		meanER=(pitchER+missRate+extraRate+RCRate+offsetER)/5.;
		double voiceER=0;
		double newMeanER=(5*meanER+voiceER)/6.;
//		hmeanER=5./(1./pitchER+1./missRate+1./extraRate+1./RCRate+1./offsetER);
ofsDetail<<"# #(Matched notes) < 2, rhythm correction cost cannot be calculated"<<"\n";
ofsDetail<<"# -> onsetER, offsetER, voiceER are set to 0\n";
ofsDetail<<"PitchER,MissRate,ExtraRate,OnsetER,OffsetER,MeanER,VoiceER,NewMeanER,Pvoi,Rvoi,Fvoi,ScaleErr:\t";
ofsDetail<<pitchER<<"\t"<<missRate<<"\t"<<extraRate<<"\t"<<RCRate<<"\t"<<offsetER<<"\t"<<meanER<<"\t"<<voiceER<<"\t"<<
newMeanER<<"\t"<<0<<"\t"<<0<<"\t"<<0<<"\t"<<scaleErr<<"\n";

cout<<"PitchER,MissRate,ExtraRate,OnsetER,OffsetER,MeanER,VoiceER,NewMeanER,Pvoi,Rvoi,Fvoi,ScaleErr:\t";
cout<<pitchER<<"\t"<<missRate<<"\t"<<extraRate<<"\t"<<RCRate<<"\t"<<offsetER<<"\t"<<meanER<<"\t"<<voiceER<<"\t"<<
newMeanER<<"\t"<<0<<"\t"<<0<<"\t"<<0<<"\t"<<scaleErr<<endl;

		return 0;
	}//endif


// 	int lowerMaxPos;
// 	int higherMinPos;
	//matchedNoteEst[i].lowerMaxPos -> position of note with onset time equal to or just before the offset time of i (if there are multiple choices choose the one with pitch closest to i)
	//matchedNoteEst[i].lowerMaxPos -> position of note with onset time equal to or just after the offset time of i (if there are multiple choices choose the one with pitch closest to i)
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
	nvTrue=(matchedNoteTrue[1].onstime-matchedNoteTrue[0].onstime)*fmt3xEST.TPQN;
	nvEst=(matchedNoteEst[1].onstime-matchedNoteEst[0].onstime)*fmt3x.TPQN;
	for(int k=0;k<scales.size();k+=1){
		LP[k]+=((nvTrue*scales[k].den==nvEst*scales[k].num)? 0:1);
	}//endfor k

	double logP;
	for(int n=1;n<matchedNoteTrue.size()-1;n+=1){
		nvTrue=(matchedNoteTrue[n+1].onstime-matchedNoteTrue[n].onstime)*fmt3xEST.TPQN;
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

	for(int n=1;n<optPath.size();n++){
		if(optPath[n]!=optPath[n-1]){
ofsDetail<<"OnsetError(scale):\tGtID\t"<<matchedNoteTrue[n].fmt1ID<<"\tEstID\t"<<matchedNoteEst[n].trxID<<"\tGtSitch\t"<<matchedNoteTrue[n].sitch<<"\tEstSitch\t"<<matchedNoteEst[n].sitch<<"\tGtOnstime\t"<<matchedNoteTrue[n].onstime<<"\tEstOnstime\t"<<matchedNoteEst[n].onstime<<"\n";
		}//endif
		nvTrue=(matchedNoteTrue[n].onstime-matchedNoteTrue[n-1].onstime)*fmt3xEST.TPQN;
		nvEst=(matchedNoteEst[n].onstime-matchedNoteEst[n-1].onstime)*fmt3x.TPQN;
		if(nvTrue*scales[optPath[n-1]].den!=nvEst*scales[optPath[n-1]].num){
ofsDetail<<"OnsetError(shift):\tGtID\t"<<matchedNoteTrue[n].fmt1ID<<"\tEstID\t"<<matchedNoteEst[n].trxID<<"\tGtSitch\t"<<matchedNoteTrue[n].sitch<<"\tEstSitch\t"<<matchedNoteEst[n].sitch<<"\tGtOnstime\t"<<matchedNoteTrue[n].onstime<<"\tEstOnstime\t"<<matchedNoteEst[n].onstime<<"\n";
		}//endif
	}//endfor n

	/// Correct onset and offset stime
	vector<ScoreNote> matchedNoteEstCorrected;
	matchedNoteEstCorrected=matchedNoteEst;
	for(int n=0;n<matchedNoteTrue.size();n+=1){
		matchedNoteEstCorrected[n].onstime=matchedNoteTrue[n].onstime;

		if(matchedNoteEst[n].higherMinPos<0){//offstime is larger than the last onstime
			matchedNoteEstCorrected[n].offstime=matchedNoteEstCorrected[n].onstime+int(((matchedNoteEst[n].offstime-matchedNoteEst[n].onstime)*fmt3x.TPQN*lastScale.num)/(fmt3xEST.TPQN*lastScale.den));
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
	int nOffsetError=0;
	double countER=0;
	double scaleErr=0;
	double countSE=0;
	for(int n=0;n<matchedNoteTrue.size();n+=1){
		countER+=1;

		if(matchedNoteEstCorrected[n].offstime!=matchedNoteTrue[n].offstime){
			nOffsetError+=1;
			er+=1;

ofsDetail<<"OffsetError:\tGtID\t"<<matchedNoteTrue[n].fmt1ID<<"\tEstID\t"<<matchedNoteEst[n].trxID<<"\tGtSitch\t"<<matchedNoteTrue[n].sitch<<"\tEstSitch\t"<<matchedNoteEst[n].sitch<<"\tGtOnstime\t"<<matchedNoteTrue[n].onstime<<"\tEstOnstime\t"<<matchedNoteEst[n].onstime<<"\tGtLength\t"<<(matchedNoteTrue[n].offstime-matchedNoteTrue[n].onstime)<<"\tEstLength(rescaled)\t"<<(matchedNoteEstCorrected[n].offstime-matchedNoteTrue[n].onstime)<<"\n";

		}//endif

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
	extraRate=double(nExtraNote)/double(nEstNote)*100;
	RCRate=double(RCC)/double(matchedNoteTrue.size())*100;
	offsetER=er*100;
	meanER=(pitchER+missRate+extraRate+RCRate+offsetER)/5.;
	hmeanER=5./(1./pitchER+1./missRate+1./extraRate+1./RCRate+1./offsetER);


	/////////////// Voice metrics

	// Set voice labels
{
	vector<int> handpart;//partXML*100 + staffXML
	vector<int> voiceXML;
	for(int l=0;l<matchedNoteTrue.size();l++){
		s[0]=matchedNoteTrue[l].voiceStr;
		s[1]=s[0].substr(0,s[0].find_first_of('-'));
		v[0]=atoi(s[1].c_str());//partXML
		s[0]=s[0].substr(s[0].find_first_of('-')+1,s[0].size());
		s[1]=s[0].substr(0,s[0].find_first_of('-'));
		v[1]=atoi(s[1].c_str());//staffXML
		s[0]=s[0].substr(s[0].find_first_of('-')+1,s[0].size());
		v[2]=atoi(s[0].c_str());//voiceXML
		handpart.push_back(100*v[0]+v[1]);
		voiceXML.push_back(v[2]);
	}//endfor l
// 	vector<int> distinctHandpart;
// 	for(int l=0;l<handpart.size();l++){
// 		if(find(distinctHandpart.begin(),distinctHandpart.end(),handpart[l])==distinctHandpart.end()){
// 			distinctHandpart.push_back(handpart[l]);
// 		}//endif
// 	}//endfor l
// //	assert(distinctHandpart.size()<=2); if one wants to restrict the number of staffs to be at most 2
// 	sort(distinctHandpart.begin(),distinctHandpart.end());
// 	for(int l=0;l<matchedNoteTrue.size();l++){
// 		matchedNoteTrue[l].voice=voiceXML[l]+((handpart[l]==distinctHandpart[0])? 0:4);
// 	}//endfor l
	vector<int> distinctHandpart;
	vector<int> smallestVoiceLabel;//smallest label in each hand part
	for(int l=0;l<handpart.size();l++){
		int pos=-1;
		for(int k=0;k<distinctHandpart.size();k++){
			if(distinctHandpart[k]==handpart[l]){pos=k;break;}
		}//endfor k
		if(pos<0){
			pos=distinctHandpart.size();
			distinctHandpart.push_back(handpart[l]);
			smallestVoiceLabel.push_back(voiceXML[l]);
		}//endif
		if(voiceXML[l]<smallestVoiceLabel[pos]){smallestVoiceLabel[pos]=voiceXML[l];}
	}//endfor l
	for(int l=0;l<matchedNoteTrue.size();l++){
		int pos=-1;
		for(int k=0;k<distinctHandpart.size();k++){
			if(distinctHandpart[k]==handpart[l]){pos=k;break;}
		}//endfor k
		voiceXML[l]-=smallestVoiceLabel[pos];
	}//endfor l
//	assert(distinctHandpart.size()<=2); if one wants to restrict the number of staffs to be at most 2
	sort(distinctHandpart.begin(),distinctHandpart.end());
	for(int l=0;l<matchedNoteTrue.size();l++){
		matchedNoteTrue[l].voice=voiceXML[l]+((handpart[l]==distinctHandpart[0])? 0:4);
	}//endfor l
}//
{
	vector<int> handpart;//partXML*100 + staffXML
	vector<int> voiceXML;
	for(int l=0;l<matchedNoteEst.size();l++){
		s[0]=matchedNoteEst[l].voiceStr;
		s[1]=s[0].substr(0,s[0].find_first_of('-'));
		v[0]=atoi(s[1].c_str());//partXML
		s[0]=s[0].substr(s[0].find_first_of('-')+1,s[0].size());
		s[1]=s[0].substr(0,s[0].find_first_of('-'));
		v[1]=atoi(s[1].c_str());//staffXML
		s[0]=s[0].substr(s[0].find_first_of('-')+1,s[0].size());
		v[2]=atoi(s[0].c_str());//voiceXML
		handpart.push_back(100*v[0]+v[1]);
		voiceXML.push_back(v[2]);
	}//endfor l
// 	vector<int> distinctHandpart;
// 	for(int l=0;l<handpart.size();l++){
// 		if(find(distinctHandpart.begin(),distinctHandpart.end(),handpart[l])==distinctHandpart.end()){
// 			distinctHandpart.push_back(handpart[l]);
// 		}//endif
// 	}//endfor l
// //	assert(distinctHandpart.size()<=2); if one wants to restrict the number of staffs to be at most 2
// 	sort(distinctHandpart.begin(),distinctHandpart.end());
// 	for(int l=0;l<matchedNoteEst.size();l++){
// 		matchedNoteEst[l].voice=voiceXML[l]+((handpart[l]==distinctHandpart[0])? 0:4);
// 	}//endfor l
	vector<int> distinctHandpart;
	vector<int> smallestVoiceLabel;//smallest label in each hand part
	for(int l=0;l<handpart.size();l++){
		int pos=-1;
		for(int k=0;k<distinctHandpart.size();k++){
			if(distinctHandpart[k]==handpart[l]){pos=k;break;}
		}//endfor k
		if(pos<0){
			pos=distinctHandpart.size();
			distinctHandpart.push_back(handpart[l]);
			smallestVoiceLabel.push_back(voiceXML[l]);
		}//endif
		if(voiceXML[l]<smallestVoiceLabel[pos]){smallestVoiceLabel[pos]=voiceXML[l];}
	}//endfor l
	for(int l=0;l<matchedNoteEst.size();l++){
		int pos=-1;
		for(int k=0;k<distinctHandpart.size();k++){
			if(distinctHandpart[k]==handpart[l]){pos=k;break;}
		}//endfor k
		voiceXML[l]-=smallestVoiceLabel[pos];
	}//endfor l
//	assert(distinctHandpart.size()<=2); if one wants to restrict the number of staffs to be at most 2
	sort(distinctHandpart.begin(),distinctHandpart.end());
	for(int l=0;l<matchedNoteEst.size();l++){
		matchedNoteEst[l].voice=voiceXML[l]+((handpart[l]==distinctHandpart[0])? 0:4);
//cout<<l<<"\t"<<matchedNoteTrue[l].sitch<<"\t"<<matchedNoteTrue[l].voice<<"\t"<<matchedNoteEst[l].sitch<<"\t"<<matchedNoteEst[l].voice<<endl;
	}//endfor l
}//


	//Voice ER
	int nVoiceError=0;
	int nHandError=0;
	for(int l=0;l<matchedNoteEst.size();l++){
		if(matchedNoteEst[l].voice!=matchedNoteTrue[l].voice){
			nVoiceError+=1;
ofsDetail<<"VoiceError:\tGtID\t"<<matchedNoteTrue[l].fmt1ID<<"\tEstID\t"<<matchedNoteEst[l].trxID<<"\tGtSitch\t"<<matchedNoteTrue[l].sitch<<"\tEstSitch\t"<<matchedNoteEst[l].sitch<<"\tGtVoice\t"<<matchedNoteTrue[l].voice<<"\tEstVoice\t"<<matchedNoteEst[l].voice<<"\n";
		}//endif

		if(matchedNoteEst[l].voice/4!=matchedNoteTrue[l].voice/4){
			nHandError+=1;
ofsDetail<<"HandError:\tGtID\t"<<matchedNoteTrue[l].fmt1ID<<"\tEstID\t"<<matchedNoteEst[l].trxID<<"\tGtSitch\t"<<matchedNoteTrue[l].sitch<<"\tEstSitch\t"<<matchedNoteEst[l].sitch<<"\tGtVoice\t"<<matchedNoteTrue[l].voice<<"\tEstVoice\t"<<matchedNoteEst[l].voice<<"\n";
		}//endif

	}//endfor l
	double voiceER=100*double(nVoiceError)/double(matchedNoteEst.size());
	double handER=100*double(nHandError)/double(matchedNoteEst.size());

	//Voice F measures
	vector<string> voiceEdgesTrue,voiceEdgesEst;//noteID-noteIDの形の要素
	vector<double> degreeTrue,degreeEst;//後続のエッジの数
	vector<double> degreeFromTrue,degreeFromEst;//前続のエッジの数
{
	//streaming and chord clustering
	for(int k=0;k<8;k++){
		vector<vector<int> > chords;
		int preOnstime=-1;
		vector<int> curChord;
		for(int l=0;l<matchedNoteTrue.size();l++){
			if(matchedNoteTrue[l].voice!=k){continue;}
			if(matchedNoteTrue[l].onstime!=preOnstime){
				if(curChord.size()>0){chords.push_back(curChord);}//endif
				curChord.clear();
			}//endif
			curChord.push_back(l);
			preOnstime=matchedNoteTrue[l].onstime;
		}//endfor l
		if(curChord.size()>0){chords.push_back(curChord);}//endif
		for(int i=1;i<chords.size();i++){
			for(int j=0;j<chords[i-1].size();j++){
			for(int jp=0;jp<chords[i].size();jp++){
				ss.str(""); ss<<chords[i-1][j]<<"-"<<chords[i][jp];
				voiceEdgesTrue.push_back(ss.str());
				degreeTrue.push_back(double(chords[i].size()));
				degreeFromTrue.push_back(double(chords[i-1].size()));
			}//endfor jp
			}//endfor j
		}//endfor i
	}//endfor k
}//
{
	//streaming and chord clustering
	for(int k=0;k<8;k++){
		vector<vector<int> > chords;
		int preOnstime=-1;
		vector<int> curChord;
		for(int l=0;l<matchedNoteEst.size();l++){
			if(matchedNoteEst[l].voice!=k){continue;}
			if(matchedNoteEst[l].onstime!=preOnstime){
				if(curChord.size()>0){chords.push_back(curChord);}//endif
				curChord.clear();
			}//endif
			curChord.push_back(l);
			preOnstime=matchedNoteEst[l].onstime;
		}//endfor l
		if(curChord.size()>0){chords.push_back(curChord);}//endif
		for(int i=1;i<chords.size();i++){
			for(int j=0;j<chords[i-1].size();j++){
			for(int jp=0;jp<chords[i].size();jp++){
				ss.str(""); ss<<chords[i-1][j]<<"-"<<chords[i][jp];
				voiceEdgesEst.push_back(ss.str());
				degreeEst.push_back(double(chords[i].size()));
				degreeFromEst.push_back(double(chords[i-1].size()));
			}//endfor jp
			}//endfor j
		}//endfor i
	}//endfor k
}//

	double CTP1=0,CT1=0,CP1=0,Pvoi1,Rvoi1,Fvoi1;
	double CTP2=0,CT2=0,CP2=0,Pvoi2,Rvoi2,Fvoi2;
	double CTP3P=0,CTP3R=0,CT3=0,CP3=0,Pvoi3,Rvoi3,Fvoi3;

	for(int i=0;i<voiceEdgesTrue.size();i++){
		CT1+=1;
		CT2+=4./((degreeTrue[i]+degreeEst[i])*(degreeFromTrue[i]+degreeFromEst[i]));
		CT3+=1./degreeTrue[i];
		if(find(voiceEdgesEst.begin(),voiceEdgesEst.end(),voiceEdgesTrue[i])==voiceEdgesEst.end()){continue;}
		CTP1+=1;
		CTP2+=4./((degreeTrue[i]+degreeEst[i])*(degreeFromTrue[i]+degreeFromEst[i]));
		CTP3R+=1./degreeTrue[i];
	}//endfor i
	for(int i=0;i<voiceEdgesEst.size();i++){
		CP1+=1;
		CP2+=4./((degreeTrue[i]+degreeEst[i])*(degreeFromTrue[i]+degreeFromEst[i]));
		CP3+=1./degreeEst[i];
		if(find(voiceEdgesTrue.begin(),voiceEdgesTrue.end(),voiceEdgesEst[i])==voiceEdgesTrue.end()){continue;}
		CTP3P+=1./degreeEst[i];
	}//endfor i

	Pvoi1=CTP1/CP1; Rvoi1=CTP1/CT1; Fvoi1=(2*Pvoi1*Rvoi1)/(Pvoi1+Rvoi1);
	Pvoi2=CTP2/CP2; Rvoi2=CTP2/CT2; Fvoi2=(2*Pvoi2*Rvoi2)/(Pvoi2+Rvoi2);
	Pvoi3=CTP3P/CP3; Rvoi3=CTP3R/CT3; Fvoi3=(2*Pvoi3*Rvoi3)/(Pvoi3+Rvoi3);

//cout<<"PitchER,MissRate,ExtraRate,RCRate,OffsetER,MeanER,HMeanER,ScaleErr:\t";
//cout<<pitchER<<"\t"<<missRate<<"\t"<<extraRate<<"\t"<<RCRate<<"\t"<<offsetER<<"\t"<<meanER<<"\t"<<hmeanER<<"\t"<<scaleErr<<endl;

	double newMeanER=(5*meanER+voiceER)/6.;

ofsDetail<<"# Correspondence of matched notes (GtID vs EstID)\n";
	for(int n=0;n<matchedNoteEst.size();n++){
ofsDetail<<matchedNoteEst[n].fmt1ID<<"\t"<<matchedNoteEst[n].trxID<<"\n";
	}//endfor n
ofsDetail<<"\n";

ofsDetail<<"#notes in GT score:\t"<<nScoreNote<<"\n";
ofsDetail<<"#notes in EST score:\t"<<nEstNote<<"\n";
ofsDetail<<"nPitchError:\t"<<nPitchError<<"\n";
ofsDetail<<"nMissNote:\t"<<nMissNote<<"\n";
ofsDetail<<"nExtraNote:\t"<<nExtraNote<<"\n";
ofsDetail<<"nOnsetError(RhythmCorrectionCost):\t"<<RCC<<"\n";
ofsDetail<<"nOffsetError:\t"<<nOffsetError<<"\n";
ofsDetail<<"nVoiceError:\t"<<nVoiceError<<"\n";
ofsDetail<<"nHandError:\t"<<nHandError<<"\n";
ofsDetail<<"\n";

ofsDetail<<"PitchER(%):\t"<<pitchER<<"\n";
ofsDetail<<"MissRate(%):\t"<<missRate<<"\n";
ofsDetail<<"ExtraRate(%):\t"<<extraRate<<"\n";
ofsDetail<<"OnsetER(%):\t"<<RCRate<<"\n";
ofsDetail<<"OffsetER(%):\t"<<offsetER<<"\n";
ofsDetail<<"MeanER(%):\t"<<meanER<<"\n";
ofsDetail<<"VoiceER(%):\t"<<voiceER<<"\n";
ofsDetail<<"NewMeanER(%):\t"<<newMeanER<<"\n";
ofsDetail<<"Pvoi(%):\t"<<100*Pvoi3<<"\n";
ofsDetail<<"Rvoi(%):\t"<<100*Rvoi3<<"\n";
ofsDetail<<"Fvoi(%):\t"<<100*Fvoi3<<"\n";
ofsDetail<<"ScaleErr:\t"<<scaleErr<<"\n";
ofsDetail<<"HandER(%):\t"<<handER<<"\n";
ofsDetail<<"\n";

ofsDetail<<"PitchER,MissRate,ExtraRate,OnsetER,OffsetER,MeanER,VoiceER,NewMeanER,Pvoi,Rvoi,Fvoi,ScaleErr,HandER:\t";
ofsDetail<<pitchER<<"\t"<<missRate<<"\t"<<extraRate<<"\t"<<RCRate<<"\t"<<offsetER<<"\t"<<meanER<<"\t"<<voiceER<<"\t"<<
newMeanER<<"\t"<<100*Pvoi3<<"\t"<<100*Rvoi3<<"\t"<<100*Fvoi3<<"\t"<<scaleErr<<"\t"<<handER<<"\n";

cout<<"PitchER,MissRate,ExtraRate,OnsetER,OffsetER,MeanER,VoiceER,NewMeanER,Pvoi,Rvoi,Fvoi,ScaleErr,HandER:\t";
cout<<pitchER<<"\t"<<missRate<<"\t"<<extraRate<<"\t"<<RCRate<<"\t"<<offsetER<<"\t"<<meanER<<"\t"<<voiceER<<"\t"<<
newMeanER<<"\t"<<100*Pvoi3<<"\t"<<100*Rvoi3<<"\t"<<100*Fvoi3<<"\t"<<scaleErr<<"\t"<<handER<<endl;

	ofsDetail.close();

	return 0;
}//end main
