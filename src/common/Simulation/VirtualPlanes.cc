#include "Simulation/VirtualPlanes.hh"
#include "Interface/VirtualHit.hh"
#include "Interface/MICEEvent.hh"
#include "Config/MiceModule.hh"
#include "Interface/Squeal.hh"

#include "BeamTools/BTField.hh"
#include "BeamTools/BTTracker.hh"
#include "BeamTools/BTFieldGroup.hh"

#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"

extern MICEEvent simEvent;

BTField*               VirtualPlane::_field    = NULL;
VirtualPlane::stepping VirtualPlane::_stepping = VirtualPlane::integrate;

/////////////////////// VirtualPlane //////////////////

VirtualPlane::VirtualPlane()
{
  _planeType      = BTTracker::z;
  _numberOfPasses = 0;
  _step           = 0.1;
  _position       = CLHEP::Hep3Vector(0.,0.,0.);
  _rotation       = CLHEP::HepRotation();
  _radialExtent   = -1;
  _independentVariable = 0.;
  _globalCoordinates   = false;
}

VirtualPlane VirtualPlane::BuildVirtualPlane(CLHEP::HepRotation rot, CLHEP::Hep3Vector pos, double radialExtent, bool globalCoordinates, double indie, BTTracker::var type, multipass_handler pass, bool allowBackwards)
{
  VirtualPlane vp;
  vp._planeType = type;
  vp._position  = pos;
  vp._rotation  = rot;
  vp._independentVariable  = indie; //maybe! should be the same as GetIndependentVariable(...) for u
  vp._radialExtent         = radialExtent;
  vp._globalCoordinates    = globalCoordinates;
  vp._multipass            = pass;
  vp._allowBackwards       = allowBackwards;
  if (type != BTTracker::tau_field && type != BTTracker::u && 
      type != BTTracker::z && type != BTTracker::t) {
    throw(Squeal(Squeal::recoverable, 
                 "Virtual plane type not implemented",
                 "VirtualPlane::BuildVirtualPlane(...)"));
  }
  return vp;
}

bool VirtualPlane::SteppingOver(const G4Step* aStep) const
{
  double pre  = GetIndependentVariable(aStep->GetPreStepPoint()  );
  double post = GetIndependentVariable(aStep->GetPostStepPoint() );

  if(pre  <= _independentVariable && post > _independentVariable) return true; //stepping forwards through the plane
  if(_allowBackwards && post <= _independentVariable && pre  > _independentVariable) return true; //stepping backwards through the plane
  return false;
}

void VirtualPlane::FillStaticData(VirtualHit * aHit, const G4Step * aStep) const
{
  G4Track * theTrack = aStep->GetTrack();

  aHit->SetTrackID(theTrack->GetTrackID());
  aHit->SetPID(theTrack->GetDynamicParticle()->GetDefinition()->GetPDGEncoding());            //  particle id code
  aHit->SetMass(theTrack->GetDynamicParticle()->GetMass());           //  particle mass
  aHit->SetCharge(theTrack->GetDynamicParticle()->GetCharge());         //  charge of particle
}

void VirtualPlane::FillBField(VirtualHit * aHit, const G4Step * aStep) const
{
  G4double point[4] = {aHit->GetPos()[0], aHit->GetPos()[1], aHit->GetPos()[2], aHit->GetTime()};
  G4double field[6] = {0., 0., 0., 0., 0., 0.};
  _field->GetFieldValue(point, field);
  aHit->SetBField(ThreeVector(field[0], field[1], field[2]));
  aHit->SetEField(ThreeVector(field[3], field[4], field[5]));
}

bool VirtualPlane::InRadialCut(CLHEP::Hep3Vector position) const
{
  CLHEP::Hep3Vector point = _rotation*(position - _position);
  if(point[0]*point[0] + point[1]*point[1] > _radialExtent*_radialExtent && _radialExtent > 0.)
    return false;
  return true;
}

void VirtualPlane::FillKinematics(VirtualHit * aHit, const G4Step * aStep) const
{
  double  x[8];
  double* x_from_beginning = NULL;
  double* x_from_end = NULL;
  switch(_stepping)
  {
    case integrate:
    {
      x_from_beginning = Integrate(aStep->GetPreStepPoint());
      x_from_end       = Integrate(aStep->GetPostStepPoint());
      break;
    }
    case linear_interpolate:
    {
      x_from_beginning = ExtractPointData(aStep->GetPreStepPoint());
      x_from_end       = ExtractPointData(aStep->GetPostStepPoint());
      break;
    }
  }
  double indep_beg     = GetIndependentVariable(aStep->GetPreStepPoint());
  double indep_end     = GetIndependentVariable(aStep->GetPostStepPoint());
  for(int i=0; i<8; i++)
    x[i] = (x_from_end[i] - x_from_beginning[i])/(indep_end-indep_beg)*(_independentVariable-indep_beg)+x_from_beginning[i];

  if(!InRadialCut(CLHEP::Hep3Vector(x[1],x[2],x[3]))) throw(Squeal(Squeal::recoverable, "Hit outside radial cut", "VirtualPlane::FillKinematics"));
  CLHEP::Hep3Vector position(x[1],x[2],x[3]);
  CLHEP::Hep3Vector momentum(x[5],x[6],x[7]);
  if(!_globalCoordinates) 
  {
    position = _rotation*(position - _position);
    momentum = _rotation*momentum;
  }
  aHit->SetPos     (position);
  aHit->SetMomentum(momentum);
  double mass = aStep->GetPostStepPoint()->GetMass();
  x[4] = sqrt(x[5]*x[5]+x[6]*x[6]+x[7]*x[7]+mass*mass); // FORCE mass shell condition
  aHit->SetEnergy  (x[4]);
  aHit->SetTime    (x[0]);
  delete [] x_from_beginning;
  delete [] x_from_end;
}

double   VirtualPlane::GetIndependentVariable(G4StepPoint* aPoint) const {
  switch (_planeType) {
    case BTTracker::z: return aPoint->GetPosition()[2];
    case BTTracker::t: return aPoint->GetGlobalTime();
    case BTTracker::tau_field: return aPoint->GetProperTime();
    case BTTracker::u: return ( _rotation*(aPoint->GetPosition() - _position)  )[2]; //update RotatedVirtualPlane(...) if this changes
    default: return 0.;
  }
}

double * VirtualPlane::ExtractPointData(G4StepPoint *aPoint) const
{
  double* x_in = new double[8];
  for(int i=0; i<3; i++)
  {
    x_in[i+1] = aPoint->GetPosition()[i];
    x_in[i+5] = aPoint->GetMomentum()[i];
  }
  x_in[0] = aPoint->GetGlobalTime();
  x_in[4] = aPoint->GetTotalEnergy();
  return x_in;
}

double * VirtualPlane::Integrate(G4StepPoint* aPoint) const
{
  double* x_in = ExtractPointData(aPoint);
  double  step = _step;
  if(GetIndependentVariable(aPoint) > _independentVariable) step *= -1.;
  BTTracker::integrate(_independentVariable, x_in, _field, _planeType, step, aPoint->GetCharge(), _position, _rotation);
  return x_in;
}
// 
VirtualHit VirtualPlane::BuildNewHit(const G4Step * aStep, int station) const
{
  VirtualHit aHit;
  try {
    FillStaticData(&aHit, aStep);
    FillKinematics(&aHit, aStep);
    FillBField    (&aHit, aStep);
    aHit.SetStationNumber(station);
  }
  catch(Squeal squee) {
    if(squee.GetErrorLevel()==Squeal::nonRecoverable) throw squee;
    throw squee;
  } //catch e.g. tracking errors; don't record a hit in this case
  return aHit;
}

//////////////////////// VirtualPlaneManager //////////////////////////

BTField*             VirtualPlaneManager::_field            = NULL;
VirtualPlaneManager* VirtualPlaneManager::_instance         = VirtualPlaneManager::getVirtualPlaneManager();
bool                 VirtualPlaneManager::_useVirtualPlanes = false;
std::vector<VirtualPlane*> VirtualPlaneManager::_planes      = std::vector<VirtualPlane*>(0);
std::vector<int>           VirtualPlaneManager::_nHits       = std::vector<int>(0); //initialised in StartOfEvent
std::map<VirtualPlane*, const MiceModule*>  VirtualPlaneManager::_mods;

VirtualPlaneManager* VirtualPlaneManager::getVirtualPlaneManager()
{
  if(_instance == NULL)
  {
    _instance = new VirtualPlaneManager(); //MUST CALL ConstructVirtualPlanes to do anything useful!
    return _instance;
  }
  else                  return _instance;
}

void VirtualPlaneManager::VirtualPlanesSteppingAction(const G4Step* aStep)
{
  if(!_useVirtualPlanes) return;
  for(unsigned int i=0; i<_planes.size(); i++) 
    if(_planes[i]->SteppingOver(aStep))
    {//if I go round e.g. a ring, station number should be (number_of_passes * number_of_stations) + i
      if(_nHits[i]>0)
      {
        if(_planes[i]->GetMultipassAlgorithm() == VirtualPlane::new_station)
          _planes[i]->BuildNewHit(aStep, _planes.size()*_nHits[i]+i+1);
        if(_planes[i]->GetMultipassAlgorithm() == VirtualPlane::same_station)
          _planes[i]->BuildNewHit(aStep, i+1);
      }
      else
        _planes[i]->BuildNewHit(aStep, i+1);
      _nHits[i]++; 
    }
}

void VirtualPlaneManager::StartOfEvent()
{
  _nHits = std::vector<int>(_planes.size(), 0);
}

void VirtualPlaneManager::ConstructVirtualPlanes(BTField* field, MiceModule* model)
{
  if(_instance == NULL) _instance = new VirtualPlaneManager();
  _field = field;
  std::vector<const MiceModule*> modules   = model->findModulesByPropertyString("SensitiveDetector", "Virtual");
  std::vector<const MiceModule*> envelopes = model->findModulesByPropertyString("SensitiveDetector", "Envelope");
  modules.insert(modules.end(), envelopes.begin(), envelopes.end());
  for(unsigned int i=0; i<modules.size(); i++)
  {
    _planes.push_back(new VirtualPlane( ConstructFromModule(modules[i]) ) );
    _mods[_planes.back()] = modules[i];
  }
  sort(_planes.begin(), _planes.end(), VirtualPlane::ComparePosition);
  _useVirtualPlanes = _planes.size() > 0;
  if(_field == NULL) _field = new BTFieldGroup();
  VirtualPlane::_field = _field;
}

VirtualPlane VirtualPlaneManager::ConstructFromModule(const MiceModule* mod)
{
    std::string variable          = "z";
    double      radialExtent      = -1.;
    double      indie             = 0.;
    bool        globalCoordinates = true;
    bool        allowBackwards    = true;
    if(mod->propertyExistsThis("IndependentVariable", "string")) 
      variable = mod->propertyStringThis("IndependentVariable");
    if(mod->propertyExistsThis("RadialExtent", "double")) 
      radialExtent = mod->propertyDoubleThis("RadialExtent");
    if(mod->propertyExistsThis("GlobalCoordinates", "bool")) 
      globalCoordinates = mod->propertyBoolThis("GlobalCoordinates");
    VirtualPlane::multipass_handler pass = VirtualPlane::ignore;
    if(mod->propertyExistsThis("MultiplePasses", "string"))
    {
      /**/ if(mod->propertyString("MultiplePasses") == "Ignore")      pass = VirtualPlane::ignore;
      else if(mod->propertyString("MultiplePasses") == "SameStation") pass = VirtualPlane::same_station;
      else if(mod->propertyString("MultiplePasses") == "NewStation")  pass = VirtualPlane::new_station;
      else throw(Squeal(Squeal::recoverable, "Did not recognise MultiplePasses option "+mod->propertyStringThis("MultiplePasses")+" in VirtualPlane module "+mod->fullName(), 
                                             "VirtualPlaneManager::ConstructFromModule") );
    }
    if(mod->propertyExistsThis("AllowBackwards", "bool"))
      allowBackwards = mod->propertyBoolThis("AllowBackwards");

    BTTracker::var var_enum  = BTTracker::z;

    for(unsigned int i=0; i<variable.size(); i++) variable[i] = tolower(variable[i]);
    if     (variable == "time" || variable == "t")
    {
      indie = mod->propertyDoubleThis("PlaneTime");
      var_enum = BTTracker::t;
    }
    else if(variable == "tau")
    {
      indie = mod->propertyDoubleThis("PlaneTime");
      var_enum = BTTracker::tau_field;
    }
    else if(variable == "z")
    {
      indie    = mod->globalPosition()[2];
      var_enum = BTTracker::z;
    }
    else if(variable == "u")
      var_enum = BTTracker::u;
    else
      throw(Squeal(Squeal::recoverable, "Did not recognise IndependentVariable in Virtual detector module "+mod->fullName(), "VirtualPlaneManager::ConstructFromModule"));
    return VirtualPlane::BuildVirtualPlane(mod->globalRotation(), mod->globalPosition(), radialExtent, 
                                           globalCoordinates, indie, var_enum, pass, allowBackwards);
}

std::string VirtualPlaneManager::ModuleName(int stationNumber)
{
  if(_planes.size() == 0) throw(Squeal(Squeal::recoverable, "No Virtual planes initialised", "VirtualPlaneManager::ModuleName"));
  //map from module name to _planes index; if stationNumber > planes.size, it means we've gone round twice or more, subtract off
  while(stationNumber > int(_planes.size())) stationNumber -= int(_planes.size());
  return _mods[ _planes[stationNumber-1] ]->fullName();
}

const MiceModule*  VirtualPlaneManager::Module(int stationNumber) //map from module* to station number
{
  if(_planes.size() == 0) throw(Squeal(Squeal::recoverable, "No Virtual planes initialised", "VirtualPlaneManager::Module"));
  //map from module name to _planes index; if stationNumber > planes.size, it means we've gone round twice or more, subtract off
  while(stationNumber > int(_planes.size())) stationNumber -= int(_planes.size());
  return _mods[ _planes[stationNumber-1] ];
}

int         VirtualPlaneManager::StationNumber(const MiceModule* module)
{
  if(_planes.size() == 0) throw(Squeal(Squeal::recoverable, "No Virtual planes initialised", "VirtualPlaneManager::ModuleName"));
  VirtualPlane* plane = NULL;
  for(std::map<VirtualPlane*, const MiceModule*>::iterator it=_mods.begin(); it!=_mods.end() && plane==NULL; it++)
    if(it->second == module) plane = it->first;
  for(size_t i=0; i<_planes.size(); i++)
    if(plane == _planes[i]) return i+1;
  throw(Squeal(Squeal::recoverable, "Failed to find VirtualPlane for module "+module->fullName(), "VirtualPlaneManager::ModuleName"));
}


