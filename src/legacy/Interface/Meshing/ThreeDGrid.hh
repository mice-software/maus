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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

#ifndef _SRC_LEGACY_INTERFACE_MESHING_THREEDGRID_HH_
#define _SRC_LEGACY_INTERFACE_MESHING_THREEDGRID_HH_

#include <math.h>

#include <algorithm>
#include <vector>

#include "src/legacy/Interface/Meshing/Mesh.hh"

/** ThreeDGrid holds grid information for a rectangular grid used in e.g.
 *  interpolation
 *
 *  ThreeDGrid holds the necessary information for regular and irregular grid
 *  data for interpolation routines.
 *
 *  Constructor is provided to generate a regular grid given grid spacing etc
 *  and an irregular grid given lists of x, y, z data for the grid points.
 *  Nearest either calculates nearest grid point a priori for regular grids or
 *  uses std::lower_bound to calculate nearest grid point (list bisection or
 *  somesuch). Controlled by _constantSpacing flag. The regular spacing lookup
 *  is quicker, so prefer constant spacing.
 *
 *  ThreeDGrid holds a list of pointers to VectorMaps that use the grid for
 *  interpolation and functions to Add and Remove VectorMaps. If the Remove
 *  function removes the last VectorMap, then the ThreeDGrid is deleted. It's a
 *  sort of smart pointer.
 */
class ThreeDGrid : public Mesh {
  public:
    class Iterator;
    /** Deep copy the grid */
    Mesh * Clone() {return new ThreeDGrid(*this);}

    /** Not implemented (returns NULL) */
    Mesh * Dual () {return NULL;}

    /** Default constructor */
    ThreeDGrid();

    /** Make a regular rectangular ThreeDGrid
     *
     *  Set up a ThreeDGrid with regular grid spacing (same distance between
     *  every point)
     *  \param dX step size in x; should be positive
     *  \param dY step size in y; should be positive
     *  \param dZ step size in z; should be positive
     *  \param minX x position of grid lower edge
     *  \param minY y position of grid lower edge
     *  \param minZ z position of grid lower edge
     *  \param numberOfXCoords number of x coordinates on grid; should be > 1
     *  \param numberOfYCoords number of y coordinates on grid; should be > 1
     *  \param numberOfZCoords number of z coordinates on grid; should be > 1
     */
    ThreeDGrid(double dX, double dY, double dZ,
               double minX, double minY, double minZ,
               int numberOfXCoords, int numberOfYCoords, int numberOfZCoords);

    /** Make a irregular rectangular ThreeDGrid
     *
     *  Set up a ThreeDGrid with irregular grid spacing
     *  \param xSize number of points in x
     *  \param x x points array
     *  \param ySize number of points in y
     *  \param y y points array
     *  \param zSize number of points in Z
     *  \param z z point array
     *  ThreeDGrid deep copies the data pointed to by x, y, z (i.e. it does not
     *  take ownership of these arrays).
     */
    ThreeDGrid(int xSize, const double *x,
               int ySize, const double *y,
               int zSize, const double *z);

    /** Make a irregular rectangular ThreeDGrid
     *
     *  Set up a ThreeDGrid with irregular grid spacing
     *  \param x x points array
     *  \param y y points array
     *  \param z z point array
     */
    ThreeDGrid(std::vector<double> x,
               std::vector<double> y,
               std::vector<double> z);

    /** Destructor */
    ~ThreeDGrid();

    /** Get ith coordinate in x
     *
     *  Round bracket indexing starts at 1 goes to nCoords
     */
    inline double& x(const int& i) {return _x[i-1];}

    /** Get ith coordinate in y
     *
     *  Round bracket indexing starts at 1 goes to nCoords
     */
    inline double& y(const int& j) {return _y[j-1];}

    /** Get ith coordinate in z
     *
     *  Round bracket indexing starts at 1 goes to nCoords
     */
    inline double& z(const int& k) {return _z[k-1];}

    /** Get ith coordinate in x
     *
     *  Round bracket indexing starts at 1 goes to nCoords
     */
    inline const double& x(const int& i) const {return _x[i-1];}

    /** Get ith coordinate in y
     *
     *  Round bracket indexing starts at 1 goes to nCoords
     */
    inline const double& y(const int& j) const {return _y[j-1];}

    /** Get ith coordinate in z
     *
     *  Round bracket indexing starts at 1 goes to nCoords
     */
    inline const double& z(const int& j) const {return _z[j-1];}

    /** Get size of x data */
    inline int     xSize() const {return static_cast<int>(_x.size());}

    /** Get size of y data */
    inline int     ySize() const {return static_cast<int>(_y.size());}

    /** Get size of z data */
    inline int     zSize() const {return static_cast<int>(_z.size());}

    /** Get vector containing x grid data */
    std::vector<double> xVector() {return std::vector<double>(_x);}

    /** Get vector containing y grid data */
    std::vector<double> yVector() {return std::vector<double>(_y);}

    /** Get vector containing z grid data */
    std::vector<double> zVector() {return std::vector<double>(_z);}

    /** Allocate a new array containing x grid data */
    inline double* newXArray();

    /** Allocate a new array containing y grid data */
    inline double* newYArray();

    /** Allocate a new array containing z grid data */
    inline double* newZArray();

    /** Find the index of the nearest point less than x
     *
     *  Note that in the case of a regular grid there is no bound checking; in
     *  the case of an irregular grid we always return an element in the grid
     */
    inline void xLowerBound(const double& x, int& xIndex) const;

    /** Find the index of the nearest point less than y
     *
     *  Note that in the case of a regular grid there is no bound checking; in
     *  the case of an irregular grid we always return an element in the grid
     */
    inline void yLowerBound(const double& y, int& yIndex) const;

    /** Find the index of the nearest point less than z
     *
     *  Note that in the case of a regular grid there is no bound checking; in
     *  the case of an irregular grid we always return an element in the grid
     */
    inline void zLowerBound(const double& z, int& zIndex) const;

    /** Find the indices of the nearest point on the grid less than x, y, z
     *
     *  Note that in the case of a regular grid there is no bound checking; in
     *  the case of an irregular grid we always return an element in the grid
     */
    inline void LowerBound(const double& x, int& xIndex,
                           const double& y, int& yIndex,
                           const double& z, int& zIndex) const;

    /** Find the iterator of the nearest point on the grid less than x, y, z
     *
     *  Note that in the case of a regular grid there is no bound checking; in
     *  the case of an irregular grid we always return an element in the grid
     */
    inline void LowerBound(const double& x, const double& y, const double& z,
                           Mesh::Iterator& it) const;

    /** Return the smallest value of x in the grid */
    inline double MinX() const;

    /** Return the largest value of x in the grid */
    inline double MaxX() const;

    /** Return the smallest value of y in the grid */
    inline double MinY() const;

    /** Return the largest value of y in the grid */
    inline double MaxY() const;

    /** Return the smallest value of z in the grid */
    inline double MinZ() const;

    /** Return the largest value of z in the grid */
    inline double MaxZ() const;

    /** Reset x grid points - note may need to set SetConstantSpacing() */
    inline void SetX(int nXCoords, double * x);

    /** Reset y grid points - note may need to set SetConstantSpacing() */
    inline void SetY(int nYCoords, double * y);

    /** Reset z grid points - note may need to set SetConstantSpacing() */
    inline void SetZ(int nZCoords, double * z);

    /** Add *map to the _maps list if it has not already been added */
    void Add(VectorMap* map);

    /** Remove *map from the _maps list if it has not already been removed. If
     *  there are no more maps in the list, delete this*/
    void Remove(VectorMap* map);

    /** Return an iterator pointing at the first element in the ThreeDGrid */
    Mesh::Iterator Begin() const;

    /** Return an iterator pointing at the last+1 element in the ThreeDGrid */
    Mesh::Iterator End()   const;

    /** Fill position with the position of it
     *
     *  \param it iterator pointing at a grid position
     *  \param position array of length >= 3 that will be filled with the
     *         position of it
     */
    virtual void Position(const Mesh::Iterator& it, double * position) const;

    /** Returns 3, the dimension of the space of the grid */
    inline int PositionDimension() const;

    /** Converts from iterator to integer value
     *
     *  0 corresponds to the mesh beginning. integers index (right most index)
     *  indexes z, middle index is y, most significant (left most index) indexes
     *  x.
     */
    inline int ToInteger(const Mesh::Iterator& lhs) const;

    /** Set to true to use constant spacing
     *
     *  If constant spacing is true, assume regular grid with spacing given by
     *  separation of first two elements in each dimension
     */
    inline void SetConstantSpacing(bool spacing);

    /** Autodetect constant spacing with float tolerance of 1e-9
     */
    void SetConstantSpacing();

    /** Return true if using constant spacing */
    inline bool GetConstantSpacing() const;

    /** Return the point in the mesh nearest to position */
    Mesh::Iterator Nearest(const double* position) const;

  protected:
    // Change position
    virtual Mesh::Iterator& AddEquals(Mesh::Iterator& lhs, int difference) const;
    virtual Mesh::Iterator& SubEquals(Mesh::Iterator& lhs, int difference) const;
    virtual Mesh::Iterator& AddEquals
                        (Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const;
    virtual Mesh::Iterator& SubEquals
                        (Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const;
    virtual Mesh::Iterator& AddOne(Mesh::Iterator& lhs) const;
    virtual Mesh::Iterator& SubOne(Mesh::Iterator& lhs) const;
    // Check relative position
    virtual bool IsGreater
                  (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const;

  private:
    std::vector<double>     _x;
    std::vector<double>     _y;
    std::vector<double>     _z;
    int                     _xSize;
    int                     _ySize;
    int                     _zSize;
    std::vector<VectorMap*> _maps;
    bool                    _constantSpacing;

    friend Mesh::Iterator  operator++(Mesh::Iterator& lhs, int);
    friend Mesh::Iterator  operator--(Mesh::Iterator& lhs, int);
    friend Mesh::Iterator& operator++(Mesh::Iterator& lhs);
    friend Mesh::Iterator& operator--(Mesh::Iterator& lhs);
    friend Mesh::Iterator  operator-
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend Mesh::Iterator  operator+
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend Mesh::Iterator& operator-=
                        (Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend Mesh::Iterator& operator+=
                        (Mesh::Iterator& lhs, const Mesh::Iterator& rhs);

    friend bool operator==(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend bool operator!=(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend bool operator>=(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend bool operator<=(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend bool operator< (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend bool operator> (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
};

double* ThreeDGrid::newXArray() {
    double *x = new double[_x.size()];
    for (unsigned int i = 0; i < _x.size(); i++)
        x[i] = _x[i];
    return x;
}

double* ThreeDGrid::newYArray() {
    double *y = new double[_y.size()];
    for (unsigned int i = 0; i < _y.size(); i++)
        y[i] = _y[i];
    return y;
}

double* ThreeDGrid::newZArray() {
    double *z = new double[_z.size()];
    for (unsigned int i = 0; i < _z.size(); i++)
        z[i] = _z[i];
    return z;
}

void ThreeDGrid::xLowerBound(const double& x, int& xIndex) const {
    if (_constantSpacing)
        xIndex = static_cast<int>(floor((x - _x[0])/(_x[1]-_x[0]) ));
    else
        xIndex = std::lower_bound(_x.begin(), _x.end(), x)-_x.begin()-1;
}

void ThreeDGrid::yLowerBound(const double& y, int& yIndex) const {
    if (_constantSpacing)
        yIndex = static_cast<int>(floor((y - _y[0])/(_y[1]-_y[0])));
    else
        yIndex = std::lower_bound(_y.begin(), _y.end(), y)-_y.begin()-1;
}

void ThreeDGrid::zLowerBound(const double& z, int& zIndex) const {
    if (_constantSpacing)
        zIndex = static_cast<int>(floor((z - _z[0])/(_z[1]-_z[0])));
    else
        zIndex = std::lower_bound(_z.begin(), _z.end(), z)-_z.begin()-1;
}

void ThreeDGrid::LowerBound(const double& x, int& xIndex,
                            const double& y, int& yIndex,
                            const double& z, int& zIndex) const {
    xLowerBound(x, xIndex);
    yLowerBound(y, yIndex);
    zLowerBound(z, zIndex);
}

void ThreeDGrid::LowerBound(const double& x,
                                   const double& y,
                                   const double& z,
                                   Mesh::Iterator& it) const {
    xLowerBound(x, it[0]);
    yLowerBound(y, it[1]);
    zLowerBound(z, it[2]);
    it[0]++;
    it[1]++;
    it[2]++;
}

double ThreeDGrid::MinX() const {
    return _x[0];
}

double ThreeDGrid::MaxX() const {
    return _x[_xSize-1];
}

double ThreeDGrid::MinY() const {
    return _y[0];
}

double ThreeDGrid::MaxY() const {
    return _y[_ySize-1];
}

double ThreeDGrid::MinZ() const {
    return _z[0];
}

double ThreeDGrid::MaxZ() const {
    return _z[_zSize-1];
}

void ThreeDGrid::SetX(int nXCoords, double * x) {
    _x = std::vector<double>(x, x+nXCoords);
}

void ThreeDGrid::SetY(int nYCoords, double * y) {
    _y = std::vector<double>(y, y+nYCoords);
}

void ThreeDGrid::SetZ(int nZCoords, double * z) {
    _z = std::vector<double>(z, z+nZCoords);
}

int ThreeDGrid::PositionDimension() const {
    return 3;
}

int ThreeDGrid::ToInteger(const Mesh::Iterator& lhs) const {
    return (lhs.State()[0]-1)*_zSize*_ySize+
           (lhs.State()[1]-1)*_zSize+
           (lhs.State()[2]-1);
}

void ThreeDGrid::SetConstantSpacing(bool spacing) {
    _constantSpacing = spacing;
}

bool ThreeDGrid::GetConstantSpacing() const {
    return _constantSpacing;
}

#endif

