#! /bin/bash

ProgramFolder="./Programs"
CodeFolder="./Code"

mkdir $ProgramFolder

g++ -O2 $CodeFolder/Fmt3xToSpr_v220118.cpp -o $ProgramFolder/Fmt3xToSpr
g++ -O2 $CodeFolder/ScoreMatchEvaluation_VoicePlus_v220118.cpp -o $ProgramFolder/ScoreMatchEvaluation_VoicePlus

g++ $CodeFolder/midi2pianoroll_v170504.cpp -o $ProgramFolder/midi2pianoroll
g++ -O2 $CodeFolder/MusicXMLToFmt3x_v170104.cpp -o $ProgramFolder/MusicXMLToFmt3x
g++ -O2 $CodeFolder/MusicXMLToHMM_v170104.cpp -o $ProgramFolder/MusicXMLToHMM
g++ -O2 $CodeFolder/ScorePerfmMatcher_v170503.cpp -o $ProgramFolder/ScorePerfmMatcher
g++ -O2 $CodeFolder/ErrorDetection_v170503.cpp -o $ProgramFolder/ErrorDetection
g++ -O2 $CodeFolder/RealignmentMOHMM_v190326.cpp -o $ProgramFolder/RealignmentMOHMM
g++ -O2 $CodeFolder/ExtractOrnaments_v170503.cpp -o $ProgramFolder/ExtractOrnaments
g++ -O2 $CodeFolder/MergeOldHandMatchAndv170503.cpp -o $ProgramFolder/MergeOldHandMatchAndv170503
g++ -O2 $CodeFolder/TransposeSpr_v200708.cpp -o $ProgramFolder/TransposeSpr

g++ -O2 $CodeFolder/MusicXMLToQpr_v191003.cpp -o $ProgramFolder/MusicXMLToQpr
g++ -O2 $CodeFolder/GetCorresp_v220117.cpp -o $ProgramFolder/GetCorresp

g++ -O2 $CodeFolder/ScoreMatchEvaluation_v190326.cpp -o $ProgramFolder/ScoreMatchEvaluation
g++ -O2 $CodeFolder/TakeStatistics.cpp -o $ProgramFolder/TakeStatistics

