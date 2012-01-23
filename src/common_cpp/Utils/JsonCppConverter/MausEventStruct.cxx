#include "MausEventStruct.h"

MausData::MausData():digits(new Digits()),mc(new MC()){}

MausData::MausData(const MausData& md):digits(new Digits()),mc(new MC()){
  (*digits) = *(md.digits);
  (*mc) = *(md.mc);
}
MausData::MausData(Digits* d, MC* m):digits(d),mc(m){} //pass on memory responsibility to MausData

MausData& MausData::operator=(const MausData& md){
  delete digits;
  delete mc;
  digits = new Digits(*md.digits);
  mc = new MC(*md.mc);
  /*     (*digits) = *(md.digits); */
  /*     (*mc) = *(md.mc); */
  return *this;
}

MausData::~MausData(){
  clear();
  delete digits;
  delete mc;
  digits = 0;
  mc = 0;
}

void MausData::digits_push_back(DigitsElement* d){digits->push_back(d);}
void MausData::mc_push_back(MCElement* m){mc->push_back(m);}

void MausData::set_digits(const Digits& d){*digits = d;}
void MausData::set_mc(const MC& m){*mc = m;}
const Digits& MausData::get_digits() const {return *digits;}
const MC& MausData::get_mc() const {return *mc;}

void MausData::clear(){
  digits->clear();
  mc->clear();
}
