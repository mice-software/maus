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

#include <ostream>
#include <vector>

class VectorMap;

#ifndef _SRC_LEGACY_INTERFACE_MESHING_MESH_HH_
#define _SRC_LEGACY_INTERFACE_MESHING_MESH_HH_

/** Mesh Base class for meshing routines
 *
 *  Aim is to describe a set of points in various dimensions etc for use in
 *  interpolation routines. Concrete classes are TwoDGrid, ThreeDGrid, NDGrid
 *  and TriangularMesh (two dimensional delaunay mesh).
 *
 *  Iterator is defined for easy access to all member data (e.g. for IO) but no
 *  const_iterator type.
 */

class Mesh {
  public:
    /** Iterable sub-class */
    class Iterator;

    /** Constructor (does nothing) */
    inline Mesh();
    /** Return the first point on the mesh */
    virtual Mesh::Iterator Begin() const = 0;

    /** Return the last+1 point on the mesh */
    virtual Mesh::Iterator End()   const = 0;

    /** Return a copy of child object */
    virtual Mesh* Clone() = 0;

    /** Return the "Dual" of the mesh or NULL if not implemented
     *
     *  Dual is a polyhedron that has centre of each face as a point on the mesh
     */
    virtual Mesh* Dual() = 0;

    /** Destructor - does nothing */
    inline virtual ~Mesh();

    /** Return the position of a point in the mesh */
    virtual void Position(const Mesh::Iterator& it, double * position) const = 0;

    /** Return the dimension of the mesh */
    virtual int  PositionDimension() const = 0;

    /** Map from iterator to an integer used to index the iterator */
    virtual int  ToInteger(const Mesh::Iterator& lhs) const = 0;

    /** Find the point on the mesh nearest to given point */
    virtual Mesh::Iterator Nearest(const double* position) const = 0;

  protected:
    /** Add difference to lhs and then return lhs */
    virtual Mesh::Iterator& AddEquals
                                (Mesh::Iterator& lhs, int difference) const = 0;

    /** Subtract difference from lhs and then return lhs */
    virtual Mesh::Iterator& SubEquals
                                (Mesh::Iterator& lhs, int difference) const = 0;

    /** Add difference to lhs and then return lhs */
    virtual Mesh::Iterator& AddEquals
                    (Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const = 0;

    /** Subtract difference from lhs and then return lhs */
    virtual Mesh::Iterator& SubEquals
                     (Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const = 0;

    /** Increment position of lhs by one and then return lhs */
    virtual Mesh::Iterator& AddOne(Mesh::Iterator& lhs) const = 0;

    /** Decrement position of lhs by one and then return lhs */
    virtual Mesh::Iterator& SubOne(Mesh::Iterator& lhs) const = 0;

    /** Return true if rhs is greater than lhs */
    virtual bool IsGreater
               (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const = 0;

  public:
    // Iterator needs to know about internal workings of the Mesh to work
    // properly
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
    friend Mesh::Iterator  operator-
                        (const Mesh::Iterator& lhs, const int& rhs);
    friend Mesh::Iterator  operator+
                        (const Mesh::Iterator& lhs, const int& rhs);
    friend Mesh::Iterator& operator-=
                        (Mesh::Iterator& lhs, const int& rhs);
    friend Mesh::Iterator& operator+=
                        (Mesh::Iterator& lhs, const int& rhs);

    friend bool operator==
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend bool operator!=
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend bool operator>=
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend bool operator<=
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend bool operator<
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend bool operator>
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
};


/** \class Mesh::Iterator object
 *
 *  Used to loop over some, or all, points in the mesh, as in stl Enables e.g.
 *  generic file I/O operations on a field map without knowing the details of
 *  what is in the mesh or what shape it has.
 *
 *  Define standard operators (increment, decrement, booleans, etc) and
 *  conversion to integer.
 *
 *  Iterator generalises internal state to a stl vector of integers and a
 *  pointer to the mesh over which we iterate. Meaning of integers is mesh-
 *  dependent, but prefer convention that left-most index state[0] is most
 *  significant index and right-most index state[n] is least significant index
 *  i.e. it+1 increments state[n] by 1 first. Convention is chosen to reflect
 *  arabic numerals where right-most index is least significant.
 */
class Mesh::Iterator {
  public:
    /** Default constructor does nothing */
    inline Iterator();

    /** Shallow copy in._mesh */
    inline Iterator(const Mesh::Iterator& in);

    /** Constructor initialising internal state and mesh
     *
     *  \param state integer corresponding to internal state
     *  \param mesh mesh over which the iterator acts - Iterator borrows
     *         reference to mesh (is only valid while mesh pointer is valid)
     */
    inline Iterator(std::vector<int> state, const Mesh* mesh);

    /** Destructor */
    inline virtual ~Iterator();

    /** Shallow copy rhs into this */
    inline const Mesh::Iterator&  operator=(const Mesh::Iterator& rhs);

    /** Return the position referenced by the iterator;
     *
     *  Point should be initialised to the PositionDimension() of the mesh 
     */
    inline virtual void Position(double* point) const;

    /** Return a vector corresponding to the position referenced by the iterator
     *
     *  vector will be of length corresponding to mesh PointDimension()
     */
    inline virtual std::vector<double> Position() const;

    /** Return an integer representation of the iterator
     *
     *  mesh->Begin() should return an iterator whose integer representation is
     *  0. Iterator++ should return an iterator whose integer representation is
     *  one higher - i.e. integer representation and iterator should behave in
     *  the same way
     */
    inline int ToInteger() const;

    /** Return the internal state of the iterator */
    inline std::vector<int> State() const;

    /** Return the ith element of the iterator state vector */
    inline int& operator[](int i);

    /** Return the ith element of the iterator state vector */
    inline const int& operator[](int i) const;

    /** Return the mesh over which the iterator acts */
    inline const Mesh* GetMesh() const;

    friend class Mesh;
    friend class TwoDGrid;
    friend class ThreeDGrid;
    friend class NDGrid;
    friend class TriangularMesh;

    friend Mesh::Iterator  operator++(Mesh::Iterator& lhs, int);
    friend Mesh::Iterator  operator--(Mesh::Iterator& lhs, int);
    friend Mesh::Iterator& operator++(Mesh::Iterator& lhs);
    friend Mesh::Iterator& operator--(Mesh::Iterator& lhs);
    friend Mesh::Iterator  operator-
                        (const Mesh::Iterator& lhs, const int& difference);
    friend Mesh::Iterator  operator+
                        (const Mesh::Iterator& lhs, const int& difference);
    friend Mesh::Iterator& operator-=
                        (Mesh::Iterator& lhs, const int& difference);
    friend Mesh::Iterator& operator+=
                        (Mesh::Iterator& lhs, const int& difference);
    friend Mesh::Iterator  operator-
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend Mesh::Iterator  operator+
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend Mesh::Iterator& operator-=
                        (Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend Mesh::Iterator& operator+=
                        (Mesh::Iterator& lhs, const Mesh::Iterator& rhs);

    friend bool operator==
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend bool operator!=
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend bool operator>=
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend bool operator<=
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend bool operator<
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);
    friend bool operator>
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);

  private:
    const Mesh*      _mesh;
    std::vector<int> _state;
};

/** Prefix increment operator ++it */
inline Mesh::Iterator& operator++(Mesh::Iterator& lhs);

/** Prefix decrement operator --it retreats iterator by one  */
inline Mesh::Iterator& operator--(Mesh::Iterator& lhs);

/** Postfix increment operator it++ advances iterator by one */
inline Mesh::Iterator  operator++(Mesh::Iterator& lhs, int);

/** Postfix decrement operator it-- retreats iterator by one */
inline Mesh::Iterator  operator--(Mesh::Iterator& lhs, int);

/** Subtraction operator - subtracts offset of rhs relative to Begin() from lhs 
 */
inline Mesh::Iterator  operator-
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);

/** Addition operator - adds offset of rhs of rhs relative to Begin() to lhs*/
inline Mesh::Iterator  operator+
                        (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);

/** Subtraction operator - subtracts offset of rhs relative to Begin() from lhs 
 *  and returns lhs
 */
inline Mesh::Iterator& operator-=
                              (Mesh::Iterator& lhs, const Mesh::Iterator& rhs);

/** Addition operator - adds offset of rhs of rhs relative to Begin() to lhs
 *  and returns lhs
 */
inline Mesh::Iterator& operator+=
                              (Mesh::Iterator& lhs, const Mesh::Iterator& rhs);

/** Subtraction operator - subtracts from lhs and returns copy of lhs */
inline Mesh::Iterator  operator- (const Mesh::Iterator& lhs, const int& rhs);

/** Addition operator - adds rhs to lhs and returns copy of lhs */
inline Mesh::Iterator  operator+ (const Mesh::Iterator& lhs, const int& rhs);

/** Subtraction operator - subtracts rhs from lhs and returns lhs */
inline Mesh::Iterator& operator-=(Mesh::Iterator& lhs, const int& rhs);

/** Addition operator - subtracts rhs from lhs and returns lhs */
inline Mesh::Iterator& operator+=(Mesh::Iterator& lhs, const int& rhs);

/** Return true if lhs indexes the same position as rhs */
inline bool operator==(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);

/** Return true if lhs does not index the same position as rhs */
inline bool operator!=(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);

/** Return true if lhs indexes a position greater than or equal to rhs 
 *  i.e. successive operation of increment operator would eventually reach lhs
 */
inline bool operator>=(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);

/** Return true if lhs indexes a position less than or equal to rhs 
 *  i.e. successive operation of decrement operator would eventually reach lhs
 */
inline bool operator<=(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);

/** Return true if lhs indexes a position less than rhs 
 *  i.e. successive operation of decrement operator would eventually reach lhs
 */
inline bool operator< (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);

/** Return true if lhs indexes a position greater than rhs 
 *  i.e. successive operation of increment operator would eventually reach lhs
 */
inline bool operator> (const Mesh::Iterator& lhs, const Mesh::Iterator& rhs);

/** Output iterator to a std ostream object
 */
std::ostream& operator<<(std::ostream& out, const Mesh::Iterator& it);

#include "src/legacy/Interface/Meshing/Mesh-inl.hh"

#endif

