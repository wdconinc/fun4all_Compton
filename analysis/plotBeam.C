void plotBeam(string fnm){
  TFile *fin=TFile::Open(fnm.c_str(),"READ");
  TTree *t=(TTree*)fin->Get("t");
  auto *c1=new TCanvas();
  c1->Divide(4,2);
  for(int i=0;i<=7;i++){
    c1->cd(i+1);
    t->Draw("y:x",Form("pdgID==11 && detNr==%d",i),"colz");
  }
  //fin->Close();
}
