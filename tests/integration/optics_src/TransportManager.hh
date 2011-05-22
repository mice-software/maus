#ifndef TransportManager_hh
#define TransportManager_hh

#include "TransferMap.hh"
#include "TransferMapCalculator.hh"
#include "Material.hh"
#include "PhaseSpaceVector.hh"
#include "CovarianceMatrix.hh"

#include "src/common/Config/MiceModule.hh"
#include "src/common/BeamTools/BTPhaser.hh"

class TransportManager
{
public:
	//BUG assumes shallow copy in OpticsModel
	TransportManager(std::string rootModuleName);
	TransportManager(OpticsModel model, std::string rootModuleName);
	TransportManager(OpticsModel model); //no materials
	~TransportManager();
	//Set transport (e.g. integration etc) from position of refParticleIn to zOut
	void               SetTransport(PhaseSpaceVector referenceParticleIn, double zOut, int order=1);
	//Set errors on numerical integration of transfer map
	void               SetRelativeError(double error)   {_mapCalc.SetRelativeError(error);}
	void               SetAbsoluteError(double error)   {_mapCalc.SetAbsoluteError(error);}
	//Print out transfer map stuff
	void               PrintMaterial(std::ostream& out); 
	void               PrintMaps(std::ostream& out);
	void               PrintRotatedMaps(std::ostream& out, double angle);
	//Return the optics model (fields)
	const OpticsModel& GetOpticsModel() const {return _model;}
	//Transport a particle or covariance matrix after SetTransport
	PhaseSpaceVector   Transport(const PhaseSpaceVector psIn);
	CovarianceMatrix   Transport(const CovarianceMatrix covIn);	
	//Transport using F=q v^B
	PhaseSpaceVector   SlowTransport(PhaseSpaceVector psIn);
	PhaseSpaceVector   SlowTransportBack(PhaseSpaceVector psIn);
	//As above, but ignore material
	PhaseSpaceVector   SlowTransportNoMaterial(PhaseSpaceVector psIn);
	PhaseSpaceVector   SlowTransportBackNoMaterial(PhaseSpaceVector psIn);
	//Transport a particle or covariance matrix backwards after SetTransport
	PhaseSpaceVector   TransportBack(const PhaseSpaceVector psIn);
	CovarianceMatrix   TransportBack(const CovarianceMatrix covIn);	
	//Return the trace of the product of transfer maps
	double             Get2DTrace(int axis) const;
	double             Get4DTrace()         const;
	double             Get6DTrace()         const;
	//Get the larmor angle that decouples the transfer matrix
	double             GetLarmorAngle();
	//Get the lattice Twiss parameters of the product of transfer maps
	double             GetTwoDBeta(int axis)    const;
	double             GetTwoDAlpha(int axis)   const;
	double             GetTwoDGamma(int axis)   const;
	//Trans assumes no quadrupoles in the lattice - rf are alright
	double             GetBetaTrans()    const;
	double             GetAlphaTrans()   const;
	double             GetGammaTrans()   const;
	//Print reference trajectory in transfer map calculator
	void               PrintReferenceTrajectory(std::ostream & out) {_mapCalc.PrintReferenceTrajectory(out);}
	//Add a new material by hand
	void               AddMaterial(double thickness, std::string type, double zPosition) 
	                         {_material.push_back(Material(thickness, type, zPosition));}
	//Get the decoupled transfer maps for solenoids
	std::vector<TransferMap> GetDecoupledTransferMaps() const;
	std::vector<TransferMap> GetTransferMaps()          const              {return _map;}
	void                     SetTransferMaps(std::vector<TransferMap> map) {_map=map;}
	CovarianceMatrix         Decompose(double em_t, double em_x, double em_y) const;
	OpticsModel              GetOpticsModel() {return _model;}

private:
	//Return a HepSymMatrix that gives the effect of a material on a covmatrix
	HepSymMatrix GetMaterialEffect(int previousMap);
	void         SetDaughterMaterials(MiceModule * parent);
	void         SetMaps();
	Material     BuildMaterial(MiceModule * module);
	void         SortMaterials(); //by z

	std::vector<TransferMap>   _map;
	std::vector<Material>      _material;
	int                   _startMaterial;
	MiceModule*           _rootModule;
	OpticsModel           _model; 
	TransferMapCalculator _mapCalc;
};

#endif




