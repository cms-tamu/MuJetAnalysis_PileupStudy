# Placeholder
1. Generate nominal, +5%, and -5% data distribution with pileupCalc.py
  * Settings for 2016 at https://twiki.cern.ch/twiki/bin/view/CMS/PileupJSONFileforData#Pileup_JSON_Files_For_Run_II
  * .sh files have been included in this repo; command lines are in "inst.txt" files. 
  * pileupCalc.py generates histogram in root file, use hist->Print(“all”) 
2. Taked MC disribution from mixing file mix_2016_25ns_Moriond17MC_PoissonOOTPU_cfi
3. Match the domains of the two distributions, normalize
  * Justification: the tails extend far, far past the peak in all distributions
  * May need to change for high luminosity in future
4. Divide data by MC to find “r”
  * Slightly different from prescription in email
  * Before: divide then normalize
  * Now: normalize then divide (Previous procedure may have been when “MC” range is extremely short)
5. Calculate alpha
  * ![alt text](https://github.com/cms-tamu/PileupStudy/blob/master/2018_PU_Scale_Factor_Method/Equation.png "Equation for Alpha")
  * Where "pu" isthe number of pileup events, "MC_{truth}" is taken from the mixing file, and "r" was calculated in step 4
  * Not strictly needed since normalization has already occurred
  * Serves as cross-check on step 4
6. Use "r" to to calculate reweighted efficiency for up, down, and nominal
