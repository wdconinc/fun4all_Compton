#include "anaConst.h"

TFile *fin,*fout;
TTree *t;
int tDetNr,tevNr,tPDGid,tTrackID,tParentID;
double tE,tX,tY,tZ,tpX,tpY,tpZ,tTime,txSecU,txSecP;

int readTree(string);
void initHisto();
void writeHisto(string);

void anaTruth(string finNm, double electronEnergy=18, double lambda=532){

  gLambda = lambda*1e-9;
  eEnergy = electronEnergy*1e9;
  cout<<gMaxEnergy<<" after recalcuation ";
  recalcMaxEnergy();
  cout<<gMaxEnergy<<endl;
  pid[11]=2;
  pid[22]=1;

  if(!readTree(finNm))
    return;
  initHisto();

  long nHits = t->GetEntries();
  int nEv = -1;
  double rho(-1);
  double uX(-1),pX(-1);
  int stepSize(10),stepV(1);
  for(int i=0;i<nHits;i++){
    t->GetEntry(i);

    if(tTrackID==2){
      rho = tE/gMaxEnergy;
      uX = txSecU;
      pX = txSecP;
    }

    if(tevNr > nEv){
      //cout<<nEv<<" "<<tevNr<<" "<<i<<endl;
      nEv = tevNr;
      if(rho>0){
	hXsec[0]->Fill(rho,uX);
	iXsec[0]->Fill(rho);
	hXsec[1]->Fill(rho,pX);
	iXsec[1]->Fill(rho);
	rho = -1;
      }
    }

    if(int(float(i)/nHits*100) > stepV ){
      cout<<"\tProcessed : "<<nEv<<" "<<float(i)/nHits*100<<endl;
      stepV += stepSize;
    }
    //if(nEv>10000) break;

    if(tTrackID!=1 && tTrackID!=2) continue;
    int part = pid[tPDGid] -1 ;
    if(part < 0){
      cout<<i<<" "<<tPDGid<<endl;
    }
    
    double asym[nWght] ={1, txSecU, txSecP, tE*txSecU, tE*txSecP, sqrt(txSecU)*txSecP};
    for(int k=0;k<nWght;k++){
      xy[part][tDetNr][k]->Fill(tX,tY,asym[k]);

      if(part==1 && tDetNr==5)
	for(int j=0;j<nSeg;j++)
	  yElec[k][j]->Fill(tY,asym[k]);
    }
  }

  writeHisto(finNm);
  fin->Close();
}

void initHisto(){
  for(int i=0;i<nXsec;i++){
    hXsec[i] = new TH1D(xSecNm[i].c_str(),Form("%s;#rho;%s",xSecTi[i].c_str(),xSecTi[i].c_str()),100,0,1);
    iXsec[i] = new TH1D((string("i")+xSecNm[i]).c_str(),
			Form(";#rho;%s",xSecTi[i].c_str()),100,0,1);
  }

  double phLimits[6][2]={{-0.2,0.2},{-10,-9},{-13,-12},{-32,-29},{-32.5,-31.5},{-49,-47}};
  for(int ip=0;ip<nPart;ip++)
    for(int j=0;j<nDet;j++)
      for(int k=0;k<nWght;k++){
	if(ip==0){
	  partXBinLimit[0][0] = phLimits[j][0]; 
	  partXBinLimit[0][1] = phLimits[j][1]; 
	}
	xy[ip][j][k]=new TH2D(Form("xy_%s_Zpos%d_Wght%d",partN[ip].c_str(),j,k),
			      Form("%s  %s z=%4.2f m;x[cm];y[cm]",partT[ip].c_str(),wghtTit[k].c_str(),zPos[j]),
			      n2Dbins,partXBinLimit[ip][0],partXBinLimit[ip][1],
			      n2Dbins,partYBinLimit[ip][0],partYBinLimit[ip][1]);
      }
  for(int k=0;k<nWght;k++)
    for(int j=0;j<nSeg;j++){
      int nBins = int(segL[j]*20000/seg[j]);
      yElec[k][j]=new TH1D(Form("yElec_Seg%d_Wght%d",j,k),Form("segmentation %d um;y [cm]; wght: %s",int(seg[j]),wghtTit[k].c_str()),
			   nBins,-segL[j],segL[j]);
    }
}


void writeHisto(string finNm){

  TFile *fout = new TFile(Form("%s_anaTruth.root",finNm.substr(0,finNm.find_last_of(".")).c_str()),
			  "RECREATE");
  for(int i=0;i<nXsec;i++){
    hXsec[i]->Divide(iXsec[i]);
    hXsec[i]->Write();
  }

  for(int i=0;i<nPart;i++)
    for(int j=0;j<nDet;j++)
      for(int k=0;k<nWght;k++){
	xy[i][j][k]->Write();
      }

  for(int k=0;k<nWght;k++)
    for(int j=0;j<nSeg;j++)
      yElec[k][j]->Write();
  fout->Close();
}


int readTree(string fnm){
  
  fin=TFile::Open(fnm.c_str(),"READ");
  t = (TTree*)fin->Get("t");

  if(!t){
    cout<<"Can't find tree in file "<<fnm<<endl<<"Quitting!"<<endl;
    return 0;
  }
  t->SetBranchAddress("evNr", &tevNr);
  t->SetBranchAddress("detNr", &tDetNr);
  t->SetBranchAddress("pdgID", &tPDGid);
  t->SetBranchAddress("trackID", &tTrackID);
  t->SetBranchAddress("parentID", &tParentID);
  t->SetBranchAddress("E", &tE);
  t->SetBranchAddress("x", &tX);
  t->SetBranchAddress("y", &tY);
  t->SetBranchAddress("z", &tZ);
  t->SetBranchAddress("pX",&tpX);
  t->SetBranchAddress("pY",&tpY);
  t->SetBranchAddress("pZ",&tpZ);
  t->SetBranchAddress("time", &tTime);
  t->SetBranchAddress("xSecU", &txSecU);
  t->SetBranchAddress("xSecP", &txSecP);
  return 1;
}
