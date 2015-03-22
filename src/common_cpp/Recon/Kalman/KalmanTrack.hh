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

#ifndef KALMAN_TRACK_HH
#define KALMAN_TRACK_HH

#include "TMath.h"
#include "TMatrixD.h"

#include <vector>


namespace MAUS {
namespace Kalman {

  // Class Declarations
  class Track;
  class TrackFit;

  /** @brief Basic Kalman State object
   *
   *  It stores a state vector and associated covariance matrix object.
   *  Also stores the position of the state and the state id.
   *
   *  Users can choose to set the "HasValue" flag to false (default if no vector set)
   *   so that the filtering algorithm ignores the values.
   */
  class State {
    // Make these friends so they can access the private member variables.
    //  Faster computation!
    friend class Track;
    friend class TrackFit;

    public:
      /** @brief Initialise an empty state of the requred dimension and position
       */
      State(unsigned int dimension, double position = 0.0);

      /** @brief Initialise state with vector and covariance matrix
       */
      State(TMatrixD vector, TMatrixD covariance, double position = 0.0);

      /** @brief Destructor
       */
      virtual ~State() {}


      int GetId() const { return _id; }

      void SetId(int new_id) { _id = new_id; }

      double GetPosition() const { return _position; }

      void SetPosition(double new_pos) { _position = new_pos; }

      TMatrixD& GetVector() { return _vector; }

      void SetVetor(TMatrixD vec);

      TMatrixD& GetCovariance() { return _covariance; }

      void SetVector(TMatrixD cov);

      bool HasValue() const { return _has_value; }

      void SetHasValue(bool HV) { _has_value = HV; }

      unsigned int GetDimension() const { return _dimension; }

      operator bool() const { return _has_value; }

    protected:

    private:

      unsigned int _dimension;
      int _id;
      double _position;
      TMatrixD _vector;
      TMatrixD _covariance;

      bool _has_value;
  };


  /** @brief Stores a vector of states to make a track
   */
  class Track {
    typedef std::vector< State > StateArray;

    public:
      /** @brief Initialise track with required dimension
       */
      explicit Track(unsigned int dimension);

      State GetState(unsigned int index) const { return _track_vector[index]; }

      void SetState(unsigned int index, State state);

      State& operator[] (unsigned int index) { return _track_vector[index]; }

      const State& operator[] (unsigned int index) const { return _track_vector[index]; }

      unsigned int Length() const { return _track_vector.size(); }

      void Append(State state);

      void DeleteState(unsigned int index);
      

      void Reset(const Track& similar_track);

      unsigned int GetDimension() const { return _dimension; }

    protected:

    private:
      unsigned int _dimension;

      StateArray _track_vector;
  };


} // namespace Kalman
} // namespace MAUS

#endif
