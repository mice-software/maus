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

// TODO(rogers) - multipass logic should be handled in the plane itself; at the
//                moment it's handled in the manager

#ifndef _SRC_CPP_CORE_INTERFACE_VIRTUALPLANES_HH_
#define _SRC_CPP_CORE_INTERFACE_VIRTUALPLANES_HH_

#include <set>
#include <vector>
#include <map>
#include <string>

#include "json/json.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "src/legacy/Interface/VirtualHit.hh"
#include "src/legacy/BeamTools/BTTracker.hh"
#include "src/legacy/BeamTools/BTFieldGroup.hh"

#include "src/common_cpp/Utils/JsonWrapper.hh"

class G4Step;
class G4StepPoint;
class MiceModule;

namespace MAUS {

class VirtualPlane;
class VirtualPlaneManager;

class VirtualPlane {
  /** @class VirtualPlane
   *
   *  Looks for steps in the geant4 data that straddle one of our virtual
   *  planes. Then integrate (track) off the plane to the virtual plane. We
   *  track from both the PreStepPoint forwards and the PostStepPoint backwards.
   *  We take the  average (thus getting average of any stochastic processes on
   *  the step).
   */

 public:
  /** enumeration to control how VirtualPlane handles the interpolation between
   *  step start and end
   */
  enum stepping {integrate, linear_interpolate};

  /** enumeration to control how VirtualPlane handles multiple hits on the same
   *  station
   */
  enum multipass_handler {ignore, new_station, same_station};

  /** @brief Default constructor initialises to 0.
   */
  VirtualPlane();

  /** @brief Copy constructor makes a deepcopy of the virtual plane */
  VirtualPlane(const VirtualPlane& vplane);

  /** @brief Destructor; no memory allocated so does nothing.
   */
  ~VirtualPlane() {}

  // BUG - do constructor then init function - more "standard"

  /** @brief Set up a VirtualPlane
   *
   *  @params rot Rotation of the VirtualPlane
   *  @params pos Position of the VirtualPlane used to calculate independent
   *          variable and apply radial cuts
   *  @params radialExtent radial extent of the virtual plane; set to negative
   *          to make no radial cut
   *  @params globalCoordinates set to True to record output in
   *          globalCoordinates; set to False to record output in the translated
   *          and rotated frame of the Virtual
   *  @params independent variable (z, t, tau_field, u) at which the
   *          VirtualPlane will register hits
   *  @params type independent variable type; either z (z-axis position), t
   *          (time), tau (proper time), u (position in rotated coordinates)
   *  @params multipass_handler set how we handle multiple passes. If set to
   *          ignore, only first hit is registered; if set to new_station, we
   *          make a new station with station = N(stations)*N(hits)+station; if
   *          set to same_station, just record the hit as normal
   *  @params allowBackwards; set to True to register hits for backwards going
   *          particles as well as forwards going particles.
   */
  static VirtualPlane BuildVirtualPlane(::CLHEP::HepRotation rot,
                      ::CLHEP::Hep3Vector pos, double radialExtent,
                      bool globalCoordinates, double indie, BTTracker::var type,
                      multipass_handler mp, bool allowBackwards);

  /** @brief Return the independent variable for the point
   *
   *  @params aPoint calculate independent variable for this StepPoint
   *
   *  Calculate the independent variable for aPoint; either the time, proper
   *  time, z-position, or z-position in the rotated coordinate system
   */
  double GetIndependentVariable(G4StepPoint* aPoint) const;

  /** @brief Return true if aStep steps over the virtual plane
   *
   *  @params aStep check if plane is between Pre and Post step
   *
   *  Calculate the independent variable for aStep pre and post points; return
   *  true if the points straddle the virtual plane; note that if allow
   *  backwards is "false" then we also have condition that post point has to be
   *  after the virtual plane and pre point has to be before the virtual plane
   */
  bool SteppingOver(const G4Step* aStep) const;

  /** @brief Use the stepping data to build a new hit.
   *
   *  @params aStep interpolate between Pre and Post step to find the hit
   *  @params station set the station number (station number is handled by the
   *          VirtualPlaneManager right now)
   *
   *  Builds a new hit using stepping data. If _stepping is set to integrate,
   *  uses BTTracker routines and integrates Lorentz equation (or whatever) to
   *  the plane in z or other independent variable. Then takes the average of
   *  the tracked pre-step or post-step point, to take account of any tracking
   *  errors or physics processes (other than basic tracking)
   *
   *  If _stepping is set to linear_interpolate, just takes the linear
   *  interpolation from the pre-step point to the post-step point.
   *
   *  If _globalCoordinates is true, builds a hit with coordinates in the global
   *  system; if false, builds a hit with coordinates relative to the position
   *  and rotation of the virtual plane (position, momentum, fields)
   *
   *  If _radialExtent is positive checks radius of particles (cylindrical
   *  type radius, in coordinate system of the Virtual Plane).
   */
  VirtualHit BuildNewHit(const G4Step * aStep, int station) const;

  /** @brief Comparator for sorting by independent variable
   */
  static bool ComparePosition(VirtualPlane* p1, VirtualPlane* p2) {
    return p1->_independentVariable < p2->_independentVariable;
  }

  /** @brief Return true if a position is inside the radial cut
   *
   *  @params position transform to local coordinates and use the radius (given
   *          by r**2 = x**2+y**2) to perform a cut
   *
   *  Uses the _radialExtent to perform the cut; if _radialExtent <= 0., does
   *  nothing,
   *
   *  @returns true if the particle is in the cut
   */
  bool InRadialCut(::CLHEP::Hep3Vector position) const;

  /** @brief return the type of independent variable of the plane
   */
  BTTracker::var GetPlaneIndependentVariableType() const { return _planeType; }

  /** @brief return the value of independent variable of the plane
   */
  double GetPlaneIndependentVariable() const { return _independentVariable; }

  /** @brief return the Radial Extent (radius cut) of the plane
   */
  double GetRadialExtent() const { return _radialExtent; }

  /** @brief return true if the plane outputs in global coordinates; false if
   *         the plane outputs in local coordinate system
   */
  bool GetGlobalCoordinates() const { return _globalCoordinates; }

  /** @brief returns the algorithm used for handling multiple passes
   */
  multipass_handler GetMultipassAlgorithm() {return _multipass;}

  /** @brief returns the plane rotation
   */
  ::CLHEP::HepRotation GetRotation() {return _rotation;}

  /** @brief returns the plane position
   */
  ::CLHEP::Hep3Vector GetPosition() {return _position;}

  /** @brief return true if backwards going particles are registered on the
   *         plane; false otherwise
   */
  bool GetAllowBackwards() {return _allowBackwards;}

 private:
  // Build a new hit and send it to the MICEEvent
  void FillBField(VirtualHit * aHit, const G4Step * aStep) const;
  void FillStaticData(VirtualHit * aHit, const G4Step * aStep) const;
  void FillKinematics(VirtualHit * aHit, const G4Step * aStep) const;

  void TransformToLocalCoordinates(VirtualHit* aHit) const;

  double*   Integrate(G4StepPoint* aPoint) const;
  double*   ExtractPointData(G4StepPoint* aPoint) const;

  BTTracker::var     _planeType;
  double             _independentVariable;
  double             _step;
  double             _radialExtent;
  bool               _globalCoordinates;
  multipass_handler  _multipass;
  static stepping    _stepping;

  ::CLHEP::Hep3Vector  _position;  // if var is u, then this will give origin
  ::CLHEP::HepRotation _rotation;  // if var is u, then this will give rotation
  bool               _allowBackwards;

  const BTField* GetField() const;

  friend class VirtualPlaneManager;
};

/** @class VirtualPlaneManager
 *
 *  Manages the VirtualPlanes\n
 *   - handles interface with MiceModules used to build
 *     VirtualPlanes\n
 *   - gives mapping from station number to MiceModule* (used by Optics
 *     optimiser for example).\n
 *   - enables setting/getting of field map used to integrate the virtual plane.
 *   - handles multipass logic to decide whether to allocate a step\n
 */

class VirtualPlaneManager {
 public:
  /** @brief Constructor
   */
  VirtualPlaneManager();

  /** @brief Copy constructor makes a deep copy of the Manager */
  VirtualPlaneManager(VirtualPlaneManager& rhs);

  /** @brief Destructor
   *
   *  delete constructed virtual planes. If this is instance, resets all the
   *  static variables to 0. Does not delete MiceModules or field - this is
   *  controlled from elsewhere.
   */
  ~VirtualPlaneManager();

  /** @brief Construct the VirtualPlanes
   *
   *  Looks through the MiceModules for VirtualPlanes and builds them. Planes
   *  are automatically sorted by independent variable. If ConstructVirtualPlanes
   *  is called more than once, replaces existing field pointer and appends any
   *  additional virtual plane definitions.
   *
   *  @params field pointer to the global field group. If this is NULL, will
   *          make an empty field. Caller owns memory allocated to model.
   *  @params model pointer to the global model. Will construct virtual planes
   *          off any Module with "PropertyString SensitiveDetector Envelope" or
   *          "PropertyString SensitiveDetector Virtual". Model is a borrowed
   *          reference. VirtualPlanes hopes field stays alive for as long as
   *          VirtualPlanes are required (eek).
   */
  void ConstructVirtualPlanes(MiceModule* model);

  /** @brief Check to see if a step straddles a VirtualPlane
   *
   *  Looks through the list of virtual planes and checks if the step straddles
   *  the plane. If so, makes a hit.
   *
   *  @params aStep Check whether presteppoint and poststeppoint sit on either
   *          side of one (or more) virtual planes; try to find the VirtualHit
   *          if this is the case
   *  @params track puts tracks into the "Virtual" branch of the track
   */
  void VirtualPlanesSteppingAction(const G4Step * aStep);

  /** @brief Clear count of Virtual Hits for the next track
   *
   * Each VirtualPlane stores a count of the number of hits. This is used e.g.
   * to allocate station number and reject hits if the multipass_handler is set
   * to ignore.
   */
  void StartOfEvent();

  /** @brief Get a pointer to the MiceModule based on StationNumber
   *
   *  @returns MiceModule or NULL if no MiceModule registered
   */
  const MiceModule*   GetModuleFromStationNumber(int stationNumber);

  /** @brief Get the primary StationNumber based on a pointer to the MiceModule
   */
  int GetStationNumberFromModule(const MiceModule* module);

  /** @brief Return a vector of planes controlled by the plane manager
   *
   *  VirtualPlaneManager owns the memory pointed at by GetPlanes()
   */
  std::vector<VirtualPlane*> GetPlanes() {return _planes;}

  /** @brief Get the number of hits recorded on a station based on the primary
   *         station number.
   */
  int GetNumberOfHits(int stationNumber);

  /** @brief Write the hit to the Json::Value
   */
  Json::Value WriteHit(VirtualHit hit);

  /** @brief Read the hit from the Json::Value
   */
  VirtualHit ReadHit(Json::Value value);

  /** @brief Add plane to the manager
   *
   *  @params plane is the plane to be added. Note VirtualPlaneManager now takes
   *          ownership of this memory.
   *  @params mod is the module which made the plane. If no module, just set
   *          this to NULL. This is used subsequently to relate stations to the
   *          originating MiceModule. VirtualPlaneManager does not own this
   *          memory.
   */
  void AddPlane(VirtualPlane* plane, const MiceModule* mod);

  /** @brief Remove planes from the manager
   *
   *  This takes a vector of stations so that several stations can be removed -
   *  note that by removing the planes, the station numbers will change (as
   *  stations are always numbered incrementally from 1)
   *
   *  @param station numbers of the plane to be removed
   */
  void RemovePlanes(std::set<int> station);

  /** @brief Remove a plane from the manager
   *
   *  @param pointer to the plane to be removed. Note that the memory pointed to
   *         is deleted by this function call.
   */
  void RemovePlane(VirtualPlane* plane);

  /** @brief Get Json array of all recorded virtual hits since StartOfEvent()
   */
  std::vector<VirtualHit>* GetVirtualHits() {return _hits;}

  /** @brief Set Json array of virtual hits
   *
   *  @params hits array of virtual hits; if hits is not a Json::arrayValue,
   *          throws an exception
   */
  void SetVirtualHits(std::vector<VirtualHit>* hits);

  /** @brief Get flag to tell whether VirtualPlanes are active
   *
   *  Default is to activate VirtualPlanes if they are found in the geometry
   */
  bool GetWillUseVirtualPlanes() {return _useVirtualPlanes;}

  /** @brief Set flag to tell whether VirtualPlanes are active
   */
  void SetWillUseVirtualPlanes(bool willUse) {_useVirtualPlanes = willUse;}

 private:
  VirtualPlane ConstructFromModule(const MiceModule* mod);
  VirtualPlane* PlaneFromStation(int station);  // get plane from station number

  ::CLHEP::Hep3Vector JsonToThreeVector(Json::Value value, std::string name);

  bool                      _useVirtualPlanes;
  std::vector<VirtualPlane*> _planes;
  // associate MiceModule with each plane in _planes
  std::map<VirtualPlane*, const MiceModule*>  _mods;
  std::vector<int>          _nHits;  // numberOfHits in each plane
  std::vector<VirtualHit>*  _hits;
};
}
#endif
