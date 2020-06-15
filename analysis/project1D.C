TH2D *hRef2D,*hRefXsec;
TH1D *hRef;
TF1 *fR;
double refFunc(double*,double*);
void fitDet(TH2D*,double,double,double);
TH1D *getDetResponse(double,double,double);
TH1D *projectY(TH2D*);

void project1D(string fnm="o_f4a_e18GeV_g532nm_5e6_withWghts_anaTruthProc.root"){
  TFile *fin=TFile::Open(fnm.c_str(),"READ");
  hRef2D=(TH2D*)fin->Get("he_xy_e_Zpos5_Wght2");
  hRefXsec=(TH2D*)fin->Get("he_xy_e_Zpos5_Wght1");

  double pol = 1.;
  double xShift = 0.;
  double yShift = 0.;

  auto *c2=new TCanvas("c2","1D sample");

  TH1D *hRef = projectY(hRef2D);
  TH1D *hDet1 = getDetResponse(pol,yShift,0.005);
  
  c2->cd();
  hRef->SetLineColor(2);
  hRef->SetLineWidth(2);
  hRef->DrawCopy("h");
  hDet1->DrawCopy("same");

  fin->Close();
}

void fitDet(TH1D *hFit, double trueNorm, double trueY){

  TF1 *fR = new TF1("fR",refFunc, -0.05, 0.05 ,2);
  //fR->SetParameter(0,1);
  fR->SetParameters(1,0);
  fR->SetParLimits(0,0,1);
  fR->SetParLimits(1,-0.02,0.02);
  hFit->Fit("fR","");
  cout<<"Norm: "<<trueNorm<<" >>>>>> "<<fR->GetParameter(0)<<" pm "<<fR->GetParError(0)<<endl;
  cout<<"yOff: "<<trueY<<" >>>>>> "<<fR->GetParameter(1)<<" pm "<<fR->GetParError(1)<<endl;

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

TH1D *getDetResponse(double pol, double yShift, double yWidth){
  
  int nBinY = 0.1/yWidth;

  TH1D *hDet = new TH1D(Form("hDet_y%d",nBinY), "hdet",nBinY,-0.05,0.05);

  for(int i=0;i<100000;i++){
    double val = hRef->GetRandom() + yShift; 
    hDet->Fill(val,pol);
  }
  return hDet;
}

TH1D *projectY(TH2D *h){
  int nBinX = h->GetXaxis()->GetNbins();
  int nBinY = h->GetYaxis()->GetNbins();
  double yL = h->GetYaxis()->GetBinLowEdge(1);
  double yH = h->GetYaxis()->GetBinLowEdge(nBinY);

  TH1D *h1=new TH1D(Form("pY_%s",h->GetName()),"y projection", nBinY,yL,yH);

  for(int i=1;i<=nBinY;i++){
    double sumA(0),sumW(0);
    for(int j=1;j<=nBinX;j++){
      double val = h->GetBinContent(j,i);
      double dval = h->GetBinError(j,i);
      if(dval==0) continue;
      sumA += val/(dval*dval);
      sumW += 1/(dval*dval);
    }
    if(sumW==0) continue;
    h1->SetBinContent(i,sumA/sumW);
    h1->SetBinError(i, sqrt(1/sumW));
  }

  return h1;
}
