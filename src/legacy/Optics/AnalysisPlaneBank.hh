// CTR - 05/02/05

#ifndef AnalysisPlaneBank_h
#define AnalysisPlaneBank_h

#include <stdio.h>
#include <iostream>
#include "CLHEP/Matrix/Matrix.h"
using CLHEP::HepMatrix;

class AnalysisPlaneBank
{
public:

  AnalysisPlaneBank();
	AnalysisPlaneBank(const AnalysisPlaneBank& rhs);
	~AnalysisPlaneBank();

	const AnalysisPlaneBank& operator= (const AnalysisPlaneBank& rhs);


	void Read(std::istream&);
	void Write(std::ostream&)       const;
	void WriteHeader(std::ostream&) const;

	HepMatrix CovarianceMatrix;
	//Plane# Type Weight <z> <t> <pz> <E> betaPerp 2d(t) 2d(x) 2d(y) 4D(xy) 6D(txy)

	int planeNumber, planeType;
	double weight, z, t, pz, E, beta_p, beta[3], l_can, em2d[3], em4dXY, em6dTXY, sums[6], amplitudePzCovariance;


};

std::ostream& operator << (std::ostream& out, const AnalysisPlaneBank& bank );
std::istream& operator >> (std::istream& in,  AnalysisPlaneBank& bank );


#endif
