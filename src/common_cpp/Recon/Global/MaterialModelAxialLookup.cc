#include <math.h>

#include <algorithm>

#include "Geant4/G4Material.hh"

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Simulation/GeometryNavigator.hh"

#include "src/common_cpp/Recon/Global/MaterialModelAxialLookup.hh"

namespace MAUS {

std::vector<std::pair<double, G4Material*> > MaterialModelAxialLookup::_lookup;

MaterialModelAxialLookup::MaterialModelAxialLookup(double x, double y, double z) {
    SetMaterial(x, y, z);
}

MaterialModelAxialLookup::MaterialModelAxialLookup(const MaterialModelAxialLookup& mat) {
    *this = mat;
}

MaterialModelAxialLookup& MaterialModelAxialLookup::operator=(const MaterialModelAxialLookup& mat) {
    if (&mat == this) {
        return *this;
    }
    _material = mat._material;
    _n_e = mat._n_e;
    _I = mat._I;
    _x_0 = mat._x_0;
    _x_1 = mat._x_1;
    _C = mat._C;
    _a = mat._a;
    _k = mat._k;
    _rad_len_ratio = mat._rad_len_ratio;
    _density = mat._density;
    _z_over_a = mat._z_over_a;
    return *this;    
}

MaterialModelAxialLookup* MaterialModelAxialLookup::Clone() {
    throw MAUS::Exception(MAUS::Exception::recoverable, "Not implemented", "MaterialModelDynamic::Clone");
}

bool compare(std::pair<double, G4Material*> a_pair, double z) {
    return z < a_pair.first;
}

void MaterialModelAxialLookup::SetMaterial(double x, double y, double z) {
    if (_lookup.size() == 0)
        throw MAUS::Exception(MAUS::Exception::recoverable,
                              "Attempt to set material without building lookup table first",
                              "MaterialModelAxialLookup::Clone");
    std::pair<double, G4Material*> a_pair =
                  *std::lower_bound(_lookup.begin(), _lookup.end(), z, compare);
    MaterialModel::_material = a_pair.second;

}

void MaterialModelAxialLookup::BuildLookupTable(double z_start, double z_end) {
    //Squeak::mout(Squeak::debug) << "Compliance!" << std::endl;
    GeometryNavigator* navigator = Globals::GetMCGeometryNavigator();
    ThreeVector dir(0., 0., 1.);
    G4Material* old_mat = NULL;
    G4Material* new_mat = NULL;
    double z = z_start;
    while (z < z_end) {
        navigator->SetPoint(ThreeVector(0., 0., z));
        new_mat = navigator->GetMaterial();
        if (new_mat != old_mat) {
            std::pair<double, G4Material*> new_pair(z, new_mat);
            _lookup.push_back(new_pair);
        }
        old_mat = new_mat;
        double delta_z = navigator->ComputeStep(ThreeVector(0., 0., z), dir, 1.);
        if (fabs(delta_z) < _z_tolerance)
            delta_z  = _z_tolerance;
        z += delta_z;
    }
}

void MaterialModelAxialLookup::PrintLookupTable(std::ostream& out) {
    for (size_t i = 0; i < _lookup.size(); ++i) {
        out << _lookup[i].first << " " << _lookup[i].second->GetName() << std::endl;
    }
}

} // namespace MAUS

