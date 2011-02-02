#ifndef Differentiator_hh
#define Differentiator_hh 1

#include "Interpolator.hh"
#include "PolynomialVector.hh"
#include <map>


//Differentiator.hh
//Contains:

//Differentiator, an arbitrary order numerical differentiation class
//  i.e. differentiates a VectorMap with arbitrary PointDimension and ValueDimension to a certain order
//  return value is a vector of differentials up to that order
//  so Differentiator is itself a VectorMap

//PolynomialInterpolator, an arbitrary order polynomial interpolation
//  Interpolates from a mesh of arbitrary dimension with values and arbitrary order differentials stored at each point
//  Creates at each point a polynomial
//  Returns values taken from this polynomial
//  BUG - I can't mix interpolation from neighbouring points and differentials at each point
//  So if I know y(x1) dy/dx(x1), y(x2), dy/dx(x2), I can EITHER use y, dy/dx to do the interpolation OR use y(x1), y(x2)
//  but not both. I will fix this if asked, or if I need it. Probably non-trivial to fix.

//Differentiator
//Finds arbitrary order differentials for an arbitrary VectorMap or function pointer
class Differentiator : public VectorMap
{
public:
    //Differentiate at each point in mesh
    //Differentiate using d^(n)y_j/dx_i^(n) = magnitude_n*delta_i^(n)
    Differentiator (VectorMap* in, std::vector<double> delta, std::vector<double> magnitude);
    ~Differentiator() {;}
    //return differential coefficients at point; vectors NOT checked for size
    //return value is array of dy_i/dx_j ordered like i=0,j=0;i=0,j=1;...;i=1,j=0;i=1,j=1;...;i=ni;j=nj 
    void  F    (const double*    point, double* value)                          const;
    //return value is matrix of dy_i/dx_j
    void  F    (const MVector<double>& point, MMatrix<double>& differentials) const;
    void  Y    (const double* point,    double* value)                  const {return _y->F(point, value);}
    //Return coefficients for polynomial fit to taylor expansion around point
    //PolynomialMap returns coefficients for polynomial about 0 that has correct values at point
    MMatrix<double>  PolynomialMap       (const MVector<double>& point) const;
    //CentredPolynomialMap returns coeffs for polynomial about inPoint that has correct values at point
    MMatrix<double>  CentredPolynomialMap(const MVector<double>& point) const;
    //Tell me the required dimension of the input point and output value
    unsigned int PointDimension()    const { return _inSize;  }
    unsigned int ValueDimension()    const { return _outSize; }
    unsigned int NumberOfDiffRows()  const { return _diffKey.size(); }
    //"Copy constructor"
    Differentiator*  Clone()         const { return new Differentiator(*this); }
    //Return the original function ("0th differential")
    VectorMap*   FunctionMap()     const { return _y; }
    //Make a polynomial using the differential coefficients
    PolynomialVector* PolynomialFromDifferentials(const MVector<double>& point) const;
    PolynomialVector* PolynomialFromDifferentials(double* point)                 const;

    friend std::ostream& operator<<(std::ostream&, const Differentiator&); 

private:
    std::vector< std::vector<double> > SetupInPoints(const MVector<double>& inPoint) const;
    MMatrix<double>                   SetupMatrixIn (std::vector< std::vector<double> > inVector, const MVector<double>& inPoint) const;
    MMatrix<double>                   SetupMatrixOut(std::vector< std::vector<double> > inVector) const;

    int _inSize;
    int _outSize;

    std::vector< std::vector<int> > _diffKey;
    std::vector< int >              _factKey;
    std::vector<double>             _delta;
    std::vector<double>             _magnitude;
    int                             _diffOrder;
    VectorMap*                      _y;
    MMatrix<double>                 _polyDiffCoefficient; //d(n)y/dx = a x^n
};
std::ostream& operator<<(std::ostream&, const Differentiator&); 

//PolynomialInterpolator 
//uses differentials at different points to interpolate
//works over arbitrary order differentials/polynomials combinations
//actually generalisation of "differentiator" to work when e.g. some 
//differentials as well as values are known
//in this implementation, I assume I have all differentials up to order n at all points
//same maths/similar code can give implementation for an arbitrary set of polynomials/differentials at arbitrary points

//BUG - Works for EITHER length of differential = 0 OR length of polynomial = 0 but fails
//for mixed differentials - I think because some vectors are parallel so matrix
//inversion yields nan

class PolynomialInterpolator : public VectorMap
{
public:
    PolynomialInterpolator(Mesh* mesh, VectorMap* F, int differentialOrder, int pointOrder, std::vector<double> delta, std::vector<double> magnitude);
    //return map value; vectors NOT checked for size
    void  F    (const double*   point,       double* value)              const;
    void  F    (const Mesh::Iterator& point, double* value)              const //overload if mesh::pointdimension != vectormap::pointdimension 
    {double* PointA = new double[this->PointDimension()]; point.Position(PointA); F(PointA, value); delete PointA;}
    //Tell me the required dimension of the input point and output value
    unsigned int PointDimension()         const {return _inSize;}
    unsigned int ValueDimension()         const {return _outSize;}
    unsigned int PolynomialOrder()        const {return _totalOrder;} //highest polynomial that will be in the interpolation
    unsigned int DifferentialOrder()      const {return _differentialOrder;} //polynomial "order" to be contributed by differentials
    unsigned int NumberOfPoints()         const {return int(ceil(NumberOfIndices()/NumberOfDiffIndices())); } //number of points needed for each interpolation
    unsigned int PointOrder()             const {return _totalOrder - _differentialOrder;} //polynomial "order" to be contributed by points
    unsigned int NumberOfDiffIndices()    const 
    {return PolynomialVector::NumberOfPolynomialCoefficients(PointDimension(), DifferentialOrder()+1); } //number of differentials at each point
    unsigned int NumberOfIndices()        const
    {return PolynomialVector::NumberOfPolynomialCoefficients(_inSize, _totalOrder+1);} //number of rows in the interpolation
    //Read and write operations
    PolynomialInterpolator* Clone()       const; //copy function
    ~PolynomialInterpolator();

    //The polynomial vector at a point on the mesh
    PolynomialVector* PolyVec(Mesh::Iterator it) const {return _points[it.ToInteger()];}
    VectorMap*        Function()                       {return _func;}
    Mesh*             GetMesh()                  const {return _mesh;}

    static bool             M1_LT_M2(const Mesh::Iterator& m1,const Mesh::Iterator& m2); //comparator to sort by number of steps from _itCompCentre

private:

    Mesh*                _mesh;
    PolynomialVector**   _points;
    VectorMap*           _func;
    int                  _differentialOrder;
    int                  _totalOrder;
    int                  _inSize;
    int                  _outSize;
    std::vector<double>  _delta;
    std::vector<double>  _magnitude;

    static Mesh::Iterator _itCompCentre;

    std::vector< std::vector< int > >    _muIndex;
    std::vector< std::vector< int > >    _alphaIndex;
    std::vector< std::vector< double > > _taylorCoefficient;

    typedef std::map<PolynomialInterpolator*,Mesh*> PIMMap;
    static PIMMap        _meshCounter;

    void                          BuildFixedMeshPolynomials(VectorMap* F);
    PolynomialVector*             PolynomialFromDiffs(Mesh::Iterator point, Differentiator* diffs);
    std::vector< Mesh::Iterator > GetFixedMeshPoints(Mesh::Iterator dualIterator, int polySize);
    MMatrix<double>               GetX(std::vector< Mesh::Iterator> points );
    MMatrix<double>               GetD(std::vector< Mesh::Iterator> points, Differentiator* diff );

    //Functions to build up a set of points on the mesh nearby
    void Search(int pos, std::vector<int> search, std::vector<Mesh::Iterator>& points, const Mesh* mesh);
    bool CheckDirection(Mesh::Iterator start, Mesh::Iterator it);

};
std::ostream& operator<<(std::ostream&, const PolynomialInterpolator&);



#endif




