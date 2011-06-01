// MAUS WARNING: THIS IS LEGACY CODE.
#include "BTPhaser.hh"
#include "Interface/Squeak.hh"

bool   BTPhaser::_hasGlobalField   = false;
bool   BTPhaser::_allPhasesSet     = true;
bool   BTPhaser::_firingRefs       = false;
double BTPhaser::_phaseTolerance   = 1e-3;
double BTPhaser::_energyTolerance  = 1e-3;
BTField * BTPhaser::_globalField   = 0;
std::vector<std::string> BTPhaser::_cavityNames         = std::vector<std::string>(0);
std::vector<RFData*>     BTPhaser::_rfData              = std::vector<RFData*>(0);
std::vector<std::string> BTPhaser::_cavityDetectorNames = std::vector<std::string>(0);

BTPhaser::~BTPhaser()
{
	for(int i=0; i<(int)_rfData.size(); i++) delete _rfData[i];
	_rfData = std::vector<RFData*>(0);
}

bool BTPhaser::SetThePhase(Hep3Vector Position, double time, double eGain)
{
	bool   _thisPhaseSet = true;
	std::string phase    = " succeeded";
	RFData anRFPhase;
	if(_hasGlobalField && !IsPhaseSet())
		anRFPhase = _globalField->SetThePhase(Position, time, eGain);
	double dt = anRFPhase.GetPhaseError();
	double dE = anRFPhase.GetEnergyError();
	if(fabs(dt) < _phaseTolerance && fabs(dE) < _energyTolerance) 
	{
		_thisPhaseSet = true;
		_rfData.push_back(new RFData(anRFPhase));
	}
	else {_thisPhaseSet = false; phase = " failed";}

	Squeak::mout(Squeak::debug) << "Attempt to phase cavity at z=" << Position.z() << " with time " 
	          << time << " dt " << dt << " dt tolerance " << _phaseTolerance 
	          << " eGain " << eGain << " dE " << dE << " dE tolerance " << _energyTolerance 
	          << phase << std::endl;
//	_allPhasesSet = (_globalField->IsPhaseSet() && _thisPhaseSet);
	return _thisPhaseSet;
}

bool BTPhaser::IsCavity(std::string name)
{
//	std::cout << "IS CAVITY "+name+" ";
	for(unsigned int i=0; i<_cavityNames.size(); i++)
	{
//		std::cout << " "+_cavityNames[i]+" ";
		if(_cavityNames[i]==name) return true;
	}
	return false;
}
bool BTPhaser::IsCavityDetector(std::string name)
{
//	std::cout << "IS CAVDET "+name+" ";
	for(unsigned int i=0; i<_cavityDetectorNames.size(); i++)
	{
//		std::cout << " "+_cavityDetectorNames[i]+" ";
		if(_cavityDetectorNames[i]==name) return true;
	}
	return false;
}

std::vector<RFData*> BTPhaser::GetRFData()
{
	std::vector<RFData*> rfData;
	for(unsigned int i=0; i<_rfData.size(); i++)
		rfData.push_back( new RFData(*_rfData[i]) );
	return rfData;
}



