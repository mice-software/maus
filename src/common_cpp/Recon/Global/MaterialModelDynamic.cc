#include <math.h>
#include "Geant4/G4Material.hh"

#include "src/common_cpp/Simulation/GeometryNavigator.hh"
#include "src/common_cpp/Utils/Squeak.hh"

#include "src/common_cpp/Recon/Global/MaterialModelDynamic.hh"

namespace MAUS {

MaterialModelDynamic::MaterialModelDynamic(double x, double y, double z) {
    SetMaterial(x, y, z);
}

MaterialModelDynamic::MaterialModelDynamic(const MaterialModelDynamic& mat) {
    *this = mat;
}

MaterialModelDynamic& MaterialModelDynamic::operator=(const MaterialModelDynamic& mat) {
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

MaterialModelDynamic* MaterialModelDynamic::Clone() {
    throw Exceptions::Exception(Exceptions::recoverable,
                              "Not implemented", "MaterialModelDynamic::Clone");
}

void MaterialModelDynamic::SetMaterial(double x, double y, double z) {
    _navigator = Globals::GetMCGeometryNavigator();
    _navigator->SetPoint(ThreeVector(x, y, z));
    std::string material_name = _navigator->GetMaterial()->GetName();
    if (IsEnabledMaterial(material_name)) {
        Squeak::mout(Squeak::debug) << "MaterialModelDynamic::SetMaterial Setting "
                  << _navigator->GetMaterial()->GetName()
                  << " as it is enabled at position "
                  << "x " << x << " y " << y << " z " << z << std::endl;
        MaterialModel::SetMaterial(_navigator->GetMaterial());
    } else {
        Squeak::mout(Squeak::debug) << "MaterialModelDynamic::SetMaterial Ignoring "
                  << _navigator->GetMaterial()->GetName()
                  << " as it is disabled at position "
                  << "x " << x << " y " << y << " z " << z << std::endl;
        //_disabled_materials.insert(material_name);
        *this = MaterialModelDynamic();
    }
}
} // namespace MAUS

