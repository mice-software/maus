#ifndef transfermapcalculator_hh
#define transfermapcalculator_hh

#include "gsl/gsl_odeiv.h"
#include "gsl/gsl_errno.h"

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "PhaseSpaceVector.hh"
#include "TransferMap.hh"
#include "OpticsModel.hh"
#include "Tensor3.hh"

class TransferMapCalculator
{
public:
        enum var {z};
	TransferMapCalculator();
	~TransferMapCalculator() {;}
	//Integrate the transfer map for the reference particle at point targetZ
	static TransferMap GetTransferMap(PhaseSpaceVector referenceIn, double targetZ, int order=1);
	//Extract a transfer map by polynomial interpolation (e.g. numerical derivatives) from beam transport
	//Calculates a set of points at referenceIn_i + delta_i^n*magnitude_n
	//Then generates a polynomial off these points
	//So map order is magnitude.size() and delta should be a vector of phase space points with dimension 6
	static TransferMap GetPolynomialTransferMap(PhaseSpaceVector referenceIn, double targetZ, std::vector<double> delta, std::vector<double> magnitude); //blah
	//Extract a mapping from in to out by means of linear least squares regression
	//First element in the vectors is the reference particle
	static TransferMap GetPolynomialTransferMap(std::vector<PhaseSpaceVector> in, std::vector<PhaseSpaceVector> out, int order=1);
	//Extract a mapping from in to out by means of linear least squares regression, but here the LLS is allowed to choose which points to examine
  //Fill delta with the limit of validity (i.e. the upper corner of the box in which chi2 < chi2Max, centred on 0)
	static TransferMap GetSweepingPolynomialTransferMap(const VectorMap* trackingOutput, int order, double chi2Max, CLHEP::HepVector& delta, CLHEP::HepVector deltaMax, double deltaFactor, 
                                                      int maxNumberOfSteps, PhaseSpaceVector refIn, PhaseSpaceVector refOut);
	//Extract a mapping from in to out by integrating transfer map
	//Change in transfer matrix at each step is found by numerical differentiation of field map
	//target_indie contains the target independent variable (up to which we integrate)
	//ref are the initial coordinates; field is the field map; indep is the independent variable; 
	//step_size is the change in independent variable with each step; delta is the step size for calculating field map derivatives
	static TransferMap GetDerivativeTransferMap(double target_indie, PhaseSpaceVector x_in, const BTField* field, var indep, double step_size,
                                                    std::vector<double> delta);
	//Set the fields
	static void  SetOpticsModel(OpticsModel Optics) {_optics = Optics;}
	//calculate the reference trajectory over z
	static void  SetReferenceTrajectory(PhaseSpaceVector RefParticle, double targetZ);
	//Set the errors on the numerical integration
	static void  SetRelativeError(double relativeError) {_relativeError = relativeError;}
	static void  SetAbsoluteError(double absoluteError) {_absoluteError = absoluteError;}
	//Return the reference trajectory at a coordinate z
	static const PhaseSpaceVector& ReferenceParticle(double z);
	static void  PrintReferenceTrajectory(std::ostream& out);
	//Return a second order map
	static TransferMap GetSecondOrderMap(PhaseSpaceVector refParticle, double zOut);	
	//Integrate equations of motion
	static PhaseSpaceVector SlowTransport(PhaseSpaceVector psIn, double zOut);
	static PhaseSpaceVector SlowTransportBack(PhaseSpaceVector psIn, double zOut);
	static double           GetLarmorAngle(PhaseSpaceVector refIn, double zOut);

private:
	//return the symplectic matrix
	static HepMatrix GetSymplecticMatrix();
	//Integrate matrices
	static HepMatrix GetTransverseMatrix(double zIn, double targetZ);
	static HepMatrix GetLongitudinalMatrix(double zIn, double targetZ);
	//Calculate dM_ij/dz as a function of z, place results in f[]
	//Hard code longitudinal and transverse as separate
	static int LongFunc  (double z, const double y[], double f[], void *params);
	static int TransFunc (double z, const double y[], double f[], void *params);
	//Calculate du/dz and dM_ij/dz as a function of z, place results in f[]; y and f have 64 elements:
	//u = t,x,y,z,E,px,py,pz
	//M = 
	//Note quite optimal, but generalisable
	static int FullFunc  (double z, const double y[], double f[], void *params);
	//integration for the larmor function in solenoids
	static int LarmorFunc(double z, const double y[], double f[], void *params);
	//Calculate dM/dz, dg/dz as a function of z, place results in f[]
	static int Func2     (double z, const double y[], double f[], void *params);
	//Transport through equations of motion using BTTracker, assume z0 = _zForWrapper
	static void TransportWrapper(const double* point, double* value);
	//Calculate usual Runge Kutta integration for transport using F=qv^B
	static int FuncEqM   (double z, const double y[], double f[], void *params);
	//As before but track backwards
	static int FuncEqMBk (double z, const double y[], double f[], void *params);
	//Calculate dM_ij for numerical integration
	static HepMatrix GetLongMatrix (PhaseSpaceVector RefParticle);
	static HepMatrix GetTransMatrix(PhaseSpaceVector RefParticle);
	static HepMatrix GetFullMatrix (PhaseSpaceVector RefParticle);
	//Calculate K_3_int for the integration
	static Tensor3    GetK3Int(PhaseSpaceVector RefParticle, CLHEP::HepMatrix M);
	static Tensor3    GetK3(PhaseSpaceVector RefParticle);
	static Tensor3    ConvertK3ToInt(Tensor3 K3, CLHEP::HepMatrix M);
	//transform matrix to 1d array and back for gsl integration
	static HepMatrix ArrayToHepMatrix(const double in[], const int nrow, const int ncol);
	static void      HepMatrixToArray(const HepMatrix in, double out[]); 
	//transform tensor to 1d array and back for gsl integration
	static void      TensorToArray(Tensor3 in, double out[]);
	static Tensor3   ArrayToTensor3(const double in[], vector<int> size);
	//transform psvector to 1d array
	static void      PSVectorToArray(PhaseSpaceVector in, double out[]);
	static void      ArrayToPSVector(PhaseSpaceVector &out, double in[]);
	//Member data
	static OpticsModel _optics;
	static vector<PhaseSpaceVector> _referenceTrajectory;
	static double _relativeError, _absoluteError, _dzRef;
	static int    _maxNSteps;
	static PhaseSpaceVector _wrapperReferenceIn, _wrapperReferenceOut;
  static double _mass;

};

#endif


