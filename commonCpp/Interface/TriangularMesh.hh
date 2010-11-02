#ifndef TriangularMesh_hh
#define TriangularMesh_hh

#include "Mesh.hh"
#include <list>
#include <string>
#include <math.h>

//Creates a Triangulation of a set of points in dim dimensions
class TriangularMesh : public Mesh
{
public:
	class Point; //n-dimensional point
	class Simplex; //n-dimensional triangle
	class Hull; //n-dimensional polygon
	//read in data from a qhull output file
	TriangularMesh(std::string qHullFileName, std::string qHullFileType);
	//points is 2d array points[numberOfPoints][nDim] TriangularMesh takes ownership of points memory
	TriangularMesh(int numberOfPoints, int numberOfDimensions, double ** points, double* gridMin, double* gridMax);
	//collect the points and write input for qhull to qHullFileName
	TriangularMesh(std::string qHullFileName, int numberOfPoints, int numberOfDimensions, double ** thePoints);
	~TriangularMesh();
	//Begin and End of the mesh for stdlib-style iteration
	Mesh::Iterator Begin() const {return Mesh::Iterator(std::vector<int>(1,0), this);}
	Mesh::Iterator End()   const {return Mesh::Iterator(std::vector<int>(1,points.size()), this);}

	//QHull Interface
	//Read
	void            ReadPointsFromQHull(std::string fileName); //read in a list of points
	void            ReadMeshFromQHull  (std::string fileName); //read in a triangulation e.g. from qdelaunay i < inp > out
	void            WriteQHullInput    (std::string fileName); //write a list of points
	void            ReadTessFromQHull  (std::string fileName); //read in a tesselation of hulls e.g. from qvoronoi o < inp > out

	//Position accessors
	inline int      PositionDimension() const {return nDims;} //dimension of the mesh
	void            Position(const Mesh::Iterator& it, double * position) const;
	static void     BarycentricCoordinates(Point& test, Point& apex, Point& end0, Point& end1, double coords[2]);

	//Data reads
	std::vector< Point* >    GetPoints()    const {return points;}
	std::list  < Simplex* >  GetSimplices() const;
	std::vector< Hull* >     GetHulls()     const {return hulls;}

	//Data writes
	//Set all points from std::vector<double*>
	void                     SetPoints( std::vector< double* > points );
	//Add or remove individual Simplex
	//nb this should be called by simplex and is used for memory management
	//to add an arbitrary simplex to the triangulation use simplices.insert()
	void                     AddSimplex   (Simplex* tri) {allSimplices.push_back(tri);}
	void                     RemoveSimplex(Simplex* tri);
	//Add a point - I now control memory allocated to point
	void                     AddPoint_NoChecking(double * point); //Make triangles but don't check them - 2D only (I think)
	void                     AddPoint_Delaunay(double * point);   //Delaunay algorithm - 2D only 
	void                     AddPoint_NoTriangulation(double * point); //no triangulation

  //Inherited from Mesh; performs deep copy
  Mesh*          Clone() {return new TriangularMesh(*this);}
  Mesh*          Dual()  {return NULL;} //non-trivial and not implemented
  int            ToInteger(const Mesh::Iterator& it) const {return it._state[0];}
  Mesh::Iterator Nearest  (const double* nearest)    const {return Begin();} //non-trivial and not implemented yet

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
	bool                   IsLeaf()               const {return f_children.size()==0;}

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



