/* 
** Written for the MICE proposal simulation
** Ed McKigney - Aug 21 2002
*/

#ifndef SCIFIDIGITIZATIONPARAMS_HH
#define SCIFIDIGITIZATIONPARAMS_HH

#include <stdio.h>
#include <string.h>

/**
* SciFiDigitizationParams holds the information needed to perform the digitisation
* of the Scintillating Fibre tracker.
* This class is implemented as a singleton and accessed through the static method
* getInstance().
**/

class SciFiDigitizationParams
{
public:

	// Static method to retrieve the SciFiDigitizationParams class
	static SciFiDigitizationParams* getInstance();

private:

        // Default constructor
	SciFiDigitizationParams();

public:

        //! Default destructor
	~SciFiDigitizationParams();

	//! Return the decay constant of the fibre
	double getFiberDecayConst()	{return fiberDecayConst;}

        //! Return the conversion factor of the fibre
	double getFiberConvFactor()	{return fiberConvFactor;}

        //! Return the refractive index of the fibre
	double getFiberRefractiveIndex(){return fiberRefractiveIndex;}

        //! Return the critical angle of the fibre
	double getFiberCriticalAngle()	{return fiberCriticalAngle;}

        //! Return the trapping efficiency of the fibre
	double getFiberTrappingEff()    {return fiberTrappingEff;}

        //! Return the efficiency of the mirror on the end of the fibre
	double getFiberMirrorEff()      {return fiberMirrorEff;}

        //! Return the transmission efficiency of the fibre
	double getFiberTransmissionEff(){return fiberTransmissionEff;}

        //! Return the transmission efficiency of the multiplexing (or ganging)
	double getMUXTransmissionEff()  {return MUXTransmissionEff;}

        //! Return the length of the fibre
	double getFiberRunLength()	{return fiberRunLength;}

        //! Return the Quantum Efficiency of the VLPCs
	double getVlpcQE()		{return vlpcQE;}

        //! Return the energy resolution of the VLPCs
	double getVlpcEnergyRes()	{return vlpcEnergyRes;}

        //! Return the time resolution of the VLPCs
	double getVlpcTimeRes()		{return vlpcTimeRes;}

        //! Return the number of bits in the ADCs used to read out the VLPCs
	int getAdcBits()		{return adcBits;}

        //! Return the ADC factor
        double getAdcFactor()		{return adcFactor;}

        //! Return the number of bits in the TDCs used to read out the VLPCs
	int getTdcBits()		{return tdcBits;}

        //! Return the TDC factor
        double getTdcFactor()		{return tdcFactor;}

        //! Return the level of multiplexing (or ganging)
	int getMUXNum()			{return MUXNum;}

        //! Return the name of the file containing the dead channel list
	char *getDeadChanFName()	{return deadChanFName;}

        //! Return the number of electronic channels per plane
	int getnElecChanPerPlane()	{return nElecChanPerPlane;}

        //! Return the number of planes
	int getnPlanes()		{return nPlanes;}

        //! Return the number of stations
	int getnStations()		{return nStations;}

        //! Return the number of trackers
	int getnTrackers()		{return nTrackers;}
	      
private:

	static SciFiDigitizationParams* _instance;

	// Characteristics of the fiber
	double fiberDecayConst;         //Fiber Scintillation time const
	double fiberConvFactor;         //Number of photons per MeV dep
	double fiberRefractiveIndex;    //Refractive Index of the core
	double fiberCriticalAngle;      //radians
	double fiberTrappingEff;        //Fraction of trapped in the fiber
	double fiberMirrorEff;          //Fraction of photons reflected
	double fiberTransmissionEff;    //Fraction of photons kept
	double MUXTransmissionEff;      //Fraction of photons kept

	// Characteristics of the detector layout
	double fiberRunLength;         //in meters
	int MUXNum;			// Number of fibers per VLPC chan

	// Characteristics of the electronics
	double vlpcQE;                 //quantum efficiency of the VLPC
	double vlpcEnergyRes;          //sigma in energy for the VLPC
	double vlpcTimeRes;            //sigma in time for the VLPC

	int adcBits;                   //number of bits in the ADC
        double adcFactor;              //number of PE/count
	int tdcBits;                   //number of bits in the TDC
        double tdcFactor;              //number of ns/count

	char deadChanFName[512];	// Name of the dead channel list

	int nElecChanPerPlane;
	int nPlanes;
	int nStations;
	int nTrackers;
};

#endif
