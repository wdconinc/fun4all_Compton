// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef EICEVENTHEADER_H
#define EICEVENTHEADER_H

#include <phool/PHObject.h>

#include <iostream>
#include <map>
#include <utility>

class EicEventHeader: public PHObject
{
 public:
  EicEventHeader() {}
  virtual ~EicEventHeader();

  void Reset();

  int SetWeight(const string name, const double val);
  double GetWeight(const string);

 protected:
  std::map<string,double> evInfo;

  ClassDef(EicEventHeader,1)
};

#endif
