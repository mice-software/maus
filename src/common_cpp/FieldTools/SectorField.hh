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

#ifndef _SRC_COMMON_CPP_FIELDTOOLS_SECTORFIELD_HH_
#define _SRC_COMMON_CPP_FIELDTOOLS_SECTORFIELD_HH_

#include "src/legacy/BeamTools/BTField.hh"

namespace MAUS {

class SectorField : public BTField {
  public:
    SectorField();
  	SectorField(double bbMinR, double bbMinY, double bbMinPhi,
                double bbMaxR, double bbMaxY, double bbMaxPhi);
    virtual ~SectorField();

    virtual void GetFieldValuePolar
                             (const double* point_polar, double* field_polar) = 0;

    virtual void GetFieldValue
                           (const double* point_cartes, double* field_cartes) = 0;

    static void ConvertToPolar(double* position);

    static void ConvertToPolar(const double* position_polar, double* value);

    static void ConvertToCartesian(double* position);

    static void ConvertToCartesian(const double* position_polar, double* value);

  protected:
    void SetPolarBoundingBox(double bbMinR, double bbMinY, double bbMinPhi,
                             double bbMaxR, double bbMaxY, double bbMaxPhi);

  private:

    std::vector< std::vector<double> > GetCorners
               (double bbMinR, double bbMinPhi, double bbMaxR, double bbMaxPhi);
};
}

#endif

