std::vector<double> wghts[2];
std::vector<double> epZ;
void readInputCR(string,int,int);

void getWeights(string fnm, string crIn, int nStart=0, int nStop=5000000){
  if(epZ.size()==0){
    readInputCR(crIn,nStart,nStop);
    cout<<"Finish reading cr: "<<epZ.size()<<". Requested nEvets: "<<nStop-nStart<<endl;
  }else{
    cout<<"Skipped readin of cr file .. assume it was the same as before!"<<endl;
  }

  TFile *fin=TFile::Open(fnm.c_str(),"READ");
  TTree *tin = (TTree*)fin->Get("t");

  int evNr;
  int fDetNr;
  double fE,fX,fY,fZ,fpX,fpY,fpZ,fTime;
  int fPDGid,fTrackID,fParentID;
  tin->SetBranchAddress("evNr", &evNr);
  tin->SetBranchAddress("detNr", &fDetNr);
  tin->SetBranchAddress("pdgID", &fPDGid);
  tin->SetBranchAddress("trackID", &fTrackID);
  tin->SetBranchAddress("parentID", &fParentID);
  tin->SetBranchAddress("E", &fE);
  tin->SetBranchAddress("x", &fX);
  tin->SetBranchAddress("y", &fY);
  tin->SetBranchAddress("z", &fZ);
  tin->SetBranchAddress("pX", &fpX);
  tin->SetBranchAddress("pY", &fpY);
  tin->SetBranchAddress("pZ", &fpZ);
  tin->SetBranchAddress("time", &fTime);

  string foutNm = Form("%s_withWghts.root",fnm.substr(0,fnm.find_last_of(".")).c_str());
  TFile *fout=new TFile(foutNm.c_str(),"RECREATE");
  TTree *tout=new TTree("t","f4a with wghts");
  tout->Branch("evNr", &evNr);
  tout->Branch("detNr", &fDetNr);
  tout->Branch("pdgID", &fPDGid);
  tout->Branch("trackID", &fTrackID);
  tout->Branch("parentID", &fParentID);
  tout->Branch("E", &fE);
  tout->Branch("x", &fX);
  tout->Branch("y", &fY);
  tout->Branch("z", &fZ);
  tout->Branch("pX", &fpX);
  tout->Branch("pY", &fpY);
  tout->Branch("pZ", &fpZ);
  tout->Branch("time", &fTime);
  double xSecU,xSecP;
  tout->Branch("xSecU", &xSecU);
  tout->Branch("xSecP", &xSecP);

  int nHits = tin->GetEntries();
  for(int i=0;i<nHits;i++){
    tin->GetEntry(i);
    //cout<<evNr<<" " <<fTrackID<<" "<<fPDGid<<" "<<fDetNr<<" "<<fpZ<<" "<<epZ[evNr]<<endl;
    if(evNr > nStop-nStart-1)
      break;
    
    if(fTrackID == 1 && fDetNr==0 && abs(fpZ+epZ[evNr])>0.001)
      cout<<">>> "<<fTrackID<<" "<<fpZ<<" "<<epZ[evNr]<<" "<<fDetNr<<endl;
    xSecU = wghts[0][evNr];
    xSecP = wghts[1][evNr];
    tout->Fill();
  }

  fout->cd();
  tout->Write();
  fout->Close();
  fin->Close();
}

void readInputCR(string crFile, int nStart, int nStop){
  ifstream file(crFile.c_str(),std::fstream::in);
  if(!file.is_open()){
    cout<<"can't open file "<<crFile<<" \n\tQuitting"<<endl;
    return;
  }

  double gPx, gPy, gPz, gE;
  double ePx, ePy, ePz, eE;
  double xSec[4];//unpolarized, polarized, unpolarized order alpha corrected, polarized order alpha corrected
  
  string line;
  for(int i=0;i<3;i++){
    getline(file,line);
    cout<<line<<endl;
  }

  int evNr=-1;
  while (!file.eof() && evNr<nStop){
    evNr++;
    getline(file,line);
    //cout<<"Line: "<<line<<endl;
    if(evNr>=nStart && evNr<nStop){
      stringstream ss(line);
      ss>>gPx>>gPy>>gPz>>gE>>xSec[0]>>xSec[1]>>xSec[2]>>xSec[3]>>ePx>>ePy>>ePz>>eE;
      wghts[0].push_back(xSec[0]);
      wghts[1].push_back(xSec[1]);
      TVector3 ep(ePx,ePy,ePz);
      ep.RotateY(0.0192);
      epZ.push_back(ep.Z());
    }
  }
}
