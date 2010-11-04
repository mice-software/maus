// TextFileIoBase.cc
//
// R. Sandstrom, R. Tsenov 2005-10-28
#include "TextFileIoBase.hh"
#include <sstream>

std::string TextFileIoBase::vectorToString(std::vector<double> vect)
{
  std::ostringstream s;
  int size = vect.size(); 
  s << size;

  for (int i = 0; i < size; ++i){
    s << " " << vect[i];
  }
  return s.str();
}

std::string TextFileIoBase::vectorToString(std::vector<int> vect)
{
  std::ostringstream s;
  int size = vect.size(); 
  s << size;

  for (int i = 0; i < size; ++i){
    s << " " << vect[i];
  }
  return s.str();
}

std::vector<double> TextFileIoBase::stringToVectorDouble(std::string string)
{
  std::istringstream s(string);
  return streamToVectorDouble(s);
}

std::vector<int> TextFileIoBase::stringToVectorInteger(std::string string)
{
  std::istringstream s(string);
  return streamToVectorInteger(s) ;
}

std::vector<double> TextFileIoBase::streamToVectorDouble(std::istringstream &s)
{
  int size;
  s >> size;
  std::vector<double> vect(size);
  for (int i = 0; i < size; ++i) { s >> vect[i]; }
  return vect;
}

std::vector<int> TextFileIoBase::streamToVectorInteger(std::istringstream &s)
{
  int size;
  s >> size;
  std::vector<int> vect(size);
  for (int i = 0; i < size; ++i) { s >> vect[i]; }
  return vect;
}
