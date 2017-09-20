// MAUS WARNING: THIS IS LEGACY CODE.
#include "src/legacy/Interface/Meshing/ThreeDGrid.hh"
#include "BT3dFieldMap.hh"

void BT3dFieldMap::Print(std::ostream& out) const
{
	out << "3DFieldMap ";
	BTField::Print(out);
}

BT3dFieldMap::BT3dFieldMap(std::string interpolation, std::string fileName, std::string fileType, std::string symmetry)
{
	myFieldMap = new ThreeDFieldMap(interpolation, fileName, fileType, symmetry);
  SetBoundingBox();
}

void BT3dFieldMap::SetBoundingBox() {
  ThreeDGrid* grid = myFieldMap->GetMesh();
	bbMin[0] = grid->MinX();
	bbMin[1] = grid->MinY();
	bbMin[2] = grid->MinZ();
	bbMax[0] = grid->MaxX();
	bbMax[1] = grid->MaxY();
	bbMax[2] = grid->MaxZ();
}

BT3dFieldMap::BT3dFieldMap(const BT3dFieldMap &rhs) : myFieldMap(new ThreeDFieldMap(*rhs.myFieldMap)) {
    SetBoundingBox();
}

BT3dFieldMap& BT3dFieldMap::operator=(const BT3dFieldMap& rhs) {
    myFieldMap = new ThreeDFieldMap(*rhs.myFieldMap);
    SetBoundingBox();
    return *this;
}


