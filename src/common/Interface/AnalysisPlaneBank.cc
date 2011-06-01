// MAUS WARNING: THIS IS LEGACY CODE.

#include <fstream>
#include "Interface/AnalysisPlaneBank.hh"

using namespace std;

AnalysisPlaneBank::AnalysisPlaneBank() : CovarianceMatrix(6,6,0)

{
	planeType=0;
	planeNumber=0;
	weight=0;
	z=0;
	t=0;
	pz=0;
	E=0;
	beta_p=0;
	l_can=0;
	amplitudePzCovariance=0;
	for(int i=0; i<3; i++)
	{
		beta[i]=0;
		em2d[i]=0;
		sums[2*i]=0;
		sums[2*i+1]=0;
	}

	em4dXY=0;
	em6dTXY=0;
}


const AnalysisPlaneBank& AnalysisPlaneBank::operator= (const AnalysisPlaneBank& rhs)
{
	planeType=rhs.planeType;
	planeNumber=rhs.planeNumber;
	weight=rhs.weight;
	z=rhs.z;
	t=rhs.t;
	pz=rhs.pz;
	E=rhs.E;
	beta_p=rhs.beta_p;
	l_can=rhs.l_can;
	amplitudePzCovariance=rhs.amplitudePzCovariance;
	for(int i=0; i<3; i++)
	{
		beta[i]=rhs.beta[i];
		em2d[i]=rhs.em2d[i];
		sums[2*i]=rhs.sums[2*i];
		sums[2*i+1]=rhs.sums[2*i+1];
	}
	CovarianceMatrix = rhs.CovarianceMatrix;
	em4dXY=rhs.em4dXY;
	em6dTXY=rhs.em6dTXY;

	return rhs;
}

AnalysisPlaneBank::AnalysisPlaneBank(const AnalysisPlaneBank& rhs)
{
	planeType=rhs.planeType;
	planeNumber=rhs.planeNumber;
	weight=rhs.weight;
	z=rhs.z;
	t=rhs.t;
	pz=rhs.pz;
	E=rhs.E;
	beta_p=rhs.beta_p;
	l_can=rhs.l_can;
	amplitudePzCovariance=rhs.amplitudePzCovariance;
	for(int i=0; i<3; i++)
	{
		beta[i]=rhs.beta[i];
		em2d[i]=rhs.em2d[i];
		sums[2*i]=rhs.sums[2*i];
		sums[2*i+1]=rhs.sums[2*i+1];
	}
	CovarianceMatrix = rhs.CovarianceMatrix;
	em4dXY=rhs.em4dXY;
	em6dTXY=rhs.em6dTXY;

}


AnalysisPlaneBank::~AnalysisPlaneBank() {}

void AnalysisPlaneBank::Write(std::ostream &fout) const
{
	fout << planeType << " " << planeNumber << " " << weight << " " << z << " " << t << " "
	     << pz << " " << E << " ";
	for(int i=0; i<3; i++)
		fout << beta[i] << " ";
	fout << beta_p << " " << " " << amplitudePzCovariance << " " << l_can << " ";
	for(int i=0; i<3; i++)
		fout << em2d[i] << " ";
	fout << em4dXY << " " << em6dTXY << std::endl;
	for(int i=0; i<6; i++)
		fout << sums[i] << " " ;
	fout << CovarianceMatrix;
}

void AnalysisPlaneBank::WriteHeader(std::ostream &fout) const
{
	fout << "planeType  planeNumber  weight  <z>  <t>  <pz>  <E>  beta[0]  beta[1]  beta[2]  beta_p  <SPE*Pz> <l_kin>  "

	     << "em2d_0  em2d_1  em2d_2  em4dXY  em6dTXY" << std::endl;
	for(int i=0; i<6; i++)
		fout << " mean_" << i;
	fout << std::endl;
	for(int i=0; i<6; i++)
	{
		for(int j=0; j<6; j++)
			fout << " Cov(" << i << "," << j << ")";
		fout << std::endl;
	}
}

void AnalysisPlaneBank::Read(std::istream& fin)
{
	fin >> planeType >> planeNumber >> weight >> z >> t >> pz >> E;
	for(int i=0; i<3; i++)
		fin >> beta[i];
	fin >> beta_p >> amplitudePzCovariance >> l_can;
	for(int i=0; i<3; i++)
		fin >> em2d[i];
	fin >> em4dXY >> em6dTXY;
	for(int i=0; i<6; i++)
		fin >> sums[i];

	for(int i=0; i<6; i++)
		for(int j=0; j<6; j++)
			fin >> CovarianceMatrix[i][j];

}

std::ostream& operator << (std::ostream& out, const AnalysisPlaneBank& bank )
{
	bank.Write(out);
	return out;
}

std::istream& operator >> (std::istream& in,  AnalysisPlaneBank& bank )
{
	bank.Read(in);
	return in;
}
