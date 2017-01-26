// MAUS WARNING: THIS IS LEGACY CODE.
//PillBox
//Revamped pill box class to use new slick phasing

#ifndef BTPillBox_HH
#define BTPillBox_HH

#include <string>

#include "BTField.hh"
#include "Config/MiceModule.hh"
#include "gsl/gsl_sf_gamma.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Vector/LorentzVector.h"

class BTPillBox: public BTField
{
public:
	//Constructor with no frequency argument ie electrostatic
	//(OR use frequency=0 with the other constructors should be ok)
	BTPillBox(double length, double eField, double radius);

	// Constructor with no time delay argument (relative phase). User has to
	// run Ref.Part. mode first.
	BTPillBox(double frequency, double length, double energyGain, std::string fieldDuringPhasing);

	// Constructor with time delay argument (relative phases known and provided
	// by the user). Then, ready for beam (normal) mode.
	BTPillBox(double frequency, double length, double peakEField, double timeDelay );

	~BTPillBox() {;}

	virtual BTPillBox * Clone() const {return new BTPillBox(*this);}


	void   GetFieldValue( const  double Point[4], double *Bfield ) const;
	bool   DoesFieldChangeEnergy() const {return true;}
	//If Point[2] is inside the cavity
	//Set the phase to Point[3]
	//and the peak Efield to dE/(sin(phi)*length*T)
	//where T is the transit time factor and phi is the synchronous phase
	RFData SetThePhase(Hep3Vector Position, double time, double energy);
	//Energy of reference particle - needed to work out transit time factor
	bool   IsPhaseSet() const {return (_phaseIsSet||_isElectrostatic);}

	void   SetRefParticlePhase(double phase)         {_refParticlePhase=phase;}
	void   SetFieldDuringPhasing(std::string option);

	static void SetStaticRefParticlePhase(double phase)      {_staticRefParticlePhase=phase;}

	virtual void Print(std::ostream &out) const;
	void SetSkinDepth(double skinDepth) {_skinDepth = skinDepth;}
	CLHEP::HepLorentzVector GetVectorPotential(CLHEP::HepLorentzVector position) const;

  /** Set the maximum radius of the field, overriding calculation based on
   *  Bessel functions
   */
  void SetCavityRadius(double radius) {_cavityRadius = radius;}
  /** Return the maximum radius of the field
   */
  double GetCavityRadius() const {return _cavityRadius;}

	enum phasingFieldModel{noField, electrostatic, normal, bestGuess};
	static phasingFieldModel StringToPhaseModel(std::string phaseModel);


protected:
	//Return the peak field (in time) normalised to 1
	//Overloaded by e.g. BTRFFieldMap
	virtual void GetPeakFields( const double Point[4], double *Bfield) const;
	//Print private parameters - not happy with this implementation
	//Really BTPillBox should be a child of BTAccel or so...
	void PrintPrivates(std::ostream &out) const;

private:

	//Returns the field the reference particle sees as a function of z
	bool   _isElectrostatic;
	double _fieldLength;

	double _nPhaseAttempts;
	double _frequency, _length, _skinDepth, _peakEField, _timeDelay, _energyGain;
	double _bToEFactor, _cavityFactor, _cavityRadius;
	double _refParticlePhase;
	bool   _phaseIsSet;
	phasingFieldModel _fieldDuringPhasing;


	static double _staticRefParticlePhase;
	static double mu_mass;

	//Returns a constant field
	//Returns modification of dE to ref particle due to time it takes to cross the rf cavity
	//Transit time factor T=(beta/pi*length)*sin(pi*length/beta) (cf. Lee 3.I.1)
	double GetTransitTimeFactor(double energy);

	double GetSkinDepth();
	void   SetParameters();

}; // class BTPillBox

#endif  // BTPillBox_HH
