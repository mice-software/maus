#include "DigitsStructs.h"

ChannelID::ChannelID():channel_number(0),
		       fiber_number(0),
		       plane_number(0),
		       station_number(0),
		       tracker_number(0){
  //type(""){
  //type = new char[40]();
  //char tmp[40] = "";
  //strcpy(type,tmp);
  //memcpy(type,"",sizeof(""));
  type.assign("");
}
ChannelID::ChannelID(const ChannelID& c):channel_number(c.channel_number),
					 fiber_number(c.fiber_number),
					 plane_number(c.plane_number),
					 station_number(c.station_number),
					 tracker_number(c.tracker_number){
  //type(c.type){
  //type = new char[40]();
  //strcpy(type,c.type);
  //memcpy(type,c.type,sizeof(c.type));
  type.assign(c.type);
}

ChannelID::ChannelID(int cnumber,
		     int fnumber,
		     int pnumber,
		     int snumber,
		     int tnumber,
		     //const char* t,
		     std::string t):channel_number(cnumber),
				     fiber_number(fnumber),
				     plane_number(pnumber),
				     station_number(snumber),
				     tracker_number(tnumber){
  //type(c.type){
  //type = new char[40]();
  //strcpy(type,t);
  //memcpy(type,c.type,sizeof(c.type));
  type.assign(t);
}

ChannelID::~ChannelID(){
  //delete type;
  //type=0;
}

ChannelID& ChannelID::operator=(const ChannelID& c){
  channel_number = c.channel_number;
  fiber_number = c.fiber_number;
  plane_number = c.plane_number;
  station_number = c.station_number;
  tracker_number = c.tracker_number;
  //char tmp[40] = 
  type.assign(c.type);
  //strcpy(type,c.type);
  return *this;
}
void ChannelID::clear(){
  channel_number=0;
  fiber_number=0;
  plane_number=0;
  station_number=0;
  tracker_number=0;
  type.assign("");
    //char tmp[40] = "";
    //strcpy(type,tmp);
}

void ChannelID::set_channel_number(int cn )   {channel_number = cn;}
void ChannelID::set_fiber_number(int fn)      {fiber_number = fn;}
void ChannelID::set_plane_number(int pn)      {plane_number = pn;}
void ChannelID::set_station_number(int sn)    {station_number = sn;}
void ChannelID::set_tracker_number(int tn)    {tracker_number = tn;}
void ChannelID::set_type(const std::string t) {type.assign(t);}
  //void set_type(const char* t)       {strcpy(type,t);}
  //void set_type(const char t[40])       {strcpy(type,t);}

const int&  ChannelID::get_channel_number() const {return channel_number;}
const int&  ChannelID::get_fiber_number()   const {return fiber_number;}
const int&  ChannelID::get_plane_number()   const {return plane_number;}
const int&  ChannelID::get_station_number() const {return station_number;}
const int&  ChannelID::get_tracker_number() const {return tracker_number;}
const std::string& ChannelID::get_type()    const {return type;}
//const char* get_type()           const {return type;}

///////////////////////////////////////////////////////////////////////

MCMomentum::MCMomentum():x(0),y(0),z(0){}
MCMomentum::MCMomentum(const MCMomentum& m):x(m.x),y(m.y),z(m.z){}
MCMomentum::MCMomentum(double X,double Y, double Z):x(X),y(Y),z(Z){}
MCMomentum::~MCMomentum(){}

MCMomentum& MCMomentum::operator=(const MCMomentum& m){
  x = m.x;
  y = m.y;
  z = m.z;
  return *this;
}
void MCMomentum::clear(){
  x=0;
  y=0;
  z=0;
}

void MCMomentum::set_x(double X){x=X;}
void MCMomentum::set_y(double Y){y=Y;}
void MCMomentum::set_z(double Z){z=Z;}
const double& MCMomentum::get_x() const{ return x; }
const double& MCMomentum::get_y() const{ return y; }
const double& MCMomentum::get_z() const{ return z; }

///////////////////////////////////////////////////////////////////////

MCPosition::MCPosition():x(0),y(0),z(0){}
MCPosition::MCPosition(const MCPosition& m):x(m.x),y(m.y),z(m.z){}
MCPosition::MCPosition(double X,double Y, double Z):x(X),y(Y),z(Z){}
MCPosition::~MCPosition(){}

MCPosition& MCPosition::operator=(const MCPosition& m){
  x = m.x;
  y = m.y;
  z = m.z;
  return *this;
}
void MCPosition::clear(){
  x=0;
  y=0;
  z=0;
}

void MCPosition::set_x(double X){x=X;}
void MCPosition::set_y(double Y){y=Y;}
void MCPosition::set_z(double Z){z=Z;}
const double& MCPosition::get_x() const{ return x; }
const double& MCPosition::get_y() const{ return y; }
const double& MCPosition::get_z() const{ return z; }

///////////////////////////////////////////////////////////////////////

Digit::Digit():channel_id(),
	       mc_mom(),
	       mc_position(),
	       number_photoelectrons(0.),
	       time(0.),
	       adc_counts(0),
	       tdc_counts(0){}
Digit::Digit(const Digit& d):channel_id(d.channel_id),
			     mc_mom(d.mc_mom),
			     mc_position(d.mc_position),
			     number_photoelectrons(d.number_photoelectrons),
			     time(d.time),
			     adc_counts(d.adc_counts),
			     tdc_counts(d.tdc_counts){}

Digit::Digit(ChannelID c,
	     MCMomentum m,
	     MCPosition p,
	     double ph,
	     double t,
	     int adc,
	     int tdc): channel_id(c),
		       mc_mom(m),
		       mc_position(p),
		       number_photoelectrons(ph),
		       time(t),
		       adc_counts(adc),
		       tdc_counts(tdc){}
Digit::~Digit(){}

Digit& Digit::operator=(const Digit& d){
  
  channel_id = d.channel_id;
  mc_mom = d.mc_mom;
  mc_position = d.mc_position;
  number_photoelectrons = d.number_photoelectrons;
  time = d.time;
  adc_counts = d.adc_counts;
  tdc_counts = d.tdc_counts;
  return *this;
}

void Digit::clear(){
  channel_id.clear();
  mc_mom.clear();
  mc_position.clear();
  number_photoelectrons=0.;
  time=0.;
  adc_counts=0;
  tdc_counts=0;
}

void Digit::set_channel_id(const ChannelID& c)  {channel_id = c;}
void Digit::set_mc_mom(const MCMomentum& m)     {mc_mom = m; }
void Digit::set_mc_position(const MCPosition& m){mc_position = m;}
void Digit::set_number_photoelectrons(double p) {number_photoelectrons = p;}
void Digit::set_time(double t)                  {time=t;}
void Digit::set_adc_counts(int a)               {adc_counts=a;}
void Digit::set_tdc_counts(int t)               {tdc_counts=t;}

const ChannelID&  Digit::get_channel_id()            const { return channel_id; }
const MCMomentum& Digit::get_mc_mom()                const { return mc_mom; }
const MCPosition& Digit::get_mc_position()           const { return mc_position; }
const double&     Digit::get_number_photoelectrons() const { return number_photoelectrons; }
const double&     Digit::get_time()                  const { return time; }
const int&        Digit::get_adc_counts()            const { return adc_counts; }
const int&        Digit::get_tdc_counts()            const { return tdc_counts; }

///////////////////////////////////////////////////////////////////////

DigitsElement::DigitsElement(){}
DigitsElement::DigitsElement(const DigitsElement& de){
  digits.reserve(de.digits.size());
  std::vector<Digit*>::const_iterator itr = de.digits.begin();
  for(;itr!=de.digits.end();++itr){
    digits.push_back(new Digit(**itr));
  }
}
DigitsElement::~DigitsElement(){ clear(true); }

DigitsElement& DigitsElement::operator=(const DigitsElement& de){
  clear(true);
  digits.reserve(de.digits.size());
  std::vector<Digit*>::const_iterator itr = de.digits.begin();
  for(;itr!=de.digits.end();++itr){
    digits.push_back(new Digit(**itr));
  }
  return *this;
}
void DigitsElement::clear(bool deleteMem){
  std::vector<Digit*>::iterator itr;
    for(itr=digits.begin();itr!=digits.end();++itr){ 
      (*itr)->clear();
      if(deleteMem){ delete *itr; }
    }
    digits.clear();
}

void DigitsElement::push_back(Digit* d){digits.push_back(d);}

DigitsElement::iterator       DigitsElement::begin()      {return digits.begin();}
DigitsElement::iterator       DigitsElement::end()        {return digits.end();  }
DigitsElement::const_iterator DigitsElement::begin() const{return digits.begin();}
DigitsElement::const_iterator DigitsElement::end()   const{return digits.end();  }

///////////////////////////////////////////////////////////////////////

Digits::Digits(){}
Digits::Digits(const Digits& d){
  elements.reserve(d.elements.size());
  std::vector<DigitsElement*>::const_iterator itr = d.elements.begin();
  for(;itr!=d.elements.end();++itr){
    elements.push_back(new DigitsElement(**itr));
  }
  
}
Digits::~Digits(){ clear(true); }

Digits& Digits::operator=(const Digits& d){
  clear(true);
  elements.reserve(d.elements.size());
  std::vector<DigitsElement*>::const_iterator itr = d.elements.begin();
  for(;itr!=d.elements.end();++itr){
    elements.push_back(new DigitsElement(**itr));
  }
  return *this;
}
void Digits::clear(bool deleteMem){
  std::vector<DigitsElement*>::iterator itr;
  for(itr=elements.begin();itr!=elements.end();++itr){ 
    if((*itr)){ 
      (*itr)->clear();
      if(deleteMem){ delete *itr; }
    }
  }
  elements.clear();
}

void Digits::push_back(DigitsElement* de){ elements.push_back(de); }

Digits::iterator       Digits::begin()      {return elements.begin();}
Digits::iterator       Digits::end()        {return elements.end();  }
Digits::const_iterator Digits::begin() const{return elements.begin();}
Digits::const_iterator Digits::end()   const{return elements.end();  }
