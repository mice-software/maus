#include <vector>
#include <fstream>
#include <iostream>
#include "Interface/BetaFuncBank.hh"

using namespace std;

BetaFuncBank::BetaFuncBank()
{
	dz=0;
	zStart=0;
	zEnd=0;
	z = NULL;
	bz = NULL;
	beta = NULL;
	alpha = NULL;
	pz = NULL;
	emittance = NULL;
	kappa = NULL;
}

BetaFuncBank::BetaFuncBank(const BetaFuncBank& rhs)
{
	dz=0;
	zStart=0;
	zEnd=0;
	z = NULL;
	bz = NULL;
	beta = NULL;
	alpha = NULL;
	pz = NULL;
	emittance = NULL;
	kappa = NULL;

	*this = rhs;
}

BetaFuncBank::~BetaFuncBank()
{
	DeleteArrays();
}


const BetaFuncBank& BetaFuncBank::operator= (const BetaFuncBank& rhs)
{
	if (this==&rhs) return *this;

	dz = rhs.dz;
	zStart = rhs.zStart;
	zEnd = rhs.zEnd;
	nSteps = rhs.nSteps;
	quadBAtPoleTip = rhs.quadBAtPoleTip;
	quadZ = rhs.quadZ;
	quadRadiusAtPoleTip = rhs.quadRadiusAtPoleTip;
	quadEffectiveLength = rhs.quadEffectiveLength;

	DeleteArrays();
	NewArrays();

	for(int i=0; i<nSteps; i++)
	{
		z[i] = rhs.z[i];
		bz[i] = rhs.bz[i];
		beta[i] = rhs.beta[i];
		alpha[i] = rhs.alpha[i];
		pz[i] = rhs.pz[i];
		emittance[i] = rhs.emittance[i];
		kappa[i] = rhs.kappa[i];
	}

	return *this;
}

void BetaFuncBank::Read(std::ifstream& fin)
{
	string dummy;
	getline(fin, dummy);
	std::vector<double> inputVariables;
	double aSingleInput;

	while(fin)
	{
		for(int i=0; i<9; i++)
		{
			fin >> aSingleInput;
			inputVariables.push_back(aSingleInput);
		}
	}

	nSteps = ((int)inputVariables.size()/9)-1;
	if(z!=NULL)
		DeleteArrays();
	NewArrays();

	for(int i=0; i<nSteps; i++)
	{
		z[i] = inputVariables[i*9];
		bz[i] = inputVariables[i*9+1];
		beta[i] = inputVariables[i*9+2];
		alpha[i] = inputVariables[i*9+3];
		pz[i] = inputVariables[i*9+4];
		emittance[i] = inputVariables[i*9+5];
		kappa[i] = inputVariables[i*9+6];
	}

	zStart = z[0];
	zEnd = z[nSteps-1];
	dz = (zStart - zEnd)/(nSteps - 1);

	//nothing for quads yet
}

void BetaFuncBank::Write(std::ofstream& fout)
{
	fout << "z     bz    beta   alpha   pz   em   kappa" << endl;
	for(int i=0; i<nSteps; i++)
		fout << z[i] << "  " << bz[i] << "  " << beta[i] << "  " << alpha[i]
		     << "  " << pz[i] << "  " << emittance[i] << "  " << kappa[i]
		     << "  0  " << kappa[i] << endl;
}

void BetaFuncBank::NewArrays()
{
	z = new double[nSteps];
	bz = new double[nSteps];
	beta = new double[nSteps];
	alpha = new double[nSteps];
	pz = new double[nSteps];
	emittance = new double[nSteps];
	kappa = new double[nSteps];

	for(int i=0; i<nSteps; i++)
	{
		z[i] = bz[i] = beta[i] = alpha[i] = 0;
		pz[i] = emittance[i] = kappa[i] = 0;
	}
}

void BetaFuncBank::DeleteArrays()
{
	if(z!=NULL)
		delete [] z;
	if(bz!=NULL)
		delete [] bz;
	if(beta!=NULL)
		delete [] beta;
	if(alpha!=NULL)
		delete [] alpha;
	if(pz!=NULL)
		delete [] pz;
	if(emittance!=NULL)
		delete [] emittance;
	if(kappa!=NULL)
		delete [] kappa;

	z=0;
	bz=0;
	beta=0;
	alpha=0;
	pz=0;
	emittance=0;
	kappa=0;
}

