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

/** 
 *  @file GlobalTools.hh
 *  @brief A collection of helper functions for GlobalEvent objects
 *  @author Jan Greis, University of Warwick
 *  @date 2015/04/17
 */

#ifndef _SRC_COMMON_CPP_RECON_GLOBALTOOLS_HH_
#define _SRC_COMMON_CPP_RECON_GLOBALTOOLS_HH_

// C++ headers
#include <map>
#include <vector>
#include <string>

// Geant4 headers
#include "Geant4/G4Material.hh"

// MAUS headers
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/GlobalEvent.hh"
#include "src/common_cpp/DataStructure/Global/Track.hh"
#include "src/common_cpp/DataStructure/Global/TrackPoint.hh"
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"

// Forward declaration
class BTField;

namespace MAUS {
namespace GlobalTools {

/**
 * @brief Returns a map of all detector points (as defined by
 * DataStructure/Global/ReconEnums.hh) with flags for whether a Track or
 * SpacePoint exists in the reconstructed event for each detector.
 * 
 * @param global_event The global event
 */
std::map<DataStructure::Global::DetectorPoint, bool>
    GetReconDetectors(GlobalEvent* global_event);

/**
 * @brief Returns a vector of all Tracks in the Spill (i.e. across recon events
 * if applicable) that have the given mapper name and include TrackPoints in
 * the given detector
 * 
 * @param spill The spill
 * @param detector Detector enum for the selection
 * @param mapper_name Mapper name for the selection
 */
std::vector<DataStructure::Global::Track*>* GetSpillDetectorTracks(
    Spill* spill, DataStructure::Global::DetectorPoint detector,
    std::string mapper_name);

std::vector<DataStructure::Global::Track*>* GetEventDetectorTracks(
    GlobalEvent* global_event, DataStructure::Global::DetectorPoint detector,
    std::string mapper_name);

/**
 * @brief Returns a vector of all SpacePoints in a spill (i.e. across recon
 * events) for a given detector
 * 
 * @param spill The spill
 * @param detector Detector enum for the selection
 */
std::vector<DataStructure::Global::SpacePoint*>* GetSpillSpacePoints(
    Spill* spill, DataStructure::Global::DetectorPoint detector);

std::vector<DataStructure::Global::SpacePoint*>* GetEventSpacePoints(
    GlobalEvent* global_event, DataStructure::Global::DetectorPoint detector);

/**
 * @brief Returns a vector of all Tracks in in the global event with the given
 * mapper name
 * 
 * @param global_even The global event
 * @param mapper_name Mapper name for the selection
 */
std::vector<DataStructure::Global::Track*>* GetTracksByMapperName(
    GlobalEvent* global_event, std::string mapper_name);

/**
 * @brief Returns a vector of all Tracks in in the global event with the given
 * mapper name and the given PID
 * 
 * @param global_even The global event
 * @param mapper_name Mapper name for the selection
 * @param pid PID for the selection
 */
std::vector<DataStructure::Global::Track*>* GetTracksByMapperName(
    GlobalEvent* global_event, std::string mapper_name,
    DataStructure::Global::PID pid);

/**
 * @brief Returns a vector of ints denoting the tracker plane of a TrackPoint.
 * The first number indicates the tracker (0-1), the second the tracker station
 * (1-5), the third the tracker plane (0-2). For the latter two, numbers
 * increase away from the absorber
 * 
 * @param track_point The trackpoint for which to determine the tracker plane
 * @param z_positions An ascending-sorted vector of the z positions of all
 * tracker planes from the geometry.
 *
 * @see GetTrackerPlaneZPositions()
 */
std::vector<int> GetTrackerPlane(const DataStructure::Global::TrackPoint*
    track_point, std::vector<double> z_positions);

/**
 * @brief Returns a vector with the z positions of all tracker planes sorted
 * by ascending z position.
 *
 * @param geo_filename The filename of the used geometry file
 *
 * @see GetTrackerPlane()
 */
std::vector<double> GetTrackerPlaneZPositions(std::string geo_filename);

/**
 * @brief Returns the TrackPoint from a Track that is nearest to a given z
 * position.
 *
 * If the most upstream or most downstream TrackPoint should be returned, set
 * z_positions such as 0.0 or 1000000
 * 
 * @param track The Track from which to return the TrackPoint
 * @param z_position The z position to which the nearest TrackPoint should be
 * returned
 */
DataStructure::Global::TrackPoint* GetNearestZTrackPoint(
    const DataStructure::Global::Track* track, double z_position);

/**
 * @brief Checks whether two numbers are the same to within a given tolerance
 */
bool approx(double a, double b, double tolerance);

/**
 * @brief Calculates energy loss according to the Bethe-Bloch equation. This is
 * also implemented in G4hBetheBlochModel but we don't want to have to
 * instantiate a G4DynamicParticle object every time. Note that the output is in
 * MeV/mm, not MeV cm^2/g
 *
 * @param material the material the particle is passing through
 * @param the total energy of the particle
 * @param mass the mass of the particle
 */
double dEdx(const G4Material* material, double energy, double mass);

/**
 * @brief Reimplementation of BTTracker::Integrate with only z propagation and
 * the option of energy loss. Reimplemented to avoid changing the interface that
 * might still be used by other functions and to pull it out of legacy
 *
 * @param x array of size 8 containing t, x, y, z, E, px, py, pz
 * @param target_z z coordinate to propagate to
 * @param field the magnetic field through which the particle is propagated
 * @param step_size the maximum step size in mm for the propagation
 * @param pid the PID of the particle being propagated
 * @param energy_loss whether energy loss should be calculated
 * @param algorithm - choose how the geometry lookup is done; 0 is full geant4
 *        lookup; 1 is axial_lookup; 2 is alternative full geant4 lookup.
 */
void propagate(double* x, double target_z, const BTField* field,
               double step_size, DataStructure::Global::PID pid,
               bool energy_loss = true, int geometry_algorithm = 0);

/** As per propage(...), but using CentroidTracking routine */
void propagate2(double* x, double target_z, const BTField* field,
               double step_size, DataStructure::Global::PID pid,
               bool energy_loss, int geometry_algorithm);

/**
 * @brief Required by GlobalTools::propagate()
 */
int z_equations_of_motion(double z, const double x[8], double dxdt[8],
                          void* params);

/**
 * @brief Scales the 4-momentum of the 8-vector to decrease the energy by deltaE
 *
 * @param x array of size 8 containing t, x, y, z, E, px, py, pz
 * @param deltaE change in energy applied to the particle
 * @param mass mass of the particle
 */
void changeEnergy(double* x, double deltaE, double mass);

/**
 * @brief Returns whether the first TrackPoint has a lower z-position than the
 * second one. Used to perform an std::sort on a vector of TrackPoints.
 */
bool TrackPointSort(const DataStructure::Global::TrackPoint* tp1,
                    const DataStructure::Global::TrackPoint* tp2);
} // namespace GlobalTools
} // namespace MAUS

#endif
