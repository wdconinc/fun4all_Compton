#include "PHG4EventWght.h"

#include <cmath>
#include <cstdlib>

using namespace std;

PHG4WventWght::~PHG4WventWght()
{ 
  wghts.clear();
  return;
}

void
PHG4WventWght::AddWeight(const string name)
{
  wghts.insert(make_pair(name,-999999));
}

int
PHG4WventWght::SetWeight(const string name, const double val)
{
  map<string,double>::iterator it = wghts.find(name);

  if(it != wghts.end()){
    it->second = val;
    return it - wghts.begin();
  }
  return -1;
}

void
PHG4WventWght::Reset()
{
  wghts.find(name);
  for(map<string,double>::iterator it = wghts.begin(); it < wghts.end(); it++)
    it->second = -999999;
}

