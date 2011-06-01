#include "OpticsModel.hh"
const double OpticsModel::_fieldTolerance=1e-9;


double OpticsModel::GetKquad(PhaseSpaceVector referenceParticle) const
{
	// k = 1e3 sqrt(q[e] dB/dx[T/m] / pz [eV/c]
	// minimum quad aperture 0.1 mm!
	double x        = 0.1;
	double B[6]     = {0,0,0,0,0,0};
	double point[4] = {x,0,referenceParticle.z(),0};
	_theField->GetFieldValue(point, B);

	double dBdx     = B[1]*CLHEP::tesla/point[0]*CLHEP::meter; //dBy/dx
	double p        = referenceParticle.P()/CLHEP::electronvolt;

	double k = 1e3 * sqrt( fabs(CLHEP::eplus * dBdx / p) );
	return k;
}

double OpticsModel::GetKrf_t(PhaseSpaceVector referenceParticle) const
{
	SetFields(referenceParticle);
	double Ez    = referenceParticle.Ez();
	double omega = 2*CLHEP::pi*_data->opticsRfFrequency();
	double phi0  = _data->rfAccelerationPhase();
	double k     = 0.003405; //mm^-1
	return Ez*sin(phi0)*k*k/(4*omega);//Natural unit for omega is GHz
}

double OpticsModel::GetKrf_l(PhaseSpaceVector referenceParticle) const
{
	SetFields(referenceParticle);
	double Ez    = referenceParticle.Ez();
	double omega = 2*CLHEP::pi*_data->opticsRfFrequency();
	double phi0  = _data->rfAccelerationPhase();
	return omega*Ez*sin(phi0);//Natural unit is GHz 
}

double OpticsModel::GetBz(PhaseSpaceVector referenceParticle) const
{
	double point[4]  = {0,0,referenceParticle.z(),0};
	double bfield[6] = {0,0,0,0,0,0};
	_theField->GetFieldValue(point, bfield);
	return bfield[2];
}

double OpticsModel::GetB0(PhaseSpaceVector referenceParticle) const
{
//compare with solenoid kappa = (0.15*1e3*Bz/p)^2 = (eplus*Bz/2*p)^2
	double B0        = CLHEP::c_light*GetBz(referenceParticle)*CLHEP::eplus;
	return B0;
}

double OpticsModel::GetdEdz(PhaseSpaceVector referenceParticle) const
{
	SetFields(referenceParticle);
	double RFEGain = referenceParticle.Ez()*CLHEP::eplus;
	return RFEGain;  
}

void OpticsModel::SetFields(PhaseSpaceVector& referenceParticle) const
{
	double point[4] = {referenceParticle.x(), referenceParticle.y(), referenceParticle.z(), referenceParticle.t()};
	double field[6] = {0,0,0,0,0,0};
	_theField->GetFieldValue(point, field);
	Hep3Vector b(field[0], field[1], field[2]);
	Hep3Vector e(field[3], field[4], field[5]);
	HepLorentzVector A(_theField->GetVectorPotential(referenceParticle.fourPosition()));
	referenceParticle.setB(b);
	referenceParticle.setE(e);
	referenceParticle.setFourPotential(A);
}

CLHEP::Hep3Vector OpticsModel::GetBField(CLHEP::HepLorentzVector position) const
{
	double pos[4]   = {position.x(), position.y(), position.z(), position.t()};
	double field[6] = {0,0,0,0,0,0};
	_theField->GetFieldValue(pos, field);
	return Hep3Vector(field[0], field[1], field[2]);
}


CLHEP::Hep3Vector OpticsModel::GetEField(CLHEP::HepLorentzVector position) const
{
	double pos[4]   = {position.x(), position.y(), position.z(), position.t()};
	double field[6] = {0,0,0,0,0,0};
	_theField->GetFieldValue(pos, field);
	return Hep3Vector(field[3], field[4], field[5]);
}

double* OpticsModel::NewFourierDecomposition(int power, double zStart, double zEnd) const
{
	int    nPoints = 1;
	for(int i=0; i<power; i++) nPoints *= 2;

	double* data    = new double[nPoints*2]; //complex data allowed
	double* zPos    = new double[nPoints];
	double* ft      = new double[nPoints];

	for(int i=0; i<nPoints; i++)
	{
		zPos[i]     = zStart + (double)i*(zEnd - zStart)/(double)nPoints;
		data[i*2]   = GetBField(HepLorentzVector(0,0,zPos[i],0)).z();
		data[i*2+1] = 0;
	}
	gsl_fft_complex_radix2_forward (data, 1, nPoints);
	for(int i=0; i<nPoints; i++)
		ft[i] = data[i*2+0];

	delete [] data;
	return ft;
}

std::vector<double*> OpticsModel::MomentumOfStopBands(int power, double zStart, double zEnd, int nMax) const
{
	std::vector<double*> theStopBands;

	double* FourierTransform = NewFourierDecomposition(power, zStart, zEnd);
	double  MeanSquareBz     = GetMeanSquareBz(power, zStart, zEnd);
	double  length           = zEnd - zStart;

	for(int n = 0; n<nMax; n++)
	{
		double * stopBand  = new double[2];
		double thetaFunc   = 0;//1./2.*fabs(FourierTransform[n]/theta0) +
// 		                     5./16.*fabs(FourierTransform[n]/theta0)*fabs(FourierTransform[n]/theta0);
		stopBand[0] = n + thetaFunc;
		stopBand[1] = n - thetaFunc;
		std::cout << n << "  " << FourierTransform[n] << "  " << thetaFunc << "  " 
		          << stopBand[0] << "  " << stopBand[1];
		stopBand[0] = CLHEP::c_light*length*sqrt(MeanSquareBz) / (2.*CLHEP::pi*stopBand[0]);
		stopBand[1] = CLHEP::c_light*length*sqrt(MeanSquareBz) / (2.*CLHEP::pi*stopBand[1]);
		std::cout << "  " << stopBand[0] << "  " << stopBand[1] << std::endl; 
		theStopBands.push_back(stopBand);
	}

	delete [] FourierTransform;
	return theStopBands;
}

double OpticsModel::BetaFromFT(double length, int power, double* FourierTransform, PhaseSpaceVector RefParticle,
                               double trace4d) const
{
	int     nPoints = 1;
	double mu      = acos(trace4d/4.);
	double* ft      = FourierTransform;
	double  theta0  = ft[0];
	double  sum1    = 0;//first order
	double  sum2    = 0;//second order
	double  sum3    = 0;//second order
	if(mu != mu) return -1; //beta_lattice not defined

	for(int i=0; i<power; i++)
		nPoints *= 2;
	for(int i=1; i<nPoints; i++) //Re(exp(ix)) = cos(x)
	{
		sum1 += ft[i]*cos(2*i*CLHEP::pi*RefParticle.z()/length)/(i*i - theta0);
		sum2 += ft[i]*ft[i]/(i*i-theta0);
		for(int j=1; j<nPoints; j++) //note there is a term here that is 0 for even symmetry
			sum3 += (j*j + j*i + i*i - 3.*theta0)*ft[i]*ft[j]*cos(2.*(i+j)*CLHEP::pi*RefParticle.z()/length)/
				(j*j-theta0)/(i*i-theta0)/((i+j)*(i+j)-theta0)/4.+0.;
	}
	sum2 *= CLHEP::pi/4./tan(CLHEP::pi*sqrt(theta0))/sqrt(theta0);


	double beta = length * sin(CLHEP::pi*sqrt(theta0))/(CLHEP::pi*sqrt(theta0)*sin(mu)) * (1+sum1+sum2+sum3);
	return beta;
}


double OpticsModel::GetMeanSquareBz(int power, double zStart, double zEnd) const
{
	double MeanSquareBz = 0;
	int    nPoints      = 1;
	for(int i=0; i<power; i++) nPoints *= 2;
	for(int i=0; i<power; i++) 
	{
		double zPos   = zStart + (double)i*(zEnd - zStart)/(double)nPoints;
		double bz     = GetBField(HepLorentzVector(0,0,zPos,0)).z();
		MeanSquareBz += bz*bz;
	}
	return MeanSquareBz;
}




