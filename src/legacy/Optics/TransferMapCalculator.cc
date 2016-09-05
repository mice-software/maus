#include "TransferMapCalculator.hh"

#include "src/legacy/BeamTools/BTTracker.hh"
#include "Maths/PolynomialMap.hh"

OpticsModel TransferMapCalculator::_optics;
vector<PhaseSpaceVector>  TransferMapCalculator::_referenceTrajectory = vector<PhaseSpaceVector>();
double TransferMapCalculator::_mass           = 0.;
double TransferMapCalculator::_relativeError  = 1e-5;
double TransferMapCalculator::_absoluteError  = 1e-5;
double TransferMapCalculator::_dzRef          = 1;
int    TransferMapCalculator::_maxNSteps      = 1000;
PhaseSpaceVector TransferMapCalculator::_wrapperReferenceIn;
PhaseSpaceVector TransferMapCalculator::_wrapperReferenceOut;


void TransferMapCalculator::TransportWrapper(const double* point, double* value)
{
  double pointV[8];

  pointV[0] = point[0];
  pointV[1] = point[2];
  pointV[2] = point[4];
  pointV[3] = _wrapperReferenceIn.z();
  pointV[4] = point[1];
  pointV[5] = point[3];
  pointV[6] = point[5];
  pointV[7] = sqrt(pointV[4]*pointV[4] - pointV[5]*pointV[5] - pointV[6]*pointV[6] - _mass*_mass) * _wrapperReferenceIn.Pz()/fabs(_wrapperReferenceIn.Pz());

  BTTracker::integrate(_wrapperReferenceOut.z(), pointV, _optics.GetTheField(), BTTracker::z, 1., _wrapperReferenceIn.charge());

  value[0] = pointV[0] - _wrapperReferenceOut.t();
  value[2] = pointV[1] - _wrapperReferenceOut.x();
  value[4] = pointV[2] - _wrapperReferenceOut.y();
  value[1] = pointV[4] - _wrapperReferenceOut.E();
  value[3] = pointV[5] - _wrapperReferenceOut.Px();
  value[5] = pointV[6] - _wrapperReferenceOut.Py();
}


TransferMapCalculator::TransferMapCalculator()
{;}

TransferMap TransferMapCalculator::GetPolynomialTransferMap(PhaseSpaceVector referenceIn, double targetZ, std::vector<double> delta, std::vector<double> magnitude)
{
  int               dimension = 6;

  double pointV[8];
  for(int i=0; i<3; i++) {pointV[i+1] = referenceIn[i]; pointV[i+5] = referenceIn[i+4];}
  _wrapperReferenceIn = referenceIn;
  pointV[0] = referenceIn.t();
  pointV[4] = referenceIn.E();
  BTTracker::integrate(targetZ, pointV, _optics.GetTheField(), BTTracker::z, 1., referenceIn.charge());
  CLHEP::HepVector sixVec(6,0);
  for(int i=0; i<3; i++) {sixVec[2.*i]   = pointV[i]; sixVec[2.*i+1] = pointV[i+4];}
  _mass = referenceIn.m();
  _wrapperReferenceOut = referenceIn;
  _wrapperReferenceOut.setSixVector(sixVec, _mass);
  _wrapperReferenceOut.setZ(targetZ);

  Function*         func = new Function(TransferMapCalculator::TransportWrapper, dimension, dimension);
  Differentiator*   diff = new Differentiator(func, delta, magnitude);
  MAUS::PolynomialMap* poly = diff->PolynomialFromDifferentials(MAUS::Vector<double>(referenceIn.getSixVector()));
  TransferMap       map;
  map.SetPolynomialMap(poly);
  map.SetReferenceIn (_wrapperReferenceIn);
  map.SetReferenceOut(_wrapperReferenceOut);
  map.IsCanonical    (false);

  delete func;
  delete diff;
  delete poly;
  return map;
}

TransferMap TransferMapCalculator::GetPolynomialTransferMap(std::vector<PhaseSpaceVector> in, std::vector<PhaseSpaceVector> out, int order)
{
  if(in.size() < 1 || out.size() != in.size()) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Badly conditioned input", "TransferMapCalculator::GetPolynomialTransferMap"));
  std::vector< std::vector<double> > in_v (in.size(), std::vector<double>(6,0.));
  std::vector< std::vector<double> > out_v(in.size(), std::vector<double>(6,0.));
  for(size_t i=0; i<in.size(); i++) 
  {
    HepVector inSixVec  = in [i].getSixVector()-in [0].getSixVector();
    HepVector outSixVec = out[i].getSixVector()-out[0].getSixVector();
    for(unsigned int j=0; j<6; j++)
    {
      in_v [i][j] = inSixVec [j];
      out_v[i][j] = outSixVec[j];
    }
  }
  MAUS::PolynomialMap* poly  = MAUS::PolynomialMap::PolynomialLeastSquaresFit(in_v, out_v, order);
  TransferMap       map;
  map.SetPolynomialMap(poly);
  map.SetReferenceIn (in [0]);
  map.SetReferenceOut(out[0]);
  return map;
}

TransferMap TransferMapCalculator::GetSweepingPolynomialTransferMap(const VectorMap* trackingOutput, int order, double chi2Max, CLHEP::HepVector& delta, CLHEP::HepVector deltaMax, double deltaFactor, 
                                   int maxNumberOfSteps, PhaseSpaceVector refIn, PhaseSpaceVector refOut)
{
  std::vector<double> deltaV(6), deltaMaxV(6);
  deltaV[0]=delta[0]; deltaV[1]=delta[1]; deltaV[2]=delta[2]; deltaV[3]=delta[3]; deltaV[4]=delta[4]; deltaV[5]=delta[5]; 
  deltaMaxV[0]=deltaMax[0]; deltaMaxV[1]=deltaMax[1]; deltaMaxV[2]=deltaMax[2]; deltaMaxV[3]=deltaMax[3]; deltaMaxV[4]=deltaMax[4]; deltaMaxV[5]=deltaMax[5]; 
  MAUS::PolynomialMap* pvec = MAUS::PolynomialMap::Chi2SweepingLeastSquaresFitVariableWalls(
    *trackingOutput, order, std::vector< MAUS::PolynomialMap::PolynomialCoefficient >(), 
                               chi2Max, deltaV, deltaFactor, maxNumberOfSteps, deltaMaxV);
  if(!pvec) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Failed to make any polynomial fit at all - try tweaking control parameters", "TransferMapCalculator::GetSweepingPolynomialTransferMap"));
  for(size_t i=0; i<deltaV.size(); i++) delta[i] = deltaV[i];
  TransferMap       map;
  map.SetPolynomialMap(pvec);
  map.SetReferenceIn (refIn );
  map.SetReferenceOut(refOut);
  return map;

}

TransferMap TransferMapCalculator::GetTransferMap(PhaseSpaceVector referenceIn, double targetZ, int order)
{
	TransferMap::SetOrder(order);
	if(_referenceTrajectory.size()==0)
		SetReferenceTrajectory(referenceIn, targetZ);
	else if(!referenceIn.isEqual(_referenceTrajectory[0]) || targetZ > _referenceTrajectory.back().z()) 
		SetReferenceTrajectory(referenceIn, targetZ);
	double z = referenceIn.z();
	if(order == 1)
	{
		HepMatrix transverseMatrix   = GetTransverseMatrix(z, targetZ);
		HepMatrix longitudinalMatrix = GetLongitudinalMatrix(z, targetZ);
		return TransferMap(transverseMatrix, longitudinalMatrix, referenceIn, ReferenceParticle(targetZ), &_optics);
	}
	else
		return GetSecondOrderMap(referenceIn, targetZ);
}


HepMatrix TransferMapCalculator::GetLongitudinalMatrix(double zIn, double targetZ)
{
	const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
	gsl_odeiv_step    * step    = gsl_odeiv_step_alloc(T,4);
	gsl_odeiv_control * control = gsl_odeiv_control_y_new(_absoluteError, _relativeError);	
	gsl_odeiv_evolve  * evolve  = gsl_odeiv_evolve_alloc(4);
	gsl_odeiv_system    system  = {LongFunc, NULL, 4, NULL};
	double z = zIn, z1 = targetZ;
	double h = 0.1;
	double y[4];
	HepMatrixToArray(HepMatrix(2,2,1), y);
	while(z<z1)
	{
		int status =  gsl_odeiv_evolve_apply(evolve, control, step, &system, &z, z1, &h, y);
		if(status != GSL_SUCCESS)
		{
			std::cerr << "Integration of transfer map failed" << std::endl;
			break;
		}
	}
	gsl_odeiv_evolve_free (evolve);
	gsl_odeiv_control_free(control);
	gsl_odeiv_step_free   (step);
	return ArrayToHepMatrix(y,2,2);
}

HepMatrix TransferMapCalculator::GetTransverseMatrix(double zIn, double targetZ)
{
	const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
	gsl_odeiv_step    * step    = gsl_odeiv_step_alloc(T,16);
	gsl_odeiv_control * control = gsl_odeiv_control_y_new(_absoluteError, _relativeError);	
	gsl_odeiv_evolve  * evolve  = gsl_odeiv_evolve_alloc(16);
	gsl_odeiv_system    system  = {TransFunc, NULL, 16, NULL};
	double z = zIn, z1 = targetZ;
	double h = 0.1;
	double y[16];
	HepMatrixToArray(HepMatrix(4,4,1), y);
	while(z<z1)
	{
		int status =  gsl_odeiv_evolve_apply(evolve, control, step, &system, &z, z1, &h, y);
		if(status != GSL_SUCCESS)
		{
			std::cerr << "Integration of transfer map failed" << std::endl;
			break;
		}
	}
	gsl_odeiv_evolve_free (evolve);
	gsl_odeiv_control_free(control);
	gsl_odeiv_step_free   (step);
	return ArrayToHepMatrix(y,4,4);
}

TransferMap TransferMapCalculator::GetSecondOrderMap(PhaseSpaceVector refParticle, double targetZ)
{
	const gsl_odeiv_step_type *T = gsl_odeiv_step_rk4;
	gsl_odeiv_step    * step     = gsl_odeiv_step_alloc(T,6*36+36);
	gsl_odeiv_control * control  = gsl_odeiv_control_y_new(_absoluteError, _relativeError);	
	gsl_odeiv_evolve  * evolve   = gsl_odeiv_evolve_alloc(6*36+36);
	gsl_odeiv_system    system   = {Func2, NULL, 6*36+36, NULL};
	double z = refParticle.z(), z1 = targetZ;
	double h = 0.1;
	double y[6*36+36];
	HepMatrixToArray(HepMatrix(6,6,1), y);
	TensorToArray(Tensor3(6,6,6,0), y+36);
	while(z<z1)
	{
		int status =  gsl_odeiv_evolve_apply(evolve, control, step, &system, &z, z1, &h, y);
		if(status != GSL_SUCCESS)
		{
			std::cerr << "Integration of transfer map failed" << std::endl;
			break;
		}
	}
	gsl_odeiv_evolve_free (evolve);
	gsl_odeiv_control_free(control);
	gsl_odeiv_step_free   (step);
	TransferMap myMap(ArrayToHepMatrix(y,6,6), refParticle, ReferenceParticle(targetZ), &_optics);
	vector<int> size(3);
	size[0] = size[1] = size[2] = 6;
	myMap.SetSecondOrderMap(ArrayToTensor3(y+36, size));
	return myMap;
}

int TransferMapCalculator::LongFunc(double z, const double y[], double f[], void *params)
{
	HepMatrix dMdz = GetLongMatrix(ReferenceParticle(z))*ArrayToHepMatrix(y,2,2);
	HepMatrixToArray(dMdz, f);
	return GSL_SUCCESS;
}

int TransferMapCalculator::TransFunc(double z, const double y[], double f[], void *params)
{
	HepMatrix transMatrix;
	transMatrix    = GetTransMatrix(ReferenceParticle(z));
	HepMatrix dMdz = transMatrix*ArrayToHepMatrix(y,4,4);
	HepMatrixToArray(dMdz, f);

	return GSL_SUCCESS;
}

int TransferMapCalculator::FullFunc(double z, const double y[], double f[], void *params)
{
  BTField* _field    = NULL;
  double   _delta[6] = {1,1,1,1,1,1};
  double   _q        = 1;
  //pointless definitions to keep me sane
  for(int i=0; i<72; i++) f[i] = 0;
  const double*  x       = y;
  const double*  m[8]    = {y+8, y+16, y+24, y+32, y+40, y+48, y+56, y+64};
  double*        dxdz    = f;
  double*        dmdz[8] = {f+8, f+16, f+24, f+32, f+40, f+48, f+56, f+64};

  //fields and derivatives
  if(fabs(x[7]) < 1e-9) { return GSL_ERANGE;}
  double xfield[4] = {x[1], x[2], x[3], x[0]};
  double field    [6]    = {0.,0.,0.,0.,0.,0.};
  _field->GetFieldValue(xfield, field);

  for(int i=0; i<4; i++)
  {
    double der_pos[6]   = {0,0,0,0,0,0};
    double der_neg[6]   = {0,0,0,0,0,0};
    double pos_field[4] = {0.,0.,0.,0.};
    pos_field[i]        =  _delta[i];
    _field->GetFieldValue(pos_field, der_pos);
    pos_field[i]        = -_delta[i];
    _field->GetFieldValue(pos_field, der_neg);
  }

  //constants
  double q_c    = _q;
  double c_l    = CLHEP::c_light;
  double dtdz   = x[4]/x[7]; //NOTE to self:- why not use x[4]/fabs(x[7]) to simplify things?
  double dir    = fabs(x[7])/x[7]; //direction of motion

  //dx/dz
  //dx/dz = px/pz
  dxdz[0] = dtdz/c_l;
  dxdz[1] = x[5]/x[7];
  dxdz[2] = x[6]/x[7];
  dxdz[3] = 1.;
  dxdz[4] = (q_c*field[3]+q_c*field[4]+q_c*field[5])*dir; //QUERY is this correct? looks grossly wrong to me...
  dxdz[5] = q_c*c_l*( dxdz[2]*field[2] - dxdz[3]*field[1] ) + q_c*field[3]*dtdz*dir; //dpx/dz = qc/pz py*bz
  dxdz[6] = q_c*c_l*( dxdz[3]*field[0] - dxdz[1]*field[2] ) + q_c*field[4]*dtdz*dir;
  dxdz[7] = q_c*c_l*( dxdz[1]*field[1] - dxdz[2]*field[0] ) + q_c*field[5]*dtdz*dir;

  //d/du_j du_i/dz
  dmdz[0][4] = 1./x[7]/c_l*m[0][4];
  dmdz[1][5] = 1./x[7]*m[1][5];
  dmdz[2][6] = 1./x[7]*m[2][6];
  dmdz[3][7] = 1./x[7]*m[3][7];

  return GSL_SUCCESS;
}

double TransferMapCalculator::GetLarmorAngle(PhaseSpaceVector refIn, double zOut)
{
	if(_referenceTrajectory.size()==0)
		SetReferenceTrajectory(refIn, zOut);
	else if(!refIn.isEqual(_referenceTrajectory[0]) || zOut > _referenceTrajectory.back().z()) 
		SetReferenceTrajectory(refIn, zOut);
	const gsl_odeiv_step_type *T = gsl_odeiv_step_rk4;
	gsl_odeiv_step    * step     = gsl_odeiv_step_alloc(T,1);
	gsl_odeiv_control * control  = gsl_odeiv_control_y_new(_absoluteError, _relativeError);	
	gsl_odeiv_evolve  * evolve   = gsl_odeiv_evolve_alloc(1);
	gsl_odeiv_system    system   = {LarmorFunc, NULL, 1, NULL};
	double z    = refIn.z();
	double z1   = zOut;
	double h    = 0.1;
	double y[1] = {0};
	while(z<z1)
	{
		int status =  gsl_odeiv_evolve_apply(evolve, control, step, &system, &z, z1, &h, y);
		if(status != GSL_SUCCESS)
		{
			std::cerr << "Integration of larmor angle failed" << std::endl;
			break;
		}
	}
	gsl_odeiv_evolve_free (evolve);
	gsl_odeiv_control_free(control);
	gsl_odeiv_step_free   (step);
	return y[0];
}

int TransferMapCalculator::LarmorFunc(double z, const double y[], double f[], void *params)
{
	//dy/dt = -qBz/(2p)
	f[0] = -_optics.GetB0(ReferenceParticle(z))/(2*ReferenceParticle(z).P());
	return GSL_SUCCESS;
}

int TransferMapCalculator::Func2(double z, const double y[], double f[], void *params)
{
	CLHEP::HepMatrix Mz         =  ArrayToHepMatrix(y, 6, 6);
	CLHEP::HepMatrix dMdz       =  GetFullMatrix( ReferenceParticle(z) ) * ArrayToHepMatrix(y,6,6);
	Tensor3 dgdz                = -GetK3Int(ReferenceParticle(z), Mz);
	HepMatrixToArray(dMdz, f);
	TensorToArray(dgdz, f+36);
	vector<int> size(3);
	size[0] = size[1] = size[2] = 6;
	return GSL_SUCCESS;
}

HepMatrix TransferMapCalculator::GetLongMatrix(PhaseSpaceVector RefParticle)
{
	HepMatrix longmap(2,2,0);
	double p   = RefParticle.P();
	double K_c = _optics.GetKrf_l(RefParticle); 
	double bg  = RefParticle.P()/RefParticle.m();
	longmap[0][1] = -1/(CLHEP::c_light*bg*bg*p);
	longmap[1][0] =  K_c;
	return longmap;
}

HepMatrix TransferMapCalculator::GetTransMatrix(PhaseSpaceVector RefParticle)
{
	HepMatrix transmap(4,4,0);
	double p   = RefParticle.P();
	double B_0 = _optics.GetB0(RefParticle);
	double B_c = B_0; //multiplier for extra terms when dealing with canonical momenta
	double K_c = _optics.GetKrf_t(RefParticle);
	double K_q = _optics.GetKquad(RefParticle);
	transmap[0][1] =   1/p;
	transmap[0][2] =   B_c/(2*p);
	transmap[1][0] =  -(K_c-K_q+B_c*B_c/(4*p));
	transmap[1][3] =   B_0/p-B_c/(2*p);
	transmap[2][0] =  -B_c/(2*p); 
	transmap[2][3] =   1/p;
	transmap[3][1] =  -B_0/p + B_c/(2*p);
	transmap[3][2] =  -(K_c+K_q+B_c*B_c/(4*p));
	return transmap;
}

HepMatrix TransferMapCalculator::GetFullMatrix(PhaseSpaceVector RefParticle)
{
	HepMatrix full(6,6,0);
	HepMatrix transM = GetTransMatrix(RefParticle);
	HepMatrix longM  = GetLongMatrix(RefParticle);
	for(int i=0; i<2; i++)
		for(int j=0; j<2; j++)
			full[i][j] = longM[i][j];
	for(int i=2; i<6; i++)
		for(int j=2; j<6; j++)
			full[i][j] = transM[i-2][j-2];
	return full;
}

Tensor3 TransferMapCalculator::GetK3Int(PhaseSpaceVector RefParticle, CLHEP::HepMatrix M)
{
	Tensor3   K3 = GetK3(RefParticle);//(6,6,6,0);
	return ConvertK3ToInt(K3, M);
}

Tensor3 TransferMapCalculator::GetK3(PhaseSpaceVector RefParticle)
{
	Tensor3   K3(6,6,6,0);
	HepMatrix K2(6,6,0);
	double    P   = RefParticle.P();
	double    bg  = RefParticle.P()/RefParticle.m();
	double    B0  = _optics.GetB0(RefParticle);
	K2[1][1]      =  1/(2*bg*bg*P);
	K2[3][3]      =  1/(2*P);
	K2[5][5]      =  1/(2*P);
	K2[2][5]      = -B0/(2*P);
	K2[3][4]      =  B0/(2*P);
	K2[2][2]      =  B0*B0/(8*P);
	K2[4][4]      =  B0*B0/(8*P);
	K3[1]         =  K2*RefParticle.E()/(P*P);
	return K3;
}

Tensor3 TransferMapCalculator::ConvertK3ToInt(Tensor3 K3, CLHEP::HepMatrix M)
{
	int dimension = 6;
	Tensor3 K3Int(6,6,6,0);
	//Horrible nested loops
	//K^{int}_{abc} = K_{ijk} M_{ia} M_{jb} M_{kc}
	for(int a=0; a<dimension; a++) for(int b=a; b<dimension; b++) for(int c=b; c<dimension; c++)
		for(int i=0; i<dimension; i++) for(int j=i; j<dimension; j++) for(int k=j; k<dimension; k++)
			K3Int[a][b][c] += K3[i][j][k] * M[i][a] * M[j][b] * M[k][c]; 
	return K3Int;
}

void TransferMapCalculator::SetReferenceTrajectory(PhaseSpaceVector RefParticle, double targetZ)
{
	_referenceTrajectory.clear();
	_referenceTrajectory = vector<PhaseSpaceVector>();
	_referenceTrajectory.push_back(RefParticle);
	double z=RefParticle.z();
	while(z < targetZ)
	{
		PhaseSpaceVector newParticle = _referenceTrajectory[_referenceTrajectory.size()-1];
		z        += _dzRef;
	        newParticle = SlowTransport(newParticle, z);
		if(newParticle.Pz()!=newParticle.Pz()) 
		{	
			std::cerr << "Optics error - reference particle going backwards!\n" 
			          << "RefPart: " << RefParticle << " zOut: " << targetZ << std::endl;
			exit(1);
		}
		_optics.SetFields(newParticle);
		_referenceTrajectory.push_back(newParticle);
	}
}

HepMatrix TransferMapCalculator::GetSymplecticMatrix()
{
	CLHEP::HepMatrix J(6,6,0);
	J[0][1] =  1;
	J[2][3] =  1;
	J[4][5] =  1;
	J[1][0] = -1;
	J[3][2] = -1;
	J[5][4] = -1;
	return J;
}

void TransferMapCalculator::HepMatrixToArray(const HepMatrix in, double out[])
{
	int nrow = in.num_row(), ncol=in.num_col();
	for(int i=0; i<nrow; i++)
		for(int j=0; j<ncol; j++)
			out[i*ncol+j] = in[i][j];

}

HepMatrix TransferMapCalculator::ArrayToHepMatrix(const double in[], const int nrow, const int ncol)
{
	HepMatrix out(nrow,ncol,0);
	for(int i=0; i<nrow; i++)
		for(int j=0; j<ncol; j++)
			out[i][j] = in[i*ncol+j];
	return out;

}

void TransferMapCalculator::TensorToArray(Tensor3 in, double out[])
{
	vector<int> size = in.Size();
	for(int i=0; i<size[0]; i++)
		for(int j=0; j<size[1]; j++)
			for(int k=0; k<size[2]; k++)
				out[i*size[0]*size[1]+j*size[0]+k] = in[i][j][k]; //CHECK
}

Tensor3 TransferMapCalculator::ArrayToTensor3(const double in[], vector<int> size)
{
	Tensor3 out(size[0], size[1], size[2], 0);
	for(int i=0; i<size[0]; i++)
		for(int j=0; j<size[1]; j++)
			for(int k=0; k<size[2]; k++)
				out[i][j][k] = in[i*size[0]*size[1]+j*size[0]+k];			
	return out;
}

const PhaseSpaceVector& TransferMapCalculator::ReferenceParticle(double z)
{
	double zMin = _referenceTrajectory[0].z();
	int    zRef = (int)((z-zMin)/_dzRef);
	return _referenceTrajectory[zRef];
}

void TransferMapCalculator::PrintReferenceTrajectory(std::ostream & out)
{
	for(unsigned int i=0; i<_referenceTrajectory.size(); i++)
	{
		_referenceTrajectory[i].Print(out);
		out << "\n";
	}
	
}

PhaseSpaceVector TransferMapCalculator::SlowTransport(PhaseSpaceVector psIn, double zOut)
{
	
	const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
	gsl_odeiv_step    * step    = gsl_odeiv_step_alloc(T,8);
	gsl_odeiv_control * control = gsl_odeiv_control_y_new(_absoluteError, _relativeError);	
	gsl_odeiv_evolve  * evolve  = gsl_odeiv_evolve_alloc(8);
	gsl_odeiv_system    system  = {FuncEqM, NULL, 8, NULL};
	double z = psIn.z(), z1 = zOut;
	double h = 0.1;
	double y[8];
	bool   fail=false;
	PSVectorToArray(psIn, y);	
	int    nsteps=0;
	while(z<z1)
	{
		nsteps++;
		int status =  gsl_odeiv_evolve_apply(evolve, control, step, &system, &z, z1, &h, y);
		if(status != GSL_SUCCESS)
		{
			std::cerr << "Tracking failed" << std::endl;
			fail = true;
			break;
		}
		if(nsteps > _maxNSteps)
		{
			std::cerr << "\nExceeded max number of steps" << std::endl;
			std::cerr << "Killing tracking at z = " << z << "\n" 
			          << "pos: " << y[0] << " " << y[2] << " " << y[4] << " " << y[6] << "\n"
			          << "mom: " << y[1] << " " << y[3] << " " << y[5] << " " << y[7] << std::endl; 
			fail = true;
			break;
		}
	}
	gsl_odeiv_evolve_free (evolve);
	gsl_odeiv_control_free(control);
	gsl_odeiv_step_free   (step);
	PhaseSpaceVector psOut;
	if(!fail)
		ArrayToPSVector(psOut, y);
	else psOut.setPz(-1);
	return psOut;
}

PhaseSpaceVector TransferMapCalculator::SlowTransportBack(PhaseSpaceVector psIn, double zOut)
{	
	const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
	gsl_odeiv_step    * step    = gsl_odeiv_step_alloc(T,8);
	gsl_odeiv_control * control = gsl_odeiv_control_y_new(_absoluteError, _relativeError);	
	gsl_odeiv_evolve  * evolve  = gsl_odeiv_evolve_alloc(8);
	gsl_odeiv_system    system  = {FuncEqMBk, NULL, 8, NULL};
	double z1 = psIn.z(), z = zOut; //zOut < zIn
	double h = 0.1;
	double y[8];
	int    nsteps=0;
	bool   fail = false;
	PSVectorToArray(psIn, y);	
	while(z<z1)
	{
		nsteps++;
		int status =  gsl_odeiv_evolve_apply(evolve, control, step, &system, &z, z1, &h, y);
		if(status != GSL_SUCCESS)
		{
			std::cerr << "Tracking failed" << std::endl;
			fail = true;
			break;
		}
		if(nsteps > _maxNSteps)
		{
			std::cerr << "\nExceeded max number of steps" << std::endl;
			std::cerr << "Killing tracking at z = " << z << "\n" 
			          << "pos: " << y[0] << " " << y[2] << " " << y[4] << " " << y[6] << "\n"
			          << "mom: " << y[1] << " " << y[3] << " " << y[5] << " " << y[7] << std::endl; 
			fail = true;
			break;
		}
	}
	gsl_odeiv_evolve_free (evolve);
	gsl_odeiv_control_free(control);
	gsl_odeiv_step_free   (step);
	PhaseSpaceVector psOut;
	if(!fail)
		ArrayToPSVector(psOut, y);
	else psOut.setPz(-1);
	return psOut;
}

int TransferMapCalculator::FuncEqM(double z, const double y[], double f[], void *params)
{
	CLHEP::HepLorentzVector position(y[2],y[4],y[6],y[0]);
	CLHEP::Hep3Vector       bfield = _optics.GetBField(position);
	CLHEP::Hep3Vector       efield = _optics.GetEField(position);
	double q_c    = CLHEP::eplus;
	double c_l    = CLHEP::c_light;
        double beta_z = q_c * y[7] / y[1];
	//dx/dz = p_x/p_z
	f[0] = y[1]/y[7]/c_l;
	f[2] = y[3]/y[7];
	f[4] = y[5]/y[7];
	f[6] = 1;
	//E = const (assume no E-fields)	
	f[3] = q_c*c_l *( y[5]/y[7]*bfield[2] - y[7]/y[7]*bfield[1]) + beta_z*efield[0]; //dpx/dz = qc/pz py*bz
	f[5] = q_c*c_l *( y[7]/y[7]*bfield[0] - y[3]/y[7]*bfield[2]) + beta_z*efield[1];
	f[7] = q_c*c_l *( y[3]/y[7]*bfield[1] - y[5]/y[7]*bfield[0]) + beta_z*efield[2];
        f[1] = beta_z  *( efield[0]*y[3]+efield[1]*y[5]+efield[2]*y[7] )/y[1];

	if(!y[7]>0)
	{
		std::cerr << "\nBad pz\nKilling tracking at z = " << z << "\n" 
		          << "pos: " << y[0] << " " << y[2] << " " << y[4] << " " << y[6] << "\n"
		          << "mom: " << y[1] << " " << y[3] << " " << y[5] << " " << y[7] << std::endl; 
		return 1;
	}
	return GSL_SUCCESS;
}

int TransferMapCalculator::FuncEqMBk(double z, const double y[], double f[], void *params)
{
	CLHEP::HepLorentzVector position(y[2],y[4],y[6],y[0]);
	CLHEP::Hep3Vector       bfield = _optics.GetBField(position);
	double q_c = CLHEP::eplus;
	double c_l = CLHEP::c_light;
	//dx/dz = p_x/p_z
	f[0] = -y[1]/y[7]/c_l;
	f[2] = -y[3]/y[7];
	f[4] = -y[5]/y[7];
	f[6] = -1;
	//E = const (assume no E-fields)	
	f[1] =  0;
	f[3] = -q_c*c_l *( y[5]/y[7]*bfield[2] - y[7]/y[7]*bfield[1]); //dpx/dz = qc/E py*bz
	f[5] = -q_c*c_l *( y[7]/y[7]*bfield[0] - y[3]/y[7]*bfield[2]);
	f[7] = -q_c*c_l *( y[3]/y[7]*bfield[1] - y[5]/y[7]*bfield[0]);
	if(!y[7]>0)
	{
		std::cerr << "\nBad pz\nKilling tracking at z = " << z << "\n" 
		          << "pos: " << y[0] << " " << y[2] << " " << y[4] << " " << y[6] << "\n"
		          << "mom: " << y[1] << " " << y[3] << " " << y[5] << " " << y[7] << std::endl; 
		return 1;
	}
	return GSL_SUCCESS;
}

void TransferMapCalculator::PSVectorToArray(PhaseSpaceVector in, double out[])
{
	out[0] = in.t();
	out[1] = in.E();
	out[2] = in.x();
	out[3] = in.Px();
	out[4] = in.y();
	out[5] = in.Py();
	out[6] = in.z();
	out[7] = in.Pz();
}

void TransferMapCalculator::ArrayToPSVector(PhaseSpaceVector& out, double in[])
{
	out.setT(in[0]);
	out.setE(in[1]);
	out.setX(in[2]);
	out.setPx(in[3]);
	out.setY(in[4]);
	out.setPy(in[5]);
	out.setZ(in[6]);
	out.setPz(in[7]);

}



