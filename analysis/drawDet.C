#include "anaConst.h"

bool verbosity = false;
//bool verbosity = true;
TFile *fout;

void drawDet(string fnm="/phenix/spin/phnxsp01/ciprian/eic/comptonOut/o_f4a_e18GeV_g532nm_5e6_withWghts_anaTruth.root"){

  string foutNm = fnm.substr(fnm.find_last_of("/")+1,fnm.find_last_of(".")-fnm.find_last_of("/")-1) + string("Det.root");
  fout=new TFile(foutNm.c_str(),"RECREATE");
  TFile *fin=TFile::Open(fnm.c_str());
  for(int i=0;i<nPart;i++)
    for(int j=0;j<nDet;j++)
      for(int k=0;k<nWght;k++){
	xy[i][j][k]=(TH2D*)fin->Get(Form("xy_%s_Zpos%d_Wght%d",partN[i].c_str(),j,k));
	if(!xy[i][j][k])
	   cout<<"could not get "<<i<<" "<<j<<" "<<k<<" "<<Form("xy_%s_Zpos%d_Wght%d",partN[i].c_str(),j,k)<<endl;
      }

  double xPadSize = 0.5;//cm
  double yPadSize = 0.005;//cm

  TH2D *eAsym=(TH2D*)xy[1][5][2]->Clone("eAvgAsym");
  TH2D *uXsec=(TH2D*)xy[1][5][1]->Clone("eAvgAsym");

  int nBinsX = eAsym->GetXaxis()->GetNbins();
  int nBinsY = eAsym->GetYaxis()->GetNbins();
  int rBinX = int( (eAsym->GetXaxis()->GetBinCenter(nBinsX) + eAsym->GetXaxis()->GetBinWidth(nBinsX)/2 -
		    eAsym->GetXaxis()->GetBinCenter(1)      + eAsym->GetXaxis()->GetBinWidth(1)/2      )/xPadSize);
  int rBinY = int( (eAsym->GetYaxis()->GetBinCenter(nBinsX) + eAsym->GetYaxis()->GetBinWidth(nBinsX)/2 -
		    eAsym->GetYaxis()->GetBinCenter(1)      + eAsym->GetYaxis()->GetBinWidth(1)/2      )/yPadSize);

  if(rBinX%2==1) rBinX++;
  if(rBinY%2==1) rBinY++;

  double xRange[2], yRange[2];
  xRange[0] = eAsym->GetXaxis()->GetBinCenter(int(nBinsX/2)) - rBinX/2 *xPadSize;
  xRange[1] = eAsym->GetXaxis()->GetBinCenter(int(nBinsX/2)) + rBinX/2 *xPadSize;

  yRange[0] = eAsym->GetYaxis()->GetBinCenter(int(nBinsY/2)) - rBinY/2 *yPadSize;
  yRange[1] = eAsym->GetYaxis()->GetBinCenter(int(nBinsY/2)) + rBinY/2 *yPadSize;

  TH2D *detA =  new TH2D("detA","average asymmetry;x[cm];y[cm]",
			 rBinX,xRange[0],xRange[1],
			 rBinY,yRange[0],yRange[1]);
  TH2D *detS =  new TH2D("detS","average unpolarized xSection;x[cm];y[cm]",
			 rBinX,xRange[0],xRange[1],
			 rBinY,yRange[0],yRange[1]);
  
  for(int i=1;i<=rBinX;i++)
    for(int j=1;j<=rBinY;j++){
      double xL = detA->GetXaxis()->GetBinCenter(i) - detA->GetXaxis()->GetBinWidth(i)/2;
      double xH = detA->GetXaxis()->GetBinCenter(i) + detA->GetXaxis()->GetBinWidth(i)/2;
      double yL = detA->GetYaxis()->GetBinCenter(j) - detA->GetYaxis()->GetBinWidth(j)/2;
      double yH = detA->GetYaxis()->GetBinCenter(j) + detA->GetYaxis()->GetBinWidth(j)/2;
      int bxl = eAsym->GetXaxis()->FindBin(xL);
      int bxh = eAsym->GetXaxis()->FindBin(xH);
      int byl = eAsym->GetYaxis()->FindBin(yL);
      int byh = eAsym->GetYaxis()->FindBin(yH);
      double aV = eAsym->Integral(bxl,bxh,byl,byh);
      double sV = uXsec->Integral(bxl,bxh,byl,byh);
      if(sV>0){
	detA->SetBinContent(i,j,aV/sV);
	detS->SetBinContent(i,j,sV);
      }
    }

  auto *c1=new TCanvas();
  c1->Divide(2);
  c1->cd(1);
  eAsym->Divide(uXsec);
  eAsym->DrawCopy("colz");
  c1->cd(2);
  detA->DrawCopy("colz");

  fout->cd();
  detA->Write();
  detS->Write();

  fout->Close();
}
  
