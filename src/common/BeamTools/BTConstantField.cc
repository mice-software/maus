#include "BTConstantField.hh"

BTConstantField::BTConstantField(double length, double radius, CLHEP::Hep3Vector field) 
    : _isCylindrical(true), _length(length), _x(0), _y(0), _rSquared(radius*radius)
{
	BTField::bbMin[0] = -radius;
	BTField::bbMin[1] = -radius;
	BTField::bbMin[2] = -length;
	BTField::bbMax[0] = radius;
	BTField::bbMax[1] = radius;
	BTField::bbMax[2] = length;
	for(int i=0; i<3; i++)
	{
		_field[i]   = field[i];
		_field[i+3] = 0;
	}
}

BTConstantField::BTConstantField(double length, double width, double height, CLHEP::Hep3Vector field)
    : _isCylindrical(false), _length(length), _x(width), _y(height), _rSquared(0)
{
	BTField::bbMin[0] = -width;
	BTField::bbMin[1] = -height;
	BTField::bbMin[2] = -length/2.;
	BTField::bbMax[0] = width;
	BTField::bbMax[1] = height;
	BTField::bbMax[2] = length/2.;
	for(int i=0; i<3; i++)
	{
		_field[i]   = field[i];
		_field[i+3] = 0;
	}
}


void BTConstantField::Print(std::ostream & out) const 
{
	out << "ConstantField Length: " << _length ;
	if(_isCylindrical) out << " Radius: " << sqrt(_rSquared);
	else out << " Width: " << _x << " Height: " << _y ;
	out << " Field: " << _field[0] << " " << _field[1] << " " << _field[2]; 
	BTField::Print(out);
}


bool BTConstantField::IsInsideRectangularVolume(const double Point[4]) const
{
        if(_x>fabs(Point[0]) && _y>fabs(Point[1]) && _length/2.>fabs(Point[2]) ) return true;
	else return false;
}

bool BTConstantField::IsInsideCylindricalVolume(const double Point[4]) const
{
        double radiusSquared = Point[0]*Point[0]+Point[1]*Point[1];
        if(radiusSquared<_rSquared && _length/2.>fabs(Point[2]) ) return true;
	else return false;
}



void BTConstantField::GetFieldValue(const double Point[4], double * bfield) const
{
        if(!_isCylindrical && IsInsideRectangularVolume(Point))
        for(int i=0; i<6; i++) bfield[i] = _field[i];
        else if(_isCylindrical && IsInsideCylindricalVolume(Point))
                for(int i=0; i<6; i++) bfield[i] = _field[i];
        else for(int i=0; i<6; i++) bfield[i]    = 0;
}


