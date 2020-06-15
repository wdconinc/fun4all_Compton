TH1D *hRef;
double refFunc(double *x, double *par);

void drawSeg(string fnm="o_f4a_e18GeV_g532nm_5e6_withWghts_anaTruthProc.root"){
  TFile *fin=TFile::Open(fnm.c_str(),"READ");
  hRef = (TH1D*)fin->Get("yElec_Seg8_Wght2_Proc");

  string opdf = fnm.substr(fnm.find_last_of("/")+1,fnm.find_last_of(".")-fnm.find_last_of("/")-1) + string("_Seg.pdf");
  auto *c1=new TCanvas("c1","c1",1800,1000);
  c1->Print(Form("%s[",opdf.c_str()),"pdf");
  TF1 *fct = new TF1("fRef",refFunc,-0.05,0.05,1);

  int cls[8]  = {1,1,1,2,3,4,7,6};
  int draw[8] = {1,1,1,1,1,1,1,1};
  for(int i=0;i<8;i++){
    if(!draw[i]) continue;
    TH1D *h=(TH1D*)fin->Get(Form("yElec_Seg%d_Wght2_Proc",i));
    h->SetLineColor(cls[i]);
    // if(i==1){
    h->GetXaxis()->SetRangeUser(-0.04,0.04);
    h->GetYaxis()->SetRangeUser(-0.25,0.25);
    h->DrawCopy();
    // }else
    //   h->DrawCopy("same");

    gPad->SetGridx(1);
    gPad->SetGridx(1);
    fct->SetParameter(0,1);
    h->Scale(0.73);
    h->Fit("fRef","Q");
    cout<<h->GetTitle()<<" "<<fct->GetParameter(0)*100<<" pm "<<fct->GetParError(0)*100<<endl;
    c1->Print(opdf.c_str(),"pdf");
  }

  TH1D *h1=(TH1D*)fin->Get("yElec_Seg5_Wght2_Proc");
  TH1D *h2=(TH1D*)fin->Get("yElec_Seg4_Wght2_Proc");
  double uncer100 = h2->GetBinError(9)/h2->GetBinContent(9);
  double uncer050 = sqrt(pow(h1->GetBinError(17)/h1->GetBinContent(17),2) + pow(h1->GetBinError(18)/h1->GetBinContent(18),2));
  cout<<"100 50 "<<uncer100<<" "<<uncer050<<endl;

  c1->Print(Form("%s]",opdf.c_str()),"pdf");

  fin->Close();
}

double refFunc(double *x, double *par){
  double norm = par[0];
  double xVal = x[0] + par[1];
  int bx = hRef->GetXaxis()->FindBin(xVal);

  // cout<<x[0] <<" "<<x[1]<<" | "<<par[0]<<" "<<par[1]<<" "<<par[2]<<endl;
  // cout<<xVal<<" "<<bx<<" "<<yVal<<" "<<by<<" "<<hRef->GetBinContent(bx,by)<<endl<<endl;
  // std::cin.ignore();

  return norm*hRef->GetBinContent(bx);
}
