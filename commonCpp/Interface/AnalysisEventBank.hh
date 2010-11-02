// CTR - April 05
//

#ifndef AnalysisEventBank_h
#define AnalysisEventBank_h

#include <stdio.h>
#include <fstream>

class AnalysisEventBank
{
public:
  AnalysisEventBank();
	AnalysisEventBank(const AnalysisEventBank& rhs);
	~AnalysisEventBank() {}

	const AnalysisEventBank& operator= (const AnalysisEventBank& rhs);


  void Read(std::ifstream&);
  void Write(std::ofstream&);

	int evtNo, planeNumber, planeType;
	double position[4], momentum[4], bfield[3];
	double twoDSpe[3], fourDSpe, sixDSpe;
	double l_can, weight;
};

#endif
