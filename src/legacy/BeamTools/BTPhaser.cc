// MAUS WARNING: THIS IS LEGACY CODE.
#include "BTPhaser.hh"
#include "Interface/Squeak.hh"
#include "src/legacy/Interface/MICERun.hh"
#include "src/legacy/BeamTools/BTFieldConstructor.hh"

BTPhaser::~BTPhaser()
{
	for(int i=0; i<(int)_rfData.size(); i++) delete _rfData[i];
	_rfData = std::vector<RFData*>(0);
}

bool BTPhaser::SetThePhase(Hep3Vector Position, double time)
{
	bool   _thisPhaseSet = true;
	std::string phase    = " succeeded";
	RFData anRFPhase;
	if(_hasGlobalField && !IsPhaseSet())
		anRFPhase = MICERun::getInstance()->btFieldConstructor->SetThePhase(Position, time, 0.);
	double dt = anRFPhase.GetPhaseError();
	if(fabs(dt) < _phaseTolerance) 
	{
		_thisPhaseSet = true;
		_rfData.push_back(new RFData(anRFPhase));
	}
	else {_thisPhaseSet = false; phase = " failed";}

	Squeak::mout(Squeak::debug) << "Attempt to phase cavity at z=" << Position.z() << " with time " 
	          << time << " dt " << dt << " dt tolerance " << _phaseTolerance 
	          << phase << std::endl;
	return _thisPhaseSet;
}

std::vector<RFData*> BTPhaser::GetRFData()
{
	std::vector<RFData*> rfData;
	for(unsigned int i=0; i<_rfData.size(); i++)
		rfData.push_back( new RFData(*_rfData[i]) );
	return rfData;
}



