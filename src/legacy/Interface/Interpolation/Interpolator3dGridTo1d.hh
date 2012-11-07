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

#ifndef _SRC_LEGACY_INTERFACE_INTERPOLATION_INTERPOLATOR3DGRIDTO1D_HH_
#define _SRC_LEGACY_INTERFACE_INTERPOLATION_INTERPOLATOR3DGRIDTO1D_HH_

#include "src/legacy/Interface/Interpolation/VectorMap.hh"

/** Interpolator3dGridTo1d is an abstraction for lookup on a 3D mesh to get a 1D
 *  value.
 *
 *  Interpolator3dGridTo1d abstracts the lookup from a 3D mesh to get a 1D value
 *  This is a building block for higher dimension (e.g. field vector lookup)
 *  where we can do multiple lookups one for each field, at the cost of doing
 *  multiple access/interpolation operations to the grid.
 */
class Interpolator3dGridTo1d : public VectorMap {
 public:
  /** Constructor for grids with constant spacing
   *
   *  \param grid 3d mesh on which data is stored - adds a reference to *this
   *              into the mesh smart pointer thing.
   *  \param F function data with points on each element of the grid. Indexing
   *           goes like [index_x][index_y][index_z]. Interpolator3dGridTo1d now
   *           owns this memory
   */
  inline Interpolator3dGridTo1d(ThreeDGrid* grid, double ***F);

  /** Default constructor - initialises data to NULL
   *
   *  Note I have no NULL checking on most member functions so be careful
   */
  inline Interpolator3dGridTo1d();

  /** Deletes F data and removes *this from ThreeDGrid smart pointer thing. */
  inline virtual ~Interpolator3dGridTo1d();

  /** Get the interpolated function data */
  virtual void F(const double Point[3], double Value[1]) const = 0;

  /** Copy constructor */
  virtual Interpolator3dGridTo1d* Clone() const = 0;

  /** Number of x coordinates in the grid */
  inline int NumberOfXCoords() const;

  /** Number of y coordinates in the grid */
  inline int NumberOfYCoords() const;

  /** Number of z coordinates in the grid
   */
  inline int NumberOfZCoords() const;

  /** Dimension of input points */
  inline unsigned int PointDimension() const;

  /** Dimension of output values */
  inline unsigned int ValueDimension() const;

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

  /** Set function data, deleting any existing function data
   */
  inline void SetF(double*** inF);

  /** Delete any existing function data
   */
  void DeleteFunc(double*** func);

  /** Return a pointer to the function data
   */
  inline double*** F() const;

  /** Set function and mesh data
   */
  inline void Set(ThreeDGrid* grid, double *** F);

  /** Clear all private data */
  inline void Clear();

  /** Options for interpolation algorithms */
  enum interpolationAlgorithm {biCubic, linearCubic};
 protected:
  ThreeDGrid   *_coordinates;
  double     ***_F;
};

Interpolator3dGridTo1d::Interpolator3dGridTo1d(ThreeDGrid* grid, double ***F)
      : _coordinates(NULL), _F(NULL) {
    Set(grid, F);
}

Interpolator3dGridTo1d::Interpolator3dGridTo1d()
      : _coordinates(NULL), _F(NULL) {
}

Interpolator3dGridTo1d::~Interpolator3dGridTo1d() {
    Clear();
}

int Interpolator3dGridTo1d::NumberOfXCoords() const {
    return _coordinates->xSize();
}

int Interpolator3dGridTo1d::NumberOfYCoords() const {
    return _coordinates->ySize();
}

int Interpolator3dGridTo1d::NumberOfZCoords() const {
    return _coordinates->zSize();
}

unsigned int Interpolator3dGridTo1d::PointDimension()  const {
    return 3;
}

unsigned int Interpolator3dGridTo1d::ValueDimension()  const {
    return 1;
}

ThreeDGrid* Interpolator3dGridTo1d::GetMesh() {
    return _coordinates;
}

void Interpolator3dGridTo1d::SetF(double*** inF) {
    DeleteFunc(_F);
    _F = inF;
}

void Interpolator3dGridTo1d::SetGrid(ThreeDGrid* grid) {
    if (_coordinates != NULL)
        _coordinates->Remove(this);
    grid->Add(this);
    _coordinates = grid;
}

void Interpolator3dGridTo1d::SetX(int nCoords, double* x) {
    if (_coordinates != NULL)
        _coordinates->SetX(nCoords, x);
}

void Interpolator3dGridTo1d::SetY(int nCoords, double* y) {
    if (_coordinates != NULL)
        _coordinates->SetY(nCoords, y);
}

void Interpolator3dGridTo1d::SetZ(int nCoords, double* z) {
    if (_coordinates != NULL)
        _coordinates->SetZ(nCoords, z);
}

double*** Interpolator3dGridTo1d::F() const {
    return _F;
}

void Interpolator3dGridTo1d::Set(ThreeDGrid* grid, double *** F) {
    SetGrid(grid);
    SetF(F);
}

void Interpolator3dGridTo1d::Clear() {
    DeleteFunc(_F);
    _coordinates->Remove(this);
}

#endif
