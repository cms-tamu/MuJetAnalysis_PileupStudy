# Instructions

## Create CMSSW Area
The pileup study requires access to the amount of pileup in each event (from PileupSummaryInfo); this is not included in our standard MC generation for HIG-18-003 (AN-16-455). Checkout the PileupStudy branch to remedy this. 
~~~~
export SCRAM_ARCH=slc6_amd64_gcc493
source /cvmfs/cms.cern.ch/cmsset_default.sh 
source /cvmfs/cms.cern.ch/crab3/crab.sh
cmsrel CMSSW_8_0_20
cd CMSSW_8_0_20/src/
cmsenv
git cms-init
git cms-merge-topic --unsafe cms-tamu:from-CMSSW_8_0_20-tamu-muonjet-physics-analysis
git cms-addpkg DataFormats/TrackReco
git cms-addpkg GeneratorInterface/GenFilters
git submodule add git@github.com:cms-tamu/MuJetAnalysis.git
cd MuJetAnalysis
git checkout -b for-CMSSW-80X-NoPHR-RAWAODSIM-PileupStudy origin/for-CMSSW-80X-NoPHR-RAWAODSIM-PileupStudy
cd ../
scram b
~~~~

## Generate Signal MC
Follow steps for generating signal MC listed in the twiki using the above CMSSW area for the PAT ANA step.

## Modify Cutflow Table Producer
1. Generate nominal, +5%, and -5% data distribution with pileupCalc.py
  * Settings for 2016 at https://twiki.cern.ch/twiki/bin/view/CMS/PileupJSONFileforData#Pileup_JSON_Files_For_Run_II
  * .sh files have been included in this repo; command lines are in "inst.txt" files. 
  * pileupCalc.py generates histogram in root file, use hist->Print(“all”) 
2. Take MC disribution from mixing file (mix_2016_25ns_Moriond17MC_PoissonOOTPU_cfi for HIG-18-003)
3. Match the domains of the data and MC distributions, normalize each distribution
  * Justification: the tails extend far, far past the peak in all distributions
  * May need to change for high luminosity in future
4. Divide up, down, and nominal PU distributions each by MC distribution to find “r” for each PU value. Record each as look up table
  * Slightly different from prescription in email
  * Before: divide then normalize
  * Now: normalize then divide (Previous procedure may have been when “MC” range is extremely short)
5. Calculate alpha (cross-check on step 4)
  * ![alt text](https://github.com/cms-tamu/PileupStudy/blob/master/2018_PU_Scale_Factor_Method/Equation.png "Equation for Alpha")
  * Where "pu" isthe number of pileup events, "MC_{truth}" is taken from the mixing file, and "r" was calculated in step 4
  * Not strictly needed since normalization has already occurred
6. Edit cutflow table producers to include LUT for each of the three r as arrays
7. Use LUTs to reweight each event according to their PU value
8. Run cutflow table producers
9. Compare epislon/alpha outputs to estimate PU uncertainty 
