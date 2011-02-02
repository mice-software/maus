#include "Interface/Squeal.hh"
#include "Interface/Mesh.hh"
#include "Interface/Interpolator.hh"

#include "BTSpaceChargeField.hh"


#include "CLHEP/Units/PhysicalConstants.h"




double BTSpaceChargeField::e_const     = 1.;
const double BTTruncatedCoulomb_SC::r2Tol= 1e-9;

BTSpaceChargeField::BTSpaceChargeField()  {e_const = 1./4./CLHEP::pi/CLHEP::epsilon0;}
BTSpaceChargeField::~BTSpaceChargeField() {;}

void   BTSpaceChargeField::TruncatedCoulombField    (const double* Point, const double* u, const double& q, const double& m, const double& rMin2, double* EMfield)
{
  double r[3]  = {u[1]-Point[0],u[2]-Point[1],u[3]-Point[2]}; //u goes like t,x,y,z; Point goes like x,y,z,t
  double b[3]  = {u[5]/u[4], u[6]/u[4], u[7]/u[4]}; //beta vector
  double r2    = r[0]*r[0]+r[1]*r[1]+r[2]*r[2];
  double gamma = u[4]/m;
  if(r2 < rMin2) r2 = rMin2;
  double field = -q*e_const*gamma / pow(r2 + r2*(b[0]*b[0]+b[1]*b[1]+b[2]*b[2])*gamma*gamma, 1.5); //check!
  EMfield[3] += field*r[0];
  EMfield[4] += field*r[1];
  EMfield[5] += field*r[2];

  EMfield[0] += (b[1]*EMfield[2] - b[2]*EMfield[1])/CLHEP::c_light;
  EMfield[1] += (b[2]*EMfield[0] - b[0]*EMfield[2])/CLHEP::c_light;
  EMfield[2] += (b[0]*EMfield[1] - b[1]*EMfield[0])/CLHEP::c_light;  
//  std::cout << "sc " << field << " " << q  << " " << r2 << " " << gamma << " " << e_const << " ** ";
//  for(int i=0; i<6; i++)
//    std::cout << EMfield[i] << " "; std::cout << std::endl;
}

void BTSpaceChargeField::TruncatedCoulombPotential(const double* Point, const double* u, const double& q, const double& m, const double& rMin2, double* potential)
{
  double r[3]  = {u[1]-Point[0],u[2]-Point[1],u[3]-Point[2]}; //u goes like t,x,y,z; Point goes like x,y,z,t
  double b[3]  = {u[5]/u[4], u[6]/u[4], u[7]/u[4]}; //beta vector
  double r2    = r[0]*r[0]+r[1]*r[1]+r[2]*r[2];
  if(r2 < rMin2) r2 = rMin2;
  //phi/c - I replace with phi below... save allocating an extra double blah
  potential[3] = e_const / (sqrt(r2) + r[0]*b[0]+r[1]*b[1]+r[2]*b[2])/CLHEP::c_light;

  potential[0] = potential[3]*b[0];
  potential[1] = potential[3]*b[1];
  potential[2] = potential[3]*b[2];
  potential[3] = potential[3]*CLHEP::c_light;
}


///////////// BTTRUNCATEDCOULOMB_SC ////////////////////
//E_vec = -e/(4 pi eps0) gamma r_vec / r^3/(1+beta^2 gamma^2)^(3/2)
//B_vec = 1/c beta_vec X E_vec
void BTTruncatedCoulomb_SC::GetFieldValue( const double Point[4], double *EMfield ) const
{
  int n_events = u_vec.size()/8;
  for(int i=0; i<n_events; i++)
    BTSpaceChargeField::TruncatedCoulombField    (Point, &u_vec[i*8], q_vec[i], m_vec[i], rMin2, EMfield);
}

void BTTruncatedCoulomb_SC::GetVectorPotential( const double Point[4], double *potential ) const
{
  int n_events = u_vec.size()/8;
  for(int i=0; i<n_events; i++)
    BTSpaceChargeField::TruncatedCoulombPotential(Point, &u_vec[i*8], q_vec[i], m_vec[i], rMin2, potential);
}


void BTTruncatedCoulomb_SC::CalculateField(int n_events, const double * u_vector, const double * q_vector, const double* macro_vector)
{
  u_vec = std::vector<double>(u_vector, u_vector+n_events*8);
  q_vec.resize(n_events);
  m_vec.resize(n_events);
  for(int i=0; i<n_events; i++)
  {
    double* u    = &u_vec[i*8];
    m_vec[i]     = sqrt(u[4]*u[4]-u[5]*u[5]-u[6]*u[6]-u[7]*u[7]);
    q_vec[i]     = q_vector[i]*macro_vector[i];
    if(m_vec[i] < 1e-9 || std::isnan(m_vec[i]) || std::isinf(m_vec[i])) 
      throw(Squeal(Squeal::recoverable, "Non-physical mass encountered during space charge calculation", "BTTruncatedCoulomb_SC::CalculateField"));
  }
}

///////////// BTTRUNCATEDCOULOMB_SC END ////////////

///////////// BTPIC_SC ////////////////

BTPIC_SC::BTPIC_SC (bool willCalculateFields, bool willCalculatePotential, int sizeX, int sizeY, int sizeZ, 
                    double rmin)
    : _calculateFields(willCalculateFields), _calculatePot( willCalculatePotential ), _grid(NULL),
      _bInterpolator(NULL), _eInterpolator(NULL), _aInterpolator(4), _gridSize(3), _rMin2(rmin*rmin)
{
  _gridSize[0] = sizeX; _gridSize[1] = sizeY; _gridSize[2] = sizeZ;
  for(int i=0; i<4; i++) _aInterpolator[i] = NULL;
}


BTPIC_SC::~BTPIC_SC()
{
  if(_grid != NULL)          delete _grid;
  if(_bInterpolator != NULL) delete _bInterpolator;
  if(_eInterpolator != NULL) delete _bInterpolator;
  for(int i=0; i<int(_aInterpolator.size()); i++)
    if(_aInterpolator[i] != NULL) delete _aInterpolator[i];
}

//Build PIC mesh and populate interpolators
void BTPIC_SC::CalculateField(int n_events, const double * u_vector, const double * q_vector, const double* macro_vector)
{
//  std::cout << "Calculate field " << n_events << " " << u_vector << " "  << q_vector << " " << macro_vector << std::endl;
  //if(_grid != NULL) delete _grid; //Comment out - should be handled as a smart pointer
  if(_bInterpolator != NULL) delete _bInterpolator;
  if(_eInterpolator != NULL) delete _eInterpolator;
  for(int i=0; i<4; i++) if(_aInterpolator[i] != NULL) delete _aInterpolator[i];

  //Find limits of mesh
  std::vector<double> m_vec(n_events);
  double xMin=u_vector[1], xMax=xMin, yMin=u_vector[2], yMax=yMin, zMin=u_vector[3], zMax=zMin;
  for(int i=0; i<n_events; i++) 
  {
    const double* u = &u_vector[i*8];
    m_vec[i] = sqrt(u[4]*u[4]-u[5]*u[5]-u[6]*u[6]-u[7]*u[7]);
    if(u[1] < xMin) xMin = u[1];
    if(u[1] > xMax) xMax = u[1];
    if(u[2] < yMin) yMin = u[2];
    if(u[2] > yMax) yMax = u[2];
    if(u[3] < zMin) zMin = u[3];
    if(u[3] > zMax) zMax = u[3];
  }
  //Build mesh
  xMin -= 1.;
  yMin -= 1.;
  zMin -= 1.;
  xMax += 1.;
  yMax += 1.;
  zMax += 1.;
  _grid = new ThreeDGrid((xMax-xMin)/double(_gridSize[0]-1), (yMax-yMin)/double(_gridSize[1]-1), 
                         (zMax-zMin)/double(_gridSize[2]-1), xMin, yMin, zMin, 
                         _gridSize[0], _gridSize[1], _gridSize[2]);


  double ***Ex,***Ey,***Ez,***Bx,***By,***Bz;
  double ***A[4];

  if(_calculateFields) {Build(&Ex); Build(&Ey); Build(&Ez); Build(&Bx); Build(&By); Build(&Bz);}
  if(_calculatePot)    {Build(&A[0]); Build(&A[1]); Build(&A[2]); Build(&A[3]);}
  
  for(int i=0; i<n_events; i++)
  {
    for(Mesh::Iterator it=_grid->Begin(); it!=_grid->End(); it++)
    {
      double pos[4]   = {0,0,0,0};
      it.Position(pos);
      if(_calculateFields)
      {
        double field[6] = {0,0,0,0,0,0};
        BTSpaceChargeField::TruncatedCoulombField(pos, &u_vector[i*8], q_vector[i]*macro_vector[i], m_vec[i], _rMin2, field);
        Bx[it[0]-1][it[1]-1][it[2]-1] += field[0];
        By[it[0]-1][it[1]-1][it[2]-1] += field[1];
        Bz[it[0]-1][it[1]-1][it[2]-1] += field[2];
        Ex[it[0]-1][it[1]-1][it[2]-1] += field[3];
        Ey[it[0]-1][it[1]-1][it[2]-1] += field[4];
        Ez[it[0]-1][it[1]-1][it[2]-1] += field[5];
      }
      if(_calculatePot)
      {
        double pot[4] = {0,0,0,0};
        BTSpaceChargeField::TruncatedCoulombPotential(pos, &u_vector[i*8], q_vector[i]*macro_vector[i], m_vec[i], _rMin2, pot);
        for(int i=0; i<4; i++)
          A[i][it[0]-1][it[1]-1][it[2]-1] += pot[i];
      }
    } 
  }
  if(_calculateFields) 
  {
    _bInterpolator = new Interpolator3dGridTo3d(_grid, Bx, By, Bz); 
    _eInterpolator = new Interpolator3dGridTo3d(_grid, Ex, Ey, Ez);;
  }
  if(_calculatePot) 
    for(int i=0; i<4; i++)
      _aInterpolator[i] = new TriLinearInterpolator(_grid, A[i]);
}

void BTPIC_SC::Build(double**** x)
{
  (*x) = new double**[_gridSize[0]];
  for(int i=0; i<_gridSize[0]; i++)
  {
    (*x)[i] = new double*[_gridSize[1]];
    for(int j=0; j<_gridSize[1]; j++)
    {
      (*x)[i][j] = new double[_gridSize[2]];
      for(int k=0; k<_gridSize[2]; k++) (*x)[i][j][k] = 0; 
    }
  }
}

void BTPIC_SC::GetFieldValue( const double Point[4], double *EMfield ) const
{
  _bInterpolator->F(Point, EMfield);
  _eInterpolator->F(Point, EMfield+3);
}

void BTPIC_SC::GetVectorPotential( const double Point[4], double *potential ) const
{
  for(int i=0; i<4; i++)
    _aInterpolator[i]->F(Point, &potential[i]);
}

BTField* BTPIC_SC::Clone() const
{
  BTPIC_SC* clone = new BTPIC_SC(*this);
  if(_grid != NULL)          clone->_grid          = (ThreeDGrid*)_grid->Clone();
  if(_bInterpolator != NULL) clone->_bInterpolator = _bInterpolator->Clone();
  if(_eInterpolator != NULL) clone->_eInterpolator = _eInterpolator->Clone();
  for(unsigned int i=0; i<_aInterpolator.size(); i++)
    if(_aInterpolator[i] != NULL) clone->_aInterpolator[i] = _aInterpolator[i]->Clone();
  return clone;
}



///////////// BTPIC_SC END ////////////

