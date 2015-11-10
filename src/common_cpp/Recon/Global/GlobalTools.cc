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

#include "src/common_cpp/Recon/Global/GlobalTools.hh"
#include "src/legacy/BeamTools/BTField.hh"
#include "src/common_cpp/Recon/Global/Particle.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "Geant4/G4Navigator.hh"
#include "src/common_cpp/Simulation/GeometryNavigator.hh"
#include <Geant4/G4TransportationManager.hh>
#include <Geant4/G4NistManager.hh> 
#include "gsl/gsl_odeiv.h"
#include "gsl/gsl_errno.h"

namespace MAUS {
namespace GlobalTools {

std::map<MAUS::DataStructure::Global::DetectorPoint, bool>
    GetReconDetectors(MAUS::GlobalEvent* global_event) {
  std::map<MAUS::DataStructure::Global::DetectorPoint, bool> recon_detectors;
  for (int i = 0; i < 27; i++) {
    recon_detectors[static_cast<MAUS::DataStructure::Global::DetectorPoint>(i)] =
      false;
  }
  MAUS::DataStructure::Global::TrackPArray* imported_tracks =
    global_event->get_tracks();

  MAUS::DataStructure::Global::TrackPArray::iterator imported_track_iter;
  for (imported_track_iter = imported_tracks->begin();
       imported_track_iter != imported_tracks->end();
       ++imported_track_iter) {
    std::vector<MAUS::DataStructure::Global::DetectorPoint> track_detectors =
        (*imported_track_iter)->GetDetectorPoints();
    for (size_t i = 0; i < track_detectors.size(); i++) {
      recon_detectors[track_detectors[i]] = true;
    }
  }

  std::vector<MAUS::DataStructure::Global::SpacePoint*>*
      imported_spacepoints = global_event->get_space_points();
  std::vector<MAUS::DataStructure::Global::SpacePoint*>::iterator sp_iter;
  for (sp_iter = imported_spacepoints->begin();
       sp_iter != imported_spacepoints->end();
       ++sp_iter) {
    recon_detectors[(*sp_iter)->get_detector()] = true;
  }
  return recon_detectors;
}

std::vector<MAUS::DataStructure::Global::Track*>* GetSpillDetectorTracks(
    MAUS::Spill* spill, MAUS::DataStructure::Global::DetectorPoint detector,
    std::string mapper_name) {
  std::vector<MAUS::DataStructure::Global::Track*>* detector_tracks = new
      std::vector<MAUS::DataStructure::Global::Track*>;
  ReconEventPArray* recon_events = spill->GetReconEvents();
  if (recon_events) {
    ReconEventPArray::iterator recon_event_iter;
    for (recon_event_iter = recon_events->begin();
         recon_event_iter != recon_events->end();
         ++recon_event_iter) {
      GlobalEvent* global_event = (*recon_event_iter)->GetGlobalEvent();
      if (global_event) {
        std::vector<MAUS::DataStructure::Global::Track*>* global_tracks =
            global_event->get_tracks();
        std::vector<MAUS::DataStructure::Global::Track*>::iterator track_iter;
        for (track_iter = global_tracks->begin();
             track_iter != global_tracks->end();
             ++track_iter) {
          // The third condition is a bit of a dirty hack here to make sure that
          // if we select for EMR tracks, we only get primaries.
          if (((*track_iter)->HasDetector(detector)) and
              ((*track_iter)->get_mapper_name() == mapper_name) and
              ((*track_iter)->get_emr_range_secondary() < 0.001)) {
            detector_tracks->push_back((*track_iter));
          }
        }
      }
    }
  }
  return detector_tracks;
}

std::vector<MAUS::DataStructure::Global::Track*>* GetTracksByMapperName(
    MAUS::GlobalEvent* global_event,
    std::string mapper_name) {
  std::vector<MAUS::DataStructure::Global::Track*>* global_tracks =
          global_event->get_tracks();
  std::vector<MAUS::DataStructure::Global::Track*>* selected_tracks = new
      std::vector<MAUS::DataStructure::Global::Track*>;
  std::vector<MAUS::DataStructure::Global::Track*>::iterator
      global_track_iter;
  for (global_track_iter = global_tracks->begin();
       global_track_iter != global_tracks->end();
       ++global_track_iter) {
    if ((*global_track_iter)->get_mapper_name() == mapper_name) {
      selected_tracks->push_back(*global_track_iter);
    }
  }
  return selected_tracks;
}

std::vector<MAUS::DataStructure::Global::Track*>* GetTracksByMapperName(
    MAUS::GlobalEvent* global_event,
    std::string mapper_name,
    MAUS::DataStructure::Global::PID pid) {
  std::vector<MAUS::DataStructure::Global::Track*>* global_tracks =
          global_event->get_tracks();
  std::vector<MAUS::DataStructure::Global::Track*>* selected_tracks = new
      std::vector<MAUS::DataStructure::Global::Track*>;
  std::vector<MAUS::DataStructure::Global::Track*>::iterator
      global_track_iter;
  for (global_track_iter = global_tracks->begin();
       global_track_iter != global_tracks->end();
       ++global_track_iter) {
    if ((*global_track_iter)->get_mapper_name() == mapper_name) {
      if ((*global_track_iter)->get_pid() == pid) {
        selected_tracks->push_back(*global_track_iter);
      }
    }
  }
  return selected_tracks;
}

// TODO Need to get this geometry-independent
std::vector<int> GetTrackerPlane(const MAUS::DataStructure::Global::TrackPoint*
    track_point) {
  std::vector<int> tracker_plane (3,0);
  double z = track_point->get_position().Z();
  if (approx(z, 11755, 555)) {
    tracker_plane[0] = 0;
    if (approx(z, 11206, 5)) {
      tracker_plane[1] = 5;
      if (approx(z, 11205.7, 0.2)) {
        tracker_plane[2] = 2;
      } else if (approx(z, 11206.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 11207.0, 0.2)) {
        tracker_plane[2] = 0;
      }
    } else if (approx(z, 11556, 5)) {
      tracker_plane[1] = 4;
      if (approx(z, 11555.6, 0.2)) {
        tracker_plane[2] = 2;
      } else if (approx(z, 11556.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 11556.9, 0.2)) {
        tracker_plane[2] = 0;
      }
    } else if (approx(z, 11856, 5)) {
      tracker_plane[1] = 3;
      if (approx(z, 11855.6, 0.2)) {
        tracker_plane[2] = 2;
      } else if (approx(z, 11856.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 11856.9, 0.2)) {
        tracker_plane[2] = 0;
      }
    } else if (approx(z, 12106, 5)) {
      tracker_plane[1] = 2;
      if (approx(z, 12105.5, 0.2)) {
        tracker_plane[2] = 2;
      } else if (approx(z, 12106.1, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 12106.8, 0.2)) {
        tracker_plane[2] = 0;
      }
    } else if (approx(z, 12306, 5)) {
      tracker_plane[1] = 1;
      if (approx(z, 12305.5, 0.2)) {
        tracker_plane[2] = 2;
      } else if (approx(z, 12306.1, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 12306.8, 0.2)) {
        tracker_plane[2] = 0;
      }
    }
  } else if (approx(z, 16573, 555)) {
    tracker_plane[0] = 1;
    if (approx(z, 16022, 5)) {
      tracker_plane[1] = 1;
      if (approx(z, 16021.6, 0.2)) {
        tracker_plane[2] = 0;
      } else if (approx(z, 16022.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 16022.9, 0.2)) {
        tracker_plane[2] = 2;
      }
    } else if (approx(z, 16222, 5)) {
      tracker_plane[1] = 2;
      if (approx(z, 16221.6, 0.2)) {
        tracker_plane[2] = 0;
      } else if (approx(z, 16222.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 16222.9, 0.2)) {
        tracker_plane[2] = 2;
      }
    } else if (approx(z, 16472, 5)) {
      tracker_plane[1] = 3;
      if (approx(z, 16471.6, 0.2)) {
        tracker_plane[2] = 0;
      } else if (approx(z, 16472.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 16472.9, 0.2)) {
        tracker_plane[2] = 2;
      }
    } else if (approx(z, 16772, 5)) {
      tracker_plane[1] = 4;
      if (approx(z, 16771.6, 0.2)) {
        tracker_plane[2] = 0;
      } else if (approx(z, 16772.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 16772.9, 0.2)) {
        tracker_plane[2] = 2;
      }
    } else if (approx(z, 17122, 5)) {
      tracker_plane[1] = 5;
      if (approx(z, 17121.6, 0.2)) {
        tracker_plane[2] = 0;
      } else if (approx(z, 17122.3, 0.2)) {
        tracker_plane[2] = 1;
      } else if (approx(z, 17122.9, 0.2)) {
        tracker_plane[2] = 2;
      }
    }
  }
  return tracker_plane;
}

std::vector<MAUS::DataStructure::Global::SpacePoint*>* GetSpillSpacePoints(
    Spill* spill, MAUS::DataStructure::Global::DetectorPoint detector) {
  std::vector<MAUS::DataStructure::Global::SpacePoint*>* spill_spacepoints =
      new std::vector<MAUS::DataStructure::Global::SpacePoint*>;
  MAUS::ReconEventPArray* recon_events = spill->GetReconEvents();
  MAUS::ReconEventPArray::iterator recon_event_iter;
  if (recon_events) {
    for (recon_event_iter = recon_events->begin();
         recon_event_iter != recon_events->end();
         ++recon_event_iter) {
      MAUS::GlobalEvent* global_event = (*recon_event_iter)->GetGlobalEvent();
      if (global_event) {
        std::vector<MAUS::DataStructure::Global::SpacePoint*>*
            spacepoints = global_event->get_space_points();
        std::vector<MAUS::DataStructure::Global::SpacePoint*>::iterator sp_iter;
        for (sp_iter = spacepoints->begin(); sp_iter != spacepoints->end();
             ++sp_iter) {
          if ((*sp_iter)->get_detector() == detector) {
            spill_spacepoints->push_back(*sp_iter);
          }
        }
      }
    }
  }
  if (spill_spacepoints->size() > 0) {
    return spill_spacepoints;
  } else {
    return 0;
  }
}

bool approx(double a, double b, double tolerance) {
  if (std::abs(a - b) > std::abs(tolerance)) {
    return false;
  } else {
    return true;
  }
}

MAUS::DataStructure::Global::TrackPoint* GetNearestZTrackPoint(
    const MAUS::DataStructure::Global::Track* track, double z_position) {
  std::vector<const MAUS::DataStructure::Global::TrackPoint*> trackpoints =
      track->GetTrackPoints();
  size_t nearest_index = 0;
  double z_distance = 1.0e20;
  for (size_t i = 0; i < trackpoints.size(); i++) {
    double current_distance = std::abs(z_position -
                                       trackpoints.at(i)->get_position().Z());
    if (current_distance < z_distance) {
      nearest_index = i;
      z_distance = current_distance;
    }
  }
  MAUS::DataStructure::Global::TrackPoint* nearest_track_point = new
      MAUS::DataStructure::Global::TrackPoint(*trackpoints.at(nearest_index));
  return nearest_track_point;
}

double dEdx(const G4Material* material, double E, double m) {
  double constant = 2.54955123375e-23;
  double m_e = 0.510998928;
  double beta = std::sqrt(1 - (m*m)/(E*E));
  double beta2 = beta*beta;
  double gamma = 1/std::sqrt(1 - beta2);
  double bg = beta*gamma;
  double bg2 = bg*bg;
  double mRatio = m_e/m;
  double T_max = 2.0*m_e*bg2/(1.0 + 2.0*gamma*mRatio + mRatio*mRatio);

  double n_e = material->GetElectronDensity();
  double I = material->GetIonisation()->GetMeanExcitationEnergy();
  double x_0 = material->GetIonisation()->GetX0density();
  double x_1 = material->GetIonisation()->GetX1density();
  double C = material->GetIonisation()->GetCdensity();
  double a = material->GetIonisation()->GetAdensity();
  double k = material->GetIonisation()->GetMdensity();

  double logterm = std::log(2.0*m_e*bg2*T_max/(I*I));
  double x = std::log(bg)/std::log(10);

  // density correction
  double delta = 0.0;
  if (x > x_0) {
    delta = 2*std::log(10)*x - C;
    if (x < x_1) {
      delta += a*std::pow((x_1 - x), k);
    }
  }
  double dEdx = -constant*n_e/beta2*(logterm - 2*beta2 - delta);
  return dEdx;
}

// Need some global variables here
static const BTField* _field;
static int _charge;

void propagate(double* x, double target_z, const BTField* field,
               double step_size, MAUS::DataStructure::Global::PID pid,
               bool energy_loss) {
  if (std::abs(target_z) > 100000) {
    throw(MAUS::Exception(MAUS::Exception::recoverable, "Extreme target z",
                          "GlobalTools::propagate"));
  }
  int prop_dir = 1;
  _field = field;
  _charge = MAUS::recon::global::Particle::GetInstance().GetCharge(pid);
  double mass = MAUS::recon::global::Particle::GetInstance().GetMass(pid);
  G4Navigator* g4navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  G4NistManager* manager = G4NistManager::Instance();
  bool backwards = false;
  // If we propagate backwards, reverse momentum 4-vector
  if (target_z < x[3]) {
    backwards = true;
    _charge *= -1;
    prop_dir = -1;
    for (size_t i = 4; i < 8; i++) {
      x[i] *= -1;
    }
  }
  const gsl_odeiv_step_type * T = gsl_odeiv_step_rk4;
  double absolute_error = (*MAUS::Globals::GetInstance()
                           ->GetConfigurationCards())
                           ["field_tracker_absolute_error"].asDouble();;
  double relative_error = (*MAUS::Globals::GetInstance()
                           ->GetConfigurationCards())
                           ["field_tracker_relative_error"].asDouble();;
  gsl_odeiv_step    * step    = gsl_odeiv_step_alloc(T,8);
  gsl_odeiv_control * control = gsl_odeiv_control_y_new(absolute_error,
                                                        relative_error);
  gsl_odeiv_evolve  * evolve  = gsl_odeiv_evolve_alloc(8);
  int (*FuncEqM)(double z, const double y[], double f[], void *params)=NULL;
  FuncEqM = z_equations_of_motion;
  gsl_odeiv_system system  = {FuncEqM, NULL, 8, NULL};
  double h = step_size*prop_dir;
  size_t max_steps = 10000000;
  size_t n_steps = 0;
  double z = x[3];
  while (fabs(z - target_z) > 1e-6) {
    n_steps++;
    h = step_size*prop_dir; // revert step size as large step size problematic for dEdx
    int status;
    if (energy_loss) {
      const CLHEP::Hep3Vector posvector(x[1], x[2], x[3]);
      double mommag = std::sqrt(x[5]*x[5] + x[6]*x[6] + x[7]*x[7]);
      const CLHEP::Hep3Vector momvector(x[5]/mommag, x[6]/mommag, x[7]/mommag);
      G4VPhysicalVolume* volume = g4navigator->LocateGlobalPointAndSetup(posvector, &momvector);
      MAUS::GeometryNavigator geometry_navigator;
      geometry_navigator.Initialise(g4navigator->GetWorldVolume());
      double safety = 10;
      double boundary_dist = g4navigator->ComputeStep(posvector, momvector, h, safety);
      if (boundary_dist > 1e6) {
        boundary_dist = safety;
      }
      double z_dist = boundary_dist*momvector.z();
      // Check if z distance to next material boundary is smaller than step size
      // if yes, we impose a tight limit on the step size to avoid issues
      // arising from the track not being straight
      bool temp = false;
      if (std::abs(z_dist) < std::abs(h)) {
        temp = true;
        
        if (std::abs(z_dist) > 2.0) {
          h = 2.0*prop_dir;
        } else {
          h = z_dist; // will have proper sign from momvector
        }
      }
      if (std::abs(h) < 0.1) {
        h = 0.1*prop_dir;
      }
      double x_prev[] = {x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7]};
      status = gsl_odeiv_evolve_apply(evolve, control, step, &system, &z,
                                        target_z, &h, x);
                                        if (temp) {
      }
    // Calculate energy loss for the step
      geometry_navigator.SetPoint(MAUS::ThreeVector((x[1] + x_prev[1])/2,
                                  (x[2] + x_prev[2])/2, (x[3] + x_prev[3])/2));
      double step_distance = std::sqrt((x[1]-x_prev[1])*(x[1]-x_prev[1]) +
                                       (x[2]-x_prev[2])*(x[2]-x_prev[2]) +
                                       (x[3]-x_prev[3])*(x[3]-x_prev[3]));
      G4Material* material = manager->FindOrBuildMaterial(geometry_navigator.GetMaterialName());
      double step_energy_loss = dEdx(material, x[4], mass)*step_distance;
      changeEnergy(x, step_energy_loss, mass);
    } else {
      status = gsl_odeiv_evolve_apply(evolve, control, step, &system, &z,
                                        target_z, &h, x);
    } 
    if (status != GSL_SUCCESS) {
      throw(MAUS::Exception(MAUS::Exception::recoverable, "Propagation failed",
                            "GlobalTools::propagate"));
    }

    if (n_steps > max_steps) {
      std::stringstream ios;
      ios << "Stopping at step " << n_steps << " of " << max_steps << "\n" 
          << "t: " << x[0] << " pos: " << x[1] << " " << x[2] << " " << x[3] << "\n"
          << "E: " << x[4] << " mom: " << x[5] << " " << x[6] << " " << x[7] << std::endl;
      throw(MAUS::Exception(MAUS::Exception::recoverable, ios.str()+
            "Exceeded maximum number of steps", "GlobalTools::propagate"));
      break;
    }

    // Need to catch the case where the particle is stopped
    if (std::abs(x[4]) < (mass + 0.01)) {
      std::stringstream ios;
      ios << "t: " << x[0] << " pos: " << x[1] << " " << x[2] << " " << x[3] << std::endl;
      throw(MAUS::Exception(MAUS::Exception::recoverable, ios.str()+
            "Particle terminated with 0 momentum", "GlobalTools::propagate"));
    }
  }
  gsl_odeiv_evolve_free (evolve);
  gsl_odeiv_control_free(control);
  gsl_odeiv_step_free   (step);

  // If we propagate backwards, reverse momentum 4-vector back to original sign
  if (backwards) {
    for (size_t i = 4; i < 8; i++) {
      x[i] *= -1;
    }
  }
}

int z_equations_of_motion (double z, const double x[8], double dxdz[8],
                                   void* params) {
  if (fabs(x[7]) < 1e-9) {
  // z-momentum is 0
    return GSL_ERANGE;
  }
  const double c_l = 299.792458; //mm*ns^{-1}
  double field[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  double xfield[4] = {x[1], x[2], x[3], x[0]};
  _field->GetFieldValue(xfield, field);
  double dtdz = x[4]/x[7];
  double dir = fabs(x[7])/x[7]; // direction of motion
  dxdz[0] = dtdz/c_l; // dt/dz
  dxdz[1] = x[5]/x[7]; // dx/dz = px/pz
  dxdz[2] = x[6]/x[7]; // dy/dz = py/pz
  dxdz[3] = 1.0; // dz/dz
  // dE/dz only contains electric field as B conserves energy, not relevant at
  // least in step 4 as all fields are static.
  dxdz[4] = (dxdz[1]*_charge*field[3] + dxdz[2]*_charge*field[4] +
             _charge*field[5])*dir; // dE/dz
  // dpx/dz = q*c*(dy/dz*Bz - dz/dz*By) + q*Ex*dt/dz
  dxdz[5] = _charge*c_l*(dxdz[2]*field[2] - dxdz[3]*field[1])
            + _charge*field[3]*dtdz*dir; // dpx/dz
  dxdz[6] = _charge*c_l*(dxdz[3]*field[0] - dxdz[1]*field[2])
            + _charge*field[4]*dtdz*dir; // dpy/dz
  dxdz[7] = _charge*c_l*(dxdz[1]*field[1] - dxdz[2]*field[0])
            + _charge*field[5]*dtdz*dir; // dpz/dz
  return GSL_SUCCESS;
}

void changeEnergy(double* x, double deltaE, double mass) {
  double old_momentum = std::sqrt(x[5]*x[5] + x[6]*x[6] + x[7]*x[7]);
  x[4] += deltaE;
  double new_momentum, momentum_ratio;
  if (std::abs(x[4]) > mass) {
    new_momentum = std::sqrt(x[4]*x[4] - mass*mass);
  } else {
    new_momentum = 0.0;
  }
  momentum_ratio = new_momentum / old_momentum;
  x[5] *= momentum_ratio;
  x[6] *= momentum_ratio;
  x[7] *= momentum_ratio;
}

bool TrackPointSort(const MAUS::DataStructure::Global::TrackPoint* tp1,
                    const MAUS::DataStructure::Global::TrackPoint* tp2) {
  return (tp1->get_position().Z() < tp2->get_position().Z());
}

} // ~namespace GlobalTools
} // ~namespace MAUS
