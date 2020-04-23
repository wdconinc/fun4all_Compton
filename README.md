# fun4all_Compton

##To run (on racf):

###Visualize
```
root.exe
.x Fun4All_G4_BeamLine.C
.L DisplayOn.C
PHG4Reco *g4 = QTGui();  
```

###Run gun
```
root.exe
.x Fun4All_G4_BeamLine.C
.L DisplayOn.C
PHG4Reco *g4 = DisplayOn(); // to still have the command line
Fun4AllServer *se = Fun4AllServer::instance();
se->run(1); // to shoot the geantinos
```