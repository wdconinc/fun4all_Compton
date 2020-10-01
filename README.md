# fun4all_Compton

## Setup on RACF

The easiest to get started is to use your RACF account. For setup instructions on other platforms, you will need to run the following commands inside the singularity container as outlined below.

### Installing comptonRad

Download and compile the comptonRad generator sources:
```bash
mkdir -p ~/fun4all/compton
cd ~/fun4all/compton
git clone https://gitlab.com/eic/mceg/comptonRad
cd comptonRad
make
```
You can then run events by running `./main.exe`. Modify the configuration by looking at the top of `comrad.f` and recompiling.

### Installing fun4all_Compton

Download the fun4all_Compton macros and IO classes:
```bash
mkdir -p ~/fun4all/compton
cd ~/fun4all/compton
git clone https://github.com/cipriangal/fun4all_Compton
cd fun4all_Compton/source
mkdir build
cd build
../autogen.sh --prefix=$MYINSTALL
make install
```

### Setup script

After installing fun4all_Compton, you will need to reload your environment (script see below):
```bash
source setup.sh
```
or you can add the `~/fun4all/lib` directory to your LD_LIBRARY_PATH by hand:
```bash
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
```

## Running

### Visualize
```
root.exe
.x Fun4All_G4_BeamLine.C
.L DisplayOn.C
PHG4Reco *g4 = QTGui();  
```

### Run gun
```
root.exe
.x Fun4All_G4_BeamLine.C
.L DisplayOn.C
PHG4Reco *g4 = DisplayOn(); // to still have the command line
Fun4AllServer *se = Fun4AllServer::instance();
se->run(1); // to shoot the geantinos
```

### To run vizualizer with defaults:
```
root
.x Fun4All_G4_IP12Compton.C
.L DisplayOn.C
PHG4Reco *g4 = QTGui()
```
To run test electrons on the G4 terminal, use:
```
/Fun4All/run 1
```

### To run events:
```
root
.x Fun4All_G4_IP12Compton.C(100,"./milouIn.root","o_ComptonTst.root",false,18")
```

## Setup on Linux with CVMFS and Singularity

If you have a Linux system, you can [install CVMFS](https://cvmfs.readthedocs.io/en/stable/cpt-quickstart.html) and [install Singularity](https://sylabs.io/guides/3.0/user-guide/installation.html). This will allow you to start a shell inside the singularity container at /cvmfs/eic.opensciencegrid.org/singularity/rhic_sl7_ext with
```bash
singularity shell -B /cvmfs:/cvmfs /cvmfs/eic.opensciencegrid.org/singularity/rhic_sl7_ext
```
You should get a shell that looks like this:
```bash
Singularity>
```
You environment will be inherited from the host system. You may need to `unset ROOTSYS` for the following to work.

### Singularity_shell script
You can create a 'singularity_shell.sh` script to run when you want to start the singularity container.

`~/fun4all/singularity_shell.sh` contains
```bash
#!/bin/bash
singularity shell -e -B /cvmfs:/cvmfs /cvmfs/eic.opensciencegrid.org/singularity/rhic_sl7_ext
```
Start the container with by running:
```bash
bash ~/fun4all/singularity_shell.sh
```

### Setup script
You can create a `setup.sh` script to source when you enter the singularity container.

`~/fun4all/setup.sh` contains
```bash
#!/bin/bash
source /cvmfs/eic.opensciencegrid.org/x8664_sl7/opt/fun4all/core/bin/eic_setup.sh
export MYINSTALL=~/fun4all
source ${OPT_FUN4ALL}/bin/setup_local.sh $MYINSTALL
```
After starting the container, source this script:
```bash
source ~/fun4all/setup.sh
```

## Platform independent setup (Windows, Mac, Linux):

Go to [VirtualBox.md](https://github.com/eic/Singularity/blob/master/VirtualBox.md) to download the "EIC Ubuntu 18.04LTS Image" virtual box image. Follow instructions there to enter the singularity shell.

