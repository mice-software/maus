//Optics Model
//Chris Rogers, March 05
//

#ifndef opticsmodel_hh
#define opticsmodel_hh

#include <string>
#include <vector>

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "gsl/gsl_errno.h"
#include "gsl/gsl_fft_complex.h"

#include "src/common/BeamTools/BTFieldConstructor.hh"
#include "src/common/BeamTools/BTField.hh"
#include "src/common/Config/MiceModule.hh"
#include "src/common/Config/RFParameters.hh"

#include "PhaseSpaceVector.hh"

class OpticsModel
{
public:
	//Optics model owns memory of field
	OpticsModel(MiceModule * rootModule) : _data(RFParameters::getInstance())
	{ BuildModel(rootModule); }
	OpticsModel() : _data(NULL)
	{_theField = new BTFieldConstructor();}
	OpticsModel(BTField * aField) : _data(RFParameters::getInstance())
	{_theField = aField;}
	~OpticsModel() {;} //delete _theField;} this should be active

	//Get optics output for calculating stuff
	double GetKquad(PhaseSpaceVector referenceParticle) const;
	double GetKrf_t(PhaseSpaceVector referenceParticle) const;
	double GetKrf_l(PhaseSpaceVector referenceParticle) const;
	double GetB0(PhaseSpaceVector referenceParticle)    const;
	double GetBz(PhaseSpaceVector referenceParticle)    const;
	double GetdEdz(PhaseSpaceVector referenceParticle)  const;
	//Return the B-field at a point
	CLHEP::Hep3Vector GetBField(CLHEP::HepLorentzVector position)  const;
	CLHEP::Hep3Vector GetEField(CLHEP::HepLorentzVector position)  const;
	
	//***  Material Stuff ***
	//Set optics component parameters
	double GetSolenoidCurrent(int index) {return 0;}//return _solenoid[index]->GetCurrent();}
	void   ResetSolenoidCurrent(int index, double current) {;}//_solenoid[index]->ResetCurrent(current);}
	double GetQuadBAtPoleTip(int index) {return 0;}//return _quadrupole[index]->GetBAtPoleTip();}
	void   ResetQuadBAtPoleTip(int index, double bAtPoleTip) {;}// _quadrupole[index]->ResetBAtPoleTip(bAtPoleTip);}
	double GetRfPeakE(int index)             {return 0;} //SOME CODE HERE
	void   ResetRfPeakE(int index, double E) {;} //SOME CODE HERE
	//Build optics from MiceModules
	void   BuildModel(MiceModule * rootModule) {_theField = new BTFieldConstructor(rootModule);}
	void   SetData() {_data = RFParameters::getInstance();}
	//Set fields on a reference particle
	void   SetFields(PhaseSpaceVector& referenceParticle) const;
	BTField * GetTheField() const {return _theField;}
	//Get a fourier decomposition of the on-axis Bz (Wang & Kim, Phys Rev E 63, 056502, 2001)
	//Allocates memory for the FT (needs delete [])
	//power gives the number of points in the FT (2^power points), zStart -> zEnd is the range in z of the FT
	double* NewFourierDecomposition(int power, double zStart, double zEnd) const;
	//Get the beta function using the FT calculated above
	double BetaFromFT(double length, int power, double* FourierTransform, PhaseSpaceVector RefParticle,
	                  double trace4d) const;
	//Get the upper and lower momentum band of the n\pi resonance using the FT calculated above
	std::vector<double*> MomentumOfStopBands(int power, double zStart, double zEnd, int nMax) const;


private:
	double GetMeanSquareBz(int power, double zStart, double zEnd) const;
	BTField *            _theField;
	RFParameters *       _data;
	static const double  _fieldTolerance;
};

#endif
