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

#ifndef _SRC_LEGACY_INTERFACE_INTERPOLATION_INTERPOLATOR3DGRIDTO3D_HH_
#define _SRC_LEGACY_INTERFACE_INTERPOLATION_INTERPOLATOR3DGRIDTO3D_HH_

#include "Interface/Mesh.hh"
#include "src/legacy/Interface/Interpolation/VectorMap.hh"
#include "src/legacy/Interface/Interpolation/Interpolator3dGridTo1d.hh"
#include "src/legacy/Interface/Interpolation/TriLinearInterpolator.hh"

/** Interpolator3dGridTo3d interpolates from 3d grid to a 3d vector
 * 
 *  Wraps three Interpolator3dGridTo1d, one for each variable in the output
 *  vector B. At the moment the wrapped Interpolator3dGridTo1d is encoded by
 *  means of an enumeration, with the only possible value trilinear interpolator
 *
 *  Function data for each of the 3d output data on the mesh is written as 3d
 *  arrays Bx, By, Bz and we make a TriLinear interpolator for each array
 *
 *  Could be that we make Interpolator3dGridTo3d an interface class and then
 *  make specific implementations for different interpolation routines (of which
 *  three trilinear interpolators is an implementation of the interface).
 */


class Interpolator3dGridTo3d : public VectorMap {
 public:
  /** Enumerator encoding possible interpolation routines */
  enum interpolationAlgorithm {triLinear};

  /** Constructor for grids with constant spacing
   *
   *  \param grid 3d grid that indexes the data. *this is added to the smart
   *         pointer in the mesh
   *  \param Bx value[0] data. 3D arrays go like [index_x][index_y][index_z],
   *            and Interpolator3dGridTo3d now owns this memory
   *  \param By value[1] data. 3D arrays go like [index_x][index_y][index_z],
   *            and Interpolator3dGridTo3d now owns this memory
   *  \param Bz value[2] data. 3D arrays go like [index_x][index_y][index_z],
   *            and Interpolator3dGridTo3d now owns this memory
   *  \param algo interpolation algorithm (defaults to triLinear)
   */
  inline Interpolator3dGridTo3d(ThreeDGrid* grid,
                         double ***Bx, double ***By, double ***Bz,
                         interpolationAlgorithm algo = triLinear);

  /** Copy constructor deep copies 1d interpolators and mesh
   *
   *  Note this makes a whole bunch of extra meshes because each interpolator
   *  deep copy makes a new mesh - that's a bit of a mess
   */
  Interpolator3dGridTo3d(const Interpolator3dGridTo3d& interpolator);

  /** Delete member interpolators and remove *this from the mesh smart pointer
   */
  ~Interpolator3dGridTo3d() {Clear();}

  /** Copy function (can be called on parent class) */
  Interpolator3dGridTo3d* Clone() const {return new Interpolator3dGridTo3d(*this);}

  /** Return the interpolated data
   *
   *  Calls each child interpolator in turn and sets the value. Note that this
   *  means we get 3 sets of bound checks plus bound checking on the parent
   *  which is a bit unpleasant. I think that the actual interpolation however
   *  does have to be done independently for each variable.
   */
  void F(const double Point[3], double Value[3]) const;

  /** Number of x coordinates in the grid */
  inline int NumberOfXCoords() const;

  /** Number of y coordinates in the grid */
  inline int NumberOfYCoords() const;

  /** Number of z coordinates in the grid */
  inline int NumberOfZCoords() const;

  /** Dimension of input points */
  inline unsigned int PointDimension()  const;

  /** Dimension of output values */
  inline unsigned int ValueDimension()  const;

  /** Return a pointer to the mesh*/
  inline ThreeDGrid* GetMesh();

  /** Reset the mesh
   *
   *  Removes reference to this from mesh smart pointer if appropriate
   */
  inline void SetGrid(ThreeDGrid* grid);

  /** Set x coordinates in the mesh to an arbitrary set of points
   *
   *  If mesh is NULL, does nothing
   */
  inline void SetX(int nCoords, double* x);

  /** Set y coordinates in the mesh to an arbitrary set of points
   *
   *  If mesh is NULL, does nothing
   */
  inline void SetY(int nCoords, double* y);

  /** Set z coordinates in the mesh to an arbitrary set of points
   *
   *  If mesh is NULL, does nothing
   */
  inline void SetZ(int nCoords, double* z);

  /** Set function and mesh data
   */
  void Set(ThreeDGrid* grid, double *** Bx, double *** By, double *** Bz,
           interpolationAlgorithm algo = triLinear);

  /** Clear all private data */
  inline void Clear();

 protected:
  ThreeDGrid             *_coordinates;
  Interpolator3dGridTo1d *_interpolator[3];
};

int Interpolator3dGridTo3d::NumberOfXCoords() const {
    return _coordinates->xSize();
}

int Interpolator3dGridTo3d::NumberOfYCoords() const {
    return _coordinates->ySize();
}

int Interpolator3dGridTo3d::NumberOfZCoords() const {
    return _coordinates->zSize();
}

unsigned int Interpolator3dGridTo3d::PointDimension()  const {
    return 3;
}

unsigned int Interpolator3dGridTo3d::ValueDimension()  const {
    return 3;
}

ThreeDGrid* Interpolator3dGridTo3d::GetMesh() {
    return _coordinates;
}

void Interpolator3dGridTo3d::SetGrid(ThreeDGrid* grid) {
    if (_coordinates != NULL)
        _coordinates->Remove(this);
    grid->Add(this);
    _coordinates = grid;
}

void Interpolator3dGridTo3d::SetX(int nCoords, double* x) {
    if (_coordinates != NULL)
        _coordinates->SetX(nCoords, x);
}

void Interpolator3dGridTo3d::SetY(int nCoords, double* y) {
    if (_coordinates != NULL)
        _coordinates->SetY(nCoords, y);
}

void Interpolator3dGridTo3d::SetZ(int nCoords, double* z) {
    if (_coordinates != NULL)
        _coordinates->SetZ(nCoords, z);
}

inline Interpolator3dGridTo3d::Interpolator3dGridTo3d(ThreeDGrid* grid,
                       double ***Bx, double ***By, double ***Bz,
                       interpolationAlgorithm algo)
      : _coordinates(NULL) {
    for (int i = 0; i < 3; i++)
        _interpolator[i] = NULL;
    Set(grid, Bx, By, Bz);
}

void Interpolator3dGridTo3d::Clear() {
    for (int i = 0; i < 3; i++)
        delete _interpolator[i];
    _coordinates->Remove(this);
}

#endif

