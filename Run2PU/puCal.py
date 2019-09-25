#!/usr/bin/env python
import os
if os.getenv("CMSSW_BASE") == None:
  print "no CMSSW env"
  exit()
if not  os.getenv("HOSTNAME").startswith("lxplus"):
  print "run on lxplus"
  exit()

from puCalInPut import *
upXsec = nomXsec*(1.0+uncertainty)
dnXsec = nomXsec*(1.0-uncertainty)

nomH1 = "puNom"
upH1 = "puUp"
dnH1 = "puDn"

tmpSh = """#!/bin/bash
JSON={json}
echo $JSON >> {log}
PUJSON={pujson}
echo $PUJSON >> {log}
echo "nominal {nomXsec}"  >> {log}
echo "dn {dnXsec}"  >> {log}
echo "up {upXsec}"  >> {log}
echo "nominal log"  >> {log}
pileupCalc.py -i $JSON --inputLumiJSON $PUJSON --calcMode true --minBiasXsec {nomXsec} --maxPileupBin {maxBin} --numPileupBins {nBins} --pileupHistName={nomH1} {nomROOT}| tee >> {log}
echo "dn log"  >> {log}
pileupCalc.py -i $JSON --inputLumiJSON $PUJSON --calcMode true --minBiasXsec {dnXsec} --maxPileupBin {maxBin} --numPileupBins {nBins} --pileupHistName={dnH1} {dnROOT}| tee >> {log}
echo "up log"  >> {log}
pileupCalc.py -i $JSON --inputLumiJSON $PUJSON --calcMode true --minBiasXsec {upXsec} --maxPileupBin {maxBin} --numPileupBins {nBins} --pileupHistName={upH1} {upROOT}| tee >> {log}
echo "output ROOT file: {totROOT}">> {log}
hadd {totROOT} {nomROOT} {upROOT} {dnROOT}
sha1sum {totROOT} | tee >> {log}
rm {nomROOT} {upROOT} {dnROOT}""" 

def calPU(year): 
  yList = [16,17,18]
  mcMap = {16:nMC16, 17:nMC17, 18:nMC18} 
  jsonMap = {16:JSON16, 17:JSON17, 18:JSON18}
  pujsonMap = {16:PUJSON16, 17:PUJSON17, 18:PUJSON18}
  if year not in yList: 
    print "wrong year input"
    return
  print "start pu{} cal.".format(year) 
  rfName = "pu{year}_{nBins}.root".format(**{'year':year,'nBins':nBins})
  logName = rfName.replace(".root",".log") 
  if rfName not in os.listdir("."):
    print "{} to be generated.".format(rfName)
    os.system(tmpSh.format(**{'json':jsonMap[year], 'pujson':pujsonMap[year], 'nomXsec':nomXsec, 'upXsec':upXsec, 'dnXsec':dnXsec, 'nBins':nBins, 'maxBin':maxBin, 'nomH1':nomH1, 'upH1':upH1, 'dnH1':dnH1, 'nomROOT':"pu{}nom.root".format(year), 'upROOT':"pu{}up.root".format(year), 'dnROOT':"pu{}dn.root".format(year), 'totROOT':rfName, 'log':logName}))
  else:
    try: 
      logF = open(logName)
      if not logF.readline().startswith(jsonMap[year]): print "WARNING! JSON is not matched with log"
      if not logF.readline().startswith(pujsonMap[year]): print "WARNING! PU JSON  is not matched with log"
      if float(logF.readline().split()[1]) != nomXsec: print "WARNING! nomXsec is not matched with log"
      if float(logF.readline().split()[1]) != dnXsec: print "WARNING! dnXsec is not matched with log"
      if float(logF.readline().split()[1]) != upXsec: print "WARNING! upXsec is not matched with log"
      if logF.readlines()[-1] != os.popen("sha1sum "+rfName).read(): print "WARNING! root file hash is not matched with log"
    except: print "WARNING! no log file, can't check log"
  c = TCanvas("","",800,500)
  color = {'puNom':kBlack, 'puUp':kRed-4, 'puDn':kBlue-4, 'mc':kViolet-4}
  rf = TFile(rfName)
  mc = TH1D("mc","mc",nBins,0,maxBin)
  if len(mcMap[year]) < nBins: mcMap[year].extend([0.0]*(nBins-len(mcMap[year])))
  for x in range(nBins):
    mc.SetBinContent(x+1,mcMap[year][x])
  hList = [mc, rf.Get(nomH1), rf.Get(upH1), rf.Get(dnH1)]
  for x in hList:
    x.SetLineWidth(2)
    x.SetLineColor(color[x.GetName()])
    x.SetXTitle("N_{TrueInteractions}")
    if x.Integral() > 1.1: x.Scale(1./x.Integral())
    x.Draw("HIST")
    c.SaveAs("nom{}_".format(year)+x.GetName()+".png")

  maxY = max([x.GetMaximum() for x in hList])*1.1
  hList[0].SetMaximum(maxY)
  hList[0].Draw("HIST")
  le = TLegend(0.5,0.7,0.8,0.9)
  le.SetTextSize(0.035)
  le.SetFillStyle(0)
  le.SetBorderSize(0)
  for x in hList:
    x.Draw("HIST same")
    le.AddEntry(x, x.GetName())

  le.Draw()
  rwH = []
  c.SaveAs("tot{}.png".format(year))
  for x in hList[1:]:
    tmp = x.Clone("reWeight_"+x.GetName())
    tmp.Divide(hList[0])
    rwH.append(tmp)
    tmp.SetYTitle("puileup re-weight")
    tmp.Draw("HIST")
    c.SaveAs("rw{}_".format(year)+x.GetName()+".png")
    tmp.GetXaxis().SetRange(1,51)
    tmp.Draw("HIST")
    c.SaveAs("rw{}_".format(year)+x.GetName()+"_range.png")
  rfo = TFile("pu{}reWeight.root".format(year),"RECREATE") 
  for h in hList:
    h.Write()
  for h in rwH:
    h.Write()
  rfo.Write()
  rfo.Close()
  nom = [rwH[0].GetBinContent(x+1) for x in range(nBins)]
  up = [rwH[1].GetBinContent(x+1) for x in range(nBins)]
  dn = [rwH[2].GetBinContent(x+1) for x in range(nBins)]
  outF = file("pu{}reWeigt.py".format(year),"w")

  outF.write("nom = [")
  for x in nom: 
    outF.write("{:.8e}".format(x))
  outF.write("]\n")
  outF.write("dn = [")
  for x in dn: 
    outF.write("{:.8e}".format(x))
  outF.write("]\n")
  outF.write("up = [")
  for x in up: 
    outF.write("{:.8e}".format(x))
  outF.write("]\n")
  outF.close()

if __name__ == '__main__':
  from ROOT import *
  gROOT.SetBatch(1)
  gStyle.SetOptStat(0)

  if cal16: calPU(16) 
  if cal17: calPU(17)
  if cal18: calPU(18)
   
