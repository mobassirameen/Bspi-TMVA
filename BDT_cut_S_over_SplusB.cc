// **********************************
// --- Original Author ---
// Niladri Sahoo
// Nov 14, 2018
// -----------------------
// Implemented By: Mohammad Mobassir Ameen
// Jun 3, 2022
// **********************************
  
//Script needs to go through different bdt cuts and record the S and B values to an array, errors too. Do this in a loop. Then needs to plot the value of S/sqrt(S+B) with errors versus bdt cut value. This outside the loop.
//*************************************


#include <sstream>



void BDT_cut_S_over_SplusB(){
    
  //const std::string filenameS("SIGNAL_forBDT_Scan.root");
  const std::string filenameS("/Users/mohammadmobassirameen/Desktop/BcToBsPi-analysis/BDT_BcToBsPi/TMVAApplication_OUTPUT/TMVAppOutputS_v1.root");
  //const std::string filenameS("/Users/mohammadmobassirameen/Desktop/BcToBsPi-analysis/BDT_BcToBsPi/TMVAApplication_OUTPUT/TMVAppOutputS_v2_25Jun2022.root");
  const std::string trnameS = "treeS";
  //const std::string filenameB("BKG_ForBDT_Scan.root");
  //const std::string filenameB("BKG_ForBDT_Scan_Data2018.root");
  const std::string filenameB("/Users/mohammadmobassirameen/Desktop/BcToBsPi-analysis/BDT_BcToBsPi/TMVAApplication_OUTPUT/TMVAppOutputB_v1.root");
  //const std::string filenameB("/Users/mohammadmobassirameen/Desktop/BcToBsPi-analysis/BDT_BcToBsPi/TMVAApplication_OUTPUT/TMVAppOutputB_v2_25Jun2022.root");
  const std::string trnameB = "treeB";
    

    
  TFile* fileS = TFile::Open( filenameS.c_str() );
  if( !fileS ) std::cout << "file " << filenameS << " does not exist" << std::endl;
  TTree* tree = (TTree*)fileS->Get( trnameS.c_str() );
  if( !tree ) std::cout << "tree " << trnameS << " does not exist" << std::endl;
    

  TFile* fileB = TFile::Open( filenameB.c_str() );
  if( !fileB ) std::cout << "file " << filenameB << " does not exist" << std::endl;
  TTree* tree1 = (TTree*)fileB->Get( trnameB.c_str() );
  if( !tree1 ) std::cout << "tree " << trnameB << " does not exist" << std::endl;
  

  const Int_t nBins = 100 ;
  //const Int_t nBins = 60 ;
    
  double bdt_cuts[nBins];
  double efficiencies2[nBins];
  double efficiencies2_error[nBins];
  

  double effvals[nBins];
    
  // ******************
  // loop starting here
  // ******************
  for(int i=0; i < nBins; i=i+1) {
    double step_size = double(2./nBins);
    cout << i << " step size = " << step_size << endl;
    //////////double cut_val = -1.0 + i*0.05;
      
    //double cut_val = -1.0 + step_size*i ;
  
      double cut_val = -0.5 + step_size*i ;
      //std::cout << "CUT VAL " << cut_val << std::endl;
      
    ///////////////double cut_val = -1.0; // testing
    bdt_cuts[i] = cut_val;
    std::cout << cut_val << std::endl;

    std::stringstream c;
    c << "Bdt" << " >= " << cut_val;

    const std::string cut = c.str();
        
  
    std::stringstream r;
    r << "Bdt" << " >= " << cut_val << " && (Bcmass >=5.80 && Bcmass<=6.10) || (Bcmass >= 6.38 && Bcmass <= 6.55)";
    const std::string cut2 = r.str();


    double integB_pre = tree1->GetEntries();
    double integB     = tree1->GetEntries(cut2.c_str());
  
    double MC_pre  = tree->GetEntries();
    double MC_post = tree->GetEntries(cut.c_str());
  
    //double SF = 5.420786e-04 ; // Scale factor for the case of BsToPhimumu
    double SF = 3.65878e-05 ;
    
    double MC_post_scaled = MC_post*SF ;

    double efficiency2 = (MC_post_scaled)/(sqrt(MC_post_scaled + integB));
      
    std::cout << "efficiency2 " << efficiency2 << std::endl;
  
    if (MC_post_scaled == 0 && integB == 0) efficiency2 = 0;
    efficiencies2[i] = efficiency2;
        
    
    std::cout << "B = " << integB << std::endl;
    std::cout << "S = " << MC_post_scaled << std::endl;
    //std::cout << "Scale Factor = " << SF << std::endl;
    //std::cout << "S_scaled = " << MC_post << "*" << SF << " = " << MC_post_scaled << std::endl;
    
    std::cout << "Significance = S/sqrt(S+B) = " << efficiency2 << std::endl;
    
    std::cout << "++++++++++++++++++++++++++++" << std::endl;

  }

    // find the maximum significance value (don't need to write many lines as below)
    double *i1;
    i1 = std::max_element(efficiencies2, efficiencies2 + nBins);
    std::cout << "max significance value  = " << *i1 << std::endl;


    double max_sign = -99;
    int maxCut = 99;
    for (int m = 0 ; m < nBins ; m++){
      if (efficiencies2[m] > max_sign){
    max_sign = efficiencies2[m];
    maxCut = m;
      }
    }

    //TArrow* ar = new TArrow(bdt_cuts[maxCut], 0, bdt_cuts[maxCut], max_sign, 5 ,"<|");
    //ar->Draw();

    std::cout << "max. significance value = " << max_sign << " for BDT cut value = " << bdt_cuts[maxCut] << std::endl;
       
 
    TCanvas *c2 = new TCanvas("c2", "",800,600);
    
    TGraph* graph2 = new TGraph(nBins, bdt_cuts, efficiencies2);
    
    graph2->SetTitle("Figure of Merit");
    //graph2->GetYaxis()->SetLabelSize(0.05);
    
    graph2->SetMarkerStyle(20);
    graph2->SetMarkerColor(kRed);
    
    graph2->GetXaxis()->SetTitle("#bf{BDT cut}");
    graph2->GetXaxis()->SetRangeUser(-0.5,0.25);
    graph2->GetYaxis()->SetTitle("#bf{S/#sqrt{S+B}}");
    
    graph2->Draw("APL");

    
    TArrow* ar = new TArrow(bdt_cuts[maxCut], 0, bdt_cuts[maxCut], max_sign, 0.03 ,"<|");
    ar->SetFillColor(2);
    ar->Draw();
    
    c2->SaveAs("SignificanceCurve_25Jun2022.pdf");
    //c2->SaveAs("SignificanceCurve.png");
    
}
