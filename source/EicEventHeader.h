// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef G4MAIN_PHG4EVENTWGHT_H
#define G4MAIN_PHG4EVENTWGHT_H

#include <phool/PHObject.h>

#include <iostream>
#include <map>
#include <utility>

class PHG4EventWght: public PHObject
{
 public:
  PHG4EventWght() {}
  virtual ~PHG4EventWght();

  void Reset();

  void AddWeight(const string);
  int SetWeight(const string name, const double val);

 protected:

  std::map<string,double> wghts;

  ClassDef(PHG4EventWght,1)
};

#endif
