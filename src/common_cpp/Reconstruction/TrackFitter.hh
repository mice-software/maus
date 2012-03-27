#ifndef SRC_COMMON_CPP_RECONSTRUCTION_TRACK_FITTER_HH
#define SRC_COMMON_CPP_RECONSTRUCTION_TRACK_FITTER_HH

#include <vector>

#include "DetectorEvent.hh"
#include "ParticleTrajectory.hh"

namespace MAUS {

class OpticsModel;

class TrackFitter
{
 public:
  virtual void Fit(std::vector<DetectorEvent> const * const detector_events
                   std::vector<ParticleTrajectory> * trajectories) = 0;
 protected:
  OpticsModel const * optics_model_;
};

}

#endif
