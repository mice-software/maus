// MAUS WARNING: THIS IS LEGACY CODE.
#include "Interface/Differentiator.hh"

#include <iomanip>
#include <sstream>

#include "gsl/gsl_sf_gamma.h"

////////////////////////// DIFFERENTIATOR ///////////////////////////

Differentiator::Differentiator (VectorMap* in, std::vector<double> delta, std::vector<double> magnitude)
 : _inSize(in->PointDimension()), _outSize(G4MICE::PolynomialVector::NumberOfPolynomialCoefficients(_inSize, magnitude.size())*in->ValueDimension()), _diffKey(), _factKey(),
   _delta(delta), _magnitude(magnitude), _diffOrder(magnitude.size()), _y(in)
{
    for(int i=0; i<int(G4MICE::PolynomialVector::NumberOfPolynomialCoefficients(_inSize, magnitude.size()) ); i++) 
    {
        _diffKey.push_back(G4MICE::PolynomialVector::IndexByVector(i,_inSize));
        _factKey.push_back(1);
        std::vector<int> powerKey = G4MICE::PolynomialVector::IndexByPower(i,_inSize);
        for(int j=0; j<int(powerKey.size()); j++) 
            _factKey[i] *= gsl_sf_fact(powerKey[j]);
    }
}

std::vector< std::vector<double> > Differentiator::SetupInPoints(const MVector<double>& inPoint) const
{
    std::vector< std::vector<double> > inVector( _diffKey.size() );

    for(unsigned int i=0; i<_diffKey.size(); i++)
    {
        inVector[i] = std::vector<double>(_inSize, 0);
        for(int j=0; j<_inSize; j++) inVector[i][j] = inPoint(j+1);
        for(unsigned int j=0; j<_diffKey[i].size(); j++)
            inVector[i][_diffKey[i][j]] += _magnitude[_diffKey[i].size()]*_delta[_diffKey[i][j]];
    }

    return inVector;
}

MMatrix<double> Differentiator::SetupMatrixIn(std::vector< std::vector<double> > inVector, const MVector<double>& inPoint) const
{
    MMatrix<double> matrixIn(_diffKey.size(), _diffKey.size(), 0.);

    for(unsigned int i=0; i<_diffKey.size(); i++)
    {
        for(int j=0; j<_inSize; j++)
            inVector[i][j] -= inPoint(j+1);
        for(unsigned int j=0; j<_diffKey.size(); j++)
        {
            matrixIn(j+1,i+1) = 1;
            for(unsigned int k=0; k<_diffKey[j].size(); k++)
                matrixIn(j+1,i+1) *= inVector[i][ _diffKey[j][k] ];
        }
    }
    return matrixIn;
}


MMatrix<double> Differentiator::SetupMatrixOut(std::vector< std::vector<double> > inVector) const
{
    MMatrix<double> matrixOut(_diffKey.size(), _y->ValueDimension(), 0.);

    double* out = new double[_y->ValueDimension()];
    for(unsigned int i=0; i<_diffKey.size(); i++)
    {
        for(unsigned int j=0; j<_y->ValueDimension(); j++) out[j] = 0.;
        Y(&inVector[i][0], out);
        for(unsigned int j=0; j<_y->ValueDimension(); j++) matrixOut(i+1,j+1) = out[j];
    }
    delete [] out;
    return matrixOut;
}

MMatrix<double> Differentiator::PolynomialMap(const MVector<double>& inPoint) const
{
    MVector<double> zero(PointDimension(), 0);
    std::vector< std::vector<double> > inVec = SetupInPoints (inPoint);
    MMatrix<double>                    in    = SetupMatrixIn (inVec, zero);
    MMatrix<double>                    out   = SetupMatrixOut(inVec);
//    for(int i=0; i<inVec.size(); i++) { for(int j=0; j<inVec[i].size(); j++) std::cout << std::setw(12) << inVec[i][j] << " "; std::cout << std::endl;} 
    MMatrix<double> outMap = out.T() * in.inverse();
/*
    std::cout << "InPoints" << std::endl;
    for(unsigned int i=0; i<inVec.size(); i++) { for(unsigned int j=0; j<inVec[i].size(); j++) std::cout << inVec[i][j] << " "; std::cout << std::endl;}
    std::cout << "OUT" << out.T() << "IN" << in << "ANSWER" << outMap << std::endl;
*/

    return outMap;
}

MMatrix<double> Differentiator::CentredPolynomialMap(const MVector<double>& inPoint) const
{
    MVector<double> zero(PointDimension(), 0);
    std::vector< std::vector<double> > inVec = SetupInPoints (inPoint);

    MMatrix<double>                   in    = SetupMatrixIn (inVec, inPoint);
    MMatrix<double>                   out   = SetupMatrixOut(inVec);
    MMatrix<double> outMap = out.T() * in.inverse();

    return outMap;
}


void Differentiator::F(const double*    point, double* value) const
{
    MVector<double> pHep(PointDimension(),0);
    for(int i=0; i<int(PointDimension()); i++) pHep(i+1) = point[i];
    MMatrix<double> diffs(_y->ValueDimension(), _diffKey.size());
    F  (pHep, diffs);
    int index = -1;
    for(size_t i=0; i<diffs.num_row(); i++)
        for(size_t j=0; j<diffs.num_col(); j++)
            value[++index] = diffs(i+1,j+1);
}

void Differentiator::F(const MVector<double>& point, MMatrix<double>& differentials) const
{
    //Fit a polynomial around point
    MMatrix<double> map = CentredPolynomialMap(point);
    //d^p y_j/dx^p = p_1! p_2!... p_n! a_{pj}
    //where x^p is shorthand for x_1^{p_1} x_2^{p_2}...x_n^{p_n} etc
    for(unsigned int i=0; i<NumberOfDiffRows(); i++)
    {
        for(int j=0; j<int(_y->ValueDimension()); j++)
        {
            differentials(j+1,i+1)  = map(j+1,i+1);
            differentials(j+1,i+1) *= _factKey[i];
        }
    }
}

G4MICE::PolynomialVector* Differentiator::PolynomialFromDifferentials(const MVector<double>& point) const
{
    return new G4MICE::PolynomialVector(_inSize, PolynomialMap(point) );
}

G4MICE::PolynomialVector* Differentiator::PolynomialFromDifferentials(double* point) const
{
    MVector<double> pointHep(PointDimension());
    for(int i=0; i<int(PointDimension()); i++) pointHep(i+1) = point[i];
    return PolynomialFromDifferentials(pointHep);
}



////////////////// DIFFERENTIATOR END //////////////////////


////////////////// POLYNOMIALINTERPOLATOR //////////////////////

PolynomialInterpolator::PIMMap PolynomialInterpolator::_meshCounter = PIMMap();

PolynomialInterpolator::PolynomialInterpolator(Mesh* mesh, VectorMap* F, int differentialOrder, int interpolationOrder, 
                                               std::vector<double> delta, std::vector<double> magnitude)
    : _mesh(mesh), _func(F), _differentialOrder(differentialOrder), _totalOrder(interpolationOrder+differentialOrder), 
      _inSize(F->PointDimension()), _outSize(F->ValueDimension()), _delta(delta), _magnitude(magnitude), _muIndex(), _alphaIndex(), _taylorCoefficient()
{
    for(unsigned int i=0; i<NumberOfDiffIndices(); i++) _muIndex.   push_back(G4MICE::PolynomialVector::IndexByPower(i, _inSize)); //index of differentials at each point
    for(unsigned int i=0; i<NumberOfIndices();     i++) _alphaIndex.push_back(G4MICE::PolynomialVector::IndexByPower(i, _inSize)); //index of polynomials at each interpolation
    for(unsigned int i=0; i<_muIndex.size(); i++)
    {
        _taylorCoefficient.push_back(std::vector<double>(_alphaIndex.size(), 1));
        for(unsigned int j=0; j<_alphaIndex.size(); j++)
        {
            for(int k=0; k<_inSize; k++)
            {
                _taylorCoefficient[i][j] *= double(gsl_sf_fact(_muIndex[i][k]) );//double(gsl_sf_fact(_alphaIndex[i][k]) );
            }
        }
    }
    BuildFixedMeshPolynomials(F);
    _meshCounter[this] = _mesh;
}

PolynomialInterpolator* PolynomialInterpolator::Clone()       const
{
    PolynomialInterpolator * clone =  new PolynomialInterpolator(*this);
    clone->_points = new G4MICE::PolynomialVector*[_mesh->End().ToInteger()];
    for(int i=0; i<_mesh->End().ToInteger(); i++) clone->_points[i] = _points[i]->Clone(); 
    _meshCounter[clone] = _mesh;
    return clone;
} //copy function


PolynomialInterpolator::~PolynomialInterpolator()
{
    for(Mesh::Iterator it = _mesh->Begin(); it<_mesh->End(); it++)
    {
        delete _points[it.ToInteger()];
    }
    delete [] _points;
    _meshCounter.erase(this);
    //can't believe that search by value doesn't exist in std::map
    //not sure I understand std::map - some stuff makes no sense
    bool _meshStillExists = false;
    for(PIMMap::iterator it = _meshCounter.begin(); it != _meshCounter.end(); it++)
        if(it->second == _mesh) _meshStillExists = true;
    if (!_meshStillExists) delete _mesh;
}

void PolynomialInterpolator::BuildFixedMeshPolynomials(VectorMap* F)
{
    Differentiator*    diff     = new Differentiator(F, _delta, _magnitude);
    _points     = new G4MICE::PolynomialVector*[_mesh->End().ToInteger()];
    for(Mesh::Iterator it=_mesh->Begin(); it<_mesh->End(); it++)
    {
        _points[it.ToInteger()] = PolynomialFromDiffs(it, diff);
    }
    delete diff;
}

void PolynomialInterpolator::F(const double* point, double* value) const
{
    Mesh::Iterator      it  = _mesh->Nearest(point);
    std::vector<double> pos = it.Position();
    for(unsigned int i=0; i<PointDimension(); i++) pos[i] = point[i] - pos[i];
    G4MICE::PolynomialVector* vecP = _points[it.ToInteger()];
    vecP->F(&pos[0], value);
}

bool PolynomialInterpolator::CheckDirection(Mesh::Iterator start, Mesh::Iterator it)
{
    bool direction = true;
    for(unsigned int i=0; i<PointDimension(); i++)
        if(start[i] <= start.GetMesh()->End()[i]/2.) 
            direction &= (start[i] <= it[i]);
        else direction &= start[i] >= it[i];
    return direction;
}

std::vector< Mesh::Iterator > PolynomialInterpolator::GetFixedMeshPoints(Mesh::Iterator start, int polySize)
{
    std::vector< Mesh::Iterator > points;
    for(Mesh::Iterator it = _mesh->Begin(); it<_mesh->End(); it++) if(CheckDirection(start, it))
        points.push_back(it);
    PolynomialInterpolator::_itCompCentre = start;
    std::vector< Mesh::Iterator > closest_points(NumberOfPoints());
    std::partial_sort_copy(points.begin(), points.end(), closest_points.begin(), closest_points.end(), M1_LT_M2);
    return closest_points;
}

Mesh::Iterator PolynomialInterpolator::_itCompCentre = Mesh::Iterator();
bool PolynomialInterpolator::M1_LT_M2(const Mesh::Iterator& m1, const Mesh::Iterator& m2)
{
    int dist = 0;
    int size = m1.State().size();
    for(int i=0; i<size; i++)
        dist += (m1[i] - _itCompCentre[i])*(m1[i] - _itCompCentre[i])
              - (m2[i] - _itCompCentre[i])*(m2[i] - _itCompCentre[i]);
    return dist < 0;
}

G4MICE::PolynomialVector* PolynomialInterpolator::PolynomialFromDiffs(Mesh::Iterator dualPoint, Differentiator* diff)
{
    std::vector< Mesh::Iterator > points = GetFixedMeshPoints(dualPoint, _muIndex.size());
    MMatrix<double> X = GetX(points);
    MMatrix<double> D = GetD(points, diff);
    MMatrix<double> A = D.T()*X.inverse();

    return new G4MICE::PolynomialVector(_inSize, A );
}

MMatrix<double> PolynomialInterpolator::GetX(std::vector< Mesh::Iterator> points )
{
    int size( NumberOfIndices() );
//    std::cout << size << std::endl;
    MMatrix<double> X(size, size, 0.);
    int gamma = -1;

    for(int nu=0; nu<int(NumberOfPoints()) && gamma < size; nu++)
    {
//        std::cout << points[nu] << std::endl;
        for(int mu=0; mu<int(_muIndex.size()) && gamma < size; mu++)
        {
            gamma++;
            for(int alpha=0; alpha < size; alpha++)
            {
                X(alpha+1,gamma+1)  = 1.;
                for(int k=0; k<_inSize; k++)
                {
//                    std::cout << "a" << k << " " << _alphaIndex[alpha][k] << " ";
//                    std::cout << "m" << k << " " << _muIndex[mu][k] << " ** ";
                    int dpow = _alphaIndex[alpha][k]-_muIndex[mu][k];
                    if(dpow > 0)
                    {
                       X(alpha+1,gamma+1) *= pow(points[nu].Position()[k]-points[0].Position()[k], dpow);
                    }
                    else if(dpow < 0)
                    {
                        X(alpha+1,gamma+1) = 0.;
                    }
                }
                X(alpha+1,gamma+1) *=_taylorCoefficient[mu][alpha];
//                std::cout << "a: " << alpha << " g: " << gamma << " t: " << _taylorCoefficient[mu][alpha] << " X: " << X[alpha][gamma]<< std::endl;
            }
        }
    }
    return X;
}

MMatrix<double> PolynomialInterpolator::GetD(std::vector< Mesh::Iterator> points, Differentiator* diff)
{
    int size( NumberOfIndices() );
    MMatrix<double> Delta(size, _outSize, 0.);
    MMatrix<double> Diffs(_outSize, diff->NumberOfDiffRows(), 0.);
    MVector<double> Pos  (_inSize, 0);
    int gamma = 0;
    for(unsigned int nu=0; nu<NumberOfPoints() && gamma < size; nu++)
    {
        std::vector<double> posVec = points[nu].Position();
        for(int i=0; i<_inSize; i++) Pos(i+1) = posVec[i]; 
        diff->F(Pos, Diffs);
//         std::cout << "DIFFS" << Diffs << std::endl;
        for(unsigned int mu=0; mu<_muIndex.size() && gamma < size; mu++)
        {
            for(int beta=0; beta<_outSize; beta++)
            {
                Delta(gamma+1,beta+1) = Diffs(beta+1,mu+1);
            }
            gamma++;
        }
    }
    return Delta;
}



