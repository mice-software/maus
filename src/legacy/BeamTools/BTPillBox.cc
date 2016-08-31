// MAUS WARNING: THIS IS LEGACY CODE.
//PillBox
//Revamped pill box class to use new slick phasing
#include "CLHEP/Units/PhysicalConstants.h"

#include "BTField.hh"
#include "BTPillBox.hh"
#include "BTPhaser.hh"
#include "Utils/Exception.hh"
#include "micegsl.hh"
#include <string>

using CLHEP::pi;
using CLHEP::c_light;
using CLHEP::HepLorentzVector;

double      BTPillBox::_staticRefParticlePhase=pi/2;
double      BTPillBox::mu_mass=105.658369;

BTPillBox::BTPillBox(double length, double eField, double radius)
          : BTField::BTField(0., 0., 0., 0., 0., 0.), 
            _isElectrostatic(true), _frequency(0), _length(length), _skinDepth(0), _peakEField(eField),
            _timeDelay(0), _cavityRadius(radius),
            _phaseIsSet(true), _fieldDuringPhasing(bestGuess)
{
	SetParameters();
}


BTPillBox::BTPillBox(double frequency, double length, double energyGainParameter, std::string fieldDuringPhasing)
          : BTField::BTField(0., 0., 0., 0., 0., 0.), 
            _isElectrostatic(false), _frequency(frequency), _length(length), _skinDepth(0), _peakEField(energyGainParameter),
            _timeDelay(0), _energyGain(energyGainParameter), _cavityRadius(0), _phaseIsSet(false), _fieldDuringPhasing(bestGuess)

{
	SetParameters();
	SetFieldDuringPhasing(fieldDuringPhasing);
}

BTPillBox::BTPillBox(double frequency, double length, double peakEField, double timeDelay )
          : BTField::BTField(0., 0., 0., 0., 0., 0.), 
            _isElectrostatic(false), _frequency(frequency), _length(length), _skinDepth(0), _peakEField(peakEField),
            _timeDelay(timeDelay), _cavityRadius(0), _phaseIsSet(true), _fieldDuringPhasing(bestGuess)
{
	SetParameters();
}

RFData BTPillBox::SetThePhase(Hep3Vector Position, double time, double eGain)
{
	if( (fabs(Position[2])<_length/2.)&&(Position[0]*Position[0]+Position[1]*Position[1]<_cavityRadius*_cavityRadius)&&(!_isElectrostatic) )
	{
		double dt = 0;
		double dE = 0;
		switch(_fieldDuringPhasing)
		{
			case bestGuess:
				dt   = time  - _timeDelay;
				dE   = eGain - _energyGain;
				_peakEField = _peakEField*_energyGain/eGain;
				break;
			case normal:
				dt = time  - _timeDelay;
				break;
			case noField: case electrostatic:
				break;
		}
		_timeDelay  = time;
		_phaseIsSet = true;
		return RFData(GetGlobalPosition(), _timeDelay, _peakEField, _frequency, dt, dE);
	}
	else
		return RFData();	
}


double BTPillBox::GetTransitTimeFactor(double energy)
{
	double relativisticBeta=sqrt(energy*energy-mu_mass*mu_mass)/energy; //beta = p/E
	double transitTimeFactor = sin(pi*_frequency*_length/(3e2*relativisticBeta))
	                             /(pi*_frequency*_length/(3e2*relativisticBeta));
	return transitTimeFactor;
}

void BTPillBox::GetFieldValue( const double Point[4], double *Bfield ) const
{
	Bfield[0] = 0;
	Bfield[1] = 0;
	Bfield[2] = 0;
	Bfield[3] = 0;
	Bfield[4] = 0;
	Bfield[5] = 0;
	if(Point[0]*Point[0]+Point[1]*Point[1] > _cavityRadius*_cavityRadius) return;
	if(fabs(Point[2]) > _fieldLength/2.) return;
	if(BTPhaser::GetInstance()->IsRefPart())
	{
		switch(_fieldDuringPhasing)
		{
			case electrostatic:
				if(fabs(Point[2]) < _fieldLength/2.) 
					Bfield[5] = _peakEField*sin(_refParticlePhase);
				return;
			case noField: 
				return;
			case bestGuess: break; //keep going
			case normal:    break; //keep going
		}
	}

	if(_isElectrostatic)
	{
		if(fabs(Point[2]) < _fieldLength/2.)
			Bfield[5] = _peakEField;
		return;
	}
	double particlePhase = _refParticlePhase + (Point[3] - _timeDelay)*(2*pi*_frequency);

	GetPeakFields(Point, Bfield);
	double cosPhase = cos(particlePhase);
	double sinPhase = sin(particlePhase);
	for(int i=0; i<3; i++)
	{
		Bfield[i]   *= cosPhase * _peakEField;
		Bfield[i+3] *= sinPhase * _peakEField;
	}
}

void BTPillBox::GetPeakFields( const double Point[4], double *Bfield) const
{
	//See e.g. Lee Part 3 VI.1 for bessel functions
	double r   = sqrt(Point[0]*Point[0] +  Point[1]*Point[1]);
	double rc  = r * _cavityFactor;
	double bj0 = GSL::SpecialFunctions::Bessel::J0(rc);
	if(r>0.)
	{
		double bj1 = GSL::SpecialFunctions::Bessel::J1(rc);
		Bfield[0] = -bj1 * Point[1]/r / c_light;
		Bfield[1] =  bj1 * Point[0]/r / c_light;
	}
	Bfield[5] = bj0;
}

void BTPillBox::SetParameters()
{
	double angularFrequency = 2*pi*_frequency;
	if ((_frequency == 0) || _isElectrostatic)
	{
		_isElectrostatic = true;
		_phaseIsSet      = true;
	}
	else
		_cavityRadius = 2.405/(angularFrequency/c_light);

	_cavityFactor = 2.405/_cavityRadius;
	_bToEFactor = 1./c_light;
	SetSkinDepth(0.);
	_fieldLength = _length+_skinDepth*10;
	_refParticlePhase  = _staticRefParticlePhase;
	double bbMin[3] = {-_cavityRadius/2., -_cavityRadius, -_length/2.};
	double bbMax[3] = { _cavityRadius/2.,  _cavityRadius,  _length/2.}; 
	BTField::bbMin = std::vector<double>(bbMin, bbMin+3); //3-vector that is lower corner of bounding box
	BTField::bbMax = std::vector<double>(bbMax, bbMax+3); //3-vector that is lower corner of bounding box

}

void BTPillBox::Print(std::ostream & out) const
{
	out << "PillBox ";
	PrintPrivates(out);
}


void BTPillBox::PrintPrivates(std::ostream & out) const
{
	out << "Length: " << _length << " PeakField: " << _peakEField
	    << " Delay: " << _timeDelay << " Frequency: " << _frequency;
	BTField::Print(out);
}

CLHEP::HepLorentzVector BTPillBox::GetVectorPotential(CLHEP::HepLorentzVector position) const
{
	//E_peak/omega sum(-1^l)/(l!)^2 (kr/2)^(2l) sin(omega t)
	HepLorentzVector A(0,0,0,0);
	if(fabs(position.z()) > _fieldLength/2.) return A;
	int    ORDER = 3;
	double J_new = 0;
	int    ll    = 0;
	int    lFact = 1;
	int    neg   = 1;
	double E_0   = _peakEField;
	double omega = _frequency*2*pi;
	double r2    = position.x()*position.x() + position.y()*position.y();
	double kr2   = _cavityFactor*_cavityFactor*r2; //(k*r)^2
	double krPow = kr2; //(k*r)^(2l)
	while(ll < ORDER)
	{
		J_new += neg*krPow/(lFact*lFact); 
		ll++;
		lFact *= ll;
		neg   *= -1;
		krPow *= kr2; 
	}
	A.setZ(E_0*neg*krPow*sin(omega*position.t())/lFact/lFact/omega);
	return A;
}

BTPillBox::phasingFieldModel BTPillBox::StringToPhaseModel(std::string phaseModel)
{
	for(unsigned int i=0; i<phaseModel.size(); i++) phaseModel[i] = tolower(phaseModel[i]);
	/**/ if(phaseModel == "energygainoptimised")            return bestGuess;
	else if(phaseModel == "timevarying")                    return normal;
	else if(phaseModel == "electrostatic")                  return electrostatic;
	else if(phaseModel == "nofield"||phaseModel == "none")  return noField;
	else throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Did not recognise field phasing model "+phaseModel, "BTPillBox::SetFieldDuringPhasing"));
	return noField;
}

void   BTPillBox::SetFieldDuringPhasing(std::string option)
{
	_fieldDuringPhasing = StringToPhaseModel(option);
	/**/ if(_fieldDuringPhasing == bestGuess)     _peakEField = 0.;//_refParticleCharge*_energyGain/_length;
}


