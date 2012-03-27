#ifndef SRC_COMMON_CPP_OPTICS_RECONSTRUCTION_MINUIT_TRACK_FITTER_HH
#define SRC_COMMON_CPP_OPTICS_RECONSTRUCTION_MINUIT_TRACK_FITTER_HH

#include <vector>

#include "TMinuit.h"

namespace MAUS {

class OpticsModel;

// Minuit requires a gobal, static function to minimize. This requires a
// global instance of TMinuit to use TMinuit::GetObjectFit().
TMinuit * common_cpp_optics_reconstruction_minuit_track_fitter_minuit = NULL;

void common_cpp_optics_reconstruction_minuit_track_fitter_score_function(
    Int_t &    number_of_parameters,
    Double_t * gradiants,
    Double_t & function_value,
    Double_t * phase_space_coordinate_values,
    Int_t      execution_stage_flag);

class MinuitTrackFitter : public TrackFitter
{
 public:
  MinuitTrackFitter(OpticsModel const * const optics_model);

  ~MinuitTrackFitter();
 protected:
  static const size_t kPhaseSpaceDimension;
  std::vector<double> detector_planes_;
  std::vector<CovarianceMatrix> detector_errors_;
  std::const_iterator<std::vector<DetectorEvent> > trajectories_;

  MinuitTrackFitter();
  Double_t ScoreTrack(Double_t const * const phase_space_coordinates) const;
  void GetDetectorPlanes(std::vector<double> * detector_planes);
  void GetDetectorErrors(std::vector<CovarianceMatrix> * error_matrices);
  void CorrelateDetectorEvents(
    std::vector<DetectorEvent> const * const detector_events,
    std::vector<std::vector<DetectorEvent> > * &event_sets);
};

const size_t MapCppTrackReconstructor::kPhaseSpaceDimension = 6;

}

#endif
