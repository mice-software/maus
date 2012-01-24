#ifndef DIGITS_STRUCTS_H
#define DIGITS_STRUCTS_H

#include <vector>
#include <string>
//#include <cstring>
#include <TObject.h>

/* class MausString : public std::string{ */
/*  public: */
/*   MausString():std::string(){} */
/*   MausString(const char* c): std::string(c){} */
/*     //MausString(const MausString& ms): std::string(ms.c_str()){} */
/*   ClassDef(MausString,1) */
/* }; */


////////////////////////////////////////////////////////////////////////////////////////////////////

/*!\class ChannelID
 *
 * \brief Bottom level binary data structure for MausData->Digits branch
 *
 * This is a bottom level data structure for MausData->Digits branch. 
 * It uses the ROOT ClassDef() macro in order to facilitate the generation 
 * of ROOT dictionaries. This enables the structure to be placed within ROOT
 * TTrees and be explorable using the ROOT TBrowser.
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class ChannelID{
 private:
  //! \var int channel_number
  int channel_number;
  //! \var int fiber_number
  int fiber_number;
  //! \var int plane_number
  int plane_number;
  //! \var int station_number
  int station_number;
  //! \var int tracker_number
  int tracker_number;
  //char type[40];
  //! \var std::string type 
  std::string type;
  //MausString type;
 public:
  //! \brief Default constructor
  ChannelID();
  /*!\brief Copy constructor
   * \param ChannelID& a \a ChannelID object from which to construct this.
   */
  ChannelID(const ChannelID& c);  
  /*!\brief Constructor
   * \param int channel_number
   * \param int fiber_number
   * \param int plane_number
   * \param int station_number
   * \param int tracker_number
   * \param std::string type
   */
  ChannelID(int cnumber,
	    int fnumber,
	    int pnumber,
	    int snumber,
	    int tnumber,
	    //const char* t,
	    std::string t);
  
  //! Virtual destructor
  virtual ~ChannelID();
  
  /*!\brief Assignment operator
   * \param ChannelID& a \a ChannelID object which will be copied into this one.
   */
  ChannelID& operator=(const ChannelID& c);
  /*!\brief clear and reset data structure content
   * This method clears the data structure ready to accept input for the next event.
   */  
  void clear();
  /*!\brief Setter - set the channel_number member
   * \param int channel_number
   */  
  void set_channel_number(int cn );
  /*!\brief Setter - set the fiber_number member
   * \param int fiber_number
   */  
  void set_fiber_number(int fn);
  /*!\brief Setter - set the plane_number member
   * \param int plane_number
   */  
  void set_plane_number(int pn);
  /*!\brief Setter - set the station_number member
   * \param int station_number
   */  
  void set_station_number(int sn);
  /*!\brief Setter - set the tracker_number member
   * \param int tracker_number
   */  
  void set_tracker_number(int tn);
  /*!\brief Setter - set the type member
   * \param std::string type
   */  
  void set_type(const std::string t);
  //void set_type(const char* t)       {strcpy(type,t);}
  //void set_type(const char t[40])       {strcpy(type,t);}

  /*!\brief Getter - get the channel_number member
   * \return a constant reference to the channel_number member
   */
  const int&  get_channel_number() const;
  /*!\brief Getter - get the fiber_number member
   * \return a constant reference to the fiber_number member
   */
  const int&  get_fiber_number()   const;
  /*!\brief Getter - get the plane_number member
   * \return a constant reference to the plane_number member
   */
  const int&  get_plane_number()   const;
  /*!\brief Getter - get the station_number member
   * \return a constant reference to the station_number member
   */
  const int&  get_station_number() const;
  /*!\brief Getter - get the tracker_number member
   * \return a constant reference to the tracker_number member
   */
  const int&  get_tracker_number() const;
  /*!\brief Getter - get the type member
   * \return a constant reference to the type member
   */
  const std::string& get_type()    const;
  //const char* get_type()           const {return type;}

  ClassDef(ChannelID,1)  //Simple channel_id class

};

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!\class MCMomentum
 *
 * \brief Bottom level binary data structure for MausData->Digits branch
 *
 * This is a bottom level data structure for MausData->Digits branch. 
 * It uses the ROOT ClassDef() macro in order to facilitate the generation 
 * of ROOT dictionaries. This enables the structure to be placed within ROOT
 * TTrees and be explorable using the ROOT TBrowser.
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class MCMomentum{
 private:
  /*! \var double x
   * \brief the x component of momentum
   */
  double x;
  /*! \var double y
   * \brief the y component of momentum
   */
  double y;
  /*! \var double z
   * \brief the z component of momentum
   */
  double z;
 public:
  //! \brief Default constructor
  MCMomentum();
  /*!\brief Copy constructor
   * \param MCMomentum& a \a MCMomentum object from which to construct this.
   */
  MCMomentum(const MCMomentum& m);
  /*!\brief Constructor
   * \param double x
   * \param double y
   * \param double z
   */
  MCMomentum(double X,double Y, double Z);

  //! Virtual destructor
  virtual ~MCMomentum();
  
  /*!\brief Assignment operator
   * \param MCMomentum& a \a MCMomentum object which will be copied into this one.
   */
  MCMomentum& operator=(const MCMomentum& m);
  /*!\brief clear and reset data structure content
   * This method clears the data structure ready to accept input for the next event.
   */  
  void clear();
  
  /*!\brief Setter - set the x member
   * \param double x component of momentum
   */  
  void set_x(double X);
  /*!\brief Setter - set the y member
   * \param double y component of momentum
   */  
  void set_y(double Y);
  /*!\brief Setter - set the z member
   * \param double z component of momentum
   */  
  void set_z(double Z);
  /*!\brief Getter - get the x member
   * \return a constant reference to the x member
   */
  const double& get_x() const;
  /*\brief Getter - get the y member
   * \return a constant reference to the y member
   */
  const double& get_y() const;
  /*\brief Getter - get the z member
   * \return a constant reference to the z member
   */
  const double& get_z() const;

  ClassDef(MCMomentum,1)  //Simple mc_mom class
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!\class MCPosition
 *
 * \brief Bottom level binary data structure for MausData->Digits branch
 *
 * This is a bottom level data structure for MausData->Digits branch. 
 * It uses the ROOT ClassDef() macro in order to facilitate the generation 
 * of ROOT dictionaries. This enables the structure to be placed within ROOT
 * TTrees and be explorable using the ROOT TBrowser.
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class MCPosition{
  /*! \var double x
   * \brief the x component of position
   */
  double x;
  /*! \var double y
   * \brief the y component of position
   */
  double y;
  /*! \var double z
   * \brief the z component of position
   */
  double z;
 public:
  //! \brief Default constructor
  MCPosition();
  /*!\brief Copy constructor
   * \param MCPosition& a \a MCMomentum object from which to construct this.
   */
  MCPosition(const MCPosition& m);
  /*!\brief Constructor
   * \param double x
   * \param double y
   * \param double z
   */
  MCPosition(double X,double Y, double Z);
  //! Virtual destructor
  virtual ~MCPosition();

  /*!\brief Assignment operator
   * \param MCPosition& a \a MCPosition object which will be copied into this one.
   */
  MCPosition& operator=(const MCPosition& m);
  /*!\brief clear and reset data structure content
   * This method clears the data structure ready to accept input for the next event.
   */  
  void clear();
  
  /*!\brief Setter - set the x member
   * \param double x component of position
   */
  void set_x(double X);
  /*!\brief Setter - set the y member
   * \param double y component of position
   */  
  void set_y(double Y);
  /*!\brief Setter - set the z member
   * \param double z component of position
   */  
  void set_z(double Z);
  /*\brief Getter - get the x member
   * \return a constant reference to the x member
   */
  const double& get_x() const;
  /*\brief Getter - get the y member
   * \return a constant reference to the y member
   */
  const double& get_y() const;
  /*\brief Getter - get the z member
   * \return a constant reference to the z member
   */
  const double& get_z() const;


  ClassDef(MCPosition,1)  //Simple mc_position class
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!\class Digit
 *
 * \brief Individual Digit binary data structure for MausData->Digits branch
 *
 * This is the definition of an indivitual Digit data structure for MausData->Digits branch. 
 * It uses the ROOT ClassDef() macro in order to facilitate the generation 
 * of ROOT dictionaries. This enables the structure to be placed within ROOT
 * TTrees and be explorable using the ROOT TBrowser.
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class Digit{
  /*! \var ChannelID channel_id
   * \brief ChannelID object gives channel_id branch in ROOT TTree
   */
  ChannelID channel_id;
  /*! \var MCMomentum mc_mom
   * \brief MCMomentum object gives mc_mom branch in ROOT TTree
   */
  MCMomentum mc_mom;
  /*! \var MCPosition mc_position
   * \brief MCPosition object gives mc_position branch in ROOT TTree
   */
  MCPosition mc_position;
  //! \var double number_photoelectrons
  double number_photoelectrons;
  //! \var double time
  double time;
  //! \var int adc_counts
  int adc_counts;
  //! \var int tdc_counts
  int tdc_counts;
 public:
  //! \brief Default constructor
  Digit();
  /*!\brief Copy constructor
   * \param Digit& a \a Digit object from which to construct this.
   */
  Digit(const Digit& d);
  /*!\brief Constructor
   * \param ChannelID ChannelID to copy to object attached to this digit
   * \param MCMomentum MCMomentum to copy to object attached to this digit
   * \param MCPosition MCPosition to copy to object attached to this digit
   * \param double number_photoelectrons
   * \param double time
   * \param int adc_counts
   * \param int tdc_counts
   */
  Digit(ChannelID c,
	MCMomentum m,
	MCPosition p,
	double ph,
	double t,
	int adc,
	int tdc);
  //! Virtual destructor
  virtual ~Digit();

  /*!\brief Assignment operator
   * \param Digit& a \a Digit object which will be copied into this one.
   */
  Digit& operator=(const Digit& d);

  /*!\brief clear and reset data structure content
   * This method clears the data structure ready to accept input for the next event.
   */  
  void clear();

  /*!\brief Setter - set the channel_id member
   * \param ChannelID& channel_id
   */
  void set_channel_id(const ChannelID& c);
  /*!\brief Setter - set the mc_mom member
   * \param MCMomentum& mc_mom
   */
  void set_mc_mom(const MCMomentum& m);
  /*!\brief Setter - set the mc_position member
   * \param MCPosition& mc_position
   */
  void set_mc_position(const MCPosition& m);
  /*!\brief Setter - set the number_photoelectrons member
   * \param double number_photoelectrons
   */
  void set_number_photoelectrons(double p);
  /*!\brief Setter - set the time member
   * \param double time
   */
  void set_time(double t);
  /*!\brief Setter - set the adc_counts member
   * \param int adc_counts
   */
  void set_adc_counts(int a);
  /*!\brief Setter - set the tdc_counts member
   * \param int tdc_counts
   */
  void set_tdc_counts(int t);
  
  /*\brief Getter - get the channel_id member
   * \return a constant reference to the channel_id member
   */
  const ChannelID&  get_channel_id()            const;
  /*\brief Getter - get the mc_mom member
   * \return a constant reference to the mc_mom member
   */
  const MCMomentum& get_mc_mom()                const;
  /*\brief Getter - get the mc_position member
   * \return a constant reference to the mc_position member
   */
  const MCPosition& get_mc_position()           const;
  /*\brief Getter - get the number_photoelectrons member
   * \return a constant reference to the number_photoelectrons member
   */
  const double&     get_number_photoelectrons() const;
  /*\brief Getter - get the time member
   * \return a constant reference to the time member
   */
  const double&     get_time()                  const;
  /*\brief Getter - get the adc_counts member
   * \return a constant reference to the adc_counts member
   */
  const int&        get_adc_counts()            const;
  /*\brief Getter - get the tdc_counts member
   * \return a constant reference to the tdc_counts member
   */
  const int&        get_tdc_counts()            const;

  ClassDef(Digit,1)  //Simple digit class
  
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!\class DigitsElement
 *
 * \brief Collection of \a  Digit binary data structure objects representing the elements in the MausData->Digits branch
 *
 * This is the collection of indivitual Digit data structure objects for MausData->Digits branch. 
 * It uses the ROOT ClassDef() macro in order to facilitate the generation 
 * of ROOT dictionaries. This enables the structure to be placed within ROOT
 * TTrees and be explorable using the ROOT TBrowser.
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class DigitsElement{
  //! \var std::vector<Digit*> digits
  std::vector<Digit*> digits;
 public:
  //! \brief Default constructor
  DigitsElement();
  /*!\brief Copy constructor
   * \param DigitsElement& a \a DigitsElement object from which to construct this.
   */
  DigitsElement(const DigitsElement& de);
  //! Virtual destructor
  virtual ~DigitsElement();

  /*!\brief Assignment operator
   * \param DigitsElement& a \a DigitsElement object which will be copied into this one.
   */
  DigitsElement& operator=(const DigitsElement& de);
  /*!\brief clear and reset data structure content
   * This method clears the data structure ready to accept input for the next event.
   * \param bool delete the memory associated with the container
   */  
  void clear(bool deleteMem=false);

  /*!\brief push back a \a Digit into this elements collection
   * \param Digit* the Digit object to be added to this DigitsElement's collection
   */  
  void push_back(Digit* d);

  typedef std::vector<Digit*>::iterator iterator;
  typedef std::vector<Digit*>::const_iterator const_iterator;

  /*!\brief get an iterator to the begining of this DigitsElement's collection of Digits
   * \return an iterator to the begining of the \a Digit collection
   */  
  iterator       begin();
  /*!\brief get an iterator to the end of this DigitsElement's collection of Digits
   * \return an iterator to the end of the \a Digit collection
   */  
  iterator       end();
  /*!\brief get a const_iterator to the beginning of this DigitsElement's collection of Digits
   * \return a const_iterator to the beginning of the \a Digit collection
   */  
  const_iterator begin() const;
  /*!\brief get a const_iterator to the end of this DigitsElement's collection of Digits
   * \return a const_iterator to the end of the \a Digit collection
   */  
  const_iterator end()   const;
  

  ClassDef(DigitsElement,1)  //Simple digit class
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!\class Digits
 *
 * \brief Top level binary data structure in the MausData->Digits branch
 *
 * This is the top level data structure object for the MausData->Digits branch.
 * It uses the ROOT ClassDef() macro in order to facilitate the generation 
 * of ROOT dictionaries. This enables the structure to be placed within ROOT
 * TTrees and be explorable using the ROOT TBrowser.
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class Digits{
  //! \var std::vector<DigitsElement*> elements
  std::vector<DigitsElement*> elements;
 public:

  //! \brief Default constructor
  Digits();
  /*!\brief Copy constructor
   * \param Digits& a \a Digits object from which to construct this.
   */
  Digits(const Digits& d);
  //! Virtual destructor
  virtual ~Digits();

  /*!\brief Assignment operator
   * \param Digits& a \a Digits object which will be copied into this one.
   */
  Digits& operator=(const Digits& d);
  /*!\brief clear and reset data structure content
   * This method clears the data structure ready to accept input for the next event.
   * \param bool delete the memory associated with the container
   */  
  void clear(bool deleteMem=false);

  /*!\brief push back a \a DigitsElement object into the collection
   * \param DigitsElement* the DigitsElement object to be added to the collection
   */  
  void push_back(DigitsElement* de);

  typedef std::vector<DigitsElement*>::iterator iterator;
  typedef std::vector<DigitsElement*>::const_iterator const_iterator;

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


  ClassDef(Digits,1)  //Simple digit class
};


#endif
