#ifndef TRIGGERKEY_HH
#define TRIGGERKEY_HH

#include <iostream>
using namespace std;

class TriggerKey{

public:
  
    TriggerKey();
	 TriggerKey(int station, int slA, int slB);
	 int slabA() const { return _slabA; };
	 int slabB() const { return _slabB; };
	 int station() const { return _station; };
	 
	 void SetStation(int st)  { _station = st; };
	 void SetSlabA(int sl)    { _slabA = sl; };
	 void SetSlabB(int sl)    { _slabB = sl; };
	 bool operator== ( TriggerKey key );
	 friend ostream& operator<< ( ostream& stream, TriggerKey key );
	 
private:
	 int _station;
	 int _slabA;
	 int _slabB;
};
#endif