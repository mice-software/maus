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

#include "json/json.h"

#include "src/common_cpp/Simulation/VirtualPlanes.hh"
#include "src/legacy/Interface/VirtualHit.hh"
#include "src/legacy/Config/MiceModule.hh"
#include "src/legacy/Interface/Squeal.hh"

#include "src/legacy/BeamTools/BTField.hh"
#include "src/legacy/BeamTools/BTTracker.hh"

namespace MAUS {

const BTField*         VirtualPlane::_field    = NULL;
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
  _multipass = VirtualPlane::ignore;
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
  FillStaticData(&aHit, aStep);
  FillKinematics(&aHit, aStep);
  FillBField(&aHit, aStep);
  aHit.SetStationNumber(station);
  if (!_globalCoordinates) TransformToLocalCoordinates(&aHit);
  return aHit;
}

//////////////////////// VirtualPlaneManager //////////////////////////

const BTFieldGroup VirtualPlaneManager::_default_field;

VirtualPlaneManager::VirtualPlaneManager() : _field(NULL),
      _useVirtualPlanes(false), _planes(), _mods(), _nHits(0),
      _hits(Json::arrayValue) {
}

VirtualPlaneManager::~VirtualPlaneManager() {
  _field = NULL;
  _useVirtualPlanes = false;
  _nHits = std::vector<int>();
  _mods = std::map<VirtualPlane*, const MiceModule*>();
  for (size_t i = 0; i < _planes.size(); ++i) delete _planes[i];
  _planes = std::vector<VirtualPlane*>();
}

// Point here is if I go round e.g. a ring, station number should be
// (number_of_passes * number_of_stations) + i
void VirtualPlaneManager::VirtualPlanesSteppingAction
                                    (const G4Step* aStep) {
  if (!_useVirtualPlanes) return;
  for (unsigned int i = 0; i < _planes.size(); i++)
    try {
      if (_planes[i]->SteppingOver(aStep)) {
        VirtualPlane::multipass_handler mp =
                                            _planes[i]->GetMultipassAlgorithm();
        if (_nHits[i]>0) {
          if (mp == VirtualPlane::new_station) {
            _hits.append(WriteHit(_planes[i]->BuildNewHit(aStep,
                                         _planes.size()*_nHits[i]+i+1)));
            _nHits[i]++;
          }
          if (mp == VirtualPlane::same_station) {
            _hits.append(WriteHit(_planes[i]->BuildNewHit(aStep, i+1)));
            _nHits[i]++;
          }
        } else {
          _hits.append(WriteHit(_planes[i]->BuildNewHit(aStep, i+1)));
          _nHits[i]++;
        }
      }
    } catch(Squeal squee) {}  // do nothing - just dont make a hit
}

void VirtualPlaneManager::SetVirtualHits(Json::Value hits) {
  if (!hits.isArray())
    throw(Squeal(Squeal::recoverable, "Virtual hits must be of array type",
          "VirtualPlaneManager::SetVirtualHits()"));
  _hits = hits;
}

void VirtualPlaneManager::StartOfEvent() {
  _nHits = std::vector<int>(_planes.size(), 0);
  SetVirtualHits(Json::Value(Json::arrayValue));
}

void VirtualPlaneManager::ConstructVirtualPlanes
                                     (const BTField* field, MiceModule* model) {
  std::vector<const MiceModule*> modules   =
             model->findModulesByPropertyString("SensitiveDetector", "Virtual");
  std::vector<const MiceModule*> envelopes =
            model->findModulesByPropertyString("SensitiveDetector", "Envelope");
  modules.insert(modules.end(), envelopes.begin(), envelopes.end());
  for (unsigned int i = 0; i < modules.size(); i++) {
    AddPlane(new VirtualPlane(ConstructFromModule(modules[i])), modules[i]);
  }
  _field = field;
  if (_field == NULL) _field = &_default_field;
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


void VirtualPlaneManager::AddPlane
                               (VirtualPlane* newPlane, const MiceModule* mod) {
  _planes.push_back(newPlane);
  _mods[newPlane] = mod;
  sort(_planes.begin(), _planes.end(), VirtualPlane::ComparePosition);
  _useVirtualPlanes = _planes.size() > 0;
  _nHits = std::vector<int>(_planes.size(), 0);
}


const MiceModule*  VirtualPlaneManager::GetModuleFromStationNumber
                                                           (int stationNumber) {
  return _mods[ PlaneFromStation(stationNumber) ];
}

int VirtualPlaneManager::GetStationNumberFromModule(const MiceModule* module) {
  if (_planes.size() == 0)
    throw(Squeal(Squeal::recoverable,
          "No Virtual planes initialised",
          "VirtualPlaneManager::GetStationNumberFromModule"));
  VirtualPlane* plane = NULL;
  typedef std::map<VirtualPlane*, const MiceModule*>::iterator map_it;
  for (map_it it = _mods.begin(); it != _mods.end() && plane == NULL; it++)
    if (it->second == module) plane = it->first;  // find plane from module
  if (plane == NULL) {
    throw(Squeal(Squeal::recoverable,
          "Module "+module->name()+" not found in VirtualPlaneManager",
          "VirtualPlaneManager::GetStationNumberFromModule"));
  }
  for (size_t i = 0; i < _planes.size(); i++)
    if (plane == _planes[i]) return i+1;  // find station from plane
  throw(Squeal(Squeal::recoverable,
        "Module "+module->name()+" not found in VirtualPlaneManager",
        "VirtualPlaneManager::GetStationNumberFromModule"));
}

int VirtualPlaneManager::GetNumberOfHits(int stationNumber) {
    if (stationNumber-1 >= static_cast<int>(_nHits.size()) ||
        stationNumber-1 < 0)
      throw(Squeal(
              Squeal::recoverable,
              "Station number out of range",
              "VirtualPlaneManager::GetNumberOfHits"));
    return _nHits[stationNumber-1];
}

Json::Value VirtualPlaneManager::WriteHit(VirtualHit hit) {
    Json::Value hit_v = Json::Value(Json::objectValue);
    hit_v["station_id"] = hit.GetStationNumber();
    hit_v["time"] = hit.GetTime();
    hit_v["particle_id"] = hit.GetPID();
    hit_v["track_id"] = hit.GetTrackID();
    hit_v["mass"] = hit.GetMass();
    hit_v["charge"] = hit.GetCharge();
    hit_v["position"] = Json::Value(Json::objectValue);
    hit_v["position"]["x"] = hit.GetPos().x();
    hit_v["position"]["y"] = hit.GetPos().y();
    hit_v["position"]["z"] = hit.GetPos().z();
    hit_v["momentum"] = Json::Value(Json::objectValue);
    hit_v["momentum"]["x"] = hit.GetMomentum().x();
    hit_v["momentum"]["y"] = hit.GetMomentum().y();
    hit_v["momentum"]["z"] = hit.GetMomentum().z();
    hit_v["proper_time"] = hit.GetProperTime();
    hit_v["path_length"] = hit.GetPathLength();
    hit_v["b_field"] = Json::Value(Json::objectValue);
    hit_v["b_field"]["x"] = hit.GetBField().x();
    hit_v["b_field"]["y"] = hit.GetBField().y();
    hit_v["b_field"]["z"] = hit.GetBField().z();
    hit_v["e_field"] = Json::Value(Json::objectValue);
    hit_v["e_field"]["x"] = hit.GetEField().x();
    hit_v["e_field"]["y"] = hit.GetEField().y();
    hit_v["e_field"]["z"] = hit.GetEField().z();
    return hit_v;
}

VirtualHit VirtualPlaneManager::ReadHit(Json::Value v_hit) {
    Json::Value stationId =
         JsonWrapper::GetProperty(v_hit, "station_id", JsonWrapper::intValue);
    Json::Value trackId =
         JsonWrapper::GetProperty(v_hit, "track_id", JsonWrapper::intValue);
    Json::Value pid =
         JsonWrapper::GetProperty(v_hit, "particle_id", JsonWrapper::intValue);

    Json::Value time =
         JsonWrapper::GetProperty(v_hit, "time", JsonWrapper::realValue);
    Json::Value mass =
         JsonWrapper::GetProperty(v_hit, "mass", JsonWrapper::realValue);
    Json::Value charge =
         JsonWrapper::GetProperty(v_hit, "charge", JsonWrapper::realValue);
    Json::Value tau =
         JsonWrapper::GetProperty(v_hit, "proper_time", JsonWrapper::realValue);
    Json::Value len =
         JsonWrapper::GetProperty(v_hit, "path_length", JsonWrapper::realValue);
    Json::Value pos_v =
         JsonWrapper::GetProperty(v_hit, "position", JsonWrapper::objectValue);
    Json::Value mom_v =
         JsonWrapper::GetProperty(v_hit, "momentum", JsonWrapper::objectValue);
    Json::Value b_v =
         JsonWrapper::GetProperty(v_hit, "b_field", JsonWrapper::objectValue);
    Json::Value e_v =
         JsonWrapper::GetProperty(v_hit, "e_field", JsonWrapper::objectValue);

    VirtualHit hit;
    hit.SetStationNumber(stationId.asInt());
    hit.SetTrackID(trackId.asInt());
    hit.SetPID(pid.asInt());

    hit.SetTime(time.asDouble());
    hit.SetMass(mass.asDouble());
    hit.SetCharge(charge.asDouble());
    hit.SetProperTime(tau.asDouble());
    hit.SetPathLength(len.asDouble());

    hit.SetPos(JsonWrapper::JsonToThreeVector(pos_v));
    hit.SetMomentum(JsonWrapper::JsonToThreeVector(mom_v));
    hit.SetBField(JsonWrapper::JsonToThreeVector(b_v));
    hit.SetEField(JsonWrapper::JsonToThreeVector(e_v));
    hit.SetEnergy(sqrt(hit.GetMomentum().mag2()+hit.GetMass()*hit.GetMass()));
    return hit;
}

VirtualPlane* VirtualPlaneManager::PlaneFromStation(int stationNumber) {
    if (_planes.size() == 0)
      throw(Squeal(Squeal::recoverable, "No Virtual planes initialised",
                   "VirtualPlaneManager::PlaneFromStation()"));
    if (stationNumber < 1)
      throw(Squeal(Squeal::recoverable,
      "Station number must be > 0",
      "VirtualPlaneManager::PlaneFromStation"));
    // map from module name to _planes index; if stationNumber > planes.size, it
    // means we've gone round twice or more, subtract off
    while (stationNumber > static_cast<int>(_planes.size()))
      stationNumber -= static_cast<int>(_planes.size());
    return _planes[stationNumber-1];
}

void VirtualPlaneManager::RemovePlanes(std::set<int> station) {
    std::set<VirtualPlane*> targets;
    std::set<int>::iterator it1;
    for (it1 = station.begin(); it1 != station.end(); ++it1)
      targets.insert(PlaneFromStation(*it1));

    std::set<VirtualPlane*>::iterator it2;
    for (it2 = targets.begin(); it2 != targets.end(); ++it2)
      RemovePlane(*it2);
}

void VirtualPlaneManager::RemovePlane(VirtualPlane* plane) {
    std::vector<VirtualPlane*>::iterator it =
                               std::find(_planes.begin(), _planes.end(), plane);
    _planes.erase(it);
    _mods.erase(plane);
    sort(_planes.begin(), _planes.end(), VirtualPlane::ComparePosition);
    _useVirtualPlanes = _planes.size() > 0;
    _nHits = std::vector<int>(_planes.size(), 0);
    delete plane;
}
}

