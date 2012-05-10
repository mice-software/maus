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
 */

#ifndef _SRC_COMMON_CPP_FIELDTOOLS_SECTORMAP_HH_
#define _SRC_COMMON_CPP_FIELDTOOLS_SECTORMAP_HH_

#include <string>
#include <vector>
#include <map>

#include "src/legacy/Interface/Interpolator.hh"

#include "src/common_cpp/FieldTools/SectorField.hh"

namespace MAUS {

/** @class SectorMagneticFieldMap handles field map grids with sector geometry
 *
 *  SectorMagneticFieldMap provides an interface to the 3D interpolator
 *  routines for 3D field maps in a sector geometry. Interpolation is done from
 *  points in (r, y, phi) geometry off of a 3D field map stored in cartesian
 *  coordinates.
 *  - position coordinates are left in polar because it makes the interpolation
 *    easier
 *  - field coordinates are left in cartesian because it means we can avoid
 *    doing a coordinate transformation (tracking is done in cartesian
 *    coordinates)
 *
 *  A constructor is provided which calls the SectorMagneticFieldMapIO class to
 *  generate the field map, and it is expected that this will be the usual way
 *  to generate the map.
 *
 *  The SectorMagneticFieldMap class caches field maps by file name to avoid
 *  multiple loads of the same field map. This cache should be cleared after
 *  running (otherwise we have a memory leak).
 *
 *  For details on coordinate systems etc, see SectorField class
 */
class SectorMagneticFieldMap : public SectorField {
  public:
    /** Default constructor - leaves interpolator set to NULL
     */
    SectorMagneticFieldMap();

    /** Generate the field map by calling SectorMagneticFieldMapIO
     *
     *  @param file_name name of the file to read in
     *  @param file_format format of the file to read in; only option is
     *         tosca_map_1
     *  @param units vector containing the units of the file. Each unit acts as
     *         a multiplier for the corresponding column in the field map file.
     *  @param symmetry string describing any implicit symmetry in the input
     *         file. Options are defined in SetSymmetry below.
     */
    SectorMagneticFieldMap(std::string file_name, std::string file_format,
                               std::vector<double> units, std::string symmetry);

    /** Copy constructor - makes a deep copy of the field map */
    SectorMagneticFieldMap(const SectorMagneticFieldMap& field);

    /** Destructor - delete allocated memory */
    ~SectorMagneticFieldMap();

    /** Get the field value in polar coordinates
     *
     *  @param point array holding the polar 4-position at which the field is to
     *         be evaluated, (r, y, phi, time)
     *  @param field array of at least 6 doubles, to which the field at point
     *         is written (br, by, bphi, er=0, ey=0, ephi=0)
     */
    void GetFieldValuePolar(const double* point, double* field) const;

    /** Get the field value in cartesian coordinates
     *
     *  @param point array holding the cartesian 4-position at which the field
     *         is to be evaluated, (x, y, z, time)
     *  @param field array of at least 6 doubles, to which the field at point
     *         is written (bx, by, bz, ex=0, ey=0, ez=0)
     */
    void GetFieldValue(const double* point, double* field) const;

    /** Get a pointer to the interpolator or NULL if it is not set
     *
     *  Note SectorMagneticFieldMap still owns this memory.
     */
    Interpolator3dGridTo3d* GetInterpolator();

    /** Set the interpolator
     *
     *  @param interpolator set the field map interpolator. Note
     *  SectorMagneticFieldMap now owns the memory pointed to by interpolator
     */
    void SetInterpolator
                  (Interpolator3dGridTo3d* interpolator);

    /** Get a string corresponding to the field map symmetry
     *
     *  @returns field map symmetry, either "None" or "Dipole"
     */
    std::string GetSymmetry();

    /** Set the field map symmetry
     *
     *  @param name field map symmetry, either "None" or "Dipole"
     */
    void SetSymmetry(std::string name);

    /** Delete cached fields
     */
    static void ClearFieldCache();
  private:
    enum symmetry {none, dipole};

    static symmetry StringToSymmetry(std::string name);
    static std::string SymmetryToString(symmetry sym);

    void Rotate(double* value, double angle);

    Interpolator3dGridTo3d* _interpolator;
    symmetry _symmetry;
    const std::vector<double> _units;
    const std::string _format;
    const std::string _filename;

    static std::map<std::string, SectorMagneticFieldMap*> _fields;
    friend class SectorMagneticFieldMapIO;

    // disabled - dont use
    SectorMagneticFieldMap& operator=(const SectorMagneticFieldMap& field);
};

/** @class SectorMagneticFieldMapIO handles reading sector field maps
 *
 *  SectorMagneticFieldMap provides routines to read a sector field map for
 *  input to tracking. At the moment, only one routine is implemented,
 *  "tosca_map_1"
 */
class SectorMagneticFieldMapIO {
  public:
    /** Read in the field map
     *
     *  Read in the field map with some specified geometry
     *
     *  @param file_name name of the file containing the field map
     *  @param file_format type of the file containing the field map. Options
     *         are
     *    - tosca_sector_1: 8 header lines followed by field information
     *      formatted like
     *      <r, y, phi, Bx, By, Bz>
     *  @param units vector of length equal to the number of columns in the
     *         field map file; each value gives the unit of the corresponding
     *         column
     *  @param symmetry string defining implicit the symmetry of the input field
     *         map. Options are
               - "Dipole" reflect field map about y_min
     *         - "None" do nothing
     */
    static Interpolator3dGridTo3d* ReadMap(std::string file_name,
                                           std::string file_format,
                                           std::vector<double> units,
                                           std::string symmetry);

  private:
    static const double float_tolerance;
    static const int sort_order[3];

    // read and sort the lines of the map file
    static std::vector< std::vector<double> > ReadLines
                             (std::string file_name, std::vector<double> units);

    // generate a grid based on the input map file
    static ThreeDGrid* GenerateGrid
                        (const std::vector< std::vector<double> > field_points,
                         SectorMagneticFieldMap::symmetry sym);

    // get the interpolator based on field points and grid information
    static Interpolator3dGridTo3d* GetInterpolator
                        (const std::vector< std::vector<double> > field_points,
                         ThreeDGrid* grid,
                         SectorMagneticFieldMap::symmetry sym);

    // Read field of type tosca_sector_1
    static Interpolator3dGridTo3d* ReadToscaMap(std::string file_name,
                                                     std::vector<double> units,
                                          std::string symmetry);

    // comparator for sorting field_points by r, y, phi. Sort order is given by
    // sort_order variable
    static bool Comparator
             (std::vector<double> field_item1, std::vector<double> field_item2);

    // private constructor i.e. disabled
    SectorMagneticFieldMapIO();
    // private copy constructor i.e. disabled
    SectorMagneticFieldMapIO(const SectorMagneticFieldMapIO& map);

    // private destructor i.e. disabled
    ~SectorMagneticFieldMapIO();
};
}

#endif

