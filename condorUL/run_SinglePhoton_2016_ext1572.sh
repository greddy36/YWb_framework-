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

python analyse_with_mva_batch.py --inputfile root://cmsxrootd.fnal.gov//store/user/greddy/NanoAOD2016-VER2/SinglePhoton/Run2016G-02Apr2020-v1/210810_034532/0000/tree_19.root  --datayear 2016 --outputfile $1_SinglePhoton_2016_ext1572.root --nque 1 --npro $1 


echo "xrdcp *.root root://cmseos.fnal.gov//store/user/greddy/NanoPostProc_test/2016/SAMPLE" 

xrdcp -f $1_SinglePhoton_2016_ext1572.root root://cmseos.fnal.gov//store/user/greddy/NanoPostProc_test/2016/$1_SinglePhoton_2016_ext1572.root 

rm *.root
rm *.py
cd ../../
rm -rf CMSSW_10_2_14

ls
echo "DONE!"