#include "BTCombinedFunction.hh"
#include "Interface/Squeal.hh"
#include "Interface/Squeak.hh"

BTCombinedFunction::BTCombinedFunction(double By, double fieldIndex, double length, double radiusOfCurvature, double height, double width, int maxPole,
                                       std::string curvature)
                  : _fields(), _by(By), _fieldIndex(fieldIndex), _length(length), _radiusOfCurvature(radiusOfCurvature), _height(height), _width(width)

{
	_fields.push_back( BTMultipole(1, By, length, radiusOfCurvature, height, width, curvature) );
	if(_fields[0].GetCurvatureModel() == BTMultipole::momentumBased) SetMomentumBased();
	for(int i=2; i<=maxPole; i++)
		_fields.push_back( BTMultipole(i, GetFieldAtPoleTip(i), length, radiusOfCurvature, height, width, curvature) );
	bbMin = std::vector<double>(_fields[0].BoundingBoxMin(), _fields[0].BoundingBoxMin()+3);
	bbMax = std::vector<double>(_fields[0].BoundingBoxMax(), _fields[0].BoundingBoxMax()+3);
}

BTCombinedFunction::BTCombinedFunction(double By, double fieldIndex, double length, double radiusOfCurvature, double height, double width, int maxPole,
	                   double effectiveWidth, double endLength, int endPole, std::vector<double> engeParameters, std::string curvature)
                  : _fields(), _by(By), _fieldIndex(fieldIndex), _length(length), _radiusOfCurvature(radiusOfCurvature), _height(height), _width(width)

{
	_fields.push_back( BTMultipole(1, By, length, radiusOfCurvature, height, width,
	                               effectiveWidth, endLength, endPole, engeParameters, curvature) );
	if(_fields[0].GetCurvatureModel() == BTMultipole::momentumBased) SetMomentumBased();
	for(int i=2; i<=maxPole; i++)
		_fields.push_back( BTMultipole(1, GetFieldAtPoleTip(i), length, radiusOfCurvature, height, width,
		                               effectiveWidth, endLength, endPole, engeParameters, curvature) );
	bbMin = std::vector<double>(_fields[0].BoundingBoxMin(), _fields[0].BoundingBoxMin()+3);
	bbMax = std::vector<double>(_fields[0].BoundingBoxMax(), _fields[0].BoundingBoxMax()+3);
}

void BTCombinedFunction::Print(std::ostream &out) const
{
	out << "Combined Function By: " << _by << " Index: " << _fieldIndex << " Length: " << _length 
	    << " RadiusOfCurvature: " << _radiusOfCurvature << " Height: " << _height << " Length: " << _length << " Width: " << _width;
	BTField::Print(out);
}

void BTCombinedFunction::SetMomentumBased()
{
	std::vector<double> refX, refZ, refS, angle;
	_fields[0].GetReferenceTrajectory(refX, refZ, angle, refS); 
	for(unsigned int i=1; i<_fields.size(); i++)
		_fields[i].SetReferenceTrajectory(refX, refZ, angle, refS);
	_by = _fields[0].GetMagnitude();	

}

CLHEP::HepLorentzVector BTCombinedFunction::GetVectorPotential(CLHEP::HepLorentzVector Point) const
{
	CLHEP::HepLorentzVector pot;
	for(unsigned int i=0; i<_fields.size(); i++)
		pot += _fields[i].GetVectorPotential(Point);
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
			throw (Squeal(Squeal::nonRecoverable, "Combined Function pole out of range", "BTCombinedFunction::GetFieldAtPoleTip(int)"));
	}
}

void BTCombinedFunction::GetReferenceTrajectory(std::vector<double>& x, std::vector<double>& z, std::vector<double>& angle, std::vector<double>& s)
{
	_fields[0].GetReferenceTrajectory(x,z,angle,s);
}

CLHEP::Hep3Vector BTCombinedFunction::GetExternalPoint() const
{
	return _fields[0].GetExternalPoint();
}

