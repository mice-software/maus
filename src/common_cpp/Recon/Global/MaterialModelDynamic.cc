/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
        *this = MaterialModelDynamic();
    }
}
} // namespace MAUS

