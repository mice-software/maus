//Evaluate Beta Function
//Chris Rogers, March 05
//
//Evaluates the effects on optical functions of material
//in the beamline
//look out for the useful function to evaluate the amount of material
//required to give a certain emittance kick
//only material type atm is "lead"

#ifndef material_hh
#define material_hh

#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "CLHEP/Matrix/SymMatrix.h"

#include "CovarianceMatrix.hh"
#include "PhaseSpaceVector.hh"
#include "src/common/Interface/Squeal.hh"

using namespace std;

class Material
{
public:
	//constructor for evaluating length given an emittance kick
	//position should be z position of material center
	Material(string materialType, double position);
	//constructor for material with a certain thickness and type
	Material(double thickness, string materialType, double position);
	//constructor for arbritrary material with known x0 and dEdZ
	Material(double x0_mm, double dEdZ_MeVpermm, string materialName, double position);
	~Material();

	//Print details
	void Print(ostream &out)
	{out << _materialType << "\t" << GetZ() << "\t" << _thickness << endl;}

	//Methods for TransportManager
	double           GetZ()                  const {return _z;}
	double           GetdE(double pz)        const {return _thickness*GetdEdZ(pz);}
	double           GetSigma2E(double pz)   const {return 0;} //guess
	//Use s(E') = s(E)[1 + d/dE(dE/dz)*dz]
	//          = s(E)[1 + p/E*d/dp(dE/dz)*dz]
	double           GetCurvature(double pz) const;
	HepSymMatrix     GetCovariances(PhaseSpaceVector track) const;
	//Transport muons through the material
	CovarianceMatrix Transport(CovarianceMatrix covIn)      const;
	PhaseSpaceVector Transport(PhaseSpaceVector psIn)       const;
	CovarianceMatrix TransportBack(CovarianceMatrix covIn)  const;
	PhaseSpaceVector TransportBack(PhaseSpaceVector psIn)   const;
	//
	void   SetZ(double z) {_z=z;} 

	//return the length of material required to give a desired emittance
	double GetLengthGivenEmittance(CovarianceMatrix bunch, double emittanceRequired);
	void   SetdEdXModel(std::string dEdXModel) {_dEdXModel = dEdXModel;}
	//return true if the material has been implemented - should be static
	bool   MaterialImplemented(std::string material);
	//
	double GetThickness() {return _thickness;}

private:
	//x0 mm, dEdZ
	static double m_mu, m_e, K;
	double _x0, _dEdZ, _thickness, _z;
	double m_I, _Z, m_A, _delta, _density;
	double _setsLengthOnStepEmittance;
	bool _setsLengthOnStep;
	//lists holding the data of the materials the class can access
	//initialised in SetMaterialList()
	vector<string> _materialList;
	vector<double> _x0List;
	vector<double> _ZList;
	vector<double> _dEdZList;
	vector<double> _AList;
	vector<double> _IList;
	vector<double> _deltaList;
	vector<double> _densityList;

	//the material type of this material
	std::string _materialType;
	std::string _dEdXModel;
	//set x0 and dEdZ for various materials
	void SetMaterial(string materialType);
	//set the list of material propertie - called by SetMaterial
	void SetMaterialList();

	//return dTheta2 for the given thickness
	double GetdTheta2(double pz) const;

	//return Einsteins relativistic beta and gamma for a given pz muon
	double GetBetaRel(double pz)  const {return pz / sqrt(pz*pz + m_mu*m_mu);}
	double GetGammaRel(double pz) const {return 1 / sqrt(1-GetBetaRel(pz)*GetBetaRel(pz)); }

	//get from unnormalised to normalised emittance and vv
	double NormaliseEmittance(double UnNormalisedEmittance, double pz) const {return UnNormalisedEmittance*pz/m_mu;}
	double UnnormaliseEmittance(double NormalisedEmittance, double pz) const {return NormalisedEmittance*m_mu/pz;}

	//return dEdZ as a function of mean momentum
	double GetdEdZ(double pz)    const;
	double GetdEdZ_BB(double pz) const;
	double GetdEdZ_MI()          const {return -_dEdZ;}
	double GetdEdZ_PDG();//NOT implemented
};


#endif
