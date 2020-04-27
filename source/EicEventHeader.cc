#include "EicEventHeader.h"

#include <cmath>
#include <cstdlib>

using namespace std;

EicEventHeader::~EicEventHeader()
{ 
  evInfo.clear();
  return;
}

int
EicEventHeader::SetInfo(const string name, const double val)
{
  evInfo[name]=val;
}

double
EicEventHeader::GetInfo(const string name)
{
  map<string,double>::iterator it = evInfo.find(name);

  if(it != evInfo.end())
    return it->second = val;

  return NAN;
}

void
EicEventHeader::Reset()
{
  evInfo.clear();
}

