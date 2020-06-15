void drawPlanes(string fnm="/phenix/spin/phnxsp01/ciprian/eic/comptonOut/o_f4a_e18GeV_g532nm_5e6_withWghts.root", int detID=2){
  TFile *fin=TFile::Open(fnm.c_str(),"READ");
  TTree *t=(TTree*)fin->Get("t");
  auto *c1=new TCanvas("c1","c1",1400,800);
  c1->Divide(2);
  c1->cd(1);
  t->Draw("y:x>>h1(100,-50,1,100,-20,20)",
	  //t->Draw("y:x>>h1(100,-32.05,-28.7,100,-2,2)",
	  //t->Draw("y:x>>h1(100,-54,-44,100,-2,2)",
	  Form("xSecU*(detNr==%d && pdgID==22)",detID),"colz");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  c1->cd(2);
  //t->Draw("y:x",
  t->Draw("y:x>>h2(100,-10,10,100,-0.03,0.03)",
	  Form("xSecU*(detNr==%d && pdgID==11)",detID),"colz");
  gPad->SetGridx(1);
  gPad->SetGridy(1);

  //fin->Close();
}
