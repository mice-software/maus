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
  enum stepping{integrate, linear_interpolate};

  /** enumeration to control how VirtualPlane handles multiple hits on the same
   *  station
   */
  enum multipass_handler{ignore, new_station, same_station};

  /** @brief Default constructor initialises to 0. 
   */
  VirtualPlane ();

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
  static VirtualPlane BuildVirtualPlane (CLHEP::HepRotation rot, CLHEP::Hep3Vector pos, double radialExtent, bool globalCoordinates, 
                                         double indie, BTTracker::var type, multipass_handler mp, bool allowBackwards);

  /** @brief Return the independent variable for the point
   *
   *  Calculate the independent variable for aPoint; either the time, proper
   *  time, z-position, or z-position in the rotated coordinate system
   */
  double GetIndependentVariable(G4StepPoint* aPoint) const;

  /** @brief Return true if aStep steps over the virtual plane
   *
   *  Calculate the independent variable for aStep pre and post points; return
   *  true if the points straddle the virtual plane; note that if allow
   *  backwards is "false" then we also have condition that post point has to be
   *  after the virtual plane and pre point has to be before the virtual plane 
   */
  bool SteppingOver(const G4Step* aStep) const;

  /** @brief Use the stepping data to build a new hit.
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
   *  and rotation of the virtual plane.
   *
   *  If _radialExtent is positive checks radius of particles (cylindrical
   *  type radius, in coordinate system of the Virtual Plane).
   */
  VirtualHit BuildNewHit (const G4Step * aStep, int station) const;
  //Algorithm for handling of multiple passes
  multipass_handler GetMultipassAlgorithm() {return _multipass;}

  static bool ComparePosition(VirtualPlane* p1, VirtualPlane* p2) {return p1->_independentVariable < p2->_independentVariable;}

  bool InRadialCut   (CLHEP::Hep3Vector position) const;

private:
  //Build a new hit and send it to the MICEEvent
  void FillBField    (VirtualHit * aHit, const G4Step * aStep) const;
  void FillStaticData(VirtualHit * aHit, const G4Step * aStep) const;
  void FillKinematics(VirtualHit * aHit, const G4Step * aStep) const;

  double*   Integrate       (G4StepPoint* aPoint) const;
  double*   ExtractPointData(G4StepPoint* aPoint) const;

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

/** @class VirtualPlaneManager
 *
 *  Manages the VirtualPlanes - handles interface with MiceModules and mapping
 *  from station number to MiceModule* (used by Optics optimiser for example).
 */

class VirtualPlaneManager
{
public:
  // BUG leaks virtual planes and potentially _field
  // BUG can leave floating _instance; should set _instance to NULL if this==_instance

  /** @brief Destructor - Does nothing
   */
  ~VirtualPlaneManager()      {;}

  // BUG can we call this getInstance

  /** @brief Call the singleton VirtualPlaneManager
   *
   *  Return a pointer to the VirtualPlaneManager. If the VirtualPlaneManager
   *  does not exist, this will new the VirtualPlaneManager; but need to call
   *  ConstructVirtualPlanes(...) to set up VirtualPlanes.
   */
  static VirtualPlaneManager* getVirtualPlaneManager();

  /** @brief Construct the VirtualPlanes
   *
   *  Looks through the MiceModules for VirtualPlanes and builds them.
   *
   *  @params field pointer to the global field group. If this is NULL, will
   *          make an empty field.
   *  @params model pointer to the global model. Will construct virtual planes
   *          off any Module with "PropertyString SensitiveDetector Envelope" or
   *          "PropertyString SensitiveDetector Virtual"
   */
  static void ConstructVirtualPlanes(BTField* field, MiceModule* model);

  /** @brief Check to see if a step straddles a VirtualPlane
   *
   *  Looks through the list of virtual planes and checks if the step straddles
   *  the plane
   *
   *  @params aStep Check whether presteppoint and poststeppoint sit on either
   *          side of one (or more) virtual planes; try to find the VirtualHit
   *          if this is the case
   */
  static void VirtualPlanesSteppingAction(const G4Step * aStep);

  /** @brief Clear record of Virtual Hits for the next track
   *
   *  Each VirtualPlane stores an index of the number of hits. This is used e.g.
   * to allocate station number and reject hits if the multipass_handler is set
   * to 0.
   */
  static void StartOfEvent();

  // BUG rename to GetField()
  /** @brief Return a pointer to the field object used for tracking
   */
  static BTField* Field()               { return _field;}

  // BUG rename to SetField()
  /** @brief Set the pointer to the field object used for tracking
   */
  static BTField* Field(BTField* field) {_field = field; VirtualPlane::_field = field; return _field;}

  /** @brief Get a string that stores the MiceModule based on StationNumber
   */
  static std::string         ModuleName   (int stationNumber);

  /** @brief Get a pointer to the MiceModule based on StationNumber
   */
  static const MiceModule*   Module       (int stationNumber);

  /** @brief Get the StationNumber based on a pointer to the MiceModule
   */
  static int                 StationNumber(const MiceModule* module);
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
