#ifndef MAUS_EVENT_STRUCT_H
#define MAUS_EVENT_STRUCT_H
#include "MCStructs.hh"
#include "DigitsStructs.hh"


/*!\class MausData
 *
 * \brief Top level binary data structure for MausData
 *
 * This is the top level data structure for MausData. It uses the ROOT
 * ClassDef() macro in order to facilitate the generation of ROOT
 * dictionaries. This enables the structure to be placed within ROOT
 * TTrees and be explorable using the ROOT TBrowser.
 * \author Alexander Richards, Imperial College London
 * \date 06/01/2012
 */
class MausData{
 private:
  /*! \var Digits* digits
   * \brief pointer to a \a Digits object that will be extended as part of building the output
   * \note the memory for this object belongs to this class and is deleted upon destruction
   */
  Digits* digits; //memory belongs to this class
  /*! \var MC* mc
   * \brief pointer to an \a MC object that will be extended as part of building the output
   * \note the memory for this object belongs to this class and is deleted upon destruction
   */
  MC* mc;
 public:
  //! Default constructor
  MausData();
  /*!\brief Copy constructor
   * \param MausData& a \a MausData object from which to construct this.
   * \note This is a deep copy and pointers are not shared.
   */
  MausData(const MausData& md);
  /*!\brief Constructor
   * \param Digits* a pointer to a\a Digits object.
   * \param MC* a pointer to an \a MC object.
   * \note Once constructed, this class assumes responsibility for the memory
   * associated with the argument objects.
   */
  MausData(Digits* d, MC* m);
       
  /*!\brief Assignment operator
   * \param MausData& a \a MausData object which will be copied into this one.
   * \note This is a deep copy and pointers are not shared.
   */
  MausData& operator=(const MausData& md);
  
  //! Virtual destructor
  virtual ~MausData();
  
  /*!\brief add DigitsElements to the digits collection
   * push back elements into the Digits collection.
   * \param DigitsElement* a pointer to a \DigitsElement object which will be pushed back into the collection contained in the digits member.
   */
  void digits_push_back(DigitsElement* d);
  /*!\brief add MCElement to the mc collection
   * push back elements into the mc collection.
   * \param MCElement* a pointer to a \MCElement object which will be pushed back into the collection contained in the mc member.
   */
  void mc_push_back(MCElement* m);
  
  /*!\brief Setter - set the digits member
   * This setter method sets the currect digits member to a copy of that passed in the argument.
   * \param Digits& a Digits object to copy into this
   * \note Deep copy operation
   */
  void set_digits(const Digits& d);
  /*!\brief Setter - set the mc member
   * This setter method sets the currect mc member to a copy of that passed in the argument.
   * \param MC& an mc object to copy into this
   * \note Deep copy operation
   */
  void set_mc(const MC& m);

  /*!\brief Getter - get the digts member
   * This getter method returns a constant reference to the digits member.
   *\return a constant reference to the Digits member
   */
  const Digits& get_digits() const;
  /*!\brief Getter - get the mc member
   * This getter method returns a constant reference to the mc member.
   *\return a constant reference to the MC member
   */
  const MC& get_mc() const;

  /*!\brief clear and reset data structure content
   * This method clears the data structure ready to accept input for the next event.
   */  
  void clear();

  ClassDef(MausData,1)
};

#endif
