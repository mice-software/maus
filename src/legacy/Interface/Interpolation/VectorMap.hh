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

#ifndef _SRC_LEGACY_INTERFACE_VECTORMAP_HH_
#define _SRC_LEGACY_INTERFACE_VECTORMAP_HH_

#include <vector>

#include "src/legacy/Interface/Mesh.hh"

/** VectorMap is an abstract class that defines mapping from one vector to
 *  another.
 *
 *  VectorMap is base class primarily envisaged for mesh interpolation that
 *  defines interfaces to get interpolated data at a given point, access the
 *  mesh, etc.
 *
 *  Input and output dimensions (vector lengths) are arbitrary.
 */
class VectorMap {
 public:
    /** Pure virtual function to fill the array value with data evaluated at
     *  point.
     */
    virtual void  F(const double*   point,       double* value) const = 0;

    /** Fill the array value with function data at a point on the mesh.
     *
     *  Implemented for default case where Mesh point dimension is the same as
     *  this point dimension (usual case). No default checking for array sizes.
     */
    inline virtual void  F(const Mesh::Iterator& point, double* value) const;

    /** Calculate F, appending output values to value_vec.
     *
     *  For each item in point_vec not in value_vec, calculate value_vec (urgh)
     */
    inline virtual void FAppend(
                           const std::vector< std::vector<double> >& point_vec,
                           std::vector< std::vector<double> >& value_vec
                      ) const;

    /** Return true if point.size() is the same as this->PointDimension() */
    inline virtual bool  CheckPoint(const std::vector<double>& point) const;

    /** Return true if value.size() is the same as this->ValueDimension() */
    inline virtual bool  CheckValue(const std::vector<double>& value) const;

    /** Return the dimension of the point (input) */
    virtual unsigned int PointDimension() const = 0;
    // would like to make static - but can't inherit static functions

    /** Return the dimension of the value (output) */
    virtual unsigned int ValueDimension() const = 0;
    // would like to make static - but can't inherit static functions

    /** Clone() is like a copy constructor - but copies the child class */
    virtual              VectorMap* Clone() const = 0;

    /** Destructor */
    virtual             ~VectorMap() {;}

    /** Return the mesh used by the vector map or NULL if no mesh */
    virtual Mesh*        GetMesh() const {return NULL;}
  private:
};

bool VectorMap::CheckPoint(const std::vector<double>& point) const {
  return (point.size() == this->PointDimension());
}

bool VectorMap::CheckValue(const std::vector<double>& value) const {
  return (value.size() == this->ValueDimension());
}

void VectorMap::F(const Mesh::Iterator& point, double* value) const {
  double* PointA = new double[this->PointDimension()];
  point.Position(PointA);
  F(PointA, value);
  delete PointA;
}

void VectorMap::FAppend(const std::vector< std::vector<double> >& point_vec,
                        std::vector< std::vector<double> >& value_vec) const {
  for (size_t i = value_vec.size(); i < point_vec.size(); i++) {
    value_vec.push_back(std::vector<double>(ValueDimension()));
    F(&point_vec[i][0], &value_vec[i][0]);
  }
}

#endif  // _SRC_LEGACY_INTERFACE_VECTORMAP_HH_

