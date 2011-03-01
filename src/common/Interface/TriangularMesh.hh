#ifndef TriangularMesh_hh
#define TriangularMesh_hh

#include "Mesh.hh"

#include <map>
#include <list>
#include <string>
#include <math.h>

//! Creates a Triangulation of a set of points in dim dimensions

//! TriangularMesh is a set of functions to work with triangular meshing of a set of arbitrary points. 
//! For 2D stuff, we can use exact delaunay triangulation to make the mesh. For higher dimensions (or 
//! 2D) we import either from Voronoi.hh or read output from QHull code.
//! 
//! Triangular mesh is made up of points which make a simplex (triangle) which make a hull (arbitrary 
//! polygon). So Point, Simplex, Hull are also subclasses of TriangularMesh.
//! 
//! The triangulation is represented in a hierarchy structure, where we have big triangles containing
//! little triangles. Note that the relationship is not a simple tree structure - we can have branches
//! joining and unjoining. That's just in the nature of the Delaunay algorithm.
//!

class TriangularMesh : public Mesh
{
public:
	class Point; //n-dimensional point
	class Simplex; //n-dimensional triangle
	class Hull; //n-dimensional polygon
	/// Read in data from a qhull output file with the specified name
  //! QHullFileType can be one of "points", "mesh", "tessellation"
	TriangularMesh(std::string qHullFileName, std::string qHullFileType);
	/// Build a mesh from a set of points.
  //! Points is 2d array points[numberOfPoints][nDim] where nDim is the dimension of the space in which
  //! the meshing takes place. TriangularMesh takes ownership of points's memory. gridMin and gridMax
  //! are arrays of length nDim that contain the upper and lower corner of the grid.
	TriangularMesh(int numberOfPoints, int numberOfDimensions, double ** points, double* gridMin, double* gridMax);
	/// Collect the points and write input for qhull to qHullFileName
  //! qhull input is written to qHullFileName. thePoints is a 2d array with size 
  //! thePoints[numberOfPoints][numberOfDimensions] where nDim is the dimension of each point.
	TriangularMesh(std::string qHullFileName, int numberOfPoints, int numberOfDimensions, double ** thePoints);
  /// Destructor
	~TriangularMesh();
	/// Begin of the mesh for stdlib-style iteration
	Mesh::Iterator Begin() const {return Mesh::Iterator(std::vector<int>(1,0), this);}
	/// End of the mesh for stdlib-style iteration
	Mesh::Iterator End()   const {return Mesh::Iterator(std::vector<int>(1,points.size()), this);}

  //QHull Interface
	/// Read just a list of points written in QHull style
	void            ReadPointsFromQHull(std::string fileName); //read in a list of points
	/// Read a mesh (points and neighbours) written in QHull style
  //! Generate the list using  e.g. qdelaunay i < inp > out.
	void            ReadMeshFromQHull  (std::string fileName); //read in a triangulation
	/// Write a list of just points in QHull style
	void            WriteQHullInput    (std::string fileName); //write a list of points
	/// Read in a tesselation of hulls written in QHull style
  //! Generate the list using e.g. qvoronoi o < inp > out
	void            ReadTessFromQHull  (std::string fileName); //read in a tesselation of hulls e.g. from qvoronoi o < inp > out

  /// Get the dimension of points on the mesh
	inline int      PositionDimension() const {return nDims;} //dimension of the mesh
  /// Get the position of mesh point corresponding to iterator it
	void            Position(const Mesh::Iterator& it, double * position) const;
  /// 2D only - convert test to barycentric coordinates in triangle (apex,end0,end1) and put resulting coordinates in coords
	static void     BarycentricCoordinates(const Point& test, const Point& apex, const Point& end0, const Point& end1, double coords[2]);

  /// Get the list of all points
	std::vector< Point* >    GetPoints()    const {return points;}
  /// Get the list of all simplices - note this only returns "leaf" simplexes, i.e. those with no children in the mesh
	std::list  < Simplex* >  GetSimplices() const;
  /// Get a vector of all convex hulls
	std::vector< Hull* >     GetHulls()     const {return hulls;}

	/// Add individual Simplex
	/// nb this should be called by simplex and is used for memory management
	/// to add an arbitrary simplex to the triangulation use simplices.insert()
	void                     AddSimplex   (Simplex* tri) {allSimplices.push_back(tri);}
	/// Remove individual Simplex
	/// nb this should be called by simplex and is used for memory management
	/// to add an arbitrary simplex to the triangulation use simplices.insert()
	void                     RemoveSimplex(Simplex* tri);
  /// Dumb add a point; this will add a point but does not guarantee anything about nearest neighbours
  /// so you can end up with very long thin triangles
	void                     AddPoint_NoChecking(double * point); //Make triangles but don't check them - 2D only (I think)
  /// Clever add a point; this will add a point and then check that connecting points are indeed
  /// "nearest neighbours" so that the triangulation is well conditioned for e.g. numerical
  /// interpolation. For now this only works in 2D
	void                     AddPoint_Delaunay  (double * point);   //Delaunay algorithm - 2D only 
  /// Add a point but don't change the simplex set up... so point won't be included in the
  /// mesh properly.
	void                     AddPoint_NoTriangulation(double * point); //no triangulation

  /// Find the points connected to point by looking at connecting simplices
  std::vector<Point*>      ConnectedPoints(Point* point);
  /// Deep copy of the mesh (obeying inheritance tree) 
  Mesh*          Clone() {return new TriangularMesh(*this);}
  /// Not implemented - returns NULL
  Mesh*          Dual()  {return NULL;}
  /// Convert iterator it to an integer, such that Begin() gives 0 and End() gives the length of the Mesh
  int            ToInteger(const Mesh::Iterator& it) const {return it._state[0];}
  /// Not implemented - returns Mesh::Begin()
  Mesh::Iterator Nearest  (const double* nearest)    const {return Begin();}

protected:
	Mesh::Iterator& AddEquals(Mesh::Iterator& lhs, int difference) const {lhs._state[0] += difference; return lhs;}
	Mesh::Iterator& SubEquals(Mesh::Iterator& lhs, int difference) const {return AddEquals(lhs, -difference);}
	Mesh::Iterator& AddEquals(Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const {return AddEquals(lhs, rhs._state[0]);}
	Mesh::Iterator& SubEquals(Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const {return SubEquals(lhs, rhs._state[0]);}
	Mesh::Iterator& AddOne   (Mesh::Iterator& lhs) const {return AddEquals(lhs, 1);}
	Mesh::Iterator& SubOne   (Mesh::Iterator& lhs) const {return SubEquals(lhs, 1);}
	//Check relative position
	bool IsGreater(const Mesh::Iterator& lhs, const Mesh::Iterator& rhs) const {return lhs._state[0]>rhs._state[0];}

private:
	//Add corners to the triangulation - I think there is a better way
	void  AddCorners(double* min, double* max);

	int                   nDims;
	std::vector<Point*>   points; //triangulation points
	std::vector<Point*>   hullPoints; //points of convex hull nodes
	std::list<Simplex*>   simplices; //list of eldest Simplices in this mesh
	std::vector<Simplex*> allSimplices; //list of all Simplices in this mesh
	std::vector<Hull*>    hulls;
};

class TriangularMesh::Point
{
public:
	Point(int nDimensions);
	~Point();
	double*    coords;
	void       InsertSimplex(Simplex& tri);
	void       RemoveSimplex(Simplex& tri);
	Point      operator  -(const TriangularMesh::Point&) const;
	double     dot        (const TriangularMesh::Point&) const;
	double     magnitude  ()                             const {return this->dot(*this);}

	std::vector<Simplex*> simplices; //the Simplices that end at this point

private:
	int                    nDims;
};

class TriangularMesh::Simplex
{
public:
	Simplex(TriangularMesh* parent, Point* point[]); //build edge; add to end0 and end1
	~Simplex();
	//recursively search for leaf
	Simplex&              FindLeaf(Point & test);
	std::list<Simplex*>   GetLeaves();
	bool                  IsLeaf()               const {return f_children.size()==0;}

	void    AddPoint(Point& test);
	void    AddPointNoChecking(Point& newPoint);

	Point&  MyPoint (int i)       const {return *f_point[i];}
	Point&  MyPointCyclic(int i)  const;
	bool    IsVertex(Point& test) const;// {return &test==f_point[0] || &test==f_point[1] || &test==f_point[2];}
	bool    IsInside(Point& test) const;

	void    BarycentricCoordinates(Point& test, Point& apex, double barys[2]) const;

	bool       IsInsideCircumCircle(Point& test) const;
	double     CircumCircleR() const {return sqrt(circleRSq);}
	double*    CircumCentre()  const {return circleX;}

	Simplex&   GetAdjacentSimplex (Point& opposite); //get the simplex adjacent to "simplex" and opposite "opposite"
	Point&     GetAdjacentPoint   (Point& opposite); //get the opposite apex of the simplex adjacent to "simplex" and opposite "opposite"
	void       SwapEdge           (Point& newPoint, Simplex& Simplex1, Simplex& Simplex2); //swap the shared edge between this and adjacent simplex
	void       CheckEdge          (Point& opposite); //check the edge of simplex opposite newPoint

	bool       OnBoundary();

private:

	Simplex();
	void                  CalcCircleCoords();
	Point**               f_point;
	std::list<Simplex*>   f_children;
	TriangularMesh*       f_parent;

	double*              circleX;
	double               circleRSq;
	const int            nDims;
};

class TriangularMesh::Hull
{
public:
	Hull(TriangularMesh* parent, std::vector<Point*> points, Point* centrum) : f_parent(parent), f_points(points) {;}
	~Hull() {;}
	double              GetContent(); //jargon for area/volume/hypervolume
	bool                IsInside(); //return true if a point is inside the hypervolume
	std::vector<Point*> GetPoints() {return f_points;} 

private:

	TriangularMesh*     f_parent;
	std::vector<Point*> f_points;

};

#endif



