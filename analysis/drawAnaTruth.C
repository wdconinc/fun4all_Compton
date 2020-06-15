#include "anaConst.h"
void drawXsec(TCanvas*);
void drawZpos(TCanvas*,int,int);
void drawWghts(TCanvas*,int);
void drawAUDatZ(TCanvas*,int,double);
void drawAUDblockSize(TCanvas*,int,double);
void drawFOMblockSize(TCanvas*,int,double);
void drawAUDallSum(TCanvas*,int,double);
void drawAUD(TCanvas*, int);
void sampleXsec(TCanvas*,int,double);
void process();

void calcAverageDistance(TH1D*,double);
double calcUD(TH2D*,TH2D*,double,double);
double calcFOM(TH2D*,double,double);

bool verbosity = false;
//bool verbosity = true;
TFile *fout;
const double eRange = 0.07;
const double gRange = 1;

void drawAnaTruth(string fnm="/phenix/spin/phnxsp01/ciprian/eic/comptonOut/o_f4a_e18GeV_g532nm_5e6_withWghts_anaTruth.root"){

  string foutNm = fnm.substr(fnm.find_last_of("/")+1,fnm.find_last_of(".")-fnm.find_last_of("/")-1) + string("Proc.root");
  fout=new TFile(foutNm.c_str(),"RECREATE");
  TFile *fin=TFile::Open(fnm.c_str());
  for(int i=0;i<nXsec;i++)
    hXsec[i] = (TH1D*)fin->Get(xSecNm[i].c_str());

  for(int k=0;k<nWght;k++)
    for(int j=0;j<nSeg;j++)
      yElec[k][j]=(TH1D*)fin->Get(Form("yElec_Seg%d_Wght%d",j,k));

  for(int i=0;i<nPart;i++)
    for(int j=0;j<nDet;j++)
      for(int k=0;k<nWght;k++){
	xy[i][j][k]=(TH2D*)fin->Get(Form("xy_%s_Zpos%d_Wght%d",partN[i].c_str(),j,k));
	if(!xy[i][j][k])
	   cout<<"could not get "<<i<<" "<<j<<" "<<k<<" "<<Form("xy_%s_Zpos%d_Wght%d",partN[i].c_str(),j,k)<<endl;
      }

  process();

  auto *c1=new TCanvas("xsec","xsec");
  drawXsec(c1);

  // auto *c2=new TCanvas("xSecZ","xSecZ");
  // drawZpos(c2,1,-1);
  // auto *c3=new TCanvas("asymZ","asymZ");
  // drawZpos(c3,2,1);
  // auto *c4=new TCanvas("energyZ","energyZ");
  // drawZpos(c4,3,1);
  // auto *c5=new TCanvas("EAZ","EAZ");
  // drawZpos(c5,4,3);

  auto *c6=new TCanvas("25mW","25mW");
  drawWghts(c6,5);

  auto *c7=new TCanvas("audZ","audZ");
  drawAUDatZ(c7,5,0);

  auto *c8=new TCanvas("audBlock","audBlock");
  drawAUDblockSize(c8,5,0);

  auto *c81=new TCanvas("FOMBlock","FOMBlock");
  drawFOMblockSize(c81,5,0);

  auto *c9=new TCanvas("asym","asym");
  drawAUD(c9,5);

  // auto *c10=new TCanvas("xSamp","xSamp");
  // sampleXsec(c10,5,0);
  // auto *c11=new TCanvas("xSampGblock","xSampGblock");
  // sampleXsec(c11,5,0.3);
  // auto *c12=new TCanvas("xSampEblock","xSampEblock");
  // sampleXsec(c12,5,0.1);

  fout->Close();
}

void process(){
  int scale[5]={0,1,1,3,1};
  int nW = nWght-1;
  double lumi = std::pow(365.29426,2); //such that for the 100um bin8 we get 1% uncert

  for(int j=0;j<nSeg;j++)
    for(int i=0;i<nW;i++){
      TH1D *yE = (TH1D*)yElec[i+1][j]->Clone(Form("%s_Proc",yElec[i+1][j]->GetName()));
      if(scale[i]>=0)
	yE->Divide(yElec[scale[i]][j]);
      if(i==1){
	TH1D *yS = (TH1D*)yElec[1][j]->Clone(Form("%s_sProc",yElec[1][j]->GetName()));
	//yS->Divide(yElec[0][j]);
	int nB = yE->GetXaxis()->GetNbins();

	for(int ii=1;ii<=nB;ii++){
	  double s = yS->GetBinContent(ii);
	  if(s==0) continue;
	  double a = yE->GetBinContent(ii);
	  yE->SetBinError(ii, sqrt( (1-a*a)/(s*lumi) ) );
	}
      }
      fout->cd();
      yE->Write();
    }
}
  
void drawXsec(TCanvas *c){
  c->Divide(2);
  gStyle->SetOptStat("iou");
  for(int i=0;i<nXsec;i++){
    c->cd(i+1);
    cout<<hXsec[i]->GetName()<<" xsec integral "<<hXsec[i]->Integral()<<endl;
    hXsec[i]->DrawCopy();
    fout->cd();
    hXsec[i]->Write();
    gPad->SetGridx();
    gPad->SetGridy();
  }    
}

void drawZpos(TCanvas *c,int wght, int scale){
  c->Divide(nDet,2);
  gStyle->SetOptStat(0);

  for(int i=0;i<nDet;i++){
    c->cd(i+1);
    TH2D *he = (TH2D*)xy[0][i][wght]->Clone(Form("he_%s",xy[0][i][wght]->GetName()));
    if(scale>=0)
      he->Divide(xy[0][i][scale]);
    he->DrawCopy("colz");
    gPad->SetGridx();
    gPad->SetGridy();

    c->cd(i+1+nDet);
    TH2D *hg = (TH2D*)xy[1][i][wght]->Clone(Form("hg_%s",xy[1][i][wght]->GetName()));
    if(scale>=0)
      hg->Divide(xy[1][i][scale]);
    hg->DrawCopy("colz");
    gPad->SetGridx();
    gPad->SetGridy();
    delete he;
    delete hg;
  }
}

void drawWghts(TCanvas *c,int zpos){
  int nW = nWght-1;
  c->Divide(nW,2);
  gStyle->SetOptStat(0);
  int scale[5]={0,1,1,3,1};

  for(int i=0;i<nW;i++){
    c->cd(i+1);
    TH2D *he = (TH2D*)xy[0][zpos][i+1]->Clone(Form("hg_%s",xy[0][zpos][i+1]->GetName()));
    if(scale[i]>=0)
      he->Divide(xy[0][zpos][scale[i]]);
    if(i==5)
      he->Scale(100/he->Integral());
    he->DrawCopy("colz");
    fout->cd();
    he->Write();
    gPad->SetGridx();
    gPad->SetGridy();

    c->cd(i+1+nW);
    //cout<<zpos<<" "<<i+1<<endl;
    TH2D *hg = (TH2D*)xy[1][zpos][i+1]->Clone(Form("he_%s",xy[1][zpos][i+1]->GetName()));
    if(scale[i]>=0)
      hg->Divide(xy[1][zpos][scale[i]]);
    if(i==5)
      hg->Scale(100/hg->Integral());
    hg->DrawCopy("colz");
    fout->cd();
    hg->Write();
    gPad->SetGridx();
    gPad->SetGridy();
    delete he,hg;
  }
}

double calcUD(TH2D *h, TH2D *nHits, double blockSize=0, double offset=0){
  if(verbosity){
    cout<<"calcUD blockSize offset "<<blockSize<<" "<<offset<<endl;
    cout<<h->GetTitle()<<" | "<<nHits->GetTitle()<<endl;
  }

  int yHL = h->GetYaxis()->FindBin(offset+blockSize);
  int yLH = h->GetYaxis()->FindBin(offset-blockSize);

  int nBx = h->GetXaxis()->GetNbins();
  int nBy = h->GetYaxis()->GetNbins();
  if( yHL > nBy || yLH<1 ) return 0;

  if(verbosity)
    cout<<"calcUD "<<yHL<<" <> "<<offset+blockSize<<" "<<yLH<<" <> "
	<<offset-blockSize<<endl;

  int nr[2]={0,0};
  double sum[2]={0,0};
  nr[0] = nHits->Integral(1,nBx,yHL,nBy);
  nr[1] = nHits->Integral(1,nBx,1,yLH);
  sum[0] = h->Integral(1,nBx,yHL,nBy);
  sum[1] = h->Integral(1,nBx,1,yLH);

  if(nr[0]+nr[1]==0 && verbosity) cout<<"Warning calcUD: sum on bin content is 0"<<endl;
  if(nr[0]==0 || nr[1]==0) {
    if(verbosity){
      cout<<"calcUD 0: "<<nr[0]<<" "<<nr[1]<<endl;
      cin.ignore();
    }
    return 0;
  }
  if(verbosity){
    cout<<nr[0]<<" "<<nr[1]<<" "<<sum[0]<<" "<<sum[1]<<endl;
    cout<<"calcUD"<<endl;
    cin.ignore();
  }
  return abs(sum[0]/nr[0] - sum[1]/nr[1])/2;
}

double calcFOM(TH2D *h, double blockSize=0, double offset=0){
  if(verbosity){
    cout<<"calcFOM blockSize offset "<<blockSize<<" "<<offset<<endl;
    cout<<h->GetTitle()<<" | "<<endl;
  }

  int yHL = h->GetYaxis()->FindBin(offset+blockSize);
  int yLH = h->GetYaxis()->FindBin(offset-blockSize);

  int nBx = h->GetXaxis()->GetNbins();
  int nBy = h->GetYaxis()->GetNbins();
  if( yHL > nBy || yLH<1 ) return 0;

  if(verbosity)
    cout<<"calcUD "<<yHL<<" <> "<<offset+blockSize<<" "<<yLH<<" <> "
	<<offset-blockSize<<endl;

  double sum[2]={0,0};
  sum[0] = h->Integral(1,nBx,yHL,nBy);
  sum[1] = h->Integral(1,nBx,1,yLH);

  if(verbosity){
    cout<<" "<<sum[0]<<" "<<sum[1]<<endl;
    cout<<"calcFOM"<<endl;
    cin.ignore();
  }
  return abs(sum[0] - sum[1])/2;
}

void drawAUDallSum(TCanvas *c,int zpos, double block){
  
  double binLimit[2]={gRange,eRange};
  c->Divide(2,2);
  for(int i=0;i<2;i++){
    const int nbins = 400;
    TH1D *aud=new TH1D("aud","; y position [cm];asymmetry",nbins,-binLimit[i],binLimit[i]);
    for(int j=0;j<2;j++){
      aud->Reset();
      aud->SetTitle(Form("aud Sum %s",xy[i][zpos][2+j*2]->GetTitle()));
      for(int k=1;k<=nbins;k++){
	if(verbosity)
	  cout<<"drawAUDallSum y= "<<aud->GetBinCenter(k)<<endl;
	double val = calcUD(xy[i][zpos][2+j*2],xy[i][zpos][1+j*2],block,aud->GetBinCenter(k));
	if(val!=0){
	  if(verbosity)
	    cout<<i<<" "<<j<<" "<<aud->GetBinCenter(k)<<" "<<val<<endl;
	  aud->SetBinContent(k,val);
	}else{
	  if(verbosity)
	    cout<<i<<" ! "<<j<<" "<<aud->GetBinCenter(k)<<" "<<val<<endl;
	}
      }
      c->cd(i*2+j+1);
      aud->DrawCopy();
      gPad->SetGridx();
      gPad->SetGridy();
    }
    delete aud;
  }
}

void drawAUDatZ(TCanvas *c,int zpos, double block){
  
  double binLimit[2]={gRange,eRange};
  c->Divide(2,2);
  for(int i=0;i<2;i++){
    const int nbins = 800;
    TH1D *aud=new TH1D("audZ","; y position [cm];asymmetry",nbins,-binLimit[i],binLimit[i]);
    for(int j=0;j<2;j++){
      aud->Reset();
      aud->SetTitle(Form("aud %s",xy[i][zpos][2+j*2]->GetTitle()));
      aud->SetName(Form("audZ_%s",xy[i][zpos][2+j*2]->GetName()));
      for(int k=1;k<=nbins;k++){
	if(verbosity)
	  cout<<"drawAUDatZ y= "<<aud->GetBinCenter(k)<<endl;
	double val = calcUD(xy[i][zpos][2+j*2],xy[i][zpos][1+j*2],block,aud->GetBinCenter(k));
	if(val!=0){
	  if(verbosity)
	    cout<<i<<" "<<j<<" "<<aud->GetBinCenter(k)<<" "<<val<<endl;
	  aud->SetBinContent(k,val);
	}else{
	  if(verbosity)
	    cout<<i<<" ! "<<j<<" "<<aud->GetBinCenter(k)<<" "<<val<<endl;
	}
      }
      c->cd(i*2+j+1);
      fout->cd();
      aud->Write();
      aud->DrawCopy();
      gPad->SetGridx();
      gPad->SetGridy();
    }
    delete aud;
  }
}

void drawAUDblockSize(TCanvas *c,int zpos, double pos){
  
  double binLimit[2]={gRange,eRange};
  c->Divide(2,2);
  for(int i=0;i<2;i++){
    const int nbins = 400;
    TH1D *aud=new TH1D("audB","; blockSize [cm];asymmetry",nbins,0,binLimit[i]);
    for(int j=0;j<2;j++){
      aud->Reset();
      aud->SetTitle(Form("aud center=%4.2f %s",pos,xy[i][zpos][2+j*2]->GetTitle()));
      for(int k=1;k<=nbins;k++){
	if(verbosity)
	  cout<<"drawAUDatZ y= "<<aud->GetBinCenter(k)<<endl;
	double val = calcUD(xy[i][zpos][2+j*2],xy[i][zpos][1+j*2],aud->GetBinCenter(k),pos);
	if(val!=0){
	  if(verbosity)
	    cout<<i<<" "<<j<<" "<<aud->GetBinCenter(k)<<" "<<val<<endl;
	  aud->SetBinContent(k,val);
	}else{
	  if(verbosity)
	    cout<<i<<" ! "<<j<<" "<<aud->GetBinCenter(k)<<" "<<val<<endl;
	}
      }
      c->cd(i*2+j+1);
      aud->DrawCopy();
      gPad->SetGridx();
      gPad->SetGridy();
    }
    delete aud;
  }
}

void drawFOMblockSize(TCanvas *c,int zpos, double pos){
  
  double binLimit[2]={gRange,eRange};
  c->Divide(2);
  for(int i=0;i<2;i++){
    const int nbins = 400;
    TH1D *aud=new TH1D("audB","; blockSize [cm];FOM [au]",nbins,0,binLimit[i]);
    aud->Reset();
    aud->SetTitle(Form("FOM center=%4.2f %s",pos,xy[i][zpos][5]->GetTitle()));
    for(int k=1;k<=nbins;k++){
      if(verbosity)
	cout<<"drawAUDatZ y= "<<aud->GetBinCenter(k)<<endl;
      TH2D *htemp = (TH2D*)xy[i][zpos][5]->Clone("htemp");
      htemp->Divide(xy[i][zpos][1]);
      double val = calcFOM(htemp,aud->GetBinCenter(k),pos);
      if(val!=0){
	if(verbosity)
	  cout<<i<<"  "<<aud->GetBinCenter(k)<<" "<<val<<endl;
	aud->SetBinContent(k,val);
      }else{
	if(verbosity)
	  cout<<i<<" ! "<<aud->GetBinCenter(k)<<" "<<val<<endl;
      }
      c->cd(i+1);
      //aud->Scale(100/aud->Integral());
      aud->DrawCopy();
      gPad->SetGridx();
      gPad->SetGridy();
    }
    delete aud;
  }
}

void drawAUD(TCanvas *c,int zpos){
  
  double binLimit[2]={gRange,eRange};
  c->Divide(2,2);
  for(int i=0;i<2;i++){
    const int nbins = 400;
    TH1D *aud=new TH1D("aud","; vertical position [cm];asymmetry",nbins,-binLimit[i],binLimit[i]);
    for(int j=0;j<2;j++){
      aud->Reset();
      aud->SetTitle(Form("aud %s",xy[i][zpos][2+j*2]->GetTitle()));
      aud->SetName(Form("aud_%s",xy[i][zpos][2+j*2]->GetName()));
      const int nBinx = xy[i][zpos][2+j*2]->GetXaxis()->GetNbins();
      for(int k=1;k<=nbins;k++){
	if(verbosity)
	  cout<<"drawAUDatZ y= "<<aud->GetBinCenter(k)<<endl;
	int lb = xy[i][zpos][2+j*2]->GetYaxis()->FindBin( aud->GetBinCenter(k)-aud->GetBinWidth(k) ); 
	int hb = xy[i][zpos][2+j*2]->GetYaxis()->FindBin( aud->GetBinCenter(k)+aud->GetBinWidth(k) ); 
	double top = xy[i][zpos][2+j*2]->Integral(1,nBinx,lb,hb);
	double bot = xy[i][zpos][1+j*2]->Integral(1,nBinx,lb,hb);

	if(bot!=0)
	  aud->SetBinContent(k,top/bot);
      }
      c->cd(i*2+j+1);
      fout->cd();
      aud->Write();
      aud->DrawCopy();
      gPad->SetGridx();
      gPad->SetGridy();
    }
    delete aud;
  }
}

void calcAverageDistance(TH2D *h,double binLimit){
  TH1D *hd= new TH1D("hd",Form("counts %s; distance between two events [cm]",h->GetTitle()),
		     200,0,binLimit);
  
  for(int i=0;i<10000;i++){
    double x1,y1,x2,y2;
    h->GetRandom2(x1,y1);
    h->GetRandom2(x2,y2);
    hd->Fill(sqrt(pow(x1-x2,2) + pow(y1-y2,2)));
  }
  gStyle->SetOptStat("eM");
  hd->DrawCopy();
  gPad->SetGridx();
  gPad->SetGridy();
  //gPad->SetLogy();
  delete hd;
}

void sampleXsec(TCanvas *c,int zpos, double blockSize){
  
  c->Divide(2,2);

  double binLimit[2]={30,0.5};
  for(int i=0;i<2;i++){
    TH2D *hTemp = (TH2D*)xy[i][zpos][1]->Clone("hTemp");
    hTemp->Divide(xy[i][zpos][0]);
    TH2D *hT = (TH2D*)hTemp->Clone("hT");
    int nBy = hTemp->GetYaxis()->GetNbins();
    int nBx = hTemp->GetXaxis()->GetNbins();
    if(binLimit[i]<2*blockSize) continue;

    for(int j=1;j<=nBy;j++){
      if(blockSize>0 && (hTemp->GetYaxis()->GetBinCenter(j) < blockSize && hTemp->GetYaxis()->GetBinCenter(j)> -blockSize))
	for(int k=1;k<=nBx;k++)
	  hTemp->SetBinContent(k,j,0);
      }
      c->cd(i*2+1);
      int bin0 = hTemp->GetYaxis()->FindBin(0.);
      double tot = hT->Integral(1,nBx,1,bin0);
      double left = hTemp->Integral(1,nBx,1,bin0);
      cout<<hTemp->GetTitle()<<" total int: "<<tot<<" |left over int: "<<left<<" |l/t: "<<left/tot<<endl;
      calcAverageDistance(hTemp,binLimit[i]);
      c->cd(i*2+2);
      hTemp->DrawCopy("colz");

      delete hTemp,hT;
  }
}
