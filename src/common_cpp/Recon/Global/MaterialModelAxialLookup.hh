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

#ifndef recon_global_materialmodelaxiallookup_hh_
#define recon_global_materialmodelaxiallookup_hh_

#include <utility>
#include <vector>

#include "src/common_cpp/Recon/Global/MaterialModel.hh"


class G4Material;

namespace MAUS {
class GeometryNavigator;

/** Material model that builds a lookup table to determine materials at a given
 *  z position; assumes cylinders of infinite radius, based on a search for
 *  materials on axis done using the MAUS internal geometry.
 *
 *  The lookup table is stored as a static class member.
 */
class MaterialModelAxialLookup : public MaterialModel {
  public:
    /** Constructor - does nothing */
    MaterialModelAxialLookup() {}

    /** Constructor that performs a lookup on construction
     *
     *  @param x: horizontal position of the material to be found
     *  @param y: vertical position of the material to be found
     *  @param z: longitudinal position of the material to be found; note that x
     *            and y is ignored and only z is used for axial lookup.
     *
     *  Sets up the material model for material at the corresponding position.
     *  If z is out of the lookup table bounds, material is set to the nearest
     *  material (e.g. the material at the upstream or downstream end of the
     *  lookup table)
     */
    MaterialModelAxialLookup(double x, double y, double z);

    /** Copy constructor */
    MaterialModelAxialLookup(const MaterialModelAxialLookup& mat);

    /** Destructor does nothing - there is no memory to clean up */
    virtual ~MaterialModelAxialLookup() {}

    /** Assignment operator */
    MaterialModelAxialLookup& operator=(const MaterialModelAxialLookup& mat);

    /** Inheritable copy constructor */
    MaterialModelAxialLookup* Clone();

    /** Sets the material according to a lookup
     *
     *  @param x: horizontal position of the material to be found
     *  @param y: vertical position of the material to be found
     *  @param z: longitudinal position of the material to be found; note that x
     *            and y is ignored and only z is used for axial lookup.
     *
     *  Sets up the material model for material at the corresponding position.
     *  If z is out of the lookup table bounds, material is set to the nearest
     *  material (e.g. the material at the upstream or downstream end of the
     *  lookup table)
     */
    void SetMaterial(double x, double y, double z);

    /** Set up the lookup table
     *
     *  @param z_start: start position for material lookup
     *  @param z_end: end position for material lookup
     *
     *  Walk along the line from z_start to z_end, in steps of _z_tolerance, and
     *  record the position and new material each time the material changes.
     */
    static void BuildLookupTable(double z_start, double z_end);

    /** Get the position of the boundaries for a given z position
     *
     *  @param: z_pos: the position at which the lookup is performed
     *  @param: lower_bound: filled with the lowest z position with contiguous
     *          material as material at z_pos
     *  @param: upper_bound: filled with the highest z position with contiguous
     *          material as material at z_pos
     *
     *  This is used for e.g. setting dynamic step size. If z_pos is off the end
     *  of the lookup table, fills the appropriate bound with the appropriate
     *  numeric limit.
     */
    static void GetBounds(double z_pos, double& lower_bound, double& upper_bound);

    /** Print the lookup table, i.e. print what material was found where.
     */
    static void PrintLookupTable(std::ostream& out);

    /** Return a pointer to the lookup table.
     *
     *  @returns a pointer to the lookup table. MaterialModelAxialLookup owns
     *  the returned memory.
     */
    static const std::vector<std::pair<double, G4Material*> >* GetLookupTable() {
        return &_lookup;
    }

    /** @return true if a lookup table exists (with more than 0 elements).
     */
    static bool IsReady() {return _lookup.size() > 0;}

    /** @return the z_tolerance used to construct the lookup table
     */
    static double GetZTolerance() {return _z_tolerance;}

    /** Set the z tolerance used to construct the lookup table
     */
    static void SetZTolerance(double z_tolerance) {_z_tolerance = z_tolerance;}


  private:

    static double _z_tolerance;

    // references start position of the volume and material
    static std::vector<std::pair<double, G4Material*> > _lookup;
};
}

#endif

