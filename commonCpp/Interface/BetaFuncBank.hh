// CTR - 05/02/05

#ifndef BetaFuncBank_h
#define BetaFuncBank_h

#include <fstream>
#include <vector>
#include <stdio.h>

class BetaFuncBank
{
public:

  BetaFuncBank();
	BetaFuncBank(const BetaFuncBank& rhs);
	~BetaFuncBank();

	const BetaFuncBank& operator= (const BetaFuncBank& rhs);


  void Read(std::ifstream&);
  void Write(std::ofstream&);

	//z position, axial field Bz, optical beta, optical alpha, axial momentum pz,
	//emittance, focussing function kappa
	double *z, *bz, *beta, *alpha, *pz, *emittance, *kappa;
	//z grid parameters
	double dz, zStart,zEnd;
	int nSteps;
	//quadrupole parameters (nil for now)
	std::vector<double> quadBAtPoleTip, quadZ, quadRadiusAtPoleTip, quadEffectiveLength;

private:
	//new the pointers to arrays of size nSteps
	void NewArrays();
	//Delete the arrays
	void DeleteArrays();

};

#endif
