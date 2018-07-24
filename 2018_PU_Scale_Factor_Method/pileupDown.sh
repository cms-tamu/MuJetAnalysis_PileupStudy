#!/bin/bash
JSON="/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/ReReco//Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt"
PUJSON="/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/PileUp/pileup_latest.txt"

pileupCalc.py -i $JSON --inputLumiJSON $PUJSON  --calcMode true --minBiasXsec 65740 --maxPileupBin 100 --numPileupBins 100  --pileupHistName=PUpileupDown pileupDown.root
