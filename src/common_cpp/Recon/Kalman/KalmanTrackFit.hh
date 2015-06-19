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

#include <string>

#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanPropagatorBase.hh"
#include "src/common_cpp/Recon/Kalman/KalmanMeasurementBase.hh"

#include "TMatrix.h"

namespace MAUS {
namespace Kalman {

  std::string print_state(const State& state, const char* detail = 0);

  std::string print_track(const Track& track, const char* name = 0);

  /** @brief Calculates the residual between two states */
  State CalculateResidual(const State& st1, const State& st2);

  /** @brief Fast was to calculate the chis-sq update */
  double CalculteChiSquaredUpdate(const State st);


  /** @class TrackFit
   *
   *  @brief TrackFit manages the workflow of the fitting.
   *
   */
  class TrackFit {
  public:

    /** @brief Intialise with the required measurement and propagator classes.
     */
    TrackFit(Propagator_base* propagator, Measurement_base* measurement);

    /** @brief Destructor
     */
    virtual ~TrackFit();

    /** @brief Append a new data state and filter up to it
     *
     *  A short cut function to save computations. 
     *  Useful if using this class to stream data through it
     */
    void AppendFilter(State state);

    /** @brief Filter all states using supplied propagator and measurement classes
     */
    void Filter(bool forward = true);

    /** @brief Smooth all states using supplied propagator and measurement classes
     */
    void Smooth(bool forward = true);


    /** @brief Returns the current start seed
     */
    State GetSeed() const { return _seed; }

    /** @brief Sets the current starting seed
     */
    void SetSeed(State state);

    /** @brief Set the data track - used to provide the measurments
     */
    void SetData(Track data_track);

    /** @brief Returns a copy of the current data track
     */
    Track GetData() const { return _data; }
    const Track& Data() const { return _data; }

    /** @brief Return a copy of the predicted track
     */
    Track GetPredicted() const { return _predicted; }
    const Track& Predicted() const { return _predicted; }

    /** @brief Return a copy of the filtered track
     */
    Track GetFiltered() const { return _filtered; }
    const Track& Filtered() const { return _filtered; }

    /** @brief Return a copy of the smoothed track
     */
    Track GetSmoothed() const { return _smoothed; }
    const Track& Smoothed() const { return _smoothed; }


    /** @brief Claculate the Chi-Squared value for the track
     */
    double CalculateChiSquared(const Track&) const;

    /** @brief Return the Number of Degrees of Freedom
     */
    int GetNDF() const;

    /** @brief Helper function to calculate predicted residual/pull */
    State CalculatePull(unsigned int i) const { return this->CalculatePredictedResidual(i); }
    State CalculatePredictedResidual(unsigned int i) const;

    /** @brief Helper function to calculate filtered residual */
    State CalculateFilteredResidual(unsigned int i) const;

    /** @brief Helper function to calculate smoothed residual */
    State CalculateSmoothedResidual(unsigned int i) const;

    /** @brief Return the dimension of the measurement state vector
     */
    unsigned int GetDimension() const { return _dimension; }

    /** @brief Return the dimension of the measurement state vector
     */
    unsigned int GetMeasurementDimension() const { return _measurement_dimension; }

  protected:

    void _propagate(State& first, State& second) const;
    void _filter(const State& data, State& predicted, State& filtered) const;
    void _smooth(State& first, State& second) const;

  private:
    // Private copy constructor => No copying!
    TrackFit(const TrackFit& tf);
    TrackFit& operator=(const TrackFit& tf) { return *this; }

    unsigned int _dimension;
    unsigned int _measurement_dimension;

    Propagator_base* _propagator;
    Measurement_base* _measurement;

    State _seed;

    Track _data;
    Track _predicted;
    Track _filtered;
    Track _smoothed;

    TMatrixD _identity_matrix;
  };
} // namespace Kalman
} // namespace MAUS

#endif // KALMAN_TRACKFIT_HH
