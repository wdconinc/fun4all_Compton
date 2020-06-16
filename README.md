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

## Platform independent setup:
```
1. go to https://github.com/EIC-Detector/Singularity/blob/master/VirtualBox.md and download the "sPHENIX Ubuntu 18.04LTS Image" virtual box
2. follow instructions there to enter the singularity shell
3. "mkdir compton;cd compton"
4. download generator: "> git clone https://gitlab.com/eic/mceg/comptonRad; cd comptonRad; make;". You can then run events by running "./main.exe" .. modify configuration by looking at the top of comrad.f and remaking
5. download fun4all macros and make the compton IO classes:
"> git clone https://github.com/cipriangal/fun4all_Compton; cd fun4all_Compton/source; mkdir build;cd build; ../autogen --prefix=$MYINSTALL; make install"
6. ">cd ~/fun4all/compton/fun4all_Compton"
7. ">export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH"
```

###To run vizualizer with defaults:
```
1. ">root"
2. "[] .x Fun4All_G4_IP12Compton.C"
3. "[] .L DisplayOn.C"
4. "[] PHG4Reco *g4=QTGui()"
5. to run test electron on the G4 terminal: "/Fun4All/run 1"
```

###To run events:
```
1. ">root"
2. "[] .x Fun4All_G4_IP12Compton.C(100,"./milouIn.root","o_ComptonTst.root",false,18")
```
