# PileupStudy
# Macros and associated files used in the 2016 data pileup study. The method used was to shift the pileup distribution at the 
# DIGI_HLT_PU step for one sample, generate samples with the standard and the shifted distribution, and then compare the 
# epsilon/alpha for the two samples. The alternative method was to generate a sample with the standard PU distribution and then
# reweight the final sample. The macro used for that has been included here for completeness. 


Used for AN-16-455:
Helpers.h
OnlyCutflow_Production.C  
OnlyCutflow_MorePU.C 
mix_2016_25ns_Moriond17MC_PoissonOOTPU_MorePU_cfi.py
mix_2016_25ns_Moriond17MC_PoissonOOTPU_cfi.py

Alternative method:
cutflow_with_weights.C

Common to both:         
file_list_mN1_10_mGammaD_0p4_13TeV_cT_0_events80k_FNALLPC_VertexInfo.txt
