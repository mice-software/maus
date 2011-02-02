#ifndef _VIRTUALGEOMPARAMETERS_HH_
#define _VIRTUALGEOMPARAMETERS_HH_  1
//----------------------------------------------------------------------------
//
//     File :      VirtualGeomParameters.hh
//     Purpose :   Supply geometry parameters to the VirtualStation.
//     Created :   10-NOV-2002  by Steve Kahn
//
//----------------------------------------------------------------------------

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "Interface/Memory.hh" 

using CLHEP::Hep3Vector;

class VirtualGeomParameters
{
  private:
    void initialize();
    std::map<int,float>   _zpositions;  // positions of virtual detectors 
    std::map<int,float>   _radii;  // radii of detectors
    std::vector<int>   _stationIDs;  // ID of detector
    std::map<int,float>   _lengths;  // thickness of detectors
    //std::map<int,Material*>  _materials; // pointers to material
    static VirtualGeomParameters* _instance;
    void readFile(std::string&); 
  
  public:
    VirtualGeomParameters();
    ~VirtualGeomParameters() 	{ miceMemory.addDelete( Memory::VirtualGeomParameters ); }; 
    static VirtualGeomParameters* getInstance();
    double getRadius(int=0) const;
    double getLength(int=0) const;
    Hep3Vector  getPosition(int=0) const;
    //Material*    getMaterial(int=0) const;
    int  getStationID(int=0) const;
  std::vector<int> getStationIDs() const;  
    int  numberStations() const;
    bool exists(int) const;
    friend std::ostream& operator <<(std::ostream&, const 
				   VirtualGeomParameters&); 
};
#endif
