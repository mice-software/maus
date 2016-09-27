#include "src/legacy/Optics/CovarianceMatrix.hh"

#include "Tensor.hh"
#include "Tensor3.hh"

#include "Config/MiceModule.hh"
#include "Config/ModuleConverter.hh"
#include "Interface/Differentiator.hh"
#include "Maths/PolynomialMap.hh"
#include "src/legacy/Optics/PhaseSpaceVector.hh"


using CLHEP::HepSymMatrix;
using CLHEP::HepVector;
using CLHEP::c_light;

const std::string CovarianceMatrix::_psvVars[5]    = {"amplitude_t", "amplitude_x", "amplitude_y", "amplitude_4d", "amplitude_6d"};
const std::string CovarianceMatrix::_bunchVars[23] = {"weight", "emit_6d", 
                                                     "emit_4d",  "emit_t",  "emit_x",  "emit_y",
                                                     "beta_4d",  "beta_t",  "beta_x",  "beta_y",
                                                     "alpha_4d", "alpha_t", "alpha_x", "alpha_y",
                                                     "gamma_4d", "gamma_t", "gamma_x", "gamma_y",
                                                     "disp_x", "disp_y",
                                                     "ltwiddle", "lcan", "lkin"};
const std::string CovarianceMatrix::_varTypes[5]   = {"Mean", "Covariance", "Standard_Deviation", "Correlation", "Bunch_Parameter"};
const std::string CovarianceMatrix::_phaseSpace[6] = {"t", "E", "x", "px", "y", "py"};
const std::string CovarianceMatrix::_traceSpace[6] = {"t", "t\'", "x", "x\'", "y", "y\'"};

CovarianceMatrix::CovarianceMatrix()
  : _covNorm(HepSymMatrix(6,1)), _norm(-1), _mean(PhaseSpaceVector()), _weight(1)
{}

CovarianceMatrix::CovarianceMatrix(HepSymMatrix covariances, PhaseSpaceVector mean, double weight, bool covIsKinetic)
                : _mean(mean), _weight(weight)
{
  HepSymMatrix cov;
  if(!covIsKinetic) cov = TransformToKinetic(covariances, mean.Bz(), mean.Ez(), 0.);
  else cov = covariances;
  SetCovariances(cov);
}

CovarianceMatrix::CovarianceMatrix(HepSymMatrix covariances, PhaseSpaceVector mean, 
                                   double norm, double weight, bool covIsKinetic)
                : _norm(norm), _mean(mean), _weight(weight)
{
  HepSymMatrix cov;
  if(!covIsKinetic) cov = TransformToKinetic(covariances, mean.Bz(), mean.Ez(), 0.);
  else cov = covariances;
  _covNorm = cov;
}

CovarianceMatrix::CovarianceMatrix(HepSymMatrix covariances, PhaseSpaceVector mean, bool covIsKinetic)
                : _mean(mean), _weight(1)
{
  HepSymMatrix cov;
  if(!covIsKinetic) cov = TransformToKinetic(covariances, mean.Bz(), mean.Ez(), 0.);
  else cov = covariances;
  SetCovariances(cov);
}

CovarianceMatrix::CovarianceMatrix(double emittance_t, double beta_t, double alpha_t, double Ltwiddle_t,
                                   double emittance_l, double beta_l, double alpha_l,
                                   PhaseSpaceVector mean, double weight)
    : _covNorm(HepSymMatrix(6,0)), _mean(mean), _weight(weight)
{
  SetCovariances(emittance_t, beta_t, alpha_t, Ltwiddle_t, emittance_l, beta_l, alpha_l);
}

CovarianceMatrix::CovarianceMatrix(double emittance_x, double beta_x, double alpha_x,
                                   double emittance_y, double beta_y, double alpha_y,
                                   double emittance_l, double beta_l, double alpha_l,
                                   PhaseSpaceVector mean, double weight)
    : _covNorm(HepSymMatrix(6,0)), _mean(mean), _weight(weight)
{
  SetCovariances(emittance_x, beta_x, alpha_x, emittance_y, beta_y, alpha_y, emittance_l, beta_l, alpha_l);
}

CovarianceMatrix::CovarianceMatrix(const MiceModule* mod) : _covNorm(HepSymMatrix(6,0)), _norm(1), _mean(PhaseSpaceVector()), _weight(1)
{
  MCHit               hit = ModuleConverter::ModuleToHit(mod);
  CLHEP::HepSymMatrix mat = ModuleConverter::ModuleToBeamEllipse(mod, hit);
  SetMean(PhaseSpaceVector(&hit));
  SetCovariances(mat);
}


HepSymMatrix CovarianceMatrix::GetCanonicalCovariances() const
{
  double b = _mean.Bz();
  double e = _mean.Ez();
  return _norm*TransformToCanonical(_covNorm, b, e, 0.);
}

HepSymMatrix CovarianceMatrix::GetRawNormCanCovariances(PhaseSpaceVector reference) const
{
  HepSymMatrix out = GetNormCanCovariances();
  HepVector centre = _mean.getCanonicalSixVector() - reference.getCanonicalSixVector(); 
  //Loop over upper diagonal
  for(int i=0; i<6; i++)
    for(int j=i; j<6; j++)
      out[i][j] += centre[i]*centre[j]/_norm;
  return out;
}


HepSymMatrix CovarianceMatrix::GetNormCanCovariances() const
{
  double b = _mean.Bz();
  double e = _mean.Ez();
  return TransformToCanonical(_covNorm, b, e, 0.);
}

//use units where the covariances are ~1 to avoid double point precision errors
double CovarianceMatrix::GetSixDEmit() const
{
  return pow(GetKineticCovariances().determinant()*c_light, 1./2.) / _mean.m()/ _mean.m()/ _mean.m();
}

double CovarianceMatrix::GetTwoDEmit(int axis) const
{
  double determinant = GetNormKinCovariances().sub(axis*2+1,axis*2+2).determinant();
  double emittance   = _norm*sqrt(determinant)/_mean.m();
  return emittance;  
}

double CovarianceMatrix::GetTwoDBeta(int axis) const
{
  double p  = _mean.P();
  double m  = _mean.m();
  double em = GetTwoDEmit(axis);
  if(em<=0) return -1;
  return _norm*_covNorm[axis*2][axis*2]*p/(m*em);
}

double CovarianceMatrix::GetTwoDAlpha(int axis) const
{
  double m  = _mean.m();
  double em = GetTwoDEmit(axis);
  if(em<=0) return -1;
  return -_norm*_covNorm[axis*2][axis*2+1] / (m*em);
}

double CovarianceMatrix::GetTwoDGamma(int axis) const
{
  double p  = _mean.P();
  double m  = _mean.m();
  double em = GetTwoDEmit(axis);
  if(em<=0) return -1;
  return _norm*_covNorm[axis*2+1][axis*2+1] / (m*p*em);
}

double CovarianceMatrix::GetBetaTrans() const
{
  double em = GetEmitTrans();
  double p  = _mean.P();
  double m  = _mean.m();
  if(em<=0) return -1;
  return _norm*(_covNorm(3,3)+_covNorm(5,5))*p/(2*m*em);
}

double CovarianceMatrix::GetAlphaTrans() const
{
  double em = GetEmitTrans();
  double m  = _mean.m();
  if(em<=0) return -1;
  return -_norm*(_covNorm(3,4)+_covNorm(5,6))/(2*m*em);
}

double CovarianceMatrix::GetGammaTrans() const
{
  double em = GetEmitTrans();
  double p  = _mean.P();
  double m  = _mean.m();
  return _norm*(_covNorm(3,3)+_covNorm(5,5))/(2*m*p*em);
}

double CovarianceMatrix::GetEmitTrans() const
{
  HepSymMatrix localCov = GetNormCanCovariances().sub(3,6);
  double determinant = localCov.determinant();
  if(!(determinant>0)) return -1;
  double em = sqrt(sqrt(determinant))*_norm/_mean.m();
  return em;
}

double CovarianceMatrix::GetLKin() const
{
  return _norm*(_covNorm(3,6) - _covNorm(4,5));
}

double CovarianceMatrix::GetLCan() const
{
/*
  double m  = _mean.m();
  double p  = _mean.P();
  double em = GetEmitTrans();
  double b  = GetBetaTrans();
  double bz = _mean.Bz();
  double k  = GetKappa(bz,p);
*/
  return -_norm*( (_covNorm[4][3]-_covNorm[4][4]*_mean.Bz()*_mean.q()*c_light/2.) 
                 -(_covNorm[2][5]+_covNorm[2][2]*_mean.Bz()*_mean.q()*c_light/2.) );
}

double CovarianceMatrix::GetLTwiddle() const
{
  double em = GetEmitTrans();
  double m  = _mean.m();
  return GetLCan()/(m*em);
}

void CovarianceMatrix::SetCovariances(HepSymMatrix covariances, double determinant)
{
  if(determinant < 0) 
  {
    _norm    = pow(covariances.determinant(),1./6.);
    if(_norm>1e-9) _covNorm = covariances/_norm;
    else        {_covNorm = HepSymMatrix(6,0); _norm = 1;}
  }
  else 
  {
    _norm    = determinant;
    _covNorm = covariances;
  }
}

void CovarianceMatrix::SetCovariances(double emittance_t, double beta_t, double alpha_t, double Ltwiddle_t,
                                      double emittance_l, double beta_l, double alpha_l)
{
  double m = _mean.m();
  double p = _mean.P();
  double k = GetKappa(_mean.Bz(), p);
  double gamma_t = (1+alpha_t*alpha_t+(beta_t*k - Ltwiddle_t)*(beta_t*k-Ltwiddle_t))/beta_t;
  double gamma_l = (1+alpha_l*alpha_l)/beta_l;
  double sigmaxx   =  emittance_t*m*beta_t/p;
  double sigmaxpx  = -emittance_t*m*alpha_t;
  double sigmapxpx =  emittance_t*m*p*gamma_t;
  double sigmaxpy  = -emittance_t*m*(beta_t*k-Ltwiddle_t)*_mean.q();

  _covNorm[0][0] =  emittance_l*m*beta_l/p;
  _covNorm[1][1] =  emittance_l*m*gamma_l*p;
  _covNorm[0][1] = -emittance_l*m*alpha_l;
  _covNorm[2][2] =  _covNorm[4][4] = sigmaxx;
  _covNorm[3][3] =  _covNorm[5][5] = sigmapxpx;
  _covNorm[2][3] =  _covNorm[4][5] = sigmaxpx;
  _covNorm[3][4] = -sigmaxpy;
  _covNorm[2][5] =  sigmaxpy;

  _norm          = pow(_covNorm.determinant(), 1./6.);
  _covNorm      /= _norm;
}

void CovarianceMatrix::SetCovariances(double emittance_x, double beta_x, double alpha_x,
                                      double emittance_y, double beta_y, double alpha_y,
                                      double emittance_l, double beta_l, double alpha_l)
{
  double m = _mean.m();
  double p = _mean.P();
  double gamma_x = (1+alpha_x*alpha_x)/beta_x;
  double gamma_y = (1+alpha_y*alpha_y)/beta_y;
  double gamma_l = (1+alpha_l*alpha_l)/beta_l;
  _covNorm[0][0] =  emittance_l*m*beta_l/p;
  _covNorm[1][1] =  emittance_l*m*gamma_l*p;
  _covNorm[0][1] = -emittance_l*m*alpha_l;
  _covNorm[2][2] =  emittance_x*m*beta_x/p;
  _covNorm[3][3] =  emittance_x*m*p*gamma_x;
  _covNorm[2][3] = -emittance_x*m*alpha_x;
  _covNorm[4][4] =  emittance_y*m*beta_y/p;
  _covNorm[5][5] =  emittance_y*m*p*gamma_y;
  _covNorm[4][5] = -emittance_y*m*alpha_y;

  _norm          = pow(_covNorm.determinant(), 1./6.);
  _covNorm      /= _norm;
}

void CovarianceMatrix::SetDispersions(double dispersion_x, double dispersion_y)
{
    _covNorm[1][2] =  dispersion_x*_covNorm[1][1]/_mean.E();
    _covNorm[1][4] =  dispersion_y*_covNorm[1][1]/_mean.E();
}

void CovarianceMatrix::SetDispersionsPrime(double disp_prime_x, double disp_prime_y)
{
    _covNorm[1][3] =  disp_prime_x*_covNorm[1][1]/_mean.E();
    _covNorm[1][5] =  disp_prime_y*_covNorm[1][1]/_mean.E();
}


void CovarianceMatrix::SetRawNormCanCovariances(HepSymMatrix rawCov, double norm, PhaseSpaceVector reference)
{
  HepVector centre = _mean.getCanonicalSixVector() - reference.getCanonicalSixVector(); 
  HepSymMatrix out = rawCov;
  //Loop over upper diagonal
  for(int i=0; i<6; i++)
    for(int j=i; j<6; j++)
      out[i][j] -= centre[i]*centre[j]/norm;
  _covNorm         = TransformToKinetic(out, reference.Bz(), reference.Ez(),0); //BUG what about quads?
  _norm            = norm;
}

HepSymMatrix CovarianceMatrix::TransformToCanonical(HepSymMatrix kin, double Bz, double E, double dBdx) const
{
  HepSymMatrix can = kin;
  double b = 2*GetB0(Bz);

  can[3][2] = kin[3][2]-kin[4][2]*b/2;
  can[3][3] = kin[3][3]+kin[4][4]*b*b/4-kin[4][3]*b;
  can[4][3] = kin[4][3]-kin[4][4]*b/2;
  can[5][2] = kin[5][2]+kin[2][2]*b/2;
  can[5][3] = kin[5][3]-kin[4][2]*b*b/4+kin[3][2]*b/2-kin[5][4]*b/2;
  can[5][4] = kin[5][4]+kin[4][2]*b/2;
  can[5][5] = kin[5][5]+kin[2][2]*b*b/4+kin[5][2]*b;

  return can; 
}

HepSymMatrix CovarianceMatrix::TransformToKinetic(HepSymMatrix can, double Bz, double E, double dBdx) const
{
  HepSymMatrix kin = can;
  double b = 2*GetB0(Bz);

  kin[3][2] = can[3][2]+can[4][2]*b/2;
  kin[3][3] = can[3][3]+can[4][4]*b*b/4+can[4][3]*b;
  kin[4][3] = can[4][3]+can[4][4]*b/2;
  kin[5][2] = can[5][2]-can[2][2]*b/2;
  kin[5][3] = can[5][3]-can[4][2]*b*b/4+can[5][4]*b/2-can[3][2]*b/2;
  kin[5][4] = can[5][4]-can[4][2]*b/2;
  kin[5][5] = can[5][5]+can[2][2]*b*b/4-can[5][2]*b;

  return kin;
}

AnalysisPlaneBank CovarianceMatrix::GetAnalysisPlaneBank() const
{
  AnalysisPlaneBank out;
  out.CovarianceMatrix = GetKineticCovariances();
  out.planeNumber      = 0; 
  out.planeType        = 7;
  out.weight           = _weight; 
  out.z                = _mean.z();
  out.t                = _mean.t();
  out.pz               = _mean.Pz();
  out.E                = _mean.E();
  out.beta_p           = GetBetaTrans();
  out.l_can            = GetLCan();
  out.em4dXY           = GetEmitTrans();
  out.em6dTXY          = GetSixDEmit();

  for(int i=0; i<3; i++) out.beta[i] = GetTwoDBeta(i);
  for(int i=0; i<3; i++) out.em2d[i] = GetTwoDEmit(i);
  for(int i=0; i<6; i++) out.sums[i] = _mean.getSixVector()[i]; 
  out.amplitudePzCovariance = 0;

  return out;
}

std::ostream& operator<<(std::ostream& out, CovarianceMatrix cov) 
{
  cov.Print(out); 
  return out;
}

double  GetTwoDAmp(int axis, PhaseSpaceVector event, CovarianceMatrix Covariance)
{
  int ierr=0;
  CLHEP::HepSymMatrix matrix = Covariance.GetKineticCovariances().sub(axis+1, axis+2).inverse(ierr);
  CLHEP::HepVector    mean   = Covariance.GetMean().getSixVector();
  CLHEP::HepVector    vector = (event.getSixVector() - mean).sub(axis+1, axis+2);
  double              em2D   = Covariance.GetTwoDEmit(axis);
  return em2D*(vector.T() * matrix * vector)(1,1);
}

double  GetAmpTrans(PhaseSpaceVector event, CovarianceMatrix Covariance)
{
  int ierr=0;
  CLHEP::HepSymMatrix matrix = Covariance.GetKineticCovariances().sub(3, 6).inverse(ierr);
  CLHEP::HepVector    vector = (event.getSixVector()-Covariance.GetMean().getSixVector()).sub(3,6);
  double              em4D   = Covariance.GetEmitTrans();
  return em4D*(vector.T()*matrix*vector)(1,1);
}

double  GetSixDAmp(PhaseSpaceVector event, CovarianceMatrix Covariance)
{
  int ierr=0;
  CLHEP::HepSymMatrix matrix = Covariance.GetKineticCovariances().inverse(ierr);
  CLHEP::HepVector    vector = event.getSixVector()-Covariance.GetMean().getSixVector();
  double              em6D   = Covariance.GetSixDEmit();
  return em6D*(vector.T()*matrix*vector)(1,1);

}

double  GetTwoDAmp(int axis, PhaseSpaceVector event, PhaseSpaceVector mean, HepSymMatrix inverseCovariance, double emittance)
{
  CLHEP::HepVector    vector = (event.getSixVector()-mean.getSixVector()).sub(axis+1, axis+2);
  return emittance*(vector.T()*inverseCovariance*vector)(1,1);
}

double  GetAmpTrans(PhaseSpaceVector event, PhaseSpaceVector mean, HepSymMatrix inverseCovariance, double emittance)
{
  CLHEP::HepVector    vector = (event.getSixVector()-mean.getSixVector()).sub(3, 6);
  return emittance*(vector.T()*inverseCovariance*vector)(1,1);
}

double  GetSixDAmp(PhaseSpaceVector event, PhaseSpaceVector mean, HepSymMatrix inverseCovariance, double emittance)
{
  CLHEP::HepVector    vector = event.getSixVector()-mean.getSixVector();
  return emittance*(vector.T()*inverseCovariance*vector)(1,1);
}

CovarianceMatrix CovarianceMatrix::Interpolate(std::vector<CovarianceMatrix> cov, std::string variableName, double variable)
{
  if(cov.size() < 1) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Interpolating CovarianceMatrix array with length 0", "CovarianceMatrix::interpolate"));
  int    point = -1;
  int    i     = 0;
  double delta = 0;
  std::vector<PhaseSpaceVector> mean;
  for(unsigned int j=0; j<cov.size(); j++)
  {
    mean.push_back(cov[j].GetMean());
  }

  while(i<(int)mean.size())
  {
    if(variable < mean[i](variableName))
    {
      point = i;
      if(i>0) delta = ( variable - mean[i-1](variableName) )/(mean[i](variableName) - mean[i-1](variableName));
      i     = mean.size(); //break
    }
    else    i++;
  }
  if(point == -1)  return cov.back(); //variable > all of psv
  if(point == 0)   return cov[0];     //variable < all of psv

  HepSymMatrix covMatrix = (delta*cov[point].GetKineticCovariances()
                         + (1-delta)*cov[point-1].GetKineticCovariances());
  HepVector    sixVector = delta*(mean[point].getSixVector()
                         - mean[point-1].getSixVector())+mean[point-1].getSixVector();

  return           CovarianceMatrix(covMatrix, PhaseSpaceVector(sixVector, mean[0].m()));
}

bool CovarianceMatrix::IsPositiveDefinite(HepSymMatrix mat)
{
  bool posDef = true; 
  for(int i=0; i<mat.num_row(); i++) posDef = (mat.sub(1,i+1).determinant()>0 && posDef); //sylvester criterion
  return posDef;
}

HepMatrix CovarianceMatrix::Rotation(double angle)
{
  HepMatrix rotation(6,6,1);
  double fcos = cos(angle);
  double fsin = sin(angle);

  rotation      *=  fcos;
  rotation[0][0] =  1.;
  rotation[1][1] =  1.;
  rotation[4][2] = -fsin;
  rotation[5][3] = -fsin;
  rotation[2][4] =  fsin;
  rotation[3][5] =  fsin;
  return rotation;
}

CovarianceMatrix CovarianceMatrix::Rotate(double angle) const
{
  HepMatrix        rotation    = Rotation(angle);
  PhaseSpaceVector rotatedMean = GetMean().Rotate(angle);
  HepSymMatrix     rotatedCovs = GetKineticCovariances().similarity(rotation);
  return CovarianceMatrix(rotatedCovs, rotatedMean);
}

double CovarianceMatrix::SingleParticleVariable(PhaseSpaceVector psv, std::string variable)
{
  std::vector<std::string> list = psvVariables();
  int covStart = PhaseSpaceVector::listOfVariables().size();
  int var = -1;
  for(unsigned int i=0; i<list.size(); i++)
    if(LowerCase(variable) == LowerCase(list[i])) var = i;
  switch(var - covStart)
  {
    case 0: case 1: case 2:
      return GetTwoDAmp(var - covStart, psv, *this);  
    case 3:
      return GetAmpTrans(psv, *this);  
    case 4:
      return GetSixDAmp(psv, *this);  
    default:
      return psv(variable);
  }

}

double CovarianceMatrix::BunchVariable(std::string variable) const
{
  std::vector<std::string> list = bunchVariables();
  int var = -1;
  for(unsigned int i=0; i<list.size(); i++)
    if(LowerCase(variable) == LowerCase(list[i])) var = i;
  switch(var)
  {
    case 0: return GetWeight();
    case 1: return GetSixDEmit();
    case 2: return GetEmitTrans();
    case 3: case 4: case 5: return GetTwoDEmit(var-3);
    case 6: return GetBetaTrans();
    case 7: case 8: case 9: return GetTwoDBeta(var-7);
    case 10: return GetAlphaTrans();
    case 11: case 12: case 13: return GetTwoDAlpha(var-11);
    case 14: return GetGammaTrans();
    case 15: case 16: case 17: return GetTwoDGamma(var-15);
    case 18: case 19: return GetDispersion(var-17);
    case 20: return GetLTwiddle(); 
    case 21: return GetLCan(); 
    case 22: return GetLKin();
    default:
      throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Bunch variable not recognised", "CovarianceMatrix::BunchVariable(std::string)"));
  }
}

void CovarianceMatrix::SetFullVariable(std::string variable, double value)
{
  std::vector<std::string> list    = variableTypes();
  std::vector<std::string> psvList = listOfMyPhaseSpaceVariables();
  variable = LowerCase(variable);
  for(unsigned int i=0; i<list.size(); i++) list[i]    = LowerCase(list[i]);
  for(unsigned int i=0; i<list.size(); i++) psvList[i] = LowerCase(psvList[i]);

  std::stringstream varStream(variable);
  std::string       variableType = "";
  std::string       aux1         = "";
  std::string       aux2         = "";
  int               aux1Int      = -1;
  int               aux2Int      = -1;

  varStream >> variableType >> aux1 >> aux2;
  for(unsigned int i=0; i<psvList.size(); i++)
  {
    if(aux1 == psvList[i]) aux1Int = i;
    if(aux2 == psvList[i]) aux2Int = i;
  }

  
  if     (variableType == list[0]) _mean.setConservingMass(aux1, value);
  else if(variableType == list[1]){HepSymMatrix covMatrix = GetKineticCovariances(); covMatrix[aux1Int][aux2Int] = value; SetCovariances(covMatrix);}
  else if(variableType == list[4]) SetBunchVariable(aux1, value);
  else throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Did not recognise selection "+varStream.str(), "CovarianceMatrix::SetFullVariable()"));
  

}

void CovarianceMatrix::SetBunchVariable(std::string variable, double value)
{
  HepSymMatrix cov = GetKineticCovariances();
  int varInt = -1;
  for(unsigned int i=0; i<bunchVariables().size(); i++ )
    if(LowerCase(variable) == LowerCase(bunchVariables()[i]) ) varInt = i;

  switch(varInt)
  {
    case 0: {SetWeight(value); break;}
    case 1: {_norm = _mean.m()*value; break;} //scale 6x6 matrix
    case 2: 
    {
      double emit_4d_old = GetEmitTrans();
      for(int i=2; i<6; i++) for(int j=i; j<6; j++) 
        cov[i][j] *= value/emit_4d_old; 
      break;
    }
    case 6:  //beta_4d
    {
      double l_can_old   = GetLCan();
      double beta_4d_old = GetBetaTrans(); //mod to keep product sxx*spxpx constant
      cov[2][2] *= value/beta_4d_old; 
      cov[4][4] *= value/beta_4d_old; 
      cov[3][3] /= value/beta_4d_old; 
      cov[5][5] /= value/beta_4d_old; 
      double sqrtDetM = sqrt(cov.sub(3,6).determinant()); //lcan = xpyc-ypxc; xpyc = xpyk+x2 B0/2
      cov[2][5]       =  (l_can_old/2.-cov[2][2]*_mean.Bz()*_mean.q()*c_light/2.);
      cov[3][4]       = -(l_can_old/2.-cov[4][4]*_mean.Bz()*_mean.q()*c_light/2.);
      double px2      = (sqrtDetM+cov[2][3]*cov[4][5]-cov[3][4]*cov[2][5])/(0.5*cov[2][2]+0.5*cov[4][4]);
      cov[3][3]       = cov[5][5] = px2;
      break;
    }
    case 10: //alpha_4d
    {
      double em        = GetEmitTrans (); //mod to keep product sxx*spxpx constant
      double gamma_old = GetGammaTrans();
      double gamma_new = (1. + value*value+(GetLKin()/_mean.m()/em)*(GetLKin()/_mean.m()/em))/GetBetaTrans();
      cov[2][3]  = -_mean.m()*em*value; //alpha_2d
      cov[4][5]  = -_mean.m()*em*value; //alpha_2d
      cov[3][3] *= gamma_new/gamma_old; //gamma_2d 
      cov[5][5] *= gamma_new/gamma_old; //gamma_2d 
      break;
    }
    case 14: //gamma_4d
    {
      double gamma_4d_old = GetGammaTrans(); //mod to keep product sxx*spxpx constant
      cov[3][3] *= value/gamma_4d_old; 
      cov[5][5] *= value/gamma_4d_old; 
      cov[2][2] /= value/gamma_4d_old; 
      cov[4][4] /= value/gamma_4d_old; 
      break;
    }
    case 3: case 4: case 5:   //2d em  
    {
      double emit_2d_old = GetTwoDEmit(varInt - 3);
      for(int i=varInt*2-6; i<varInt*2-4; i++) for(int j=i; j<varInt*2-4; j++) 
        cov[i][j] *= value/emit_2d_old;
      break;
    }
    case 7: case 8: case 9:  //2d beta
    {
      double beta_2d_old = GetTwoDBeta(varInt - 7); //mod to keep product sxx*spxpx constant
      cov[2*varInt-14][2*varInt-14] *= value/beta_2d_old; 
      cov[2*varInt-13][2*varInt-13] /= value/beta_2d_old; 
      break;
    }
    case 11: case 12: case 13: //2d alpha
    {
      double em        = GetTwoDEmit (varInt-11); //mod to keep product sxx*spxpx constant
      double gamma_old = GetTwoDGamma(varInt-11);
      double gamma_new = (1. + value*value)/GetTwoDBeta(varInt-11);
      cov[2*varInt-22][2*varInt-21]  = -_mean.m()*em*value; //alpha_2d
      cov[2*varInt-21][2*varInt-21] *= gamma_new/gamma_old; //gamma_2d 
      break;
    }
    case 15: case 16: case 17: //2d gamma
    {
      double gamma_2d_old = GetTwoDGamma(varInt - 15); //mod to keep product sxx*spxpx constant
      cov[2*varInt-29][2*varInt-29] *= value/gamma_2d_old; 
      cov[2*varInt-30][2*varInt-30] /= value/gamma_2d_old; 
      break;
    }
    case 20: //ltwiddle 
    {
      value           = value*GetEmitTrans()*_mean.m()*2.; //then roll into standard lcan
      double sqrtDetM = sqrt(cov.sub(3,6).determinant()); //lcan = xpyc-ypxc; xpyc = xpyk+x2 B0/2
      cov[2][5]       =  (value/2.-cov[2][2]*_mean.Bz()*_mean.q()*c_light/2.);
      cov[3][4]       = -(value/2.-cov[4][4]*_mean.Bz()*_mean.q()*c_light/2.);
      double px2      = (sqrtDetM+cov[2][3]*cov[4][5]-cov[3][4]*cov[2][5])/(0.5*cov[2][2]+0.5*cov[4][4]);
      cov[3][3]       = cov[5][5] = px2;
      break;
    }
    case 21:
    {
      double sqrtDetM = sqrt(cov.sub(3,6).determinant()); //lcan = xpyc-ypxc; xpyc = xpyk+x2 B0/2
      cov[2][5]       =  (value/2.-cov[2][2]*_mean.Bz()*_mean.q()*c_light/2.);
      cov[3][4]       = -(value/2.-cov[4][4]*_mean.Bz()*_mean.q()*c_light/2.);
      double px2      = (sqrtDetM+cov[2][3]*cov[4][5]-cov[3][4]*cov[2][5])/(0.5*cov[2][2]+0.5*cov[4][4]);
      cov[3][3]       = cov[5][5] = px2;
      break;
    } //LKin()+ 2*m*em*b*k/1e3;
    case 22: //lkin
    {
      double sqrtDetM = sqrt(cov.sub(3,6).determinant());
      cov[2][5]       =  value/2.;
      cov[3][4]       = -value/2.;
      double px2      = (sqrtDetM+cov[2][3]*cov[4][5]-cov[3][4]*cov[2][5])/(0.5*cov[2][2]+0.5*cov[4][4]);
      cov[3][3]       = cov[5][5] = px2;
      break;
    }
    default: throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Bunch variable "+variable+" not recognised", "CovarianceMatrix::SetBunchVariable(string, double)"));
  }
  SetCovariances(cov);
}// return _norm*(_covNorm(3,6) - _covNorm(4,5)); 
/*
const std::string CovarianceMatrix::_bunchVars[21] = {"weight", "emit_6d", 
                                                     "emit_4d",  "emit_t",  "emit_x",  "emit_y",
                                                     "beta_4d",  "beta_t",  "beta_x",  "beta_y",
                                                     "alpha_4d", "alpha_t", "alpha_x", "alpha_y",
                                                     "gamma_4d", "gamma_t", "gamma_x", "gamma_y",
                                                     "ltwiddle", "lcan", "lkin"};

}
*/
std::vector<std::string> CovarianceMatrix::psvVariables() 
{
  std::vector<std::string> bunchList(_psvVars,   _psvVars+5);
  std::vector<std::string> psvList   = PhaseSpaceVector::listOfVariables();
  bunchList.insert(bunchList.begin(), psvList.begin(), psvList.end());
  return bunchList;
}

double CovarianceMatrix::FullVariable(std::string variable) const
{
  std::vector<std::string> list    = variableTypes();
  std::vector<std::string> psvList = listOfMyPhaseSpaceVariables();
  variable = LowerCase(variable);
  for(unsigned int i=0; i<list.size(); i++) list[i]    = LowerCase(list[i]);
  for(unsigned int i=0; i<list.size(); i++) psvList[i] = LowerCase(psvList[i]);
  std::stringstream varStream(variable);
  std::string       variableType = "";
  std::string       aux1         = "";
  std::string       aux2         = "";
  int               aux1Int      = -1;
  int               aux2Int      = -1;

  varStream >> variableType >> aux1 >> aux2;
  for(unsigned int i=0; i<psvList.size(); i++)
  {
    if(aux1 == psvList[i]) aux1Int = i;
    if(aux2 == psvList[i]) aux2Int = i;
  }

  if(variableType == list[0])      return GetMean()(aux1);
  else if(variableType == list[1]) return GetKineticCovariances()[aux1Int][aux2Int];
  else if(variableType == list[2]) return sqrt( GetKineticCovariances()[aux1Int][aux1Int] );
  else if(variableType == list[3]) return GetKineticCovariances()[aux1Int][aux2Int] / sqrt( GetKineticCovariances()[aux1Int][aux1Int] ) 
                                                                                    / sqrt( GetKineticCovariances()[aux2Int][aux2Int] );
  else if(variableType == list[4]) return BunchVariable(aux1);
  else throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Did not recognise selection "+varStream.str(), "CovarianceMatrix::FullVariable()"));
}

bool CovarianceMatrix::isBad()
{
  if(_norm<0 || _mean!=_mean || _weight<0 || _norm!=_norm || _weight!=_weight) 
    return true;
  for(int i=1; i<=6; i++)
    for(int j=i; j<=6; j++)
      if( _covNorm.fast(j,i)!=_covNorm.fast(j,i) ) return true;
  return false;
}

std::vector<std::string> CovarianceMatrix::listOfAuxiliaryVariables1(std::string variable)
{
  std::vector<std::string> list       = variableTypes();
  for(unsigned int i=0; i<list.size(); i++)
    if     (LowerCase(list[i]) == LowerCase(variable) && i!=4) return PhaseSpaceVector::listOfVariables();
    else if(LowerCase(list[i]) == LowerCase(variable))         return bunchVariables();

  throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Unrecognised covariance variable type "+variable, "CovarianceMatrix::listOfAuxiliaryVariables1(std::string)"));
}

std::vector<std::string> CovarianceMatrix::listOfAuxiliaryVariables2(std::string variable)
{
  std::vector<std::string> list       = variableTypes();
  for(unsigned int i=0; i<list.size(); i++)
    if(LowerCase(list[i]) == LowerCase(variable) && (i==1 || i==3) ) return PhaseSpaceVector::listOfVariables();
    else  if(LowerCase(list[i]) == LowerCase(variable))              return std::vector<std::string>();
  throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Unrecognised covariance variable type "+variable, "CovarianceMatrix::listOfAuxiliaryVariables2(std::string)"));
}


////////////// MomentHeap /////////////
MomentHeap::MomentHeap(int maxOrder, HepVector mean, double mass) : m_mass(mass), m_mean(mean), m_vars(6,0)
{
  if(maxOrder>2) m_higherMoments = std::vector<Tensor*>(maxOrder-2);
  if(m_higherMoments.size()>0) m_higherMoments[0] = new Tensor3(6,6,6, 0.);
  for(unsigned int i=1; i<m_higherMoments.size(); i++) m_higherMoments[i] = new Tensor(6, i+3, 0);
}

MomentHeap::MomentHeap(CovarianceMatrix covMatrix, std::vector<Tensor*> higherMoments)
          : m_mass(covMatrix.GetMean().m()), m_mean(covMatrix.GetMean().getSixVector()), m_vars(covMatrix.GetKineticCovariances()), m_higherMoments(higherMoments)
{}

MomentHeap::~MomentHeap()
{
//  std::cout << m_higherMoments.size() << " " << << std::endl;
//  for(unsigned int i=0; i<m_higherMoments.size(); i++) delete m_higherMoments[i];
}


const Tensor* MomentHeap::GetTensor(int order) const
{
  if(order<=2 || order-2>int(m_higherMoments.size()) )
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Could not find moment", "MomentHeap::GetTensor"));
  return m_higherMoments[order-3];
}

void MomentHeap::Add(double value, std::vector<int> place)
{
  if(place.size() == 2) 
    m_vars[place[0]][place[1]] += value;
  else if(int(place.size()) <= MaxOrder()) m_higherMoments[place.size()-3]->Add(place, value);
  else throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Add rank > largest rank on heap or <= 1", "MomentHeap::Add"));
}

double MomentHeap::GetMoment(std::vector<int> position) const
{
  if(position.size()>2 && int(position.size())<=MaxOrder())
  {
    sort(position.begin(), position.end()); //better to design proper symmetric tensor class
    return GetTensor(position.size())->Get(position);
  }
  else if(position.size() == 2)
    return m_vars[position[0]][position[1]];
  else if(position.size() == 1)
    return m_mean[position[0]];
  throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Index out of range", "MomentHeap::GetMoment"));
}

std::ostream& MomentHeap::Print(std::ostream& out) const
{
  out << std::scientific << std::setprecision(5) << "MomentHeap order " << MaxOrder() <<  std::endl;
/*  out << transpose(m_mean) << "\n ************** "  << std::endl;
  out << m_vars << "\n ************* " << std::endl;
  for(unsigned int i=0; i<m_higherMoments.size(); i++)
    out << *(m_higherMoments[i]) << "\n ************* " << std::endl;*/
  for(int i = 0; i < MaxOrder(); ++i) 
  {
    int kvec_front = 1;
    for(size_t j = MAUS::PolynomialMap::NumberOfPolynomialCoefficients(6, i);
        j < MAUS::PolynomialMap::NumberOfPolynomialCoefficients(6, i+1);
        ++j) {
      std::vector<int> kvec = MAUS::PolynomialMap::IndexByVector(j, 6);
      if(kvec.front() != kvec_front) {std::cout << "\n"; kvec_front = kvec.front();}
      for(unsigned int k=0; k<kvec.size()-1; k++) std::cout << kvec[k] << ".";
      double mom = GetMoment(kvec);
      std::cout << kvec.back() << " * ";
      if(mom >=0 ) std::cout << " ";
      std::cout << mom << "    ";

    }
    std::cout << std::endl;
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const MomentHeap& heap)
{
  return heap.Print(out);
}

MAUS::PolynomialMap MomentHeap::Weighting(MomentHeap in, MomentHeap target, int order)
{
  size_t dimension = 6;
  size_t size = MAUS::PolynomialMap::NumberOfPolynomialCoefficients(
      dimension, order);
  MAUS::Vector<double> u(size-1);
  MAUS::Matrix<double> M(size-1, size-1);
  for(size_t i=1; i<size; i++)
  {
    std::vector<int> index1 = MAUS::PolynomialMap::IndexByVector(i, dimension);
    u(i)                  = target.GetMoment(index1) - in.GetMoment(index1);
    for(size_t j=1; j<size; j++)
    {
      std::vector<int> index2 = MAUS::PolynomialMap::IndexByVector(j, dimension);
      std::vector<int> index3 = index2;
      index3.insert(index3.begin(), index1.begin(), index1.end());
      M(j,i)             = in.GetMoment(index3) - in.GetMoment(index1)*target.GetMoment(index2);
    }
  }
  MAUS::Vector<double> a  = inverse(M) * u;
  MAUS::Vector<double> a2 = MAUS::Vector<double>(a.size()+1, 1.);
  for(size_t i=0; i<a.size(); i++) a2(i+2) = a(i+1);
  return MAUS::PolynomialMap(dimension, transpose(a2) );
}
