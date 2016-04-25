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
#include <map>

#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanPropagatorBase.hh"
#include "src/common_cpp/Recon/Kalman/KalmanMeasurementBase.hh"

#include "TMatrix.h"

namespace MAUS {
namespace Kalman {

  std::string print_state(const State& tp, const char* detail = 0);

  std::string print_trackpoint(const TrackPoint& tp, const char* detail = 0);

  std::string print_track(const Track& track, const char* name = 0);

  /** @brief Calculates the residual between two states */
  State CalculateResidual(const State& st1, const State& st2);

  /** @brief Fast was to calculate the chis-sq update */
  double CalculteChiSquaredUpdate(const State st);


  /** @class TrackFit
   *
   *  @brief TrackFit manages the workflow of the fitting.
   *
   *  The _measurements variable stores a map of measurement object pointers, the correspond
   *   to all the different types of measurement in the system.
   *   The ID of each measurement, the key, *MUST* correspond to a the trackpoint ID, if the 
   *   measurement object is to be used correctly.
   *
   */
  class TrackFit {
  public:

    /** @brief Intialise with the required measurement and propagator classes.
     */
    explicit TrackFit(Propagator_base* propagator);

    /** @brief Destructor
     */
    virtual ~TrackFit();

    /** @brief Append a new data state, predict from the previous trackpoint and filter.
     *
     *  A short cut function to save computations. 
     *  Useful if using this class to stream data through it
     */
    void AppendFilter(TrackPoint tp);

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

    /** @brief Set the track to filter
     */
    void SetTrack(Track track);

    /** @brief Return a copy of the kalman track
     */
    Track& GetTrack() { return _track; }
    const Track& GetTrack() const { return _track; }


    /** @brief Claculate the Chi-Squared value for the track
     */
    double CalculateSmoothedChiSquared() const;

    /** @brief Return the Number of Degrees of Freedom
     */
    int GetNDF() const;

    State CalculateCleanedState(unsigned int i) const;
    State CalculatePull(unsigned int i) const;

    /** @brief Helper function to calculate predicted residual/pull */
    State CalculatePredictedResidual(unsigned int i) const;

    /** @brief Helper function to calculate filtered residual */
    State CalculateFilteredResidual(unsigned int i) const;

    /** @brief Helper function to calculate smoothed residual */
    State CalculateSmoothedResidual(unsigned int i) const;

    /** @brief Return the dimension of the measurement state vector
     */
    unsigned int GetDimension() const { return _dimension; }


    /** @brief Add a derived measurement class to the lookup
     *
     *  The ID *MUST* correspond to the correct trackpoint ID in order for it to be used correctly.
     */
    void AddMeasurement(int id, Measurement_base* measurement)
                                         { _measurements.insert(std::make_pair(id, measurement)); }

  protected:

    void _propagate(const TrackPoint& from, TrackPoint& to) const;
    void _filter(TrackPoint& tp) const;
    void _smooth(TrackPoint& first, TrackPoint& second) const; // NOT IMPLEMENTED YET!
    State _inverse_filter(const TrackPoint& tp) const;

  private:
    // Private copy constructor => No copying!
    TrackFit(const TrackFit& tf);
    TrackFit& operator=(const TrackFit& tf) { return *this; }

    unsigned int _dimension;
    unsigned int _measurement_dimension;

    Propagator_base* _propagator;
    std::map<int, Measurement_base*> _measurements;

    State _seed;

    Track _track;

//    Track _data;
//    Track _predicted;
//    Track _filtered;
//    Track _smoothed;

    TMatrixD _identity_matrix;
  };
} // namespace Kalman
} // namespace MAUS

#endif // KALMAN_TRACKFIT_HH
