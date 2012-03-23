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

#include "src/legacy/Interface/Interpolator.hh"

#include "src/common_cpp/FieldTools/SectorField.hh"

namespace MAUS {

class SectorMagneticFieldMap : public SectorField {
  public:
    SectorMagneticFieldMap();

    SectorMagneticFieldMap(std::string file_name, std::string file_format,
                               std::vector<double> units, std::string symmetry);

    SectorMagneticFieldMap(const SectorMagneticFieldMap& field);

    SectorMagneticFieldMap& operator=(const SectorMagneticFieldMap& field);

    ~SectorMagneticFieldMap();

    void GetFieldValuePolar(const double* point, double* field);

    void GetFieldValue(const double* point, double* field);

    Interpolator3dGridTo3d* GetInterpolator();
    void SetInterpolator(Interpolator3dGridTo3d* interpolator);

    std::string GetSymmetry();
    void SetSymmetry(std::string name);

  private:
    enum symmetry {none, dipole};

    static symmetry StringToSymmetry(std::string name);
    static std::string SymmetryToString(symmetry sym);

    void Rotate(double* value, double angle);

    Interpolator3dGridTo3d* _interpolator;
    symmetry _symmetry;

    friend class SectorMagneticFieldMapIO;

};

class SectorMagneticFieldMapIO {
  public:
    static Interpolator3dGridTo3d* ReadMap(std::string file_name,
                                           std::string file_format,
                                           std::vector<double> units,
                                           std::string symmetry);    

    static Interpolator3dGridTo3d* ReadToscaMap(std::string file_name,
                                                     std::vector<double> units,
                                          std::string symmetry);

    static bool Comparator
             (std::vector<double> field_item1, std::vector<double> field_item2);
  private:
    static const double float_tolerance;
    static const int sort_order[3];

    SectorMagneticFieldMapIO();
    SectorMagneticFieldMapIO(const SectorMagneticFieldMapIO& map);
    ~SectorMagneticFieldMapIO();
};

}

#endif

