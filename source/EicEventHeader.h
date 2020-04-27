// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef EICEVENTHEADER_H
#define EICEVENTHEADER_H

#include <phool/PHObject.h>

#include <iostream>
#include <map>
#include <string>
#include <utility>

class EicEventHeader: public PHObject
{
 public:
  EicEventHeader() {}
  virtual ~EicEventHeader();

  void Reset();

  void SetInfo(const std::string, const double);
  double GetInfo(const std::string);

 protected:
  std::map<std::string,double> evInfo;

  ClassDef(EicEventHeader,1)
};

#endif
