// MAUS WARNING: THIS IS LEGACY CODE.
#include "BTPhaser.hh"
#include "Utils/Squeak.hh"
#include "src/legacy/BeamTools/BTFieldConstructor.hh"
#include "src/common_cpp/Utils/Globals.hh"

BTPhaser * BTPhaser::_instance = NULL;

BTPhaser::BTPhaser() : _allPhasesSet(false), 
                       _phaseTolerance(1e-5), _firingRefs(false), _rfData(), 
                       _fields() {
}

BTPhaser::~BTPhaser()
{
	for (size_t i=0; i<_rfData.size(); i++) delete _rfData[i];
	_rfData = std::vector<RFData*>(0);
  for (size_t i=0; i<_fields.size(); ++i) delete _fields[i];
	_fields = std::vector<FieldForPhasing*>(0);
  if (this == _instance)
    _instance = NULL;
}

bool BTPhaser::SetThePhase(Hep3Vector Position, double time)
{
	bool   _thisPhaseSet = true;
	std::string phase    = " succeeded";
	RFData anRFPhase;
	anRFPhase = MAUS::Globals::GetInstance()->GetMCFieldConstructor()->SetThePhase(Position, time, 0.);
	double dt = anRFPhase.GetPhaseError();
	if(fabs(dt) < _phaseTolerance)
	{
		_thisPhaseSet = true;
		_rfData.push_back(new RFData(anRFPhase));
	}
	else {_thisPhaseSet = false; phase = " failed";}

	MAUS::Squeak::mout(MAUS::Squeak::debug) << "Attempt to phase cavity at (x,y,z) " << Position << " [mm] with time " 
	          << time << " dt " << dt << " dt tolerance " << _phaseTolerance 
	          << phase << std::endl;
	return _thisPhaseSet;
}

bool BTPhaser::IsPhaseSet() {
  return MAUS::Globals::GetInstance()->GetMCFieldConstructor()->IsPhaseSet();
}

std::vector<RFData*> BTPhaser::GetRFData()
{
	std::vector<RFData*> rfData;
	for(unsigned int i=0; i<_rfData.size(); i++)
		rfData.push_back( new RFData(*_rfData[i]) );
	return rfData;
}

BTPhaser* BTPhaser::GetInstance() {
  if(_instance == NULL) 
    _instance = new BTPhaser();
  return _instance;
}

