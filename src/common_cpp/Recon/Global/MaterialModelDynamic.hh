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

#ifndef recon_global_materialmodeldynamic_hh_
#define recon_global_materialmodeldynamic_hh_

#include <set>
#include <iostream>

#include "src/common_cpp/Recon/Global/MaterialModel.hh"

class G4Material;

namespace MAUS {
class GeometryNavigator;

/** @class MaterialModelDynamic performs material model lookups based on the
 *         full Geant4 geometry.
 */
class MaterialModelDynamic : public MaterialModel {
  public:
    /** Default constructor does nothing */
    MaterialModelDynamic() {}

    /** Constructor that performs a lookup on construction
     *
     *  @param x: horizontal position of the material to be found
     *  @param y: vertical position of the material to be found
     *  @param z: longitudinal position of the material to be found; note that x
     *            and y is ignored and only z is used for axial lookup.
     *
     *  Sets up the material model for material at the corresponding position,
     *  based on the Geant4 geometry
     */
    MaterialModelDynamic(double x, double y, double z);

    /** Copy constructor */
    MaterialModelDynamic(const MaterialModelDynamic& mat);

    /** Destructor does nothing */
    virtual ~MaterialModelDynamic() {}

    /** Assignment operator constructor */
    MaterialModelDynamic& operator=(const MaterialModelDynamic& mat);

    /** Inherited copy constructor */
    MaterialModelDynamic* Clone();

    /*  Performs a lookup
     *
     *  @param x: horizontal position of the material to be found
     *  @param y: vertical position of the material to be found
     *  @param z: longitudinal position of the material to be found; note that x
     *            and y is ignored and only z is used for axial lookup.
     *
     *  Sets up the material model for material at the corresponding position,
     *  based on the Geant4 geometry
     */
    void SetMaterial(double x, double y, double z);

  private:
};

}

#endif

