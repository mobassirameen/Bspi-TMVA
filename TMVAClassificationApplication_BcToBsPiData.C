//
//   run:
//   root -l TMVAClassificationApplication_BcToBsPiData.C\(\"BDT\"\)
//

#include <cstdlib>
#include <vector>
#include <iostream>
#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TStopwatch.h"

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

using namespace TMVA;

void TMVAClassificationApplication_BcToBsPiData(TString myMethodList="")
{
	TMVA::Tools::Instance();
	std::map<std::string,int> Use;

	Use["BDT"]=1;
	//Use["BDTG"]=1;

	std::cout << std::endl;
	std::cout << "==> Start TMVAClassificationApplication" << std::endl;

	   if (myMethodList != "") {
                 for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

     		 std::vector<TString> mlist = gTools().SplitString( myMethodList, ',' );
     		 for (UInt_t i=0; i<mlist.size(); i++) {
        	    std::string regMethod(mlist[i]);

        		 if (Use.find(regMethod) == Use.end()) {
            			std::cout << "Method \"" << regMethod
                     			 << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
           			 for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
            			   std::cout << it->first << " ";
           				 }
           			 std::cout << std::endl;
           			 return;
       			  }
         			Use[regMethod] = 1;
      			}
   		}

	TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );

	//Float_t var1, var2, var3, var4, var5, var6, var7, var8, var9, var10;
	//Int_t IntVar1;

    // -----------------------------------------------------------
	//reader->AddVariable("Kmpt",                          &var1);
        //reader->AddVariable("Kppt",                          &var2);
    //reader->AddVariable("Kmtrkdcasigbs",                 &var3);
        //reader->AddVariable("Kptrkdcasigbs",                 &var4);
    //reader->AddVariable("Bcosalphabs2d",                 &var5);
    //reader->AddVariable("Blxysig",                       &var6);
    //reader->AddVariable("Bvtxcl",                        &var7);
    // -----------------------------------------------------------
    
    Float_t var1, var2, var3, var4, var5, var6;
    Float_t var7, var8, var9, var10, var11, var12, var13, var14, var15, var16;
    
    reader->AddVariable( "Bcpt", &var1);
    reader->AddVariable( "Bclxy", &var2);
    reader->AddVariable( "Bccosalphaxy", &var3);
    reader->AddVariable( "Bsvtxcl", &var4);
    reader->AddVariable( "Bc_pvip", &var5);
    reader->AddVariable( "Bceta", &var6);

	//Float_t spec1,spec2,spec3;
	//reader->AddSpectator( "Bmass",           &spec1 );
    //reader->AddSpectator( "Mumumass",        &spec2 );
    //reader->AddSpectator( "Mumumasserr",     &spec3 );
	
	TString dir="dataset/weights/";
	TString prefix="TMVAClassification";

	for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++)
	{
		if (it->second)
		{
			TString methodName = TString(it->first) + TString(" method");
			TString weightfile = dir + prefix + TString("_") + TString(it->first) + TString(".weights.xml");
			reader->BookMVA( methodName, weightfile );
		}
	}
	   
       UInt_t nbin = 100;
	   TH1F *histLk(0);
	   TH1F *histLkD(0);
	   TH1F *histLkPCA(0);
	   TH1F *histLkKDE(0);
	   TH1F *histLkMIX(0);
	   TH1F *histPD(0);
	   TH1F *histPDD(0);
	   TH1F *histPDPCA(0);
	   TH1F *histPDEFoam(0);
	   TH1F *histPDEFoamErr(0);
	   TH1F *histPDEFoamSig(0);
	   TH1F *histKNN(0);
	   TH1F *histHm(0);
	   TH1F *histFi(0);
	   TH1F *histFiG(0);
	   TH1F *histFiB(0);
	   TH1F *histLD(0);
	   TH1F *histNn(0);
	   TH1F *histNnbfgs(0);
       TH1F *histNnbnn(0);
	   TH1F *histNnC(0);
	   TH1F *histNnT(0);
	   TH1F *histBdt(0);
	   TH1F *histBdtG(0);
	   TH1F *histBdtB(0);
	   TH1F *histBdtD(0);
	   TH1F *histBdtF(0);
	   TH1F *histRf(0);
	   TH1F *histSVMG(0);
	   TH1F *histSVMP(0);
	   TH1F *histSVML(0);
	   TH1F *histFDAMT(0);
	   TH1F *histFDAGA(0);
	   TH1F *histCat(0);
	   TH1F *histPBdt(0);
	   TH1F *histDnnGpu(0);
	   TH1F *histDnnCpu(0);

	  // TFile *f2=new TFile("REAL_DATA_forBDTApp.root");
      // TTree *datatr_in=(TTree*)f2->Get("events");
      //TChain *ch= new TChain("events");
	  // ch->Add("./MC/Modified_sel_BsToPhiMuMu_OfficialMC_signal_2016_Jpsi_Psi_mc.lite_cut_bdt_addgentree.root");
	  // ch->Add("./MC/Modified_sel_BsToPhiMuMu_OfficialMC_signal_2016_Presel_mc.lite_cut_bdt_genadded.root");
      TChain *ch= new TChain("Tree");
	  ch->Add("/Users/mohammadmobassirameen/Desktop/BcToBsPi-analysis/BDT_BcToBsPi/bdtsamples/REAL_DATA_For_BDTApplication.root");

      TTree *datatr_in=ch;
      std::cout<<"=> entries for data tree =>       "<<datatr_in->GetEntries()<<std::endl;
	  
      // ---------------------------------------------------------------------------------------------------------------
      /*
      Double_t userVar1, userVar2, userVar3, userVar4, userVar5, userVar6, userVar7, userVar8, userVar9, userVar10;
      Int_t userIntVar1;
      Float_t cut_BDT = 0.13 ;

      datatr_in->SetBranchAddress( "Kmpt",              &userVar1 );
      datatr_in->SetBranchAddress( "Kppt",              &userVar2 );
      datatr_in->SetBranchAddress( "Kmtrkdcasigbs",     &userVar3 );
      datatr_in->SetBranchAddress( "Kptrkdcasigbs",     &userVar4 );
      datatr_in->SetBranchAddress( "Bcosalphabs2d",     &userVar5 );
      datatr_in->SetBranchAddress( "Blxysig",           &userVar6 );
      datatr_in->SetBranchAddress( "Bvtxcl",            &userVar7 );
      datatr_in->SetBranchAddress( "Bmass",             &userVar8);
      datatr_in->SetBranchAddress( "Mumumass",          &userVar9 );
      datatr_in->SetBranchAddress( "Mumumasserr",       &userVar10 );
      datatr_in->SetBranchAddress( "Triggers",          &userIntVar1 );
      */
      // ---------------------------------------------------------------------------------------------------------------
    
      Float_t userVar1, userVar2, userVar3, userVar4, userVar5, userVar6, userVar7, userVar8, userVar9, userVar10, userVar11, userVar12, userVar13, userVar14, userVar15, userVar16;
      
      Float_t cut_BDT = 0.0 ;
    
      datatr_in->SetBranchAddress( "Bcmass", &userVar1);
      datatr_in->SetBranchAddress( "Bcpt", &userVar2);
      datatr_in->SetBranchAddress( "Bceta", &userVar3);
      datatr_in->SetBranchAddress( "Bcphi", &userVar4);
      datatr_in->SetBranchAddress( "Bsmass", &userVar5);
      datatr_in->SetBranchAddress( "JPsimass", &userVar6);
      datatr_in->SetBranchAddress( "Phimass", &userVar7);
      datatr_in->SetBranchAddress( "Pionpt", &userVar8);
      datatr_in->SetBranchAddress( "Kmpt", &userVar9);
      datatr_in->SetBranchAddress( "Kppt", &userVar10);
      datatr_in->SetBranchAddress( "Mumpt", &userVar11);
      datatr_in->SetBranchAddress( "Muppt", &userVar12);
      datatr_in->SetBranchAddress( "Bsvtxcl", &userVar13);
      datatr_in->SetBranchAddress( "Bc_pvip", &userVar14);
      datatr_in->SetBranchAddress( "Bclxy", &userVar15);
      datatr_in->SetBranchAddress( "Bccosalphaxy", &userVar16);
    
	  //TFile *target  = new TFile( "TMVAppOutputData.root","RECREATE" );
      TFile *target  = new TFile( "TMVAppOutputData_14July2022.root","RECREATE" );
	  TTree *tree    = new TTree("Tree","Tree");

      tree->Branch("Bcmass", &userVar1);
      tree->Branch("Bcpt", &userVar2);
      tree->Branch("Bceta", &userVar3);
      tree->Branch("Bcphi", &userVar4);
      tree->Branch("Bsmass", &userVar5);
      tree->Branch("JPsimass", &userVar6);
      tree->Branch("Phimass", &userVar7);
      tree->Branch("Pionpt", &userVar8);
      tree->Branch("Kmpt", &userVar9);
      tree->Branch("Kppt", &userVar10);
      tree->Branch("Mumpt", &userVar11);
      tree->Branch("Muppt", &userVar12);
      tree->Branch("Bsvtxcl", &userVar13);
      tree->Branch("Bc_pvip", &userVar14);
      tree->Branch("Bclxy", &userVar15);
      tree->Branch("Bccosalphaxy", &userVar16);

	  Float_t BDT_response;
	  tree->Branch("BDT_response", &BDT_response);

	  std::cout << "--- Processing: " << datatr_in->GetEntries() << "  events in signal " << std::endl;

	  for (Long64_t ievt=0; ievt<datatr_in->GetEntries(); ievt++)
	  {
          if (ievt%1000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;
          if (ievt==1000) continue;

		  datatr_in->GetEntry(ievt);

                /*
                var1 = userVar1;
                //var2 = userVar2 ;
                var3 = userVar3 ;
                //var4 = userVar4 ;
                var5 = userVar5 ;
                var6 = userVar6 ;
                var7 = userVar7 ;
                spec1= userVar8;
                spec2= userVar9;
                spec3=userVar10;
                */
          var7  = userVar1;
          var1  = userVar2;
          var6  = userVar3;
          var8 = userVar4;
          var9 = userVar5;
          var10 = userVar6;
          var11 = userVar7;
          var12 = userVar8;
          var13 = userVar9;
          var14 = userVar10;
          var15 = userVar11;
          var16 = userVar12;
          var4  = userVar13;
          var5  = userVar14;
          var2  = userVar15;
          var3  = userVar16;

		BDT_response = reader->EvaluateMVA( "BDT method");

		if (BDT_response > cut_BDT) tree->Fill();
	}

	std::cout << "--- End of signal event loop: " << std::endl;

	tree->Write();
	target->Close();

	std::cout << "--- Created root file: \"TMVAppOutputSignal.root\" containing the MVA output histograms" << std::endl;
	delete reader;

	std::cout << "==> TMVAClassificationApplication is done!" << std::endl << std::endl;
}

int main( int argc, char** argv )
{
   TString methodList;
   for (int i=1; i<argc; i++) {
      TString regMethod(argv[i]);
      if(regMethod=="-b" || regMethod=="--batch") continue;
      if (!methodList.IsNull()) methodList += TString(",");
      methodList += regMethod;
   }
   TMVAClassificationApplication_BcToBsPiData(methodList);
   return 0;
}

