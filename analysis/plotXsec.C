void plotXsec(){
  string fnms[3]={
    "o_f4a_e5GeV_g532nm_5e6_withWghts_anaTruthProc.root",
    "o_f4a_e12GeV_g532nm_5e6_withWghts_anaTruthProc.root",
    "o_f4a_e18GeV_g532nm_5e6_withWghts_anaTruthProc.root"
  };

  int cls[3]={2,1,4};
  for(int i=0;i<3;i++){
    TFile *fin = TFile::Open(fnms[i].c_str(),"READ");
    //TH1D *h=(TH1D*)fin->Get("uX");
    TH1D *h=(TH1D*)fin->Get("aud_xy_g_Zpos5_Wght2");
    //h->Scale(5000);
    h->SetLineColor(cls[i]);
    h->SetLineWidth(3);
    if(i==0){
      //h->GetYaxis()->SetRangeUser(0,1);
      h->GetYaxis()->SetRangeUser(-0.2,0.2);
      //h->GetYaxis()->SetTitle("d#sigma/d#rho [barn]");
      h->GetYaxis()->SetTitle("A_{trans}");
      h->GetYaxis()->CenterTitle();
      h->GetYaxis()->SetTitleSize(0.05);
      h->GetYaxis()->SetTitleOffset(0.7);
      h->GetXaxis()->CenterTitle();
      h->GetXaxis()->SetTitleSize(0.05);
      h->GetXaxis()->SetTitleOffset(0.7);
      h->DrawCopy("h&&c");
    }else
      h->DrawCopy("same&&h&&c");
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    fin->Close();
  }
}
