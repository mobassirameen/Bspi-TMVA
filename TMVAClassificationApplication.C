/// \file
/// \ingroup tutorial_tmva
/// \notebook -nodraw
/// This macro provides a simple example on how to use the trained classifiers
/// within an analysis module
/// - Project   : TMVA - a Root-integrated toolkit for multivariate data analysis
/// - Package   : TMVA
/// - Exectuable: TMVAClassificationApplication
///
//   run:
//   root -l TMVAClassificationApplication.C\(\"BDT\"\)
//
/// \macro_output
/// \macro_code
/// \author Andreas Hoecker

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

void TMVAClassificationApplication( TString myMethodList = "" )
{

   //---------------------------------------------------------------
   // This loads the library
   TMVA::Tools::Instance();

   // Default MVA methods to be trained + tested
   std::map<std::string,int> Use;

   // Cut optimisation
   Use["Cuts"]            = 0;
   Use["CutsD"]           = 0;
   Use["CutsPCA"]         = 0;
   Use["CutsGA"]          = 0;
   Use["CutsSA"]          = 0;
   //
   // 1-dimensional likelihood ("naive Bayes estimator")
   Use["Likelihood"]      = 0;
   Use["LikelihoodD"]     = 0; // the "D" extension indicates decorrelated input variables (see option strings)
   Use["LikelihoodPCA"]   = 0; // the "PCA" extension indicates PCA-transformed input variables (see option strings)
   Use["LikelihoodKDE"]   = 0;
   Use["LikelihoodMIX"]   = 0;
   //
   // Mutidimensional likelihood and Nearest-Neighbour methods
   Use["PDERS"]           = 0;
   Use["PDERSD"]          = 0;
   Use["PDERSPCA"]        = 0;
   Use["PDEFoam"]         = 0;
   Use["PDEFoamBoost"]    = 0; // uses generalised MVA method boosting
   Use["KNN"]             = 0; // k-nearest neighbour method
   //
   // Linear Discriminant Analysis
   Use["LD"]              = 0; // Linear Discriminant identical to Fisher
   Use["Fisher"]          = 0;
   Use["FisherG"]         = 0;
   Use["BoostedFisher"]   = 0; // uses generalised MVA method boosting
   Use["HMatrix"]         = 0;
   //
   // Function Discriminant analysis
   Use["FDA_GA"]          = 0; // minimisation of user-defined function using Genetics Algorithm
   Use["FDA_SA"]          = 0;
   Use["FDA_MC"]          = 0;
   Use["FDA_MT"]          = 0;
   Use["FDA_GAMT"]        = 0;
   Use["FDA_MCMT"]        = 0;
   // Neural Networks (all are feed-forward Multilayer Perceptrons)
   Use["MLP"]             = 0; // Recommended ANN
   Use["MLPBFGS"]         = 0; // Recommended ANN with optional training method
   Use["MLPBNN"]          = 0; // Recommended ANN with BFGS training method and bayesian regulator
   Use["CFMlpANN"]        = 0; // Depreciated ANN from ALEPH
   Use["TMlpANN"]         = 0; // ROOT's own ANN
   Use["DNN_CPU"]         = 0;         // CUDA-accelerated DNN training.
   Use["DNN_GPU"]         = 0;         // Multi-core accelerated DNN.
   //
   // Support Vector Machine
   Use["SVM"]             = 0;
   //
   // Boosted Decision Trees
   Use["BDT"]             = 1; // uses Adaptive Boost
   Use["BDTG"]            = 0; // uses Gradient Boost
   Use["BDTB"]            = 0; // uses Bagging
   Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
   Use["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting
   //
   // Friedman's RuleFit method, ie, an optimised series of cuts ("rules")
   Use["RuleFit"]         = 0;
   // ---------------------------------------------------------------
   Use["Plugin"]          = 0;
   Use["Category"]        = 0;
   Use["SVM_Gauss"]       = 0;
   Use["SVM_Poly"]        = 0;
   Use["SVM_Lin"]         = 0;

   std::cout << std::endl;
   std::cout << "==> Start TMVAClassificationApplication" << std::endl;

   // Select methods (don't look at this code - not of interest)
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

   // --------------------------------------------------------------------------------------------------

   // Create the Reader object

   TMVA::Reader *reader = new TMVA::Reader( "V:Color:!Silent" );

   // Create a set of variables and declare them to the reader
   // - the variable names MUST corresponds in name and type to those given in the weight file(s) used
    
   // Here we define the variables which were used in training, and assign them to a variables. The name of the variables must match exactly the weight files, and must also be defined in the same order.
    
   Float_t var1, var2, var3, var4, var5, var6;
    
   Float_t var7, var89, var1011, var12, var13, var14, var15;

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


   //  Book the MVA methods

   TString dir    = "dataset/weights/";
   TString prefix = "TMVAClassification";

   // Book method(s)
   for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
      if (it->second) {
         TString methodName = TString(it->first) + TString(" method");
         TString weightfile = dir + prefix + TString("_") + TString(it->first) + TString(".weights.xml");
         reader->BookMVA( methodName, weightfile );
      }
   }

   // Book output histograms
   UInt_t nbin = 50;
   //TH1F *histLk(0);
   //TH1F *histLkD(0);
   //TH1F *histLkPCA(0);
   //TH1F *histLkKDE(0);
   //TH1F *histLkMIX(0);
   //TH1F *histPD(0);
   //TH1F *histPDD(0);
   //TH1F *histPDPCA(0);
   //TH1F *histPDEFoam(0);
   //TH1F *histPDEFoamErr(0);
   //TH1F *histPDEFoamSig(0);
   //TH1F *histKNN(0);
   //TH1F *histHm(0);
   //TH1F *histFi(0);
   //TH1F *histFiG(0);
   //TH1F *histFiB(0);
   //TH1F *histLD(0);
   //TH1F *histNn(0);
   //TH1F *histNnbfgs(0);
   //TH1F *histNnbnn(0);
   //TH1F *histNnC(0);
   //TH1F *histNnT(0);
   TH1F *histBdt(0);
   //TH1F *histBdtG(0);
   //TH1F *histBdtB(0);
   //TH1F *histBdtD(0);
   //TH1F *histBdtF(0);
   //TH1F *histRf(0);
   //TH1F *histSVMG(0);
   //TH1F *histSVMP(0);
   //TH1F *histSVML(0);
   //TH1F *histFDAMT(0);
   //TH1F *histFDAGA(0);
   //TH1F *histCat(0);
   //TH1F *histPBdt(0);
   //TH1F *histDnnGpu(0);
   //TH1F *histDnnCpu(0);

  
  

   if (Use["BDT"])           histBdt     = new TH1F( "MVA_BDT",           "MVA_BDT",           nbin, -0.8, 0.8 );
   //if (Use["BDTG"])          histBdtG    = new TH1F( "MVA_BDTG",          "MVA_BDTG",          nbin, -1.0, 1.0 );
   
   // Prepare inputs tree
   //+++++++++++++++
   TFile* mc = new TFile("/Users/mohammadmobassirameen/Desktop/BcToBsPi-analysis/BDT_BcToBsPi/bdtsamples/SIGNAL_forBDT_Training.root");
   //TFile* mc = new TFile("/Users/mohammadmobassirameen/Desktop/BcToBsPi-analysis/BDT_BcToBsPi/bdtsamples/SIGNAL_forBDT_Application.root");
   TTree* sigtr_in = (TTree*)(mc->Get("TreeS"));
    
   //+++++++++++++++++
   TFile* data = new TFile("/Users/mohammadmobassirameen/Desktop/BcToBsPi-analysis/BDT_BcToBsPi/bdtsamples/BKG_ForBDT_Training.root");
   //TFile* data = new TFile("/Users/mohammadmobassirameen/Desktop/BcToBsPi-analysis/BDT_BcToBsPi/bdtsamples/Data_ForBDT_Application.root");
   //TTree* bkgtr_in = (TTree*)(data->Get("TreeB"));
   TTree* bkgtr_in = (TTree*)(data->Get("Tree"));

   // Event loop
    
   // Prepare the event tree
   // - Here the variable names have to corresponds to your tree
   // - You can use the same variables as above which is slightly faster,
   // but of course you can use different ones and the copy the values inside the event loop
   //
   std::cout << "--- Select signal sample" << std::endl;
   Float_t userVar1, userVar2, userVar3, userVar4, userVar5, userVar6, userVar7, userVar8, userVar9, userVar10, userVar11, userVar12, userVar13, userVar14, userVar15;

    
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    sigtr_in->SetBranchAddress( "Bcmass", &userVar1);
    sigtr_in->SetBranchAddress( "Bcpt", &userVar2);
    sigtr_in->SetBranchAddress( "Bceta", &userVar3);
    sigtr_in->SetBranchAddress( "Bcphi", &userVar4);
    sigtr_in->SetBranchAddress( "Bsmass", &userVar5);
    sigtr_in->SetBranchAddress( "Phimass", &userVar6);
    sigtr_in->SetBranchAddress( "Pionpt", &userVar7);
    sigtr_in->SetBranchAddress( "Kmpt", &userVar8);
    sigtr_in->SetBranchAddress( "Kppt", &userVar9);
    sigtr_in->SetBranchAddress( "Mumpt", &userVar10);
    sigtr_in->SetBranchAddress( "Muppt", &userVar11);
    sigtr_in->SetBranchAddress( "Bsvtxcl", &userVar12);
    sigtr_in->SetBranchAddress( "Bc_pvip", &userVar13);
    sigtr_in->SetBranchAddress( "Bclxy", &userVar14);
    sigtr_in->SetBranchAddress( "Bccosalphaxy", &userVar15);
    
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    bkgtr_in->SetBranchAddress( "Bcmass", &userVar1);
    bkgtr_in->SetBranchAddress( "Bcpt", &userVar2);
    bkgtr_in->SetBranchAddress( "Bceta", &userVar3);
    bkgtr_in->SetBranchAddress( "Bcphi", &userVar4);
    bkgtr_in->SetBranchAddress( "Bsmass", &userVar5);
    bkgtr_in->SetBranchAddress( "Phimass", &userVar6);
    bkgtr_in->SetBranchAddress( "Pionpt", &userVar7);
    bkgtr_in->SetBranchAddress( "Kmpt", &userVar8);
    bkgtr_in->SetBranchAddress( "Kppt", &userVar9);
    bkgtr_in->SetBranchAddress( "Mumpt", &userVar10);
    bkgtr_in->SetBranchAddress( "Muppt", &userVar11);
    bkgtr_in->SetBranchAddress( "Bsvtxcl", &userVar12);
    bkgtr_in->SetBranchAddress( "Bc_pvip", &userVar13);
    bkgtr_in->SetBranchAddress( "Bclxy", &userVar14);
    bkgtr_in->SetBranchAddress( "Bccosalphaxy", &userVar15);
    
    // Efficiency calculator for cut method
    Int_t    nSelCutsGA = 0;
    Double_t effS       = 0.7;

    // output file which will contain the BDT response of each event:
    TFile *targetS  = new TFile( "TMVAApplication_OUTPUT/TMVAppOutputS_v2_25Jun2022.root","RECREATE" );
    TTree *treeS    = new TTree("treeS","sig tree");

    treeS->Branch("Bcmass", &userVar1);
    treeS->Branch("Bcpt", &userVar2);
    treeS->Branch("Bceta", &userVar3);
    treeS->Branch("Bcphi", &userVar4);
    treeS->Branch("Bsmass", &userVar5);
    treeS->Branch("Phimass", &userVar6);
    treeS->Branch("Pionpt", &userVar7);
    treeS->Branch("Kmpt", &userVar8);
    treeS->Branch("Kppt", &userVar9);
    treeS->Branch("Mumpt", &userVar10);
    treeS->Branch("Muppt", &userVar11);
    treeS->Branch("Bsvtxcl", &userVar12);
    treeS->Branch("Bc_pvip", &userVar13);
    treeS->Branch("Bclxy", &userVar14);
    treeS->Branch("Bccosalphaxy", &userVar15);

    
    //Float_t BDT_sig;//, BDTG_sig;
    //treeS->Branch("BDT_sig", &BDT_sig);
    //treeS->Branch("BDTG", &BDTG_bkg);
    Float_t Bdt;
    treeS->Branch("Bdt", &Bdt);
    
    std::cout << "--- Processing: " << sigtr_in->GetEntries() << " signal events" << std::endl;
    
    for (Long64_t ievt=0; ievt<sigtr_in->GetEntries(); ievt++) {
     
      if (ievt%10000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;

        sigtr_in->GetEntry(ievt);

        var1  = userVar2;
        var2  = userVar14;
        var3  = userVar15;
        var4  = userVar12;
        var5  = userVar13;
        var6  = userVar3;
        var7  = userVar1;
        var89  = TMath::Max(userVar8, userVar9);
        var1011 = TMath::Max(userVar10, userVar11);
        var12 = userVar4;
        var13 = userVar5;
        var14 = userVar6;
        var15 = userVar7;
     

       //BDT_sig = reader->EvaluateMVA( "BDT method");
        Bdt = reader->EvaluateMVA( "BDT method");
      //BDTG_sig = reader->EvaluateMVA( "BDTG method");
      treeS->Fill();

      if (Use["BDT"          ])   histBdt    ->Fill( reader->EvaluateMVA( "BDT method"           ) );
      //if (Use["BDTG"         ])   histBdtG   ->Fill( reader->EvaluateMVA( "BDTG method"          ) );
   }

  
   std::cout << "--- End of signal event loop: " << std::endl; 

   treeS->Write();


   if (Use["BDT"          ])   histBdt    ->Write();
   //if (Use["BDTG"         ])   histBdtG   ->Write();


   targetS->Close();

   std::cout << "--- Created root file: \"TMVAppOutputS_v1_25Jun2022.root\" containing the MVA output histograms" << std::endl;



   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

   TFile *targetB  = new TFile( "TMVAApplication_OUTPUT/TMVAppOutputB_v2_25Jun2022.root","RECREATE" );
   TTree *treeB    = new TTree("treeB","bkg tree");
    
   treeB->Branch("Bcmass", &userVar1);
   treeB->Branch("Bcpt", &userVar2);
   treeB->Branch("Bceta", &userVar3);
   treeB->Branch("Bcphi", &userVar4);
   treeB->Branch("Bsmass", &userVar5);
   treeB->Branch("Phimass", &userVar6);
   treeB->Branch("Pionpt", &userVar7);
   treeB->Branch("Kmpt", &userVar8);
   treeB->Branch("Kppt", &userVar9);
   treeB->Branch("Mumpt", &userVar10);
   treeB->Branch("Muppt", &userVar11);
   treeB->Branch("Bsvtxcl", &userVar12);
   treeB->Branch("Bc_pvip", &userVar13);
   treeB->Branch("Bclxy", &userVar14);
   treeB->Branch("Bccosalphaxy", &userVar15);
                                                          
   //Float_t BDT_bkg;//, BDTG_bkg;
   //treeB->Branch("BDT_bkg", &BDT_bkg);
   //treeB->Branch("BDTG", &BDTG_bkg);
    
   //Float_t Bdt;
   treeB->Branch("Bdt", &Bdt);

   std::cout << "--- Processing: " << bkgtr_in->GetEntries() << " background events" << std::endl;
   //TStopwatch sw;
   //sw.Start();
   for (Long64_t ievt=0; ievt<bkgtr_in->GetEntries(); ievt++) {

     if (ievt%10000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;
     //if (ievt==1000) continue;

     bkgtr_in->GetEntry(ievt);
       
     var1  = userVar2;
     var2  = userVar14;
     var3  = userVar15;
     var4  = userVar12;
     var5  = userVar13;
     var6  = userVar3;
     var7  = userVar1;
     var89  = TMath::Max(userVar8, userVar9);
     var1011 = TMath::Max(userVar10, userVar11);
     var12 = userVar4;
     var13 = userVar5;
     var14 = userVar6;
     var15 = userVar7;

     //BDT_bkg = reader->EvaluateMVA( "BDT method");
      Bdt = reader->EvaluateMVA( "BDT method");
     //BDTG_bkg = reader->EvaluateMVA( "BDTG method");

     treeB->Fill();

     if (Use["BDT"          ])   histBdt    ->Fill( reader->EvaluateMVA( "BDT method"           ) );
     //if (Use["BDTG"         ])   histBdtG   ->Fill( reader->EvaluateMVA( "BDTG method"          ) );
   }

   std::cout << "--- End of background event loop: " << std::endl; 

   treeB->Write();


   if (Use["BDT"          ])   histBdt    ->Write();
   //if (Use["BDTG"         ])   histBdtG   ->Write();


   targetB->Close();

   std::cout << "--- Created root file: \"TMVAppOutputB_v2_25Jun2022.root\" containing the MVA output histograms" << std::endl;

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
   TMVAClassificationApplication(methodList);
   return 0;
}

