#ifndef _VIRTUALPLANES_HH_
#define _VIRTUALPLANES_HH_

#include "BeamTools/BTTracker.hh"

#include <vector>
#include <map>

class G4Step;
class G4StepPoint;

class VirtualPlane;
class VirtualPlaneManager;
class VirtualHit;
class MiceModule;

class VirtualPlane
{
public:
  //Somewhat experimental, trying static initialisers instead of constructors
  //The problem is that if I have several constructors that need the same initialisation object(s), there is no elegant way to distinguish
  enum stepping{integrate, linear_interpolate};
  enum multipass_handler{ignore, new_station, same_station};

  VirtualPlane ();
  ~VirtualPlane() {}
  static VirtualPlane BuildVirtualPlane (CLHEP::HepRotation rot, CLHEP::Hep3Vector pos, double radialExtent, bool globalCoordinates, 
                                         double indie, BTTracker::var type, multipass_handler mp, bool allowBackwards);
  //return true if the step is stepping over this plane
  bool SteppingOver(const G4Step* aStep) const;
  //Fill data into the Hit
  void BuildNewHit (const G4Step * aStep, int station) const;
  //Algorithm for handling of multiple passes
  multipass_handler MultipassAlgorithm() {return _multipass;}

  static bool ComparePosition(VirtualPlane* p1, VirtualPlane* p2) {return p1->_independentVariable < p2->_independentVariable;}

private:
  //Build a new hit and send it to the MICEEvent
  void FillBField    (VirtualHit * aHit, const G4Step * aStep) const;
  void FillStaticData(VirtualHit * aHit, const G4Step * aStep) const;
  void FillKinematics(VirtualHit * aHit, const G4Step * aStep) const;
  bool InRadialCut   (CLHEP::Hep3Vector position) const;

  double*   Integrate       (G4StepPoint* aPoint) const;
  double*   ExtractPointData(G4StepPoint* aPoint) const;

  double    GetIndependentVariable(G4StepPoint* aPoint) const; //extract the value of the point's independent variable, z,u or t

  BTTracker::var     _planeType;
  int                _numberOfPasses;
  double             _independentVariable;
  double             _step;
  double             _radialExtent;
  bool               _globalCoordinates;
  multipass_handler  _multipass;
  static BTField*    _field;
  static stepping    _stepping;

  CLHEP::Hep3Vector  _position; //if var is u, then this will give origin
  CLHEP::HepRotation _rotation; //if var is u, then this will give rotation
  bool               _allowBackwards;
  friend class VirtualPlaneManager;
};

class VirtualPlaneManager
{
public:
  ~VirtualPlaneManager()      {;}
  static VirtualPlaneManager* getVirtualPlaneManager();

  //read in a step, see if it has crossed a plane
  //this wants to be quick!
  static void ConstructVirtualPlanes(BTField* field, MiceModule* model);
  static void VirtualPlanesSteppingAction(const G4Step * aStep);
  static void StartOfEvent();

  static BTField* Field()               { return _field;}
  static BTField* Field(BTField* field) {_field = field; VirtualPlane::_field = field; return _field;}

  static std::string         ModuleName   (int stationNumber); //map from module name to station number
  static const MiceModule*   Module       (int stationNumber); //map from module name to station number
  static int                 StationNumber(const MiceModule* module); //map from station number to module name
private:
  VirtualPlaneManager() {;}
  static VirtualPlane ConstructFromModule(const MiceModule* mod);

  static BTField*                  _field;
  static VirtualPlaneManager*      _instance;
  static bool                      _useVirtualPlanes;
  static std::vector<VirtualPlane*> _planes;
  static std::map<VirtualPlane*, const MiceModule*>  _mods; //associate MiceModule with each plane in _planes
  static std::vector<int>          _nHits; //numberOfHits in each plane
};

#endif
