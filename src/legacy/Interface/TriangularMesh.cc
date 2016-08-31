// MAUS WARNING: THIS IS LEGACY CODE.
#include "Interface/TriangularMesh.hh"
#include "Utils/Exception.hh"

#include <iostream>
#include <fstream>
#include <sstream>


//////////// TriangularMesh ///////////////
TriangularMesh::TriangularMesh(std::string qHullFileName, std::string qHullFileType)
{
	if(qHullFileType == "points")      ReadPointsFromQHull(qHullFileName);
	if(qHullFileType == "mesh")        ReadPointsFromQHull(qHullFileName);
	if(qHullFileType == "tesselation") ReadPointsFromQHull(qHullFileName);
}

TriangularMesh::TriangularMesh(int numberOfPoints, int numberOfDimensions, double ** thePoints, double* min, double* max)
               : nDims(numberOfDimensions), points(), simplices()
{///nd
	if(numberOfDimensions==2)
	{
		AddCorners(min, max);
		for(int i=0; i<numberOfPoints; i++) AddPoint_Delaunay(thePoints[i]);
	}
	else
		throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "TriangularMesh only implemented for 2d", "TriangularMesh::TriangularMesh(...)"));
}

TriangularMesh::~TriangularMesh() 
{
	while(allSimplices.size()>0) 
		delete allSimplices[0];
	for(unsigned int i=0; i<points.size(); i++) 
	{
		delete [] points[i]->coords;
		delete    points[i];
	}
}

void TriangularMesh::ReadPointsFromQHull(std::string fileName)
{
	std::ifstream     fin(fileName.c_str());
	int               nPoints;
	std::string       line;
	std::stringstream in("");
	getline(fin, line);
	in  << line;
	in  >> nDims;
	fin >> nPoints;
	points = std::vector<Point*>(nPoints);
	for(int i=0; i<nPoints; i++)
	{
		points[i] = new Point(nDims);
		points[i]->coords = new double[nDims];
		for(int j=0; j<nDims; j++)
			fin >> points[i]->coords[j];
	}
	if(!fin) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error reading file", "QHullIO::ReadQHullInputFile"));
	fin.close();
}

void TriangularMesh::ReadMeshFromQHull(std::string fileName)
{
	std::ifstream     fin(fileName.c_str());
	int               nSimplices;
	std::string       line;
	std::stringstream in("");
	getline(fin, line);
	in  << line;
	in  >> nSimplices;
	int nVertices = nDims+1;
	for(int i=0; i<nSimplices; i++)
	{
		Point ** simplexPoints = new Point*[nVertices];
		for(int j=0; j<nVertices; j++)
		{
			int pointIndex;
			fin >> pointIndex;
			simplexPoints[j] = points[pointIndex];
		}
		simplices.push_back(new Simplex(this, simplexPoints));
	}
	if(!fin) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error reading file", "QHullIO::ReadQHullInputFile"));
	fin.close();
}

void TriangularMesh::WriteQHullInput(std::string fileName)
{
	std::ofstream     fout(fileName.c_str());
	int nPoints = points.size();
	fout << nDims << " #QHull Input by G4MICE\n" << nPoints << "\n";
	for(int i=0; i<nPoints; i++)
	{
		for(int j=0; j<nDims; j++)
			fout << points[i]->coords[j] << " ";
		fout << "\n";
	}
	if(!fout) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error writing file", "TriangularMesh::WriteQHullInput"));
	fout.close();
}

void TriangularMesh::ReadTessFromQHull(std::string fileName)
{
	std::ifstream     fin(fileName.c_str());
	int               nHulls, nSites, dummy;
	std::string       line;
	std::stringstream in("");
	getline(fin, line);
	in  << line;
	in  >> nDims;
	
	fin >> nSites >> nHulls >> dummy;
	hullPoints = std::vector<Point*>(nSites);
	hulls      = std::vector<Hull*>(nHulls);
	for(int i=0; i<nSites; i++)
	{
		hullPoints[i]         = new Point (nDims);
		hullPoints[i]->coords = new double[nDims];
		for(int j=0; j<nDims; j++)
			fin >> hullPoints[i]->coords[j];
	}
	for(int i=0; i<nHulls; i++)
	{
		int nPoints;
		fin >> nPoints;
		std::vector<Point*> myHullPoints(nPoints);
		std::cout << "Reading hull "  << i << " ** ";
		for(int j=0; j<nPoints; j++)
		{
			int pointIndex;
			fin >> pointIndex;
			myHullPoints[j] = hullPoints[pointIndex];
			std:: cout << pointIndex << " ";
		}
		std::cout << std::endl;
		hulls[i] = new Hull(this, myHullPoints, NULL);
	}
	
	if(!fin) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error reading file", "QHullIO::ReadQHullInputFile"));
	fin.close();
}

void TriangularMesh::Position(const Mesh::Iterator& it, double * position) const
{
	position = points[it._state[0]]->coords;
}

void TriangularMesh::AddCorners(double* min, double* max)
{
	for(int i=0; i<4; i++)
	{
		points.push_back(new Point(nDims));
		points[i]->coords = new double[2];
	}
	points[0]->coords[0] = min[0];
	points[0]->coords[1] = min[1];
	points[1]->coords[0] = min[0];
	points[1]->coords[1] = max[1];
	points[2]->coords[0] = max[0];
	points[2]->coords[1] = max[1];
	points[3]->coords[0] = max[0];
	points[3]->coords[1] = min[1];

	//what about the boundaries that only have one adjacent?

	Point* pointsSimp1[3] = {points[0], points[1], points[2]};
	simplices.push_back( new Simplex(this, pointsSimp1 ) );
	Point* pointsSimp2[3] = {points[0], points[3], points[2]};
	simplices.push_back( new Simplex(this, pointsSimp2 ) );
}

std::vector<TriangularMesh::Point*> TriangularMesh::ConnectedPoints(TriangularMesh::Point* point) {
  std::vector<Point*> nn;
  for(size_t i=0; i<point->simplices.size(); i++) {//loop over points that are nearest neighbours of this point
    TriangularMesh::Simplex* s = point->simplices[i];
    for(int j=0; j<3; j++) {
      if(&s->MyPoint(j) != point) nn.push_back(point);
    }
  }
  return nn;
}

void TriangularMesh::AddPoint_Delaunay(double * point)
{
	points.push_back(new Point(nDims));
	points.back()->coords = point;
	//Find bounding edges surrounding point
	for(std::list<Simplex*>::iterator it = simplices.begin(); it!=simplices.end(); ++it)
		if( (*it)->IsInside(*points.back()) ) (*it)->AddPoint(*points.back());
}

void TriangularMesh::AddPoint_NoChecking(double * point)
{
	points.push_back(new Point(nDims));
	points.back()->coords = point;
	//Find bounding edges surrounding point
	for(std::list<Simplex*>::iterator it = simplices.begin(); it!=simplices.end(); ++it)
		if( (*it)->IsInside(*points.back()) ) (*it)->AddPointNoChecking(*points.back());
}

void TriangularMesh::AddPoint_NoTriangulation(double * point)
{
	points.push_back(new Point(nDims));
	points.back()->coords = point;
}

void TriangularMesh::BarycentricCoordinates(const Point& test, const Point& apex, const Point& end0, const Point& end1, double baryCoords[2])
{
	Point vec0 = end0 - apex;
	Point vec1 = end1 - apex;
	Point vecT = test - apex;

	double dot00 = vec0.dot(vec0);
	double dot01 = vec0.dot(vec1);
	double dot0T = vec0.dot(vecT);
	double dot11 = vec1.dot(vec1);
	double dot1T = vec1.dot(vecT);

	double inverse = 1 / (dot00 * dot11 - dot01 * dot01);
	baryCoords[0] = (dot11 * dot0T - dot01 * dot1T) * inverse;
	baryCoords[1] = (dot00 * dot1T - dot01 * dot0T) * inverse;
	delete [] vec0.coords;
	delete [] vec1.coords;
	delete [] vecT.coords;
}

std::list<TriangularMesh::Simplex*> TriangularMesh::GetSimplices() const
{
	std::list<Simplex*> leaves;
	for(std::list<Simplex*>::const_iterator it = simplices.begin(); it!=simplices.end(); ++it)
	{
		std::list<Simplex*> childTris = (*it)->GetLeaves();
		leaves.insert(leaves.begin(), childTris.begin(), childTris.end());
	}
	return leaves;
}

void  TriangularMesh::RemoveSimplex(Simplex* tri)
{
	for(std::vector<Simplex*>::iterator it = allSimplices.begin(); it<allSimplices.end(); it++) 
		if(*it == tri) allSimplices.erase(it);
}



/////////// Point  ////////////
void TriangularMesh::Point::InsertSimplex(Simplex& Simplex)
{
	this->simplices.push_back(&Simplex);
}

void TriangularMesh::Point::RemoveSimplex(Simplex& aSimplex)
{
 	for(std::vector<Simplex*>::iterator it=simplices.begin(); it<simplices.end(); it++)
		simplices.erase(it);
}

TriangularMesh::Point TriangularMesh::Point::operator -(const TriangularMesh::Point& p) const
{
	Point answer(nDims);
	answer.coords    = new double[nDims];
	for(int i=0; i<nDims; i++)
		answer.coords[i] = coords[i] - p.coords[i];
	return answer;
}

double TriangularMesh::Point::dot(const TriangularMesh::Point& p) const
{
	double dot=0.;
	for(int i=0; i<nDims; i++)
		dot += p.coords[i]*coords[i];
	return dot;
}

TriangularMesh::Point::Point(int nDimensions) : coords(NULL), nDims(nDimensions)
{
}

TriangularMesh::Point::~Point()
{
}

//////////// Simplex ////////////////

TriangularMesh::Simplex::Simplex(TriangularMesh* parent, Point* point[]) : nDims(parent->PositionDimension())
{
	f_parent = parent;
	f_point  = new Point*[nDims+1];
	f_parent->AddSimplex(this);
	for(int i=0; i<nDims+1; i++) f_point[i] = point[i];
	for(int i=0; i<nDims+1; i++) f_point[i]->InsertSimplex(*this);
	CalcCircleCoords();
}

TriangularMesh::Simplex::~Simplex()
{
	f_parent->RemoveSimplex(this);
	for(int i=0; i<nDims+1; i++)
		f_point[i]->RemoveSimplex(*this);
	delete [] f_point;
	delete [] circleX;
}

TriangularMesh::Point& TriangularMesh::Simplex::MyPointCyclic(int i) const
{
	i -= (nDims+1)*( i/(nDims+1) );
	return *f_point[i];
}


TriangularMesh::Simplex& TriangularMesh::Simplex::FindLeaf(Point& test)
{
	if(IsLeaf()) return *this;
	for(std::list<Simplex*>::const_iterator it = f_children.begin(); it!=f_children.end(); ++it)
		if( (*it)->IsInside(test) ) return (*it)->FindLeaf(test);
	throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Failed to find test point", "Simplex::FindLeaf"));
}

bool TriangularMesh::Simplex::IsInside(Point& test) const
{
	double barys[2] = {0,0};
	BarycentricCoordinates(test, MyPoint(0), barys);
	if(barys[0]>0 && barys[0]<1 && barys[1]>0 && barys[1]<1 && barys[0]+barys[1]<1)
		return true;
	return false;
}

bool TriangularMesh::Simplex::IsInsideCircumCircle(Point& test) const
{
	double myRSq = 0;
	for(int i=0; i<f_parent->nDims; i++) myRSq += (test.coords[i] - circleX[i])*(test.coords[i] - circleX[i]);
	if( myRSq > circleRSq ) return false;
	return true;
}

void TriangularMesh::Simplex::CalcCircleCoords()
{
	double x1 = f_point[0]->coords[0];
	double x2 = f_point[1]->coords[0];
	double x3 = f_point[2]->coords[0];
	double y1 = f_point[0]->coords[1];
	double y2 = f_point[1]->coords[1];
	double y3 = f_point[2]->coords[1];

	double a  = 2.*(+x1*(y2-y3)
	                -x2*(y1-y3)
	                +x3*(y1-y2) );
	double bx = -(x1*x1+y1*y1)*(y2-y3)
	            +(x2*x2+y2*y2)*(y1-y3)
	            -(x3*x3+y3*y3)*(y1-y2);
	double by = +(x1*x1+y1*y1)*(x2-x3)
	            -(x2*x2+y2*y2)*(x1-x3)
	            +(x3*x3+y3*y3)*(x1-x2);

	circleX      = new double[nDims];
	circleX[0]   = -bx/a;
	circleX[1]   = -by/a;
	circleRSq = (x1-circleX[0])*(x1-circleX[0])+(y1-circleX[1])*(y1-circleX[1]);
}

void TriangularMesh::Simplex::BarycentricCoordinates(Point& test, Point& apex, double barys[]) const
{
	int apexInt=0;
	Point *end0, *end1;
	while(&apex != f_point[apexInt] && apexInt < 3) apexInt++;
	if     (apexInt == 0) {end0 = f_point[1]; end1 = f_point[2];}
	else if(apexInt == 1) {end0 = f_point[0]; end1 = f_point[2];}
	else if(apexInt == 2) {end0 = f_point[0]; end1 = f_point[1];}
	else throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Trying to find BarycentricCoordinates with point not in Simplex", "Simplex::BarycentricCoordinates"));
	TriangularMesh::BarycentricCoordinates(test, apex, *end0, *end1, barys);
}

void TriangularMesh::Simplex::AddPointNoChecking(Point& newPoint)
{
	if(IsLeaf())
	{
		std::vector<Point**>  point (nDims+1);
		std::vector<Simplex*> simple(nDims+1);
		for(int i=0; i<nDims+1; i++)
		{
			point[i]    = new Point*[nDims+1];
			point[i][0] = &newPoint;
			for(int j=0; j<nDims; j++)
				point[i][j+1] = &MyPointCyclic(i+j);
			simple[i] = new Simplex(f_parent, point[i]);
		}
		f_children.insert(f_children.begin(), simple.begin(), simple.end());
		return;
	}
	for(std::list<Simplex*>::iterator it = f_children.begin(); it!=f_children.end(); ++it)
		if( (*it)->IsInside(newPoint) ) (*it)->AddPoint(newPoint);
}


void TriangularMesh::Simplex::AddPoint(Point& newPoint)
{
	if(IsLeaf())
	{
		std::vector<Point**>  point (nDims+1);
		std::vector<Simplex*> simple(nDims+1);
		for(int i=0; i<nDims+1; i++)
		{
			point[i]    = new Point*[nDims+1];
			point[i][0] = &newPoint;
			for(int j=0; j<nDims; j++)
				point[i][j+1] = &MyPointCyclic(i+j);
			simple[i] = new Simplex(f_parent, point[i]);
		}
		f_children.insert(f_children.begin(), simple.begin(), simple.end());
		for(int i=0; i<nDims+1; i++) simple[i]->CheckEdge(newPoint);
		return;
	}
	for(std::list<Simplex*>::iterator it = f_children.begin(); it!=f_children.end(); ++it)
		if( (*it)->IsInside(newPoint) ) (*it)->AddPoint(newPoint);
}

bool TriangularMesh::Simplex::IsVertex(Point& test) const
{
	for(int i=0; i<nDims+1; i++)
		if(&test == f_point[i]) return true;
	return false;
}

void TriangularMesh::Simplex::CheckEdge(Point& newPoint)
{
		Point& pointAdj = GetAdjacentPoint(newPoint);
		if(&pointAdj != &newPoint) //return point0 means I couldn't find pointAdj e.g. boundary
			if( IsInsideCircumCircle(pointAdj) )
				SwapEdge(newPoint, *this, GetAdjacentSimplex(newPoint));
}

std::list<TriangularMesh::Simplex*> TriangularMesh::Simplex::GetLeaves()
{
	if(IsLeaf()) return std::list<Simplex*>(1, this);
	std::list<Simplex*> simplices;
	for(std::list<Simplex*>::const_iterator it = f_children.begin(); it!=f_children.end(); ++it)
	{
		std::list<Simplex*> childTris = (*it)->GetLeaves();
		simplices.insert(simplices.begin(), childTris.begin(), childTris.end());
	}
	return simplices;
}

TriangularMesh::Simplex&  TriangularMesh::Simplex::GetAdjacentSimplex(Point& opposite)
{
	std::vector<Point*>   commonPoints; //shouldn't do it like this - memory reallocation is slow
	for(int i=0; i<nDims+1; i++) if(f_point[i]!=&opposite) commonPoints.push_back(f_point[i]);
	for(std::vector<Simplex*>::iterator it=commonPoints[0]->simplices.begin(); it!=commonPoints[0]->simplices.end(); it++)
	{
		bool isCommon = true;
		for(unsigned int i=1; i<commonPoints.size(); i++)
			isCommon = (*it)->IsVertex(*commonPoints[i]) && (*it)->IsLeaf();
		if(isCommon && (*it)!=this) return *(*it);
	}
	return *this;
}

TriangularMesh::Point&     TriangularMesh::Simplex::GetAdjacentPoint   (Point& opposite)
{
	Simplex& tri = GetAdjacentSimplex(opposite);
	for(int i=0; i<nDims+1; i++)
		if( !IsVertex(tri.MyPoint(i)) ) return tri.MyPoint(i);
	return opposite;
}

void                       TriangularMesh::Simplex::SwapEdge           (Point& newPoint, Simplex& Simplex1, Simplex& Simplex2) 
{
	Point*  opp[2] = {NULL, NULL};      //points opposite shared edge
	Point** edge   = new Point*[nDims]; //points currently on shared edge 
	for(int i=0; i<nDims; i++) edge[i] = NULL;

	//loop over all points in simplex1 and see if they are vertices of simplex2; if yes then add to edge, else then it is opp_0
	for(int i=0; i<nDims+1; i++)
	{
		if( Simplex2.IsVertex(*(Simplex1.f_point[i])) )
		{
			int j = 0;
			while(edge[j] != NULL) j++; 
			edge[j] = Simplex1.f_point[i];
		}
		else opp[0] = Simplex1.f_point[i];
	}
	//loop over all points in simplex2 and see if they are vertices of simplex1; if no then it is opp_1
	for(int i=0; i<nDims+1; i++)
		if( !Simplex1.IsVertex(*(Simplex2.f_point[i])) ) 
			opp[1] = Simplex2.f_point[i];

	Point*   point1[3] = {edge[0], opp[0], opp[1]};
	Simplex* tri1 = new Simplex(f_parent, point1);
	Point*   point2[3] = {edge[1], opp[0], opp[1]};
	Simplex* tri2 = new Simplex(f_parent, point2);

	Simplex1.f_children.push_back(tri1);
	Simplex1.f_children.push_back(tri2);
	Simplex2.f_children.push_back(tri1);
	Simplex2.f_children.push_back(tri2);

	Point* oldOpp = NULL;
	if(opp[0] == &newPoint) oldOpp = opp[1];
	else                    oldOpp = opp[0];
	tri1->CheckEdge(newPoint);
	tri2->CheckEdge(newPoint);

	delete [] edge;
}

/*
	Simplex& oppSimp = GetAdjacentSimplex(opposite);
	Point*   opp[2]  = {&opposite, &GetAdjacentPoint(opposite)};      //points opposite shared edge

	Point**  point1 = new Point*[nDims+1];
	Point**  point2 = new Point*[nDims+1];
	
	Simplex* tri1 = new Simplex(f_parent, point1);
	Simplex* tri2 = new Simplex(f_parent, point2);

	this->  f_children.push_back(tri1);
	this->  f_children.push_back(tri2);
	oppSimp.f_children.push_back(tri1);
	oppSimp.f_children.push_back(tri2);

	Point* oldOpp;
	if(opp[0] == &newPoint) oldOpp = opp[1];
	else                    oldOpp = opp[0];
	tri1->CheckEdge(newPoint);
	tri2->CheckEdge(newPoint);

	delete [] edge;
*/

bool TriangularMesh::Simplex::OnBoundary()
{
	bool onBoundary = false;
	//GetAdjacentPoint returns test point if it can't find an adjacent point
	//If any points don't have an adjacent point then I am on a boundary
	for(int i=0; i<nDims+1; i++)
		onBoundary = ( &MyPointCyclic(i) == &GetAdjacentPoint(MyPointCyclic(i)) ) ; 
	return onBoundary; 
}




