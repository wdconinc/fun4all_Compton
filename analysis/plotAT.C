void plotAT(){
  string fnms[3]={
    "o_f4a_e5GeV_g532nm_5e6_withWghts_anaTruthProc.root",
    "o_f4a_e12GeV_g532nm_5e6_withWghts_anaTruthProc.root",
    "o_f4a_e18GeV_g532nm_5e6_withWghts_anaTruthProc.root"
  };

  string hT[3]={"beam energy 5GeV","beam energy 12GeV","beam energy 18GeV"};
  auto *c1=new TCanvas();
  c1->Divide(3,2);
  for(int i=0;i<3;i++){
    cout<<i<<endl;
    TFile *fin = TFile::Open(fnms[i].c_str(),"READ");
    c1->cd(i+1);    
    TH2D *h=(TH2D*)fin->Get("hg_xy_g_Zpos5_Wght2");
    h->SetTitle(Form("backscatered photon (%s)",hT[i].c_str()));
    //h->Scale(5000);
    h->GetYaxis()->SetTitle("vertical position [cm]");
    h->GetYaxis()->CenterTitle();
    h->GetYaxis()->SetTitleSize(0.05);
    h->GetYaxis()->SetTitleOffset(0.9);
    h->GetXaxis()->SetTitle("horizontal position [cm]");
    h->GetXaxis()->CenterTitle();
    h->GetXaxis()->SetTitleSize(0.05);
    h->GetXaxis()->SetTitleOffset(0.9);
    h->DrawCopy("colz");
    gPad->SetGridx(1);
    gPad->SetGridy(1);

    c1->cd(i+4);    
    TH2D *h2=(TH2D*)fin->Get("he_xy_e_Zpos5_Wght2");
    h2->SetTitle(Form("scattered electron (%s)",hT[i].c_str()));
    //h2->Scale(5000);
    h2->GetYaxis()->SetTitle("vertical position [cm]");
    h2->GetYaxis()->CenterTitle();
    h2->GetYaxis()->SetTitleSize(0.05);
    h2->GetYaxis()->SetTitleOffset(0.9);
    h2->GetXaxis()->SetTitle("horizontal position [cm]");
    h2->GetXaxis()->CenterTitle();
    h2->GetXaxis()->SetTitleSize(0.05);
    h2->GetXaxis()->SetTitleOffset(0.9);
    h2->DrawCopy("colz");
    gPad->SetGridx(1);
    gPad->SetGridy(1);

    fin->Close();
  }
}

