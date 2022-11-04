#!/bin/bash                                                                                                                                                                                                                           
source /cvmfs/cms.cern.ch/cmsset_default.sh
export SCRAM_ARCH=slc6_amd64_gcc700
eval `scramv1 project CMSSW CMSSW_10_2_14`
cd CMSSW_10_2_14/src/
eval `scram runtime -sh`
echo "CMSSW: "$CMSSW_BASE

cd ${_CONDOR_SCRATCH_DIR}
echo ${_CONDOR_SCRATCH_DIR}
#let "sample=${1}+1"
echo "xrdcp root://cmseos.fnal.gov//store/user/greddy/condorAtanu/analyse_with_mva_batch.py"
xrdcp root://cmseos.fnal.gov//store/user/greddy/condorAtanu/analyse_with_mva_batch.py .
xrdcp root://cmseos.fnal.gov//store/user/greddy/condorAtanu/EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root .
xrdcp root://cmseos.fnal.gov//store/user/greddy/condorAtanu/EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root .
xrdcp root://cmseos.fnal.gov//store/user/greddy/condorAtanu/TMVAClassification_BDT.weights.xml .
xrdcp root://cmseos.fnal.gov//store/user/greddy/condorAtanu/ElectronTrigSF2016.root .
xrdcp root://cmseos.fnal.gov//store/user/greddy/condorAtanu/ElectronTrigSF2017.root .
xrdcp root://cmseos.fnal.gov//store/user/greddy/condorAtanu/ElectronTrigSF2018.root .
xrdcp root://cmseos.fnal.gov//store/user/greddy/condorAtanu/bTagEff_2016.root .
xrdcp root://cmseos.fnal.gov//store/user/greddy/condorAtanu/bTagEff_2017.root .
xrdcp root://cmseos.fnal.gov//store/user/greddy/condorAtanu/bTagEff_2018.root .
echo "run python analyzer"

python analyse_with_mva_batch.py --inputfile root://cmsxrootd.fnal.gov//store/user/greddy/NanoAOD2017-VER2/QCD_HT300to500_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17NanoAODv7-PU2017_12Apr2018_Nano02Apr2020_new_pmx_102X_mc2017_realistic_v8-v1/210809_071505/0000/tree_1.root  --datayear 2017 --outputfile $1_QCD_HT300to500_2017_ext213.root --nque 1 --npro $1 


echo "xrdcp *.root root://cmseos.fnal.gov//store/user/greddy/NanoPostProc_test/2017/SAMPLE" 

xrdcp -f $1_QCD_HT300to500_2017_ext213.root root://cmseos.fnal.gov//store/user/greddy/NanoPostProc_test/2017/$1_QCD_HT300to500_2017_ext213.root 

rm *.root
rm *.py
cd ../../
rm -rf CMSSW_10_2_14

ls
echo "DONE!"
