#ifndef MC_STRUCTS_H
#define MC_STRUCTS_H

#include <vector>
#include <string>
#include <TObject.h>

class MCChannelID{
 private:
  //! \var std::string type
  std::string type;
  //! \var int station_number
  int station_number;
 public:
  //! \brief Default constructor
  MCChannelID();
  /*!\brief Copy constructor
   * \param MCChannelID& a \a MCChannelID object from which to construct this.
   */
  MCChannelID(const MCChannelID& c);
  /*!\brief Constructor
   * \param int station_number
   * \param std::string type
   */
  MCChannelID(int sn, std::string s);
  //! Virtual destructor
  virtual ~MCChannelID();
  
  
  /*!\brief Assignment operator
   * \param MCChannelID& a \a MCChannelID object which will be copied into this one.
   */
  MCChannelID& operator=(const MCChannelID& c);
  
  /*!\brief Setter - set the station_number member
   * \param int station_number
   */  
  void set_station_number(int sn);
  /*!\brief Setter - set the type member
   * \param std::string type
   */  
  void set_type(std::string s);
  /*!\brief Getter - get the station_number member
   * \return a constant reference to the station_number member
   */
  const int& get_station_number() const;
  /*!\brief Getter - get the type member
   * \return a constant reference to the type member
   */
  const std::string& get_type()   const;
  
  /*!\brief clear and reset data structure content
   * This method clears the data structure ready to accept input for the next event.
   */
  void clear();
  ClassDef(MCChannelID,1)  //Simple channel_id struct
};


class MCHitsElement{
 private:
  /*! \var MCChannelID channel_id
   * \brief MCChannelID object gives channel_id branch in ROOT TTree
   */
  MCChannelID channel_id;
  //! \var double energy
  double energy;
  //! \var double energy_deposited
  double energy_deposited;
  //! \var double mass
  double mass;
  //! \var double time
  double time;
  //! \var int particle_id
  int particle_id;
  //! \var int charge
  int charge;
  //! \var int track_id
  int track_id;

 public:

  //! \brief Default constructor
  MCHitsElement();
  /*!\brief Copy constructor
   * \param MCHitsElement& an \a MCHitsElement object from which to construct this.
   */
  MCHitsElement(const MCHitsElement& mc);
  /*!\brief Constructor
   * \param MCChannelID& MCChannelID to copy to object attached to this MCHitsElement
   * \param double energy
   * \param double energy_deposited
   * \param double mass
   * \param double time
   * \param int particle_id
   * \param int charge
   * \param int track_id
   */
  MCHitsElement(const MCChannelID& mc,
		double e,
		double ed,
		double m,
		double t,
		int pid,
		int c,
		int trkid);
  
  
  /*!\brief Assignment operator
   * \param MCHitsElement& a \a MCHitsElement object which will be copied into this one.
   */
  MCHitsElement& operator=(const MCHitsElement& mc);
  
  /*!\brief Setter - set the channel_id member
   * \param MCChannelID& channel_id
   */
  void set_channel_id(const MCChannelID& mc);
  /*!\brief Setter - set the energy member
   * \param double energy
   */
  void set_energy(double e);
  /*!\brief Setter - set the energy_deposited member
   * \param double energy_deposited
   */
  void set_energy_deposited(double e);
  /*!\brief Setter - set the mass member
   * \param double mass
   */
  void set_mass(double m);
  /*!\brief Setter - set the time member
   * \param double time
   */
  void set_time(double t);
  /*!\brief Setter - set the particle_id member
   * \param int particle_id
   */
  void set_particle_id(int pid);
  /*!\brief Setter - set the charge member
   * \param int charge
   */
  void set_charge(int c);
  /*!\brief Setter - set the track_id member
   * \param int track_id
   */
  void set_track_id(int tid);

  /*\brief Getter - get the channel_id member
   * \return a constant reference to the channel_id member
   */
  const MCChannelID& get_channel_id() const;
  /*\brief Getter - get the energy member
   * \return a constant reference to the energy member
   */
  const double& get_energy() const;
  /*\brief Getter - get the energy_deposited member
   * \return a constant reference to the energy_deposited member
   */
  const double& get_energy_deposited() const;
  /*\brief Getter - get the mass member
   * \return a constant reference to the mass member
   */
  const double& get_mass() const;
  /*\brief Getter - get the time member
   * \return a constant reference to the time member
   */
  const double& get_time() const;
  /*\brief Getter - get the particle_id member
   * \return a constant reference to the particle_id member
   */
  const int& get_particle_id() const;
  /*\brief Getter - get the charge member
   * \return a constant reference to the charge member
   */
  const int& get_charge() const;
  /*\brief Getter - get the track_id member
   * \return a constant reference to the track_id member
   */
  const int& get_track_id() const;

  /*!\brief clear and reset data structure content
   * This method clears the data structure ready to accept input for the next event.
   */  
  void clear();

  ClassDef(MCHitsElement,1)  //Simple channel_id class

};

class MCHits{
 private:
  //! \var std::vector<MCHitsElement*> elements
  std::vector<MCHitsElement*> elements;
  
 public:
  //! \brief Default constructor
  MCHits();
  /*!\brief Copy constructor
   * \param MCHits& an \a MCHits object from which to construct this.
   */
  MCHits(const MCHits& mc);
  /*!\brief Constructor
   * \param std::vector<MCHitsElement*>& to copy to collection attached to this
   */
  MCHits(const std::vector<MCHitsElement*>& mc);
  
  /*!\brief Assignment operator
   * \param MCHits& an \a MCHits object which will be copied into this one.
   */
  MCHits& operator=(const MCHits& mc);
  /*!\brief push back an \a MCHitsElement object into the collection
   * \param MCHitsElement* the MCHitsElement object to be added to the collection
   */  
  void push_back(MCHitsElement* mc);
  
  /*!\brief clear and reset data structure content
   * This method clears the data structure ready to accept input for the next event.
   * \param bool delete the memory associated with the container
   */  
  void clear();

  typedef std::vector<MCHitsElement*>::iterator iterator;
  typedef std::vector<MCHitsElement*>::const_iterator const_iterator;
  /*!\brief get an iterator to the begining of this Digits' collection of DigitsElements
   * \return an iterator to the begining of the \a DigitsElement collection
   */  
  iterator       begin();
  /*!\brief get an iterator to the end of this Digits' collection of DigitsElements
   * \return an iterator to the end of the \a DigitsElement collection
   */  
  iterator       end();
  /*!\brief get a const_iterator to the beginning of this Digits' collection of DigitsElements
   * \return a const_iterator to the beginning of the \a DigitsElement collection
   */  
  const_iterator begin() const;
  /*!\brief get a const_iterator to the end of this Digits' collection of DigitsElements
   * \return a const_iterator to the end of the \a DigitsElement collection
   */  
  const_iterator end()   const;

  ClassDef(MCHits,1)  //Simple digit class
  
};

class MCPrimaryMomentum{
 private:
  double x;
  double y;
  double z;

 public:
  //! \brief Default constructor
  MCPrimaryMomentum();
  MCPrimaryMomentum(const MCPrimaryMomentum& mc);
  MCPrimaryMomentum(double X, double Y, double Z);

  MCPrimaryMomentum& operator=(const MCPrimaryMomentum& m);

  void set_x(double X);
  void set_y(double Y);
  void set_z(double Z);

  const double& get_x() const;
  const double& get_y() const;
  const double& get_z() const;
  void clear();

  ClassDef(MCPrimaryMomentum,1)  //Simple digit class

};

class MCPrimaryPosition{
 private:
  double x;
  double y;
  double z;
 public:
  //! \brief Default constructor
  MCPrimaryPosition();
  MCPrimaryPosition(const MCPrimaryPosition& mc);
  MCPrimaryPosition(double X, double Y, double Z);

  MCPrimaryPosition& operator=(const MCPrimaryPosition& p);

  void set_x(double X);
  void set_y(double Y);
  void set_z(double Z);

  const double& get_x() const;
  const double& get_y() const;
  const double& get_z() const;
  void clear();

  ClassDef(MCPrimaryPosition,1)  //Simple digit class

};

class MCPrimary{
 private:
  MCPrimaryMomentum momentum;
  MCPrimaryPosition position;
  double energy;
  double time;
  int particle_id;
  int random_seed;
 public:
  //! \brief Default constructor
  MCPrimary();
  MCPrimary(const MCPrimary& mc);
  MCPrimary(  MCPrimaryMomentum& m,
	      MCPrimaryPosition& p,
	      double e,
	      double t,
	      int pid,
	      int rseed);
    
  MCPrimary& operator=(const MCPrimary& mc);

  void set_momentum(const MCPrimaryMomentum& mc);
  void set_position(const MCPrimaryPosition& mc);
  void set_energy(double e);
  void set_time(double t);
  void set_particle_id(int pid);
  void set_random_seed(int rseed);

  const MCPrimaryMomentum& get_momentum() const;
  const MCPrimaryPosition& get_position() const;
  const double& get_energy() const;
  const double& get_time() const;
  const int& get_particle_id() const;
  const int& get_random_seed() const;

  void clear();

  ClassDef(MCPrimary,1)  //Simple digit class
};

class MCElement{
 private:
  MCHits* hits; //memory belongs to this class
  MCPrimary* primary;
 public:
  //! \brief Default constructor
  MCElement();
  MCElement(const MCElement& mc);
  MCElement(MCHits* mchits,
	    MCPrimary* mcprimary); //pass on memory responsibility to MCElement
  
  
  virtual ~MCElement();
  
  MCElement& operator=(const MCElement& mc);
  
  void  set_hits(const MCHits& mc);
  void  set_primary(const MCPrimary& mc); 
  const MCHits&    get_hits()      const; 
  const MCPrimary& get_primary()   const; 
  void clear();
  
  ClassDef(MCElement,1)  //Simple digit class
};

class MC{
 private:
  std::vector<MCElement*> elements;
 public:
  //! \brief Default constructor
  MC();
  MC(const MC& mc);
  MC(const std::vector<MCElement*>& mc);


  virtual ~MC();
  MC& operator=(const MC& mc);

  void push_back(MCElement* mc);

  void clear(bool deleteMem=false);

  typedef std::vector<MCElement*>::iterator iterator;
  typedef std::vector<MCElement*>::const_iterator const_iterator;
  iterator       begin();
  iterator       end();
  const_iterator begin() const;
  const_iterator end()   const;

  ClassDef(MC,1)  //Simple MC class
};


#endif
