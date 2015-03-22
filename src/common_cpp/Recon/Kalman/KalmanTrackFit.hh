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


#ifndef KALMAN_TRACKFIT_HH
#define KALMAN_TRACKFIT_HH

#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanPropgator_base.hh"
#include "src/common_cpp/Recon/Kalman/KalmanMeasurement_base.hh"

#include "TMatrix.h"


namespace MAUS {
namespace Kalman {

  TMatrixD ComputeKalmanGainMatrix(Measurement_base* measurement, State state);

  TMatrixD ComputeSmootherGainMatrix(Propagator_base* prop, State pred, State filt);

  /** @class TrackFit
   *
   *  @brief TrackFit manages the workflow of the fitting.
   *
   */
  class TrackFit {
  public:
    enum Status { Error, Intialised, Predicted, Filtered, Smoothed };

    /** @brief Intialise with the required measurement and propagator classes.
     */
    KalmanTrackFit(Propagator_base* propagator, Measurement_base* measurement);

    /** @brief Destructor
     */
    virtual ~KalmanTrackFit();


    void AppendFilter(State state);

    void Filter();

    void Smooth();


    State GetSeed() const { return _seed; }

    void SetSeed(State state);

    Status GetStatus() const { return _fitter_status; }

    void SetData(Track data_track);

    Track GetData() const { return _data; }
    
    Track GetPredicted() const { return _predicted; }

    Track GetFiltered() const { return _filtered; }

    Track GetSmoothed() const { return _smoothed; }


    unsigned int GetDimension() const { return _dimension; }

  protected:

//    State Filter(State filtered, State predicted);

  private:
    // Private copy constructor => No copying!
    TrackFit(const TrackFit& tf) {} 
    TrackFit& operator=(const TrackFit& tf) { return *this; }

    Status _fitter_status;

    unsigned int _dimension;

    Propagator_base* _propgator;
    Measurement_base* _measurement;

    State _seed;

    Track _data;
    Track _predicted;
    Track _filtered;
    Track _smoothed;

    TMatrixD _unit_matrix;
  };

} // namespace Kalman
} // namespace MAUS

#endif // KALMAN_TRACKFIT_HH
