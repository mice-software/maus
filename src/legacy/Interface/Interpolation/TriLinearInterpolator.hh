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

#ifndef _SRC_LEGACY_INTERFACE_INTERPOLATION_TRILINEARINTERPOLATOR_HH_
#define _SRC_LEGACY_INTERFACE_INTERPOLATION_TRILINEARINTERPOLATOR_HH_

#include "src/legacy/Interface/Interpolation/Interpolator3dGridTo1d.hh"

/** TriLinearInterpolator performs a linear interpolation in x then y then z
 *
 *  Performs a linear interpolation in x then y then z returning a 1D value.
 */
class TriLinearInterpolator : public Interpolator3dGridTo1d {
  public:
    /** Constructor for grids with constant spacing
     *
     *  \param grid 3d mesh on which data is stored - adds a reference to *this
     *              into the mesh smart pointer thing.
     *  \param F function data with points on each element of the grid. Indexing
     *           goes like [index_x][index_y][index_z]. Interpolator3dGridTo1d now
     *           owns this memory
     *  All the data handling is done at Interpolator3dGridTo1d
     */
    inline TriLinearInterpolator(ThreeDGrid *grid, double ***F);

    /** Copy constructor
     *
     *  Deep copies the mesh and the function data
     */
    TriLinearInterpolator(const TriLinearInterpolator& tli);

    /** Destructor - removes reference from the mesh and from the function data
     */
    inline ~TriLinearInterpolator();

    /** Get the interpolated value of the function at some point
     *
     *  First does bound checking, then makes linear interpolations using the
     *  standard 1d interpolation formula \n
     *  \f$y(x) \approx \frac{\Delta y}{\Delta x} dx + y_0\f$ \n
     *  \f$y(x) \approx \frac{y_1(x_1)-y_0(x_0)}{dx}(x-x_0) + y_0\f$ \n
     *  Interpolate along 4 x grid lines to make a 2D problem, then interpolate
     *  along 2 y grid lines to make a 1D problem, then finally interpolate in z
     *  to get the value.
     */
    void F(const double Point[3], double Value[1]) const;

    /** Copy function (can be called on parent class) */
    inline TriLinearInterpolator* Clone() const;
};

TriLinearInterpolator::TriLinearInterpolator(ThreeDGrid *grid, double ***F)
    : Interpolator3dGridTo1d(grid, F) {
}

TriLinearInterpolator::~TriLinearInterpolator() {
}

TriLinearInterpolator* TriLinearInterpolator::Clone() const {
    return new TriLinearInterpolator(*this);
}

#endif


