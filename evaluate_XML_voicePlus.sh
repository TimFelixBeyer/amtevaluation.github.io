#! /bin/bash

if [ $# -ne 3 ]; then
echo "Error in usage: $./evaluate.sh true(.xml) est(.xml) ER(.txt)"
exit 1
fi

I1=$1
I2=$2
I3=$3

PFol="./Programs"

$PFol/MusicXMLToFmt3x ${I2}.xml ${I2}_fmt3x.txt
$PFol/Fmt3xToSpr ${I2}_fmt3x.txt ${I2}_spr.txt

$PFol/MusicXMLToHMM ${I1}.xml ${I1}_hmm.txt
$PFol/MusicXMLToFmt3x ${I1}.xml ${I1}_fmt3x.txt

$PFol/ScorePerfmMatcher ${I1}_hmm.txt ${I2}_spr.txt ${I2}_pre_match.txt 0.01
$PFol/ErrorDetection ${I1}_fmt3x.txt ${I1}_hmm.txt ${I2}_pre_match.txt ${I2}_err_match.txt
$PFol/RealignmentMOHMM ${I1}_fmt3x.txt ${I1}_hmm.txt ${I2}_err_match.txt ${I2}_auto_match.txt 0.3

$PFol/ScoreMatchEvaluation_VoicePlus ${I1}_fmt3x.txt ${I2}_fmt3x.txt ${I2}_auto_match.txt ${I2}_err_detail.txt -1 > ${I3}.txt

#$PFol/MusicXMLToQpr 0 ${I1}.xml ${I1}_post_qpr.txt
#$PFol/MusicXMLToQpr 0 ${I2}.xml ${I2}_post_qpr.txt

rm ${I2}_spr.txt
rm ${I1}_hmm.txt
rm ${I2}_pre_match.txt
rm ${I2}_err_match.txt




