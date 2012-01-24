#include "MCStructs.hh"

MCChannelID::MCChannelID():station_number(0){
    type.assign("");
  }
  MCChannelID::MCChannelID(const MCChannelID& c):station_number(c.station_number){
    type.assign(c.type);
  }
  MCChannelID::MCChannelID(int sn, std::string s):station_number(sn){
    type.assign(s);
  }
  MCChannelID::~MCChannelID(){}


  MCChannelID& MCChannelID::operator=(const MCChannelID& c){
    station_number = c.station_number;
    type.assign(c.type);
    return *this;
  }
  void MCChannelID::set_station_number(int sn){ station_number = sn; }
  void MCChannelID::set_type(std::string s){ type.assign(s); }
  const int& MCChannelID::get_station_number() const{ return station_number; }
  const std::string& MCChannelID::get_type()   const{ return type; }

  void MCChannelID::clear(){
    station_number =0;
    type.assign("");
  }

//////////////////////////////////////////////////////////////////////////

MCHitsElement::MCHitsElement(): channel_id(),
				energy(0.),
				energy_deposited(0.),
				mass(0.),
				time(0.),
				particle_id(0),
				charge(0),
				track_id(0){}
MCHitsElement::MCHitsElement(const MCHitsElement& mc):channel_id(mc.channel_id),
						      energy(mc.energy),
						      energy_deposited(mc.energy_deposited),
						      mass(mc.mass),
						      time(mc.time),
						      particle_id(mc.particle_id),
						      charge(mc.charge),
						      track_id(mc.track_id){}
MCHitsElement::MCHitsElement(const MCChannelID& mc,
			     double e,
			     double ed,
			     double m,
			     double t,
			     int pid,
			     int c,
			     int trkid):
  channel_id(mc),
  energy(e),
  energy_deposited(ed),
  mass(m),
  time(t),
  particle_id(pid),
  charge(c),
  track_id(trkid){}


MCHitsElement& MCHitsElement::operator=(const MCHitsElement& mc){
  channel_id = mc.channel_id;
  energy = mc.energy;
  energy_deposited = mc.energy_deposited;
  mass = mc.mass;
  time = mc.time;
  particle_id = mc.particle_id;
  charge = mc.charge;
  track_id = mc.track_id;
  return *this;
}

void MCHitsElement::set_channel_id(const MCChannelID& mc){channel_id = mc;}
void MCHitsElement::set_energy(double e){energy = e;}
void MCHitsElement::set_energy_deposited(double e){energy_deposited = e;}
void MCHitsElement::set_mass(double m){mass = m;}
void MCHitsElement::set_time(double t){time = t;}
void MCHitsElement::set_particle_id(int pid){particle_id = pid;}
void MCHitsElement::set_charge(int c){charge = c;}
void MCHitsElement::set_track_id(int tid){track_id = tid;}

const MCChannelID& MCHitsElement::get_channel_id()const{return channel_id;}
const double& MCHitsElement::get_energy()const{return energy;}
const double& MCHitsElement::get_energy_deposited()const{return energy_deposited;}
const double& MCHitsElement::get_mass()const{return mass;}
const double& MCHitsElement::get_time()const{return time;}
const int& MCHitsElement::get_particle_id()const{return particle_id;}
const int& MCHitsElement::get_charge()const{return charge;}
const int& MCHitsElement::get_track_id()const{return track_id;}

void MCHitsElement::clear(){
  channel_id.clear();
  energy =0.;
  energy_deposited =0.;
  mass =0.;
  time =0.;
  particle_id=0;
  charge=0;
  track_id=0;
}

//////////////////////////////////////////////////////////////////////////
MCHits::MCHits(){}
MCHits::MCHits(const MCHits& mc){elements.assign(mc.elements.begin(),mc.elements.end());}
MCHits::MCHits(const std::vector<MCHitsElement*>& mc){elements.assign(mc.begin(),mc.end());}

MCHits& MCHits::operator=(const MCHits& mc){
  elements.assign(mc.elements.begin(),mc.elements.end());
  return *this;
}
void MCHits::push_back(MCHitsElement* mc){ elements.push_back(mc); }

void MCHits::clear(){
  std::vector<MCHitsElement*>::iterator itr;
  for(itr=elements.begin();itr!=elements.end();++itr){ 
    if((*itr)){ (*itr)->clear();}
  }
  elements.clear();
}

MCHits::iterator       MCHits::begin()      {return elements.begin();}
MCHits::iterator       MCHits::end()        {return elements.end();  }
MCHits::const_iterator MCHits::begin() const{return elements.begin();}
MCHits::const_iterator MCHits::end()   const{return elements.end();  }
//////////////////////////////////////////////////////////////////////////
MCPrimaryMomentum::MCPrimaryMomentum():x(0.),y(0.),z(0.){}
MCPrimaryMomentum::MCPrimaryMomentum(const MCPrimaryMomentum& mc):x(mc.x),y(mc.y),z(mc.z){}
MCPrimaryMomentum::MCPrimaryMomentum(double X, double Y, double Z):x(X),y(Y),z(Z){}

MCPrimaryMomentum& MCPrimaryMomentum::operator=(const MCPrimaryMomentum& m){
  x = m.x;
  y = m.y;
  z = m.z;
  return *this;
}

void MCPrimaryMomentum::set_x(double X){x=X;}
void MCPrimaryMomentum::set_y(double Y){y=Y;}
void MCPrimaryMomentum::set_z(double Z){z=Z;}

const double& MCPrimaryMomentum::get_x() const {return x;}
const double& MCPrimaryMomentum::get_y() const {return y;}
const double& MCPrimaryMomentum::get_z() const {return z;}
void MCPrimaryMomentum::clear(){
  x = 0.;
  y = 0.;
  z = 0.;
}


//////////////////////////////////////////////////////////////////////////
MCPrimaryPosition::MCPrimaryPosition():x(0.),y(0.),z(0.){}
MCPrimaryPosition::MCPrimaryPosition(const MCPrimaryPosition& mc):x(mc.x),y(mc.y),z(mc.z){}
MCPrimaryPosition::MCPrimaryPosition(double X, double Y, double Z):x(X),y(Y),z(Z){}

MCPrimaryPosition& MCPrimaryPosition::operator=(const MCPrimaryPosition& p){
  x = p.x;
  y = p.y;
  z = p.z;
  return *this;
}

void MCPrimaryPosition::set_x(double X){x=X;}
void MCPrimaryPosition::set_y(double Y){y=Y;}
void MCPrimaryPosition::set_z(double Z){z=Z;}

const double& MCPrimaryPosition::get_x() const {return x;}
const double& MCPrimaryPosition::get_y() const {return y;}
const double& MCPrimaryPosition::get_z() const {return z;}
void MCPrimaryPosition::clear(){
  x = 0.;
  y = 0.;
  z = 0.;
}
//////////////////////////////////////////////////////////////////////////
MCPrimary::MCPrimary():momentum(),
		       position(),
		       energy(0.),
		       time(0.),
		       particle_id(0),
		       random_seed(0){}
MCPrimary::MCPrimary(const MCPrimary& mc):momentum(mc.momentum),
					  position(mc.position),
					  energy(mc.energy),
					  time(mc.time),
					  particle_id(mc.particle_id),
					  random_seed(mc.random_seed){}
MCPrimary::MCPrimary(  MCPrimaryMomentum& m,
		       MCPrimaryPosition& p,
		       double e,
		       double t,
		       int pid,
		       int rseed):momentum(m),
				  position(p),
				  energy(e),
				  time(t),
				  particle_id(pid),
				  random_seed(rseed){}

MCPrimary& MCPrimary::operator=(const MCPrimary& mc){
  momentum = mc.momentum;
  position = mc.position;
  energy = mc.energy;
  time = mc.time;
  particle_id = mc.particle_id;
  random_seed = mc.random_seed;
  return *this;
}

void MCPrimary::set_momentum(const MCPrimaryMomentum& mc){momentum = mc;}
void MCPrimary::set_position(const MCPrimaryPosition& mc){position = mc;}
void MCPrimary::set_energy(double e){energy = e;}
void MCPrimary::set_time(double t){time = t;}
void MCPrimary::set_particle_id(int pid){particle_id = pid;}
void MCPrimary::set_random_seed(int rseed){random_seed = rseed;}

const MCPrimaryMomentum& MCPrimary::get_momentum() const {return momentum;}
const MCPrimaryPosition& MCPrimary::get_position() const {return position;}
const double& MCPrimary::get_energy() const {return energy;}
const double& MCPrimary::get_time() const {return time;}
const int& MCPrimary::get_particle_id() const {return particle_id;}
const int& MCPrimary::get_random_seed() const {return random_seed;}

void MCPrimary::clear(){
  momentum.clear();
  position.clear();
  energy = 0.;
  particle_id=0;
  random_seed = 0;
  time = 0.;
}

//////////////////////////////////////////////////////////////////////////

MCElement::MCElement():hits(new MCHits()),primary(new MCPrimary()){}
MCElement::MCElement(const MCElement& mc):hits(new MCHits()),
					  primary(new MCPrimary()){
  (*hits) = *mc.hits;
  (*primary) = *mc.primary;
}
MCElement::MCElement(MCHits* mchits,
		     MCPrimary* mcprimary):hits(mchits),
					   primary(mcprimary){} //pass on memory responsibility to MCElement


MCElement::~MCElement(){
  clear();
  delete hits;
  delete primary;
  hits = 0;
  primary = 0;
}

MCElement& MCElement::operator=(const MCElement& mc){
  delete hits;
  delete primary;
  hits = new MCHits(*mc.hits);
  primary = new MCPrimary(*mc.primary);
  /*     (*hits)= *mc.hits; */
  /*     (*primary) = *mc.primary; */
  return *this;
}

void  MCElement::set_hits(const MCHits& mc)      {(*hits) = mc;}
void  MCElement::set_primary(const MCPrimary& mc){(*primary) = mc;} 
const MCHits&    MCElement::get_hits()      const{ return *hits;} 
const MCPrimary& MCElement::get_primary()   const{ return *primary;} 
void MCElement::clear(){
  hits->clear();
  primary->clear();
}

//////////////////////////////////////////////////////////////////////////

MC::MC(){}
MC::MC(const MC& mc){
  elements.assign(mc.elements.begin(),mc.elements.end());
}
MC::MC(const std::vector<MCElement*>& mc){
  elements.assign(mc.begin(),mc.end());
}

MC::~MC(){
  clear(true);
}
MC& MC::operator=(const MC& mc){ 
  elements.assign(mc.elements.begin(),mc.elements.end());
  return *this;
}

void MC::push_back(MCElement* mc){elements.push_back(mc);}

void MC::clear(bool deleteMem){
  std::vector<MCElement*>::iterator itr;
  for(itr=elements.begin();itr!=elements.end();++itr){ 
    if((*itr)){ 
      (*itr)->clear();
      if(deleteMem){delete *itr;}
    }
  }
  elements.clear();
}

MC::iterator       MC::begin()      {return elements.begin();}
MC::iterator       MC::end()        {return elements.end();  }
MC::const_iterator MC::begin() const{return elements.begin();}
MC::const_iterator MC::end()   const{return elements.end();  }
