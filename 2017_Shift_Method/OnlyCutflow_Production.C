#include <iostream>
using namespace std;
#include <algorithm>    // std::max
#include <stdlib.h>
#include <iomanip>
#include "TMath.h"
#include <math.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include "TChain.h"
#include "TGraphErrors.h" 
#include "TCollection.h"
#include "TSystemFile.h"
#include "TSystemDirectory.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TChainElement.h" 
#include "TStyle.h"
//#include "/home/jrorie/private/EWK_MC_Production/new_procedure/CMSSW_7_4_1_patch1/src/MuJetAnalysis/CUTFLOW_TABLE/Helpers.h"
//#include "/home/jrorie/private/systematics/CMSSW_7_4_1_patch1/src/CUTFLOW_TABLE/Helpers.h"
#include "/home/jrorie/storage/CUTFLOW_TABLE/Helpers.h"

//============= Global Variables ===========================//
vector<double> mGammaD_0250_eff;
vector<double> mGammaD_0275_eff;
vector<double> mGammaD_0300_eff;
vector<double> mGammaD_2000_eff;
vector<double> mGammaD_8500_eff;
vector<double> mGammaD_0400_eff;
vector<double> mGammaD_0700_eff;
vector<double> mGammaD_1000_eff;
vector<double> mGammaD_1500_eff;

vector<double> mGammaD_0250_eff_uncert;
vector<double> mGammaD_0275_eff_uncert;
vector<double> mGammaD_0300_eff_uncert;
vector<double> mGammaD_2000_eff_uncert;
vector<double> mGammaD_8500_eff_uncert;
vector<double> mGammaD_0400_eff_uncert;
vector<double> mGammaD_0700_eff_uncert;
vector<double> mGammaD_1000_eff_uncert;
vector<double> mGammaD_1500_eff_uncert;

vector<double> mGammaD_0250_cT;
vector<double> mGammaD_0275_cT;
vector<double> mGammaD_0300_cT;
vector<double> mGammaD_2000_cT;
vector<double> mGammaD_8500_cT;
vector<double> mGammaD_0400_cT;
vector<double> mGammaD_0700_cT;
vector<double> mGammaD_1000_cT;
vector<double> mGammaD_1500_cT;

vector< vector<double> > FakeStatistics;

/*double My_dPhi (double phi1, double phi2) {
	double dPhi = phi1 - phi2;
	if (dPhi >  TMath::Pi()) dPhi -= 2*TMath::Pi();
	if (dPhi <  -TMath::Pi()) dPhi += 2*TMath::Pi();
	return abs(dPhi); //Changed to abs 1/5/2016
}
*/
int MatchRECOtoGENdimuons(double etaC, double phiC, double etaF, double phiF, double etaAC, double phiAC, double etaAF, double phiAF){
	int matchInt = -999;
	//match RECO C to GEN C 
	double dphi_CC = My_dPhi(phiC, phiAC);
	double deta_CC = etaC - etaAC;
	double dR_CC   = sqrt(dphi_CC*dphi_CC + deta_CC*deta_CC); 
	//match RECO F to GEN F
	double dphi_FF = My_dPhi(phiF, phiAF);
	double deta_FF = etaF - etaAF;
	double dR_FF   = sqrt(dphi_FF*dphi_FF + deta_FF*deta_FF);
	//match RECO C to GEN F
	double dphi_CF = My_dPhi(phiC, phiAF);
	double deta_CF = etaC - etaAF;
	double dR_CF   = sqrt(dphi_CF*dphi_CF + deta_CF*deta_CF);
	//match RECO F to GEN C
	double dphi_FC = My_dPhi(phiF, phiAC);
	double deta_FC = etaF - etaAC;
	double dR_FC   = sqrt(dphi_FC*dphi_FC + deta_FC*deta_FC);

	if((dR_CC+dR_FF) <= (dR_CF+dR_FC)){
		matchInt = 0; //Then RECO C-> GEN C and RECO F->GEN F
	}else{
		matchInt = 1; //Then RECO C->GEN F and RECO F->GEN C
	}
	return matchInt;
}

TString mass_string;
TString cT_string;
TString fileName;

std::map<TString, TString > mass_strings = {
	{"0250", "0.25"}, {"0275", "0.275"}, {"0300","0.3"}, {"0400", "0.4"}, {"0700", "0.7"},
	{"1000", "1.0"}, {"1500", "1.5"}, {"2000", "2.0"}, {"8500", "8.5"}
};
std::map<TString, double > cT_strings = {
	{"000_",0.0}, {"005_", 0.05}, {"010_", 0.10}, {"020_", 0.20}, {"050_", 0.50}, {"100_", 1.00},
	{"200_", 2.00}, {"300_", 3.00}, {"500_", 5.00}, {"1000", 10.0}, {"2000", 20.0}, {"8500", 85.0}
};

void efficiency_hitrecovery(const std::vector<std::string>& dirNames){
	bool verbose(true);
//	TString dirname(fileName);
	TChain* chain = new TChain("dummy");
	TString ext("out_ana_");

decodeFileNameMany(dirNames, mass_string, cT_string);
    fileName = "DarkSUSY_mH_125_mGammaD_" + mass_string + "_cT_" + cT_string;

	// add files to the chain
addfilesMany(chain, dirNames, ext);

	//Initialize Variables and counters
	//{{{

	Int_t event;
	Int_t run;
	Int_t lumi;

	Bool_t is4GenMu;
	Bool_t is1GenMu17;
	Bool_t is2GenMu8;
	Bool_t is3GenMu8;
	Bool_t is4GenMu8;

	Bool_t is1SelMu17;
	Bool_t is2SelMu8;
	Bool_t is3SelMu8;
	Bool_t is4SelMu8;

	Bool_t is2MuJets;
	Bool_t is2DiMuons;
	Bool_t is2DiMuonsFittedVtxOK;
	//Bool_t is2DiMuonsDzOK_FittedVtx;
	Float_t diMuons_dz_FittedVtx;
	Bool_t isDiMuonHLTFired;
	Bool_t is2DiMuonsMassOK_FittedVtx;
	//Bool_t is2DiMuonsIsoTkOK_FittedVtx;
	Float_t diMuonF_IsoTk_FittedVtx;
	Float_t diMuonC_IsoTk_FittedVtx;
	Bool_t isVertexOK;

	Float_t genA0_Lxy;
	Float_t genA1_Lxy;
	Float_t genA0_Lz;
	Float_t genA1_Lz;


	Int_t diMuonC_m1_FittedVtx_hitpix;
	Int_t diMuonC_m2_FittedVtx_hitpix;
	Int_t diMuonF_m1_FittedVtx_hitpix;
	Int_t diMuonF_m2_FittedVtx_hitpix;

	Int_t diMuonC_m1_FittedVtx_hitpix_l3inc;
	Int_t diMuonC_m2_FittedVtx_hitpix_l3inc;
	Int_t diMuonF_m1_FittedVtx_hitpix_l3inc;
	Int_t diMuonF_m2_FittedVtx_hitpix_l3inc;

	Float_t genA0_eta;
	Float_t genA0_phi;
	Float_t genA1_eta;
	Float_t genA1_phi;

	Float_t selMu0_phi;
	Float_t selMu1_phi;
	Float_t selMu2_phi;
	Float_t selMu3_phi;

	Float_t selMu0_eta;
	Float_t selMu1_eta;
	Float_t selMu2_eta;
	Float_t selMu3_eta;

	Float_t genA0_m;
	Float_t genA0_px;
	Float_t genA0_py;
	Float_t genA0_pz;

	Float_t genA1_m;
	Float_t genA1_px;
	Float_t genA1_py;
	Float_t genA1_pz;

	Float_t diMuonC_FittedVtx_m;
	Float_t diMuonC_FittedVtx_px;
	Float_t diMuonC_FittedVtx_py;
	Float_t diMuonC_FittedVtx_pz;
	Float_t diMuonC_FittedVtx_eta;
	Float_t diMuonC_FittedVtx_phi;
	Float_t diMuonC_FittedVtx_Lxy;
	Float_t diMuonC_FittedVtx_L;

	Float_t diMuonF_FittedVtx_m;
	Float_t diMuonF_FittedVtx_px;
	Float_t diMuonF_FittedVtx_py;
	Float_t diMuonF_FittedVtx_pz;
	Float_t diMuonF_FittedVtx_eta;
	Float_t diMuonF_FittedVtx_phi;
	Float_t diMuonF_FittedVtx_Lxy;
	Float_t diMuonF_FittedVtx_L;

	Float_t genA0Mu0_eta;
	Float_t genA1Mu0_eta;
	Float_t genA0Mu1_eta;
	Float_t genA1Mu1_eta;


	Float_t genA0Mu0_phi;
	Float_t genA1Mu0_phi;
	Float_t genA0Mu1_phi;
	Float_t genA1Mu1_phi;

	//Vertex information
	Float_t genA0_vx;
	Float_t genA0_vy;
	Float_t genA0_vz;

	Float_t genA1_vx;
	Float_t genA1_vy;
	Float_t genA1_vz;

	Float_t genA0Mu0_vx;
	Float_t genA0Mu1_vx;
	Float_t genA1Mu0_vx;
	Float_t genA1Mu1_vx;

	Float_t genA0Mu0_vy;
	Float_t genA0Mu1_vy;
	Float_t genA1Mu0_vy;
	Float_t genA1Mu1_vy;

	Float_t genA0Mu0_vz;
	Float_t genA0Mu1_vz;
	Float_t genA1Mu0_vz;
	Float_t genA1Mu1_vz;

	Float_t diMuonC_FittedVtx_vx;
	Float_t diMuonC_FittedVtx_vy;
	Float_t diMuonC_FittedVtx_vz;

	Float_t diMuonF_FittedVtx_vx;
	Float_t diMuonF_FittedVtx_vy;
	Float_t diMuonF_FittedVtx_vz;

	//============= Counters ===========================//

	vector<double> FakesPerSample;

	Int_t ev_all = 0;
	Int_t ev_isVtxOK = 0;
	Int_t ev_is2MuJets = 0;
	Int_t ev_is2DiMuons = 0;
	Int_t ev_is2DiMuonsFittedVtxOK = 0;
	Int_t ev_isPixelHitOK = 0;
	Int_t ev_is2DiMuonsDzOK_FittedVtx = 0;
	Int_t ev_is2DiMuonsMassOK_FittedVtx = 0;
	Int_t ev_is2DiMuonsIsoTkOK_FittedVtx = 0;
	Int_t ev_isDiMuonHLTFired = 0;

	Int_t c1genm = 0;
	Int_t c2genm = 0;
	Int_t c3genm = 0;
	Int_t c4genm = 0;
	Int_t ev_4gmlxylzcut = 0;

	Int_t c1recm = 0;
	Int_t c2recm = 0;
	Int_t c3recm = 0;
	Int_t c4recm = 0;

	Int_t uncuttableFakeCounter = 0;
	//}}}

	TObjArray *fileElements=chain->GetListOfFiles();
	TIter next(fileElements);
	TChainElement *chEl=0;

	while ((chEl=(TChainElement*)next())) {
		if (verbose) std::cout << "running on file " << chEl->GetTitle() << std::endl;
		TFile* myfile = new TFile(chEl->GetTitle());
		if (!myfile) {
			if (verbose) std::cout << "File " << chEl->GetTitle() << " does not exist" << std::endl;
			continue;
		}

		if (verbose) std::cout << "Loading directory cutFlowAnalyzerPXBL3PXFL2" << std::endl;
		myfile->cd("cutFlowAnalyzerPXBL3PXFL2");

		TTree *t = (TTree*)myfile->Get("cutFlowAnalyzerPXBL3PXFL2/Events");
		if (!t) {
			if (verbose) std::cout << "Tree cutFlowAnalyzerPXBL3PXFL2/Events does not exist" << std::endl;
			continue;
		}

		if (verbose) cout<<"  Events  "<<t->GetEntries()<<endl;

		//Pull variables from nTuple
		//{{{
		// Event info
		t->SetBranchAddress("event", &event);
		t->SetBranchAddress("run",   &run);
		t->SetBranchAddress("lumi",  &lumi);

		// GEN Level Selectors
		t->SetBranchAddress("is4GenMu",    &is4GenMu);
		t->SetBranchAddress("is1GenMu17",  &is1GenMu17);
		t->SetBranchAddress("is2GenMu8",   &is2GenMu8);
		t->SetBranchAddress("is3GenMu8",   &is3GenMu8);
		t->SetBranchAddress("is4GenMu8",   &is4GenMu8);

		// RECO Level Selectors
		t->SetBranchAddress("is1SelMu17",                     &is1SelMu17);
		t->SetBranchAddress("is2SelMu8",                      &is2SelMu8);
		t->SetBranchAddress("is3SelMu8",                      &is3SelMu8);
		t->SetBranchAddress("is4SelMu8",                      &is4SelMu8);

		t->SetBranchAddress("is2MuJets",                      &is2MuJets);
		t->SetBranchAddress("is2DiMuons",                     &is2DiMuons);
		t->SetBranchAddress("is2DiMuonsFittedVtxOK",          &is2DiMuonsFittedVtxOK);
		t->SetBranchAddress("diMuons_dz_FittedVtx",       &diMuons_dz_FittedVtx);
		t->SetBranchAddress("isDiMuonHLTFired",               &isDiMuonHLTFired);
		t->SetBranchAddress("is2DiMuonsMassOK_FittedVtx",     &is2DiMuonsMassOK_FittedVtx);
		t->SetBranchAddress("diMuonF_IsoTk_FittedVtx",    &diMuonF_IsoTk_FittedVtx);
		t->SetBranchAddress("diMuonC_IsoTk_FittedVtx",    &diMuonC_IsoTk_FittedVtx);
		t->SetBranchAddress("isVertexOK",                     &isVertexOK);
		t->SetBranchAddress("isDiMuonHLTFired",              &isDiMuonHLTFired);

		t->SetBranchAddress("genA0_Lxy", &genA0_Lxy);
		t->SetBranchAddress("genA0_Lz",  &genA0_Lz);
		t->SetBranchAddress("genA1_Lxy", &genA1_Lxy);
		t->SetBranchAddress("genA1_Lz",  &genA1_Lz);

		t->SetBranchAddress("diMuonC_m1_FittedVtx_hitpix", &diMuonC_m1_FittedVtx_hitpix);
		t->SetBranchAddress("diMuonC_m2_FittedVtx_hitpix", &diMuonC_m2_FittedVtx_hitpix);
		t->SetBranchAddress("diMuonF_m1_FittedVtx_hitpix", &diMuonF_m1_FittedVtx_hitpix);
		t->SetBranchAddress("diMuonF_m2_FittedVtx_hitpix", &diMuonF_m2_FittedVtx_hitpix);

		t->SetBranchAddress("diMuonC_m1_FittedVtx_hitpix_l3inc", &diMuonC_m1_FittedVtx_hitpix_l3inc);
		t->SetBranchAddress("diMuonC_m2_FittedVtx_hitpix_l3inc", &diMuonC_m2_FittedVtx_hitpix_l3inc);
		t->SetBranchAddress("diMuonF_m1_FittedVtx_hitpix_l3inc", &diMuonF_m1_FittedVtx_hitpix_l3inc);
		t->SetBranchAddress("diMuonF_m2_FittedVtx_hitpix_l3inc", &diMuonF_m2_FittedVtx_hitpix_l3inc);

		t->SetBranchAddress("genA1_phi",&genA1_phi);
		t->SetBranchAddress("genA1_eta",&genA1_eta);

		t->SetBranchAddress("genA0_phi",&genA0_phi);
		t->SetBranchAddress("genA0_eta",&genA0_eta);

		t->SetBranchAddress("selMu0_phi",&selMu0_phi);
		t->SetBranchAddress("selMu1_phi",&selMu1_phi);
		t->SetBranchAddress("selMu2_phi",&selMu2_phi);
		t->SetBranchAddress("selMu3_phi",&selMu3_phi);

		t->SetBranchAddress("selMu0_eta",&selMu0_eta);
		t->SetBranchAddress("selMu1_eta",&selMu1_eta);
		t->SetBranchAddress("selMu2_eta",&selMu2_eta);
		t->SetBranchAddress("selMu3_eta",&selMu3_eta);

		t->SetBranchAddress("genA0_m" , &genA0_m);
		t->SetBranchAddress("genA0_px", &genA0_px);
		t->SetBranchAddress("genA0_py", &genA0_py);
		t->SetBranchAddress("genA0_pz", &genA0_pz);

		t->SetBranchAddress("genA1_m" , &genA1_m);
		t->SetBranchAddress("genA1_px", &genA1_px);
		t->SetBranchAddress("genA1_py", &genA1_py);
		t->SetBranchAddress("genA1_pz", &genA1_pz);

		t->SetBranchAddress("diMuonF_FittedVtx_m", &diMuonF_FittedVtx_m);
		t->SetBranchAddress("diMuonF_FittedVtx_px", &diMuonF_FittedVtx_px);
		t->SetBranchAddress("diMuonF_FittedVtx_py", &diMuonF_FittedVtx_py);
		t->SetBranchAddress("diMuonF_FittedVtx_pz", &diMuonF_FittedVtx_pz);
		t->SetBranchAddress("diMuonF_FittedVtx_eta",&diMuonF_FittedVtx_eta);
		t->SetBranchAddress("diMuonF_FittedVtx_phi",&diMuonF_FittedVtx_phi);
		t->SetBranchAddress("diMuonF_FittedVtx_Lxy",&diMuonF_FittedVtx_Lxy);
		t->SetBranchAddress("diMuonF_FittedVtx_L",&diMuonF_FittedVtx_L);

		t->SetBranchAddress("diMuonC_FittedVtx_m", &diMuonC_FittedVtx_m);
		t->SetBranchAddress("diMuonC_FittedVtx_px", &diMuonC_FittedVtx_px);
		t->SetBranchAddress("diMuonC_FittedVtx_py", &diMuonC_FittedVtx_py);
		t->SetBranchAddress("diMuonC_FittedVtx_pz", &diMuonC_FittedVtx_pz);
		t->SetBranchAddress("diMuonC_FittedVtx_eta",&diMuonC_FittedVtx_eta);
		t->SetBranchAddress("diMuonC_FittedVtx_phi",&diMuonC_FittedVtx_phi);
		t->SetBranchAddress("diMuonC_FittedVtx_Lxy",&diMuonC_FittedVtx_Lxy);
		t->SetBranchAddress("diMuonC_FittedVtx_L",&diMuonC_FittedVtx_L);

		t->SetBranchAddress("genA0Mu0_eta",&genA0Mu0_eta);
		t->SetBranchAddress("genA1Mu0_eta",&genA1Mu0_eta);
		t->SetBranchAddress("genA0Mu1_eta",&genA0Mu1_eta);
		t->SetBranchAddress("genA1Mu1_eta",&genA1Mu1_eta);

		t->SetBranchAddress("genA0Mu0_phi",&genA0Mu0_phi);
		t->SetBranchAddress("genA1Mu0_phi",&genA1Mu0_phi);
		t->SetBranchAddress("genA0Mu1_phi",&genA0Mu1_phi);
		t->SetBranchAddress("genA1Mu1_phi",&genA1Mu1_phi);

		t->SetBranchAddress("genA0_vx", &genA0_vx); 
		t->SetBranchAddress("genA0_vy", &genA0_vy);
		t->SetBranchAddress("genA0_vz", &genA0_vz);

		t->SetBranchAddress("genA1_vx", &genA1_vx);
		t->SetBranchAddress("genA1_vy", &genA1_vy);
		t->SetBranchAddress("genA1_vz", &genA1_vz);

		t->SetBranchAddress("genA0Mu0_vx", &genA0Mu0_vx);
		t->SetBranchAddress("genA0Mu1_vx", &genA0Mu1_vx);
		t->SetBranchAddress("genA1Mu0_vx", &genA1Mu0_vx);
		t->SetBranchAddress("genA1Mu1_vx", &genA1Mu1_vx);

		t->SetBranchAddress("genA0Mu0_vy", &genA0Mu0_vy);
		t->SetBranchAddress("genA0Mu1_vy", &genA0Mu1_vy);
		t->SetBranchAddress("genA1Mu0_vy", &genA1Mu0_vy);
		t->SetBranchAddress("genA1Mu1_vy", &genA1Mu1_vy);

		t->SetBranchAddress("genA0Mu0_vz", &genA0Mu0_vz);
		t->SetBranchAddress("genA0Mu1_vz", &genA0Mu1_vz);
		t->SetBranchAddress("genA1Mu0_vz", &genA1Mu0_vz);
		t->SetBranchAddress("genA1Mu1_vz", &genA1Mu1_vz);

		t->SetBranchAddress("diMuonC_FittedVtx_vx", &diMuonC_FittedVtx_vx);
		t->SetBranchAddress("diMuonC_FittedVtx_vy", &diMuonC_FittedVtx_vy);
		t->SetBranchAddress("diMuonC_FittedVtx_vz", &diMuonC_FittedVtx_vz);

		t->SetBranchAddress("diMuonF_FittedVtx_vx", &diMuonF_FittedVtx_vx);
		t->SetBranchAddress("diMuonF_FittedVtx_vy", &diMuonF_FittedVtx_vy);
		t->SetBranchAddress("diMuonF_FittedVtx_vz", &diMuonF_FittedVtx_vz);
		//}}}

		for(int k=0;k<t->GetEntries();k++){
			t->GetEntry(k);

			ev_all++;

			if(is1GenMu17) c1genm++;
			if(is2GenMu8)  c2genm++;
			if(is3GenMu8)  c3genm++;
			if(is4GenMu8)  c4genm++;

			if(is1SelMu17) c1recm++;
			if(is2SelMu8)  c2recm++;
			if(is3SelMu8)  c3recm++;
			if(is4SelMu8)  c4recm++;

			int triggerFlag_std = 0;
			int triggerFlag_lxy = 0;
			int fakeFlag = 0;
			int realFlag = 0;
			int fakeDimuon1 = 0;
			int fakeDimuon2 = 0;

			int brokenCounter = 0;
			int fakeCounter_dimuon1 = 0;
			int fakeCounter_dimuon2 = 0;
			int fakeCounter_both = 0;

			//  ===========   GEN LEVEL information  ==============//

			//{{{
			if(is4GenMu8){
				Float_t dphi_gD = My_dPhi(genA1_phi,genA0_phi);
				//if(fabs(dphi_gD)>=2.5){
				if(fabs(genA0_Lxy)<9.8 && fabs(genA1_Lxy)<9.8 && fabs(genA0_Lz)<48.5 && fabs(genA1_Lz)<48.5){
					ev_4gmlxylzcut++;
				}
				//}
			}

			//}}}
			//  =============  Reco information ====================//

			//Numerator of ratio
			//{{{
			//if(is4GenMu8)
			//if(fabs(genA0_Lxy)<9.8 && fabs(genA1_Lxy)<9.8 && fabs(genA0_Lz)<48.5 && fabs(genA1_Lz)<48.5){
			Float_t dphi_gD = My_dPhi(genA1_phi,genA0_phi);
			//if(fabs(dphi_gD)>=2.5){
			if(is4SelMu8){
				brokenCounter++;
				if(isVertexOK){
					ev_isVtxOK++;
					brokenCounter++;
					if(is2MuJets){
						ev_is2MuJets++;
						brokenCounter++;
						if(is2DiMuons){
							ev_is2DiMuons++;
							brokenCounter++;
							if(is2DiMuonsFittedVtxOK){
								ev_is2DiMuonsFittedVtxOK++;
								brokenCounter++;
								//if( (diMuonC_m1_FittedVtx_hitpix==1||diMuonC_m2_FittedVtx_hitpix==1)&&(diMuonF_m1_FittedVtx_hitpix==1||diMuonF_m2_FittedVtx_hitpix==1) ) //First pixel layers
								if( (diMuonC_m1_FittedVtx_hitpix_l3inc==1||diMuonC_m2_FittedVtx_hitpix_l3inc==1)&&(diMuonF_m1_FittedVtx_hitpix_l3inc==1||diMuonF_m2_FittedVtx_hitpix_l3inc==1) ){
									ev_isPixelHitOK++;
									if(diMuons_dz_FittedVtx <= 0.1 && diMuons_dz_FittedVtx >= -10){
										ev_is2DiMuonsDzOK_FittedVtx++;
										brokenCounter++;
										if(is2DiMuonsMassOK_FittedVtx){
											ev_is2DiMuonsMassOK_FittedVtx++;
											brokenCounter++;
											if(diMuonF_IsoTk_FittedVtx <= 2 && diMuonF_IsoTk_FittedVtx >= -10){
											if(diMuonC_IsoTk_FittedVtx <= 2 && diMuonC_IsoTk_FittedVtx >= -10){
												ev_is2DiMuonsIsoTkOK_FittedVtx++;
												brokenCounter++;
												if(isDiMuonHLTFired){
													ev_isDiMuonHLTFired++;
													brokenCounter++;
													triggerFlag_std++;
												}
											}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			//}//opening angle cut
			//}// fiducial cut in numerator
			//}}}

		} // closing for loop
		myfile->Close();
	} // closing while loop
	//Print out cutflow table
	//{{{
	std::cout << "" << std::endl;
	std::cout<<" Sample: " << fileName << endl;
	std::cout<<" Events          "<<ev_all<<std::endl;
	std::cout<<" ================ GEN MUONS ========================================= "<<std::endl;
	std::cout<<" 1GenMu17                       "<<c1genm<<"   reff "<<c1genm/(ev_all*1.0)<<std::endl;
	std::cout<<" 2GenMu8                        "<<c2genm<<"   reff  "<<c2genm/(c1genm*1.0)<<std::endl;
	std::cout<<" 3GenMu8                        "<<c3genm<<"   reff  "<<c3genm/(c2genm*1.0)<<std::endl;
	std::cout<<" 4GenMu8                        "<<c4genm<<"   reff  "<<c4genm/(c3genm*1.0)<<std::endl;
	std::cout<<" 4GenMu8 Lxy/Lz                 "<<ev_4gmlxylzcut<<"   reff   "<<ev_4gmlxylzcut/c4genm<<std::endl;
	std::cout<<" ================ RECO MUONS ========================================= "<<std::endl;
	std::cout<<" 1RecMu17                       "<<c1recm<<"  reff  "<<c1recm/(ev_all*1.0)<<std::endl;
	std::cout<<" 2RecMu8                        "<<c2recm<<"  reff  "<<c2recm/(c1recm*1.0)<<std::endl;
	std::cout<<" 3RecMu8                        "<<c3recm<<"  reff  "<<c3recm/(c2recm*1.0)<<std::endl;
	std::cout<<" 4RecMu8                        "<<c4recm<<"  reff  "<<c4recm/(c3recm*1.0)<<std::endl;
	std::cout<<" ================ EVENT variables ================= "<<std::endl;
	std::cout<<" Events with VtxOK              "<<ev_isVtxOK<<"    reff  "<<ev_isVtxOK/(1.0*c4recm)<<std::endl;
	std::cout<<" Events with 2 muonjets         "<<ev_is2MuJets<<"     reff  "<<ev_is2MuJets/(1.0*ev_isVtxOK)<<std::endl;
	std::cout<<" Events with 2 Dimuons          "<<ev_is2DiMuons<<"    reff  "<<ev_is2DiMuons/(1.0*ev_is2MuJets)<<std::endl;
	std::cout<<" Events with 2DimVtxOK          "<<ev_is2DiMuonsFittedVtxOK<<"    reff  "<<ev_is2DiMuonsFittedVtxOK/(1.0*ev_is2DiMuons)<<std::endl;
	std::cout<<" Events with 2DimHitPix         "<<ev_isPixelHitOK<<"     reff  "<<ev_isPixelHitOK/(1.0*ev_is2DiMuonsFittedVtxOK)<<std::endl;
	std::cout<<" Events with 2DimDzOK           "<<ev_is2DiMuonsDzOK_FittedVtx<<"   reff   "<<ev_is2DiMuonsDzOK_FittedVtx/(1.0*ev_isPixelHitOK)<<std::endl;
	std::cout<<" Events with 2DimMassOK         "<<ev_is2DiMuonsMassOK_FittedVtx<<"  reff   "<<ev_is2DiMuonsMassOK_FittedVtx/(1.0*ev_is2DiMuonsDzOK_FittedVtx)<<endl;
	std::cout<<" Events with 2DimIsoOK          "<<ev_is2DiMuonsIsoTkOK_FittedVtx<<"   reff   "<<ev_is2DiMuonsIsoTkOK_FittedVtx/(1.0*ev_is2DiMuonsMassOK_FittedVtx)<<endl;
	std::cout<<" Events with 2DimHLT            "<<ev_isDiMuonHLTFired<<"   reff   "<<ev_isDiMuonHLTFired/(1.0*ev_is2DiMuonsIsoTkOK_FittedVtx)<<endl;
	std::cout<<" ratio reco/gen                 "<<ev_isDiMuonHLTFired/(1.0*ev_4gmlxylzcut)<<" +/-  "<<sqrt( ((ev_isDiMuonHLTFired/(1.0*ev_4gmlxylzcut))*(1- (ev_isDiMuonHLTFired/(1.0*ev_4gmlxylzcut)) ))/(1.0*ev_4gmlxylzcut))<<std::endl;
	//}}}
}

void OnlyCutflow()
{
	std::vector< std::vector<string> > DarkSUSY_mH_125_mGammaD_v;
	cout << "Vector Created" << endl;
//	readTextFileWithSamples("/fdata/hepx/store/user/bmichlin/ExtFidCut_UnHardCodeCuts_SampleList.txt", DarkSUSY_mH_125_mGammaD_v);
//	readTextFileWithSamples("./file_list.txt", DarkSUSY_mH_125_mGammaD_v);
//      readTextFileWithSamples("/home/jrorie/private/EWK_MC_Production/new_procedure/CMSSW_7_4_1_patch1/src/MuJetAnalysis/CUTFLOW_TABLE/file_list.txt", DarkSUSY_mH_125_mGammaD_v);
	readTextFileWithSamples("/home/jrorie/private/systematics/CMSSW_7_4_1_patch1/src/CUTFLOW_TABLE/file_list.txt", DarkSUSY_mH_125_mGammaD_v);
	cout << "Samples read" << endl;
	for(auto v: DarkSUSY_mH_125_mGammaD_v) efficiency_hitrecovery(v);
	cout << "For Loop completes" << endl;
}     
