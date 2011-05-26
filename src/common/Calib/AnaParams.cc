/*
** Written for the MICE proposal simulation
** Ed McKigney - Aug 21 2002
*/

#include <cstdlib>
#include <cstdio>
#include "Calib/AnaParams.hh"
#include <string>
#include "Interface/dataCards.hh"

const double AnaParams::m_mu = 105.65836;
const double AnaParams::m_pi = 139.57018;
const double AnaParams::m_p  = 938.27203;
const double AnaParams::m_e  = 0.51099892;
const double AnaParams::m_K  = 493.677;

AnaParams::AnaParams()
{
	_recOutput    = MyDataCards.fetchValueString("InputFile").c_str();
	//For now this is hard-coded... but it is a BUG
	_simOutput    = "Sim.out";
	_for009Input  = MyDataCards.fetchValueString("ICOOLfor009StyleFile");
	_virtualInput = "";
	_pidInput     = MyDataCards.fetchValueString("PidInputFile");
	_pidBranch    = MyDataCards.fetchValueString("PidTreeName");
	_particleID   = MyDataCards.fetchValueInt("AnalysisParticlePdgId");
	_anaInput     = MyDataCards.fetchValueString("AnalysisParametersFile").c_str();
	_nEvts        = MyDataCards.fetchValueInt("numEvts");
	_startEvt     = MyDataCards.fetchValueInt("StartEvent");
	_turtleInput  = ""; //needs to be set manually in the executable for now
	_mass         = getMassFromPdgPid();
}

AnaParams::~AnaParams()
{
}

AnaParams* AnaParams::_instance = (AnaParams*) NULL;

AnaParams* AnaParams::getInstance()
{
  if(!_instance) _instance = new AnaParams;
  return _instance;
}

double AnaParams::getMassFromPdgPid()
{
	int pids[10] = {-11,-13,211,321,2212,11,13,-211,-321,-2212};
	double masses[10] = {m_e, m_mu, m_pi, m_K, m_p, m_e, m_mu, m_pi, m_K, m_p};

	for(int i=0; i<10; i++)
	{
		if(_particleID==pids[i])
			return masses[i];
	}
	cerr << "PID not recognised, must be one of (pi+ pi- mu+ mu- e+ e- K+ K- p pbar). Aborting." << endl;
	exit(1);

}

