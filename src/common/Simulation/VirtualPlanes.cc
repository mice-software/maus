/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <string>
#include <algorithm>

#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"

#include "src/common/Simulation/VirtualPlanes.hh"
#include "src/common/Interface/VirtualHit.hh"
#include "src/common/Config/MiceModule.hh"
#include "src/common/Interface/Squeal.hh"

#include "src/common/BeamTools/BTField.hh"
#include "src/common/BeamTools/BTTracker.hh"

BTField*               VirtualPlane::_field    = NULL;
VirtualPlane::stepping VirtualPlane::_stepping = VirtualPlane::integrate;

/////////////////////// VirtualPlane //////////////////

VirtualPlane::VirtualPlane() {
  _planeType      = BTTracker::z;
  _step           = 0.1;
  _position       = CLHEP::Hep3Vector(0., 0., 0.);
  _rotation       = CLHEP::HepRotation();
  _radialExtent   = -1;
  _independentVariable = 0.;
  _globalCoordinates   = false;
}

VirtualPlane VirtualPlane::BuildVirtualPlane(CLHEP::HepRotation rot,
                           CLHEP::Hep3Vector pos, double radialExtent,
                           bool globalCoordinates, double indie,
                           BTTracker::var type, multipass_handler pass,
                           bool allowBackwards) {
  VirtualPlane vp;
  vp._planeType = type;
  vp._position  = pos;
  vp._rotation  = rot;
  vp._independentVariable  = indie;
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

bool VirtualPlane::SteppingOver(const G4Step* aStep) const {
  double pre  = GetIndependentVariable(aStep->GetPreStepPoint());
  double post = GetIndependentVariable(aStep->GetPostStepPoint());

  if (pre  <= _independentVariable && post > _independentVariable)
    return true;  // stepping forwards
  if (_allowBackwards && post <= _independentVariable &&
                         pre  > _independentVariable)
    return true;  // stepping backwards
  return false;
}

void VirtualPlane::FillStaticData
                               (VirtualHit * aHit, const G4Step * aStep) const {
  G4Track * theTrack = aStep->GetTrack();

  aHit->SetTrackID(theTrack->GetTrackID());
  aHit->SetPID(theTrack->GetDynamicParticle()
                                          ->GetDefinition()->GetPDGEncoding());
  aHit->SetMass(theTrack->GetDynamicParticle()->GetMass());
  aHit->SetCharge(theTrack->GetDynamicParticle()->GetCharge());
}

void VirtualPlane::FillBField(VirtualHit * aHit, const G4Step * aStep) const {
  double point[4] =
     {aHit->GetPos()[0], aHit->GetPos()[1], aHit->GetPos()[2], aHit->GetTime()};
  double field[6] = {0., 0., 0., 0., 0., 0.};
  _field->GetFieldValue(point, field);
  aHit->SetBField(CLHEP::Hep3Vector(field[0], field[1], field[2]));
  aHit->SetEField(CLHEP::Hep3Vector(field[3], field[4], field[5]));
}

bool VirtualPlane::InRadialCut(CLHEP::Hep3Vector position) const {
  CLHEP::Hep3Vector point = _rotation*(position - _position);
  if (point[0]*point[0] + point[1]*point[1] > _radialExtent*_radialExtent &&
     _radialExtent > 0.)
    return false;
  return true;
}

void VirtualPlane::FillKinematics
                               (VirtualHit * aHit, const G4Step * aStep) const {
  double  x[8];
  double* x_from_beginning = NULL;
  double* x_from_end = NULL;
  switch (_stepping) {
    case integrate: {
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
  for (int i = 0; i < 8; i++)
    x[i] = (x_from_end[i]-x_from_beginning[i])/(indep_end-indep_beg)
          *(_independentVariable-indep_beg)+x_from_beginning[i];

  aHit->SetPos(CLHEP::Hep3Vector(x[1], x[2], x[3]));
  aHit->SetMomentum(CLHEP::Hep3Vector(x[5], x[6], x[7]));

  double mass = aStep->GetPostStepPoint()->GetMass();
  // FORCE mass shell condition
  x[4] = sqrt(x[5]*x[5]+x[6]*x[6]+x[7]*x[7]+mass*mass);
  aHit->SetEnergy(x[4]);
  aHit->SetTime(x[0]);
  delete [] x_from_beginning;
  delete [] x_from_end;

  if (!InRadialCut(CLHEP::Hep3Vector(x[1], x[2], x[3])))
    throw(Squeal(Squeal::recoverable, "Hit outside radial cut",  // appropriate?
                                  "VirtualPlane::FillKinematics"));
}

void VirtualPlane::TransformToLocalCoordinates(VirtualHit* aHit) const {
  aHit->SetPos(_rotation*(aHit->GetPos() - _position));
  aHit->SetMomentum(_rotation*aHit->GetMomentum());
  aHit->SetBField(_rotation*aHit->GetBField());
  aHit->SetEField(_rotation*aHit->GetEField());
}

double   VirtualPlane::GetIndependentVariable(G4StepPoint* aPoint) const {
  switch (_planeType) {
    case BTTracker::z: return aPoint->GetPosition()[2];
    case BTTracker::t: return aPoint->GetGlobalTime();
    case BTTracker::tau_field: return aPoint->GetProperTime();
    case BTTracker::u:
      return ( _rotation*(aPoint->GetPosition() - _position) )[2];
    default: return 0.;
  }
}

double * VirtualPlane::ExtractPointData(G4StepPoint *aPoint) const {
  double* x_in = new double[8];
  for (int i = 0; i < 3; i++) {
    x_in[i+1] = aPoint->GetPosition()[i];
    x_in[i+5] = aPoint->GetMomentum()[i];
  }
  x_in[0] = aPoint->GetGlobalTime();
  x_in[4] = aPoint->GetTotalEnergy();
  return x_in;
}

double * VirtualPlane::Integrate(G4StepPoint* aPoint) const {
  double* x_in = ExtractPointData(aPoint);
  double  step = _step;
  if (GetIndependentVariable(aPoint) > _independentVariable) step *= -1.;
  BTTracker::integrate(_independentVariable, x_in, _field, _planeType, step,
                       aPoint->GetCharge(), _position, _rotation);
  return x_in;
}

VirtualHit VirtualPlane::BuildNewHit(const G4Step * aStep, int station) const {
  VirtualHit aHit;
  try {
    FillStaticData(&aHit, aStep);
    FillKinematics(&aHit, aStep);
    FillBField(&aHit, aStep);
    aHit.SetStationNumber(station);
    if (!_globalCoordinates) TransformToLocalCoordinates(&aHit);
  }
  catch(Squeal squee) {
    if (squee.GetErrorLevel() == Squeal::nonRecoverable) throw squee;
  }  // catch e.g. tracking errors; don't record a hit in this case
  return aHit;
}

//////////////////////// VirtualPlaneManager //////////////////////////

BTField*             VirtualPlaneManager::_field            = NULL;
VirtualPlaneManager* VirtualPlaneManager::_instance         = NULL;
bool                 VirtualPlaneManager::_useVirtualPlanes = false;
std::vector<VirtualPlane*> VirtualPlaneManager::_planes
                                                = std::vector<VirtualPlane*>(0);
std::vector<int>           VirtualPlaneManager::_nHits = std::vector<int>(0);
std::map<VirtualPlane*, const MiceModule*>  VirtualPlaneManager::_mods;

VirtualPlaneManager::~VirtualPlaneManager() {
  if (this == _instance) {  // always true? leave it here (future proof?)
    _instance = NULL;
    _field = NULL;
    _useVirtualPlanes = false;
    _nHits = std::vector<int>();
    _mods = std::map<VirtualPlane*, const MiceModule*>();
  }
  for (size_t i = 0; i < _planes.size(); ++i) delete _planes[i];
  _planes = std::vector<VirtualPlane*>();
}

VirtualPlaneManager* VirtualPlaneManager::GetInstance() {
  // MUST CALL ConstructVirtualPlanes to do anything useful!
  if (_instance == NULL) {
    _instance = new VirtualPlaneManager();
    return _instance;
  } else {
    return _instance;
  }
}

// Point here is if I go round e.g. a ring, station number should be
// (number_of_passes * number_of_stations) + i
std::vector<VirtualHit> VirtualPlaneManager::VirtualPlanesSteppingAction
                                                         (const G4Step* aStep) {
  std::vector<VirtualHit> hits;
  if (!_useVirtualPlanes) return hits;
  for (unsigned int i = 0; i < _planes.size(); i++)
    if (_planes[i]->SteppingOver(aStep)) {
      if (_nHits[i]>0) {
        if (_planes[i]->GetMultipassAlgorithm() == VirtualPlane::new_station) {
          hits.push_back
              (_planes[i]->BuildNewHit(aStep, _planes.size()*_nHits[i]+i+1));
          _nHits[i]++;
        }
        if (_planes[i]->GetMultipassAlgorithm() == VirtualPlane::same_station) {
          hits.push_back(_planes[i]->BuildNewHit(aStep, i+1));
          _nHits[i]++;
        }
      } else {
        hits.push_back(_planes[i]->BuildNewHit(aStep, i+1));
        _nHits[i]++;
      }
    }
  return hits;
}

void VirtualPlaneManager::StartOfEvent() {
  _nHits = std::vector<int>(_planes.size(), 0);
}

void VirtualPlaneManager::ConstructVirtualPlanes
                                          (BTField* field, MiceModule* model) {
  VirtualPlaneManager::GetInstance();
  std::vector<const MiceModule*> modules   =
             model->findModulesByPropertyString("SensitiveDetector", "Virtual");
  std::vector<const MiceModule*> envelopes =
            model->findModulesByPropertyString("SensitiveDetector", "Envelope");
  modules.insert(modules.end(), envelopes.begin(), envelopes.end());
  for (unsigned int i = 0; i < modules.size(); i++) {
    _planes.push_back(new VirtualPlane(ConstructFromModule(modules[i])));
    _mods[_planes.back()] = modules[i];
  }
  sort(_planes.begin(), _planes.end(), VirtualPlane::ComparePosition);
  _useVirtualPlanes = _planes.size() > 0;
  _field = field;
  _nHits = std::vector<int>(_planes.size(), 0);
  if (_field == NULL) _field = &_instance->_default_field;
  VirtualPlane::_field = _field;
}

VirtualPlane VirtualPlaneManager::ConstructFromModule(const MiceModule* mod) {
    std::string variable          = "z";
    double      radialExtent      = -1.;
    double      indie             = 0.;
    bool        globalCoordinates = true;
    bool        allowBackwards    = true;
    if (mod->propertyExistsThis("IndependentVariable", "string"))
      variable = mod->propertyStringThis("IndependentVariable");
    if (mod->propertyExistsThis("RadialExtent", "double"))
      radialExtent = mod->propertyDoubleThis("RadialExtent");
    if (mod->propertyExistsThis("GlobalCoordinates", "bool"))
      globalCoordinates = mod->propertyBoolThis("GlobalCoordinates");
    VirtualPlane::multipass_handler pass = VirtualPlane::ignore;
    if (mod->propertyExistsThis("MultiplePasses", "string")) {
      std::string m_pass = mod->propertyString("MultiplePasses");
      /**/ if (m_pass == "Ignore")      pass = VirtualPlane::ignore;
      else if (m_pass == "SameStation") pass = VirtualPlane::same_station;
      else if (m_pass == "NewStation")  pass = VirtualPlane::new_station;
      else
        throw(Squeal(Squeal::recoverable,
                 "Did not recognise MultiplePasses option "+m_pass,
                 "VirtualPlaneManager::ConstructFromModule") );
    }
    if (mod->propertyExistsThis("AllowBackwards", "bool"))
      allowBackwards = mod->propertyBoolThis("AllowBackwards");

    BTTracker::var var_enum  = BTTracker::z;
    for (unsigned int i = 0; i < variable.size(); i++)
      variable[i] = tolower(variable[i]);
    if     (variable == "time" || variable == "t") {
      indie = mod->propertyDoubleThis("PlaneTime");
      var_enum = BTTracker::t;
    } else if (variable == "tau") {
      indie = mod->propertyDoubleThis("PlaneTime");
      var_enum = BTTracker::tau_field;
    } else if (variable == "z") {
      indie    = mod->globalPosition()[2];
      var_enum = BTTracker::z;
    } else if (variable == "u") {
      var_enum = BTTracker::u;
    } else {
      throw(Squeal(Squeal::recoverable,
            "Did not recognise IndependentVariable in Virtual detector module "+
            mod->fullName(),
            "VirtualPlaneManager::ConstructFromModule"));
    }
    return VirtualPlane::BuildVirtualPlane(mod->globalRotation(),
                         mod->globalPosition(), radialExtent, globalCoordinates,
                         indie, var_enum, pass, allowBackwards);
}

const MiceModule*  VirtualPlaneManager::GetModuleFromStationNumber
                                                           (int stationNumber) {
  if (_planes.size() == 0)
    throw(Squeal(Squeal::recoverable, "No Virtual planes initialised",
                 "VirtualPlaneManager::Module"));
  if (stationNumber < 1)
    throw(Squeal(Squeal::recoverable,
    "Station number must be > 0",
    "VirtualPlaneManager::Module"));
  // map from module name to _planes index; if stationNumber > planes.size, it
  // means we've gone round twice or more, subtract off
  while (stationNumber > static_cast<int>(_planes.size()))
    stationNumber -= static_cast<int>(_planes.size());
  return _mods[ _planes[stationNumber-1] ];
}

int VirtualPlaneManager::GetStationNumberFromModule
                                                    (const MiceModule* module) {
  if (_planes.size() == 0)
    throw(Squeal(Squeal::recoverable,
          "No Virtual planes initialised",
          "VirtualPlaneManager::ModuleName"));
  VirtualPlane* plane = NULL;
  typedef std::map<VirtualPlane*, const MiceModule*>::iterator map_it;
  for (map_it it = _mods.begin(); it != _mods.end() && plane == NULL; it++)
    if (it->second == module) plane = it->first;
  for (size_t i = 0; i < _planes.size(); i++)
    if (plane == _planes[i]) return i+1;
  throw(Squeal(Squeal::recoverable,
               "Failed to find VirtualPlane for module "+module->fullName(),
               "VirtualPlaneManager::ModuleName"));
}

int VirtualPlaneManager::GetNumberOfHits(int stationNumber) {
    if (stationNumber-1 >= _nHits.size() || stationNumber-1 < 0)
      throw(Squeal(
              Squeal::recoverable,
              "Station number out of range",
              "VirtualPlaneManager::GetNumberOfHits"));
    return _nHits[stationNumber-1];
  }

