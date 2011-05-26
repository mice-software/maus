#include "Recon/TOF/TriggerKey.hh"

TriggerKey::TriggerKey()
{}

TriggerKey::TriggerKey(int station, int slA, int slB)
{
	 _station = station;
	 _slabA = slA;
	 _slabB = slB;
}


bool TriggerKey::operator== ( TriggerKey const key )
{
	if ( _station == key.station() && _slabA == key.slabA() && _slabB== key.slabB() )
		return true;

	else return false;
}

ostream& operator<< ( ostream& stream, TriggerKey key ) {
	stream << "TriggerKey: station " << key._station;
	stream << ", slabA " << key.slabA();
	stream << ", slabB " << key.slabB();
	return stream;
}