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

#include <vector>

#include "src/legacy/BeamTools/BTField.hh"

namespace MAUS {

/** @class SectorField
 *
 *  SectorField is an abstraction type for a sector field map i.e. a field map
 *  with a bent geometry as in a ring. Functions are provided for converting
 *  from cartesian coordinate systems to ring coordinate systems and vice versa
 *  and field calculation functions are defined for the field in cartesian
 *  coordinates and additionally ring coordinates.
 *
 *  The sector field map holds two bounding boxes - the standard rectangular
 *  bounding box in cartesian coordinates and additionally a sector bounding box
 *  in polar coordinates. The bounding box should always be given in polar
 *  coordinates - SectorField will do the conversion to cartesian coordinates
 *  (note this is a lossy procedure)
 *
 *  Ring coordinates are typically given as three vectors going like
 *  (radius, y, angle) also denoted by (r, y, phi). The ring coordinate system
 *  is defined with (x, y, z) = (0, 0, 0) the centre of the ring; y is the same
 *  in ring and cartesian coordinate systems. Positive phi is anticlockwise in
 *  the (x, z) plane.
 */
class SectorField : public BTField {
  public:
    /** Make an empty sector field; set bounding box to max double
     */
    SectorField();

    /** Make a sector field with the given bounding box
     */
  	SectorField(double bbMinR, double bbMinY, double bbMinPhi,
                double bbMaxR, double bbMaxY, double bbMaxPhi);

    /** Destructor (does nothing)
     */
    virtual ~SectorField();

    /** Return the field value in polar coordinates
     *
     *  @param point_polar position in cylindrical coordinates at which to
     *         evaluate the field. Should be an array of length 4 like
     *         (r, y, phi, t)
     *  @param field_polar pointer to an allocated block of at least 6 doubles.
     *         The function will fill the block with the value of the field in
     *         cylindrical polar coordinates,
     *          (b_r, b_y, b_phi, e_r, e_y, e_phi)
     *         Overwrites any existing data
     */
    virtual void GetFieldValuePolar
                     (const double* point_polar, double* field_polar) const = 0;

    /** Return the field value in cartesian coordinates
     *
     *  @param point_cartes position in cartesian coordinates at which to
     *         evaluate the field. Should be an array of length 4 like
     *         (x, y, z, t)
     *  @param field_cartes pointer to an allocated block of at least 6 doubles.
     *         The function will fill the block with the value of the field in
     *         cartesian polar coordinates,
     *          (b_x, b_y, b_z, e_x, e_y, e_z)
     *         Overwrites any existing data
     */
    virtual void GetFieldValue
                   (const double* point_cartes, double* field_cartes) const = 0;

    /** Convert a position from cartesian to polar coordinates
     *
     *  @param position position in cartesian coordinates to convert to
     *         cylindrical polar coordinates. Input should be an allocated block
     *         of at least 3 doubles containing values (x, y, z). This is over
     *         written with 3 double containing values (r, y, phi).
     */
    static void ConvertToPolar(double* position);

    /** Convert a 3 vector from cartesian to polar coordinate system
     *
     *  @param position position in polar coordinates at which the value is
     *         valid
     *  @param value pointer to an allocated block of at least 3 doubles. The
     *         function will apply a rotation to the existing data to render it
     *         from cartesian coordinates (a_x, a_y, a_z) to polar coordinates
     *         (a_r, a_y, a_phi) appropriate for the specified position.
     */
    static void ConvertToPolar(const double* position_polar, double* value);

    /** Convert a position from polar coordinates to cartesian
     *
     *  @param position position in polar coordinates to convert to
     *         cartesian polar coordinates. Input should be an allocated block
     *         of at least 3 doubles containing values (r, y, phi). This is over
     *         written with 3 double containing values (x, y, z).
     */
    static void ConvertToCartesian(double* position);

    /** Convert a 3 vector from polar to cartesian coordinate system
     *
     *  @param position position in polar coordinates at which the value is
     *         valid
     *  @param value pointer to an allocated block of at least 3 doubles. The
     *         function will apply a rotation to the existing data to render it
     *         from polar coordinates (a_r, a_y, a_phi) to polar coordinates
     *         (a_x, a_y, a_z) appropriate for the specified position.
     */
    static void ConvertToCartesian(const double* position_polar, double* value);

    /** Get the minimum bounding box in polar coordinates
     *
     *  @returns bounding box minimum as a 3-vector like (r_min, y_min, phi_min) 
     */
    virtual std::vector<double> GetPolarBoundingBoxMin() const;

    /** Get the maximum bounding box in polar coordinates
     *
     *  @returns bounding box maximum as a 3-vector like (r_max, y_max, phi_max) 
     */
    virtual std::vector<double> GetPolarBoundingBoxMax() const;

  protected:
    /** Set the bounding boxes from polar coordinates
     *
     *  Sets the bounding boxes, both polar and cartesian, based on a set of
     *  minimum and maximum polar coordinates
     *
     *  @param bbMinR minimum radius - must be positive
     *  @param bbMinY minimum y value
     *  @param bbMinPhi minimum phi value - must be greater than -2*pi
     *  @param bbMaxR maximum radius - must be greater than minimum radius
     *  @param bbMaxY maximum y value - must be greater than minimum y value
     *  @param bbMaxPhi maximum phi value - must be greater than minimum phi and
     *                  less than 2*pi
     */
    void SetPolarBoundingBox(double bbMinR, double bbMinY, double bbMinPhi,
                             double bbMaxR, double bbMaxY, double bbMaxPhi);

    /** bounding box minimum as a 3-vector like (r_min, y_min, phi_min) */
    std::vector<double> _polarBBMin;
    /** bounding box maximum as a 3-vector like (r_max, y_max, phi_max) */
    std::vector<double> _polarBBMax;
  private:

    std::vector< std::vector<double> > GetCorners
               (double bbMinR, double bbMinPhi, double bbMaxR, double bbMaxPhi);
};
}

#endif

