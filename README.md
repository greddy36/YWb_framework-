# YWb_framework-
Code and framework to produce NANOAOD samples from scratch and also the input files for Higgs Combine and Keras analysis.

STEPS TO COMPILE CRAB AREA  

	Copy CMSSW_10_2_14 to lxplus
	>cd CMSSW_10_2_14/src
	>cmsenv
	>source /cvmfs/cms.cern.ch/crab3/crab.sh
 	>source /cvmfs/cms.cern.ch/common/crab-setup.sh #(if on cmslpc)
	>voms-proxy-init --voms cms --valid 192:00
	>scram b vclean
	>scram b -j8
	This will complete the area compilation
	Now test the area. 
	First interactively,
	>cd $CMSSW_BASE/src/PhysicsTools/NanoAODTools/python/postprocessing/vlqAna/
	>python postproc.py --isMC 0 --passall 0 --dataYear 2016 --maxEvents 100 -redojec 0
	Try it for 2016, 2017, 2018
	Then via crab,
	>cd $CMSSW_BASE/src/PhysicsTools/NanoAODTools/crab
	(At this point open and edit crab_cfg.py file to change the user name from ‘amodak’ to yours )
	>python crab_cfg.py --isMC 1 --dataYear 2016 --tag VER --redojec 0 --run dryrun
	Make sure you provide a mcsamples_2016.txt file (keep only one sample for the dry run test)
	If this works then then everything is set.

STEPS TO MAKE SKIMS FROM CRAB OUTPUT ON CONDOR

	Please follow these set of instructions to run the condor jobs, 
	1. The code to run the local analysis is in "analyses_with_mva_batch.py
	You can interactive run the code like this, 
	>python analyse_with_mva_batch.py --inputfile root://cmsxrootd.fnal.gov//store/user/amodak/NanoAOD2018-718/WJetsToLNu_Pt-100To250_TuneCP5_13TeV-amcatnloFXFX-pythia8/RunIIAutumn18NanoAODv7-Nano02Apr2020_102X_upgrade2018_realistic_v21-v1/200818_075656/0000/tree_11.root --datayear 2018 --outputfile test.root --nque 1 --npro 0
	Just make sure you are using the intended input file and data year choices for the input arguments. 
	2. To run over condor you would need the following setup, 
	Scripts are here, /uscms_data/d3/amodak/NanoAOD/NanoPostProc/Condor
	You need to copy, runDummy.py and batchDummy.py files
	These files would help you to create the jdl and sh files for submitting the condor jobs. Open those files and edit wherever needed. 
	You need to create one folder pin your eos area like this, 
	/store/user/amodak/toConndor
	And the analysis code needs to be copied there (actually you need to copy here all the things that are needed for the analysis to be run, otherwise your carb job won’t find it)
	For eg, you can copy the analysis code like this, 
	>xrdcp -f analyse_with_mva_batch.py root://cmseos.fnal.gov//store/user/amodak/toConndor/analyse_with_mva_batch.py
	You would also need 3 txt files with the location of the input nanoaod files that you got from the CRAB running. You can look at my files in the same area and they are named as the following, skimfiles_2016.txt, skimfiles_2017.txt, skimfiles_2018.txt
	3. Then you need to create the jobs files for condor by this,
	>python runDummy.py
	>python runBatch.py
	Then to submit a condor job you need, 
	>condor_submit FILE.jdl 
	To submit jobs in bulk see the submit file in the same area and you need to do this, 
	>sh submit.csh

MAKING FITTING HISTOGRAMS FOR HIGGS COMBINE.

	So far, the Higgs Combine part has been like a blackbox to me because Andrew ran it and he gave me the template files to make the input histograms for Higgs combine.
	I use the new_lognaormal_6chan_5par.C to make the fits on B1M, B1T and B2M channels of MC backgorunds and then use the fit function to make histograms for 6channel file that has each channel seperated into plus and minu.s 
	
