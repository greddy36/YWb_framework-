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

python analyse_with_mva_batch.py --inputfile root://cmsxrootd.fnal.gov//store/user/greddy/NanoAOD2018-VER2/TTToHadronic_TuneCP5_13TeV-powheg-pythia8/RunIIAutumn18NanoAODv7-Nano02Apr2020_102X_upgrade2018_realistic_v21_ext2-v1/210907_040233/0000/tree_44.root  --datayear 2018 --outputfile $1_TTToHadronic_TuneCP5_13TeV-powheg-pythia8_2018_ext3984.root --nque 1 --npro $1 


echo "xrdcp *.root root://cmseos.fnal.gov//store/user/greddy/NanoPostProc_test/2018/SAMPLE" 

xrdcp -f $1_TTToHadronic_TuneCP5_13TeV-powheg-pythia8_2018_ext3984.root root://cmseos.fnal.gov//store/user/greddy/NanoPostProc_test/2018/$1_TTToHadronic_TuneCP5_13TeV-powheg-pythia8_2018_ext3984.root 

rm *.root
rm *.py
cd ../../
rm -rf CMSSW_10_2_14

ls
echo "DONE!"
