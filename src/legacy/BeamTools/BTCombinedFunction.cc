// MAUS WARNING: THIS IS LEGACY CODE.
#include "BTCombinedFunction.hh"
#include "Utils/Exception.hh"
#include "Utils/Squeak.hh"

BTCombinedFunction::BTCombinedFunction
      (int maxPole, double By, double fieldIndex, double length, double height,
       double width, std::string curvature, double radiusVariable, 
       const BTMultipole::EndFieldModel* endfield, int endPole) 
    : _fields(), _by(By), _fieldIndex(fieldIndex), _length(length),
      _radiusOfCurvature(radiusVariable), _height(height), _width(width) {

	_fields.push_back( new BTMultipole() );
  _fields.back()->Init(1, _by, length, height, width, curvature,
                      radiusVariable, endfield, endPole);
	if (_fields[0]->GetCurvatureModel() == BTMultipole::momentumBased) 
    SetMomentumBased();
	for (int i = 2; i <= maxPole; ++i) {
		_fields.push_back( new BTMultipole() );
    _fields.back()->Init(i, GetFieldAtPoleTip(i), length, height,
                       width, curvature, radiusVariable, endfield, endPole);
  } 
	bbMin = std::vector<double>(_fields[0]->BoundingBoxMin(),
                              _fields[0]->BoundingBoxMin()+3);
	bbMax = std::vector<double>(_fields[0]->BoundingBoxMax(),
                              _fields[0]->BoundingBoxMax()+3);
}


void BTCombinedFunction::Print(std::ostream &out) const
{
	out << "Combined Function By: " << _by << " Index: " << _fieldIndex 
      << " Length: " << _length  << " RadiusOfCurvature: " 
      << _radiusOfCurvature << " Height: " << _height << " Length: " << _length
      << " Width: " << _width;
	BTField::Print(out);
}

void BTCombinedFunction::SetMomentumBased()
{
	std::vector<double> refX, refZ, refS, angle;
	_fields[0]->GetReferenceTrajectory(refX, refZ, angle, refS); 
	for(unsigned int i=1; i<_fields.size(); i++)
		_fields[i]->SetReferenceTrajectory(refX, refZ, angle, refS);
	_by = _fields[0]->GetMagnitude();	

}

CLHEP::HepLorentzVector BTCombinedFunction::GetVectorPotential(CLHEP::HepLorentzVector Point) const
{
	CLHEP::HepLorentzVector pot;
	for(unsigned int i=0; i<_fields.size(); i++)
		pot += _fields[i]->GetVectorPotential(Point);
	return pot;
}

double BTCombinedFunction::GetFieldAtPoleTip(int pole)
{
	switch(pole)
	{
		case 1:
			return _by;
		case 2:
			return _by*_fieldIndex/_radiusOfCurvature*_width/2.; //field at pole tip
		case 3:
		{
			if(_radiusOfCurvature==0) return 0;
			double b0 = _by/pow(fabs(_radiusOfCurvature),fabs(_fieldIndex));
			return b0 * _fieldIndex*(_fieldIndex+1)/(2*_radiusOfCurvature*_radiusOfCurvature)*_width/2.*_width/2.;
		}
		default:
			throw (MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable, "Combined Function pole out of range", "BTCombinedFunction::GetFieldAtPoleTip(int)"));
	}
}

void BTCombinedFunction::GetReferenceTrajectory(std::vector<double>& x, std::vector<double>& z, std::vector<double>& angle, std::vector<double>& s)
{
	_fields[0]->GetReferenceTrajectory(x,z,angle,s);
}

CLHEP::Hep3Vector BTCombinedFunction::GetExternalPoint() const
{
	return _fields[0]->GetExternalPoint();
}

