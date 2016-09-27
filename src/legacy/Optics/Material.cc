//
#include "Material.hh"

#include <cmath>

#include "CLHEP/GenericFunctions/Ln.hh"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Units/PhysicalConstants.h"

double Material::m_mu=105.658369;
double Material::m_e=CLHEP::electron_mass_c2;
double Material::K=0.307075; // divided by A [g mol^-1]

Material::Material(std::string MaterialType, double position) 
        : _thickness(0), _z(position), _setsLengthOnStep(false), _materialType(MaterialType) 
{
	SetMaterial(_materialType);
}

Material::Material(double thickness, std::string MaterialType, double position) 
        : _thickness(thickness), _z(position), _setsLengthOnStep(false), _materialType(MaterialType)
{
	SetMaterial(_materialType);
}

Material::Material(double x0_mm, double dEdZ_MeVpermm, std::string materialName, double position)
        : _x0(x0_mm), _dEdZ(dEdZ_MeVpermm), _setsLengthOnStep(false), _materialType(materialName)
{
}

Material::~Material()
{
}

//sigma x p_x ~ 1/pz; sigma p_x p_x ~ 1/pz^2  + pz*pz*dTheta2
CovarianceMatrix Material::Transport(CovarianceMatrix covIn) const
{
	CovarianceMatrix covOut = covIn;
	covOut.SetMean(Transport(covIn.GetMean()));
	CLHEP::HepSymMatrix     covMat = covOut.GetKineticCovariances();
	double           pRatio = covOut.GetMean().P() / covIn.GetMean().P();
	double           p      = covOut.GetMean().P();
	covMat[3][3] = covMat[3][3]*pRatio*pRatio;
	covMat[3][2] = covMat[3][2]*pRatio;
	covMat[5][5] = covMat[5][5]*pRatio*pRatio;
	covMat[5][4] = covMat[5][4]*pRatio*pRatio;
	covMat[5][2] = covMat[5][2]*pRatio;
	covMat[4][3] = covMat[4][3]*pRatio;
	covMat[3][3] = covMat[3][3] + GetdTheta2(p)*p*p;
	covMat[5][5] = covMat[5][5] + GetdTheta2(p)*p*p;
	covMat[1][1] = covMat[1][1]*GetCurvature(p)*GetCurvature(p) + GetSigma2E(p);
	covMat[1][2] = covMat[1][2]*GetCurvature(p);
	covOut.SetCovariances(covMat);
	return covOut;
}

CovarianceMatrix Material::TransportBack(CovarianceMatrix covIn) const
{
	CovarianceMatrix covOut = covIn;
	covOut.SetMean(TransportBack(covIn.GetMean()));
	CLHEP::HepSymMatrix     covMat = covIn.GetKineticCovariances();
	double           pRatio = covOut.GetMean().P() / covIn.GetMean().P();
	double           p      = covIn.GetMean().P();
	covMat[3][3] = covMat[3][3] - GetdTheta2(covIn.GetMean().P())*covIn.GetMean().P()*covIn.GetMean().P();
	covMat[5][5] = covMat[5][5] - GetdTheta2(covIn.GetMean().P())*covIn.GetMean().P()*covIn.GetMean().P();
	covMat[1][1] = (covMat[1][1] - GetSigma2E(p))/(GetCurvature(p)*GetCurvature(p));
	covMat[1][2] = covMat[1][2]/GetCurvature(p);
	covMat[3][3] = covMat[3][3]*pRatio*pRatio;
	covMat[3][2] = covMat[3][2]*pRatio;
	covMat[5][5] = covMat[5][5]*pRatio*pRatio;
	covMat[5][4] = covMat[5][4]*pRatio;
	covMat[5][2] = covMat[5][2]*pRatio;
	covMat[4][3] = covMat[4][3]*pRatio;
	covOut.SetCovariances(covMat);
	if(!covOut.IsPositiveDefinite()) 
	{
		std::stringstream error;
		error << "Error - transport back failed on material at z=" << covOut.GetMean().z() 
		          << " Likely cause is that the material was too thick. Covariance matrix before material\n" 
		          << covIn << "and after\n" << covOut << std::endl;
		throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, error.str(), "Material::TransportBack(CovarianceMatrix) const"));
	}
	return covOut;
}

double Material::GetCurvature(double p) const
{
	double dp        = p/100;
	double E         = sqrt(p*p+m_mu*m_mu);
	double curvature = 1+E/p*(GetdE(p+dp) - GetdE(p-dp))/(2.*dp); 
	return curvature;
}

PhaseSpaceVector Material::Transport(PhaseSpaceVector psIn) const
{
	PhaseSpaceVector psOut   = psIn;
	double E     = psOut.E();
	double P     = psOut.P();
	double m     = psOut.m();
	double Emean = E + GetdE(P)/2.; //roughly E at material midpoint
	E           += GetdE(sqrt(Emean*Emean-m*m));
	if(!E>m) //catches nan
	{	
		psOut.setFourMomentum(m,0,0,0);
		std::cerr << "Error - transport back failed on material at z=" << psIn.z() 
		          << " Likely cause is that the material was too thick. Trajectory before material\n" 
		          << psIn << "\nand after\n" << psOut << std::endl;
		return psOut;
	}
	double newP  = sqrt(E*E-m*m);
	psOut.setPx(psIn.Px()*newP/P);
	psOut.setPy(psIn.Py()*newP/P);
	psOut.setPz(psIn.Pz()*newP/P);
	psOut.setE (E);
	return psOut;
}

PhaseSpaceVector Material::TransportBack(PhaseSpaceVector psIn) const
{
	PhaseSpaceVector psOut   = psIn;
	double E     = psOut.E();
	double P     = psOut.P();
	double m     = psOut.m();
	double Emean = E - GetdE(P)/2.; //roughly E at material midpoint
	E           -= GetdE(sqrt(Emean*Emean-m*m));
	if(!E>m) //catches nan
	{	
		psOut.setFourMomentum(m,0,0,0);
		std::cerr << "Error - transport back failed on material at z=" << psIn.z() 
		          << " Likely cause is that the material was too thick. Trajectory before material\n" 
		          << psIn << "\nand after\n" << psOut << std::endl;
		return psOut;
	}
	double newP = sqrt(E*E-m*m);
	psOut.setPx(psIn.Px()*newP/P);
	psOut.setPy(psIn.Py()*newP/P);
	psOut.setPz(psIn.Pz()*newP/P);
	psOut.setE (E);
	return psOut;
}

void Material::SetMaterial(std::string MaterialType)
{
	if(_materialList.size()==0) SetMaterialList();

	unsigned int i=0;
	while((i!=_materialList.size()-1)&&(_materialList[i]!=MaterialType))
		i++;

	_materialType = _materialList[i];
	_x0      = _x0List[i];
	m_I       = _IList[i];
	_Z       = _ZList[i];
	m_A       = _AList[i];
	_delta   = _deltaList[i];
	_density = _densityList[i];
	_dEdZ    = _dEdZList[i];
}

CLHEP::HepSymMatrix Material::GetCovariances(PhaseSpaceVector track) const
{
	CLHEP::HepSymMatrix cov(6,0);
	
	cov(3,3) = GetdTheta2(track.P());
	cov(5,5) = cov(3,3);
	return cov;
}



//A simple binary method for estimating the length required to get out a given emittance
double Material::GetLengthGivenEmittance(CovarianceMatrix bunch, double emittanceRequired)
{
	CovarianceMatrix tempBunch = bunch;
	double           pz = bunch.GetMean().Pz();
	const bool travellingForward = (emittanceRequired>bunch.GetEmitTrans());
	const double thickness0 = _thickness; //change thickness back at the end
	//use a counter to jump out of an infinite loop
	int counter=0, maxNumberIterations=100;
	//tolerance defines the accuracy required in the estimate as a fraction of emittanceRequired
	double tolerance = 0.001;

	double thickness_min = 0, thickness_max = 0;
	double emittance_upper = 0, emittance_lower = 0;
	//set the thickness initial bounds
	//thickness st the muon dE < E of muon
	//and adjust for direction of travel
	if(travellingForward)
		thickness_max = (pz*GetBetaRel(pz)/_dEdZ);
	else
		thickness_min = -1*(pz*GetBetaRel(pz)/_dEdZ);

	while( counter < maxNumberIterations )
	{
		//if the emittanceRequired sits between the bounds, then decrease the upper bound
		_thickness = thickness_min;
		emittance_lower = (bunch+GetCovariances(bunch.GetMean())).GetEmitTrans();
		if(emittance_lower!=emittance_lower) emittance_lower=0;

		_thickness = thickness_max;
		emittance_upper = (bunch+GetCovariances(bunch.GetMean())).GetEmitTrans();

		if( (emittance_lower>emittanceRequired*(1-tolerance))&&(emittance_lower<emittanceRequired*(1+tolerance)) )
		{
			_thickness = thickness0;//_thickness doesn't change
			return thickness_min;
		}

		if((emittance_lower<emittanceRequired)||(emittance_lower!=emittance_lower))
			thickness_min+=(thickness_max-thickness_min)/2.;
		else if(emittance_lower>emittanceRequired)
		{
			thickness_min-=((thickness_max-thickness_min)/2.);
			thickness_max-=((thickness_max-thickness_min)*2./3.);
		}


		std::cout << _thickness << " " << thickness_min << " " << thickness_max << " " <<
		        emittance_upper << " " << emittance_lower << std::endl;
		counter++;
	}

	_thickness = thickness0;//_thickness doesn't change
	return 0;
}

double Material::GetdTheta2(double pz) const
{
	if(_x0<0) return 0;
	double dz = fabs(_thickness);

	double betaRel = GetBetaRel(pz);//einstein beta, paraxial approx
	double dtheta2 = 13.6*13.6*dz/((pz*betaRel)*(pz*betaRel)*_x0)
	               *(1+0.038*log(dz/_x0))*(1+0.038*log(dz/_x0)); //MUCOOL note 71, PDG
	return dtheta2;
}

void Material::SetMaterialList()
{
	std::string materialList[10] = {"Pb", "POLYSTYRENE", "MYLAR", "Al",    "Be", "FLUOROCARBON", "BC600", "lH2", "LITHIUM_HYDRIDE", "unknown"};
	double x0List      [10] = {    5.64,   413.1,     500,   88.97,   352.8, 200, 400,   8904.4,    971.0,  1e10}; //mm
	double dEdZList    [10] = {  1.2247,  0.2052,     0.2,   1.615, 0.29475, 0.15, 0.24,  0.028,   0.1555,  1e-10}; //MeV/mm
	double ZList       [10] = {      82, 0.53768,     0.7,      13,       4, 0, 0,            1,  0.50321,  1}; //g mol^-1
	double AList       [10] = {   207.2,       1,       1, 26.9815, 9.01218, 0, 0,         1.00,        1,  1}; //g mol^-1
	double IList       [10] = {823.0e-6, 68.7e-6, 78.7e-6,  166e-6, 63.7e-6, 0, 0,      19.2e-6, 18.51e-6,  1}; //MeV
	double deltaList   [10] = {    0.14,       0,       0,    0.12,    0.14, 0, 0,         0.00,        0,  0}; //dimensionless
	double densityList [10] = {  11.350,   1.060,     1.4,   2.699,   1.848, 0, 0,      7.08e-2,    0.820,  1e-10}; //g cm^-3

	//Why bother with this??
	for(int i=0; i<10; i++)
	{
		_materialList.push_back(materialList[i]);
		_x0List.push_back(x0List[i]);
		_dEdZList.push_back(dEdZList[i]);
		_ZList.push_back(ZList[i]);
		_AList.push_back(AList[i]);
		_IList.push_back(IList[i]);
		_deltaList.push_back(deltaList[i]);
		_densityList.push_back(densityList[i]);
	}
	//aerogel is density 0.2 g cm-3 (cf pdg)
	//plastic is polyethylene
	//fluorocarbon values best I could manage

}

double Material::GetdEdZ(double pz) const
{
	return GetdEdZ_BB(pz);
	//if(_dEdXModel == "BetheBloch") return
	//else return GetdEdZ_MI();
}

double Material::GetdEdZ_BB(double pz) const
{
	if(_density < 0) return 0;
	double tMax, lnArg, betaRel=GetBetaRel(pz), gammaRel=GetGammaRel(pz), dEdZ;
	double betaRel2 = betaRel*betaRel, gammaRel2 = gammaRel*gammaRel;
	tMax  = 2*m_e*betaRel2*gammaRel2
	        / (1 + 2*gammaRel*m_e/m_mu + (m_e/m_mu)*(m_e/m_mu));
	lnArg = 2*m_e*betaRel2*gammaRel2*tMax/(m_I*m_I);
	dEdZ = ( K*_Z/(m_A*betaRel2) )
	     * ( log(lnArg)/2 - betaRel2 - _delta/2)*_density/10; //MeV/mm
	return -1.*dEdZ;
}

bool Material::MaterialImplemented(std::string material)
{
	for(unsigned int i=0; i<_materialList.size(); i++)
		if(_materialList[i] == material) return true;
	return false;
}


