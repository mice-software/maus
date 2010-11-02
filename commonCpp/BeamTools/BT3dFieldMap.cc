#include "BT3dFieldMap.hh"

void BT3dFieldMap::Print(std::ostream& out) const
{
	out << "3DFieldMap ";
	BTField::Print(out);
}

BT3dFieldMap::BT3dFieldMap(std::string interpolation, std::string fileName, std::string fileType, std::string symmetry)
{
	myFieldMap = new ThreeDFieldMap(interpolation, fileName, fileType, symmetry);
}

