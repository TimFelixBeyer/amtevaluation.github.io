//g++ -O2 -I/Users/eita/Dropbox/Research/Tool/All/ RealignmentMOHMM_v170304.cpp -o RealignmentMOHMM
#include<iostream>
#include<string>
#include<sstream>
#include<cmath>
#include<vector>
#include<fstream>
#include<cassert>
#include "RealignmentMOHMM_v170503.hpp"
using namespace std;

#define EPS 0.001;

class TempoTracker{
public:
	vector<int> refStimes;
	vector<double> refTimes;

	void SetData(vector<int> obsStimes, vector<double> obsTimes){
		assert(obsStimes.size()==obsTimes.size());
		refStimes.clear();
		refTimes.clear();

		for(int i=obsStimes.size()-1;i>=1;i-=1){
			if(obsStimes[i]<obsStimes[i-1]){
				obsStimes.erase(obsStimes.begin()+i);
				obsTimes.erase(obsTimes.begin()+i);
			}//endif
		}//endfor i

		int curStime;
		vector<double> tmpTimes;
		curStime=obsStimes[0];
		for(int i=0;i<obsStimes.size();i+=1){
			if(obsStimes[i]!=curStime){
				refStimes.push_back(curStime);
				refTimes.push_back(Average(tmpTimes));
				curStime=obsStimes[i];
				tmpTimes.clear();
			}//endif
			tmpTimes.push_back(obsTimes[i]);
		}//endfor i
		refStimes.push_back(curStime);
		refTimes.push_back(Average(tmpTimes));

	}//end SetData

	double GetTime(int stime){
		int sup=-1;
		for(int i=0;i<refStimes.size();i+=1){
			if(refStimes[i]>stime){
				sup=i;
				break;
			}//endif
		}//endfor i

		if(sup<0){//stime is above or equal to max
			if(stime==refStimes[refStimes.size()-1]){
				return refTimes[refStimes.size()-1];
			}else{
				return refTimes[refStimes.size()-1]+(stime-refStimes[refStimes.size()-1])*(refTimes[refStimes.size()-1]-refTimes[refStimes.size()-2])/double(refStimes[refStimes.size()-1]-refStimes[refStimes.size()-2]);
			}//endif
		}else if(sup==0){//stime is below min
				return refTimes[0]+(stime-refStimes[0])*(refTimes[1]-refTimes[0])/double(refStimes[1]-refStimes[0]);
		}else{
			if(stime==refStimes[sup-1]){
				return refTimes[sup-1];
			}else{
				return refTimes[sup-1]+(stime-refStimes[sup-1])*(refTimes[sup]-refTimes[sup-1])/double(refStimes[sup]-refStimes[sup-1]);
			}//endif
		}//endif

	}//end GetTime

	void Print(){
cout<<"################"<<endl;
		for(int i=0;i<refStimes.size();i+=1){
			cout<<i<<"\t"<<refStimes[i]<<"\t"<<refTimes[i]<<endl;
		}//endfor i
cout<<"################"<<endl;
	}//end Print

};//endclass TempoTracker

class Regions{
public:
	vector<vector<double> > regions;//Non-overlapping and ordered intervals [ regions[k][0] , regions[k][1] )

	void Add(double t1,double t2){
		if(t1>=t2){
			cout<<"Invalid region: "<<t1<<"\t"<<t2<<endl;
			assert(false);
		}//endif
		int startPos=-1,endPos=-1;
		for(int i=0;i<regions.size();i+=1){
			if(t1<regions[i][1] && t1>=regions[i][0]){
				startPos=i;
			}//endif
			if(t2<=regions[i][1] && t2>regions[i][0]){
				endPos=i;
			}//endif
		}//endfor i
//cout<<startPos<<"\t"<<endPos<<endl;

		vector<double> region(2);

		if(startPos==-1 && endPos==-1){//No overlaps
			region[0]=t1; region[1]=t2;
			for(int i=regions.size()-1;i>=0;i-=1){
				if(regions[i][0]>=region[1] || regions[i][1]<=region[0]){continue;}//endif
				regions.erase(regions.begin()+i);
			}//endfor i
		}else if(startPos==-1 && endPos!=-1){
			region[0]=t1; region[1]=regions[endPos][1];
			for(int i=endPos;i>=0;i-=1){
				if(regions[i][1]<region[0]){break;}//endif
				regions.erase(regions.begin()+i);
			}//endfor i
		}else if(startPos!=-1 && endPos==-1){
			region[0]=regions[startPos][0]; region[1]=t2;
			for(int i=regions.size()-1;i>=startPos;i-=1){
				if(regions[i][0]>region[1]){continue;}//endif
				regions.erase(regions.begin()+i);
			}//endfor i
		}else{//if startPos!=-1 && endPos!=-1
			region[0]=regions[startPos][0]; region[1]=regions[endPos][1];
			for(int i=endPos;i>=startPos;i-=1){
				regions.erase(regions.begin()+i);
			}//endfor i
		}//endif

		int insertPos=0;
		for(int i=0;i<regions.size();i+=1){
			if(region[0]>regions[i][0]){insertPos=i+1;}
		}//endfor i
		regions.insert(regions.begin()+insertPos,region);

	}//end AddRegion

	void Print(){
		for(int i=0;i<regions.size();i+=1){
cout<<"["<<regions[i][0]<<","<<regions[i][1]<<")"<<endl;
		}//endfor i
	}//end Print

	bool IsOverlapping(vector<double>& region){
		assert(region.size()==2);
		bool isOverlapping=false;
		for(int i=0;i<regions.size();i+=1){
			if(region[0]>=regions[i][0] && region[0]<regions[i][1]){isOverlapping=true;break;}
			if(region[1]>regions[i][0] && region[1]<=regions[i][1]){isOverlapping=true;break;}
			if(regions[i][0]>=region[0] && regions[i][0]< region[1]){isOverlapping=true;break;}
			if(regions[i][1]> region[0] && regions[i][1]<=region[1]){isOverlapping=true;break;}
		}//endfor i
		return isOverlapping;
	}//end IsOverlapping

	bool IsContained(double &value){
		bool isContained=false;
		for(int i=0;i<regions.size();i+=1){
			if(value>=regions[i][0] && value<regions[i][1]){isContained=true;break;}
		}//endfor i
		return isContained;
	}//end IsContained

	int ContainedRegion(double &value){
		int regionID=-1;
		for(int i=0;i<regions.size();i+=1){
			if(value>=regions[i][0] && value<regions[i][1]){regionID=i;break;}
		}//endfor i
		return regionID;
	}//end

};//end Regions


int main(int argc,char** argv){

	vector<int> v(100);
	vector<double> d(100);
	vector<string> s(100);
	stringstream ss;
	if(argc!=6){cout<<"Error in usage! : $./this in_fmt3x.txt in_hmm.txt in_err_match.txt out_realigned_match.txt widthSec"<<endl; return -1;}

	string fmt3xFile=string(argv[1]);
	string hmmFile=string(argv[2]);
	string inMatchFile=string(argv[3]);
	string outMatchFile=string(argv[4]);
	double widthSec=atof(argv[5]);

	Fmt3x fmt3x;
	fmt3x.ReadFile(fmt3xFile);
	ScorePerfmMatch match;
	match.ReadFile(inMatchFile);
	Hmm hmm;
	hmm.ReadFile(hmmFile);
	hmm.ResetInternalPosition();

	/// Normalise duplicate note labels
	for(int n=0;n<match.evts.size();n+=1){
		for(int k=0;k<hmm.duplicateOnsets.size();k+=1){
			for(int l=1;l<hmm.duplicateOnsets[k].numOnsets;l+=1){
				if(match.evts[n].fmt1ID==hmm.duplicateOnsets[k].fmt1IDs[l]){
					match.evts[n].fmt1ID=hmm.duplicateOnsets[k].fmt1IDs[0];
				}//endif
			}//endfor l
		}//endfor k
	}//endfor n

	/// Pick up performance errors
	vector<int> pitchErrPos;
	vector<int> extraNotePos;
	vector<int> reorderdNotePos;
	vector<vector<int> > missNoteIDs;

	int preStime=-1;
	vector<int> stimes;
	vector<double> times;
	for(int n=0;n<match.evts.size();n+=1){

		if(match.evts[n].errorInd==1){
			pitchErrPos.push_back(n);
		}//endif

		if(match.evts[n].errorInd==2 || match.evts[n].errorInd==3){
			extraNotePos.push_back(n);
		}//endif

		if(match.evts[n].errorInd!=2 && match.evts[n].errorInd!=3){
			stimes.push_back(match.evts[n].stime);
			times.push_back(match.evts[n].ontime);

			if(match.evts[n].stime<preStime){
				reorderdNotePos.push_back(n);
			}//endif
			preStime=match.evts[n].stime;
		}//endif

	}//endfor n

	for(int i=0;i<match.missingNotes.size();i+=1){
		missNoteIDs.push_back( fmt3x.FindFmt3xScorePos(match.missingNotes[i].fmt1ID) );
	}//endfor i

	//Output the original match file if there are too few reference notes
	if(stimes.size()<=1){
		match.WriteFile(outMatchFile);
		return 0;
	}//endif

	TempoTracker tempoTracker;
	tempoTracker.SetData(stimes,times);

	/// Get error regions
	Regions errRegions;
	Regions pitchErrRegions;
	Regions extraNoteRegions;
	Regions reorderdNoteRegions;
	Regions missNoteRegions;

	for(int k=0;k<pitchErrPos.size();k+=1){
		pitchErrRegions.Add(match.evts[pitchErrPos[k]].ontime-widthSec,match.evts[pitchErrPos[k]].ontime+widthSec);
		errRegions.Add(match.evts[pitchErrPos[k]].ontime-widthSec,match.evts[pitchErrPos[k]].ontime+widthSec);
	}//endfor k

	for(int k=0;k<extraNotePos.size();k+=1){
		extraNoteRegions.Add(match.evts[extraNotePos[k]].ontime-widthSec,match.evts[extraNotePos[k]].ontime+widthSec);
		errRegions.Add(match.evts[extraNotePos[k]].ontime-widthSec,match.evts[extraNotePos[k]].ontime+widthSec);
	}//endfor k

	for(int k=0;k<reorderdNotePos.size();k+=1){
		reorderdNoteRegions.Add(match.evts[reorderdNotePos[k]].ontime-widthSec,match.evts[reorderdNotePos[k]].ontime+widthSec);
		errRegions.Add(match.evts[reorderdNotePos[k]].ontime-widthSec,match.evts[reorderdNotePos[k]].ontime+widthSec);
	}//endfor k

	for(int k=0;k<missNoteIDs.size();k+=1){
		double evtTime=tempoTracker.GetTime(fmt3x.evts[missNoteIDs[k][0]].stime);
		missNoteRegions.Add(evtTime-widthSec,evtTime+widthSec);
		errRegions.Add(evtTime-widthSec,evtTime+widthSec);
	}//endfor k

	MOHMM mohmm;
	mohmm.SetScorePerfmMatch(match);
	mohmm.SetHmm(hmm);

	for(int i=0;i<errRegions.regions.size();i+=1){

		///Select regions
		bool includePitchErr=false;
		bool includeExtraNote=false;
		bool includeMissNote=false;
		bool includeReorderedNote=false;
		includePitchErr=pitchErrRegions.IsOverlapping(errRegions.regions[i]);
		includeExtraNote=extraNoteRegions.IsOverlapping(errRegions.regions[i]);
		includeMissNote=missNoteRegions.IsOverlapping(errRegions.regions[i]);
		includeReorderedNote=reorderdNoteRegions.IsOverlapping(errRegions.regions[i]);

		int maxStime=fmt3x.evts[0].stime;
		int minStime=fmt3x.evts[fmt3x.evts.size()-1].stime;
		double maxTime;
		double minTime=errRegions.regions[i][1];
		for(int n=0;n<match.evts.size();n+=1){
			if(match.evts[n].ontime<errRegions.regions[i][0]){continue;}
			if(match.evts[n].ontime>=errRegions.regions[i][1]){break;}
			if(match.evts[n].errorInd<2 && match.evts[n].stime>maxStime){maxStime=match.evts[n].stime;}
			if(match.evts[n].errorInd<2 && match.evts[n].stime<minStime){minStime=match.evts[n].stime;}
			if(match.evts[n].ontime<minTime){minTime=match.evts[n].ontime;}
			maxTime=match.evts[n].ontime;
		}//endfor n
		if(!( (includeExtraNote&&includeMissNote) || (includeMissNote&&includePitchErr) || (includeExtraNote&&includePitchErr) )){continue;}

		if(minTime>=maxTime){continue;}
		if(minStime>=maxStime){continue;}
		mohmm.Realign(minStime,maxStime,minTime,maxTime);
	}//endfor i

	match=mohmm.match;

	for(int n=0;n<match.evts.size();n+=1){
		if(match.evts[n].errorInd>1){
			match.evts[n].matchStatus=-1;
		}//endif
	}//endfor n

	/// Set matchStatus for ornamental notes
	for(int i=0;i<fmt3x.evts.size();i+=1){
		if(fmt3x.evts[i].eventtype!="chord"){continue;}
		for(int j=0;j<fmt3x.evts[i].notetypes.size();j+=1){
			string notetype=fmt3x.evts[i].notetypes[j].substr(0,fmt3x.evts[i].notetypes[j].find("."));
			if(notetype!="N"){continue;}
			for(int n=0;n<match.evts.size();n+=1){
				if(match.evts[n].fmt1ID==fmt3x.evts[i].fmt1IDs[j]){
					match.evts[n].matchStatus=0;
				}//endif
			}//endfor n
		}//endfor j
	}//endfor i

	match.WriteFile(outMatchFile);
	return 0;
}//end main
