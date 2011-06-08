// MAUS WARNING: THIS IS LEGACY CODE.
//Chris Rogers April 05
//AnalysisEventBank.cc
//


#include <fstream>
#include "Interface/AnalysisEventBank.hh"

using namespace std;

AnalysisEventBank::AnalysisEventBank()
{
	evtNo=planeNumber=planeType=0;
	for(int i=0; i<4; i++)
	{
		position[i]=0;
		momentum[i]=0;
	}
	for(int i=0; i<3; i++)
	{
		bfield[i]=0;
		twoDSpe[i]=0;
	}

	fourDSpe=0;
	sixDSpe=0;
	l_can=0;
	weight=0;
}

AnalysisEventBank::AnalysisEventBank(const AnalysisEventBank& rhs)
{
	evtNo = rhs.evtNo;
	planeNumber = rhs.planeNumber;
	planeType = rhs.planeType;
	for(int i=0; i<4; i++)
	{
		position[i]=rhs.position[i];
		momentum[i]=rhs.momentum[i];
	}
	for(int i=0; i<3; i++)
	{
		bfield[i]=rhs.bfield[i];
		twoDSpe[i]=rhs.twoDSpe[i];
	}

	fourDSpe=rhs.fourDSpe;
	sixDSpe=rhs.sixDSpe;
	l_can=rhs.l_can;
	weight=rhs.weight;
}

const AnalysisEventBank& AnalysisEventBank::operator= (const AnalysisEventBank& rhs)
{
	if (this == &rhs) return *this;

	evtNo = rhs.evtNo;
	planeNumber = rhs.planeNumber;
	planeType = rhs.planeType;
	for(int i=0; i<4; i++)
	{
		position[i]=rhs.position[i];
		momentum[i]=rhs.momentum[i];
	}
	for(int i=0; i<3; i++)
	{
		bfield[i]=rhs.bfield[i];
		twoDSpe[i]=rhs.twoDSpe[i];
	}

	fourDSpe=rhs.fourDSpe;
	sixDSpe=rhs.sixDSpe;
	l_can=rhs.l_can;
	weight=rhs.weight;

	return *this;
}

void AnalysisEventBank::Read(std::ifstream& fin)
{
	fin >> evtNo >> planeType >> planeNumber;
	for(int i=0; i<4; i++)
	{
		fin >> position[i];
		fin >> momentum[i];
	}
	for(int i=0; i<3; i++)
		fin >> twoDSpe[i];
	fin >> fourDSpe >> sixDSpe >> weight >> l_can;
	for(int i=0; i<3; i++)
		fin >> bfield[i];
}

void AnalysisEventBank::Write(std::ofstream& fout)
{
	fout << evtNo << " " << planeType << " " << planeNumber << " ";
	for(int i=0; i<4; i++)
	{
		fout << position[i] << " ";
		fout << momentum[i] << " ";
	}
	for(int i=0; i<3; i++)
		fout << twoDSpe[i] << " ";
	fout << fourDSpe << " " << sixDSpe << " " << weight << " " << l_can << " ";
	for(int i=0; i<3; i++)
		fout << bfield[i] << " ";
	fout << endl;

}


