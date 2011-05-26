#ifndef TRIGGERKEY_HH
#define TRIGGERKEY_HH

#include <iostream>
using namespace std;

//! A key to identify individual TOF pixels during the reconstruction of the trigger.
class TriggerKey{

  public:
    //! Default constructor.
    TriggerKey();

	 //! Constructor.
	 TriggerKey(int station, int slA, int slB);

	 int slabA() const { return _slabA; };
	 int slabB() const { return _slabB; };
	 int station() const { return _station; };

	 void SetStation(int st)  { _station = st; };
	 void SetSlabA(int sl)    { _slabA = sl; };
	 void SetSlabB(int sl)    { _slabB = sl; };

	 //! Is equal to operator.
	 bool operator== ( TriggerKey key );


	 friend ostream& operator<< ( ostream& stream, TriggerKey key );

  private:
	 //!  TOF station number.
	 int _station;

	 //! Number of the slab in plane 0.
	 int _slabA;

	 //! Number of the slab in plane 1.
	 int _slabB;
};

#endif


