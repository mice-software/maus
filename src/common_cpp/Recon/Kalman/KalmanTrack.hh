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

#include <vector>
#include <algorithm>

#include "TMath.h"
#include "TMatrixD.h"


namespace MAUS {
namespace Kalman {

  // Class and Function Declarations
  class State;
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

    friend State CalculateResidual(const State& st1, const State& st2);
    friend double CalculateChiSquaredUpdate(const State& st);

    public:
      /** @brief Initialise an empty state of the requred dimension and position
       */
      State(unsigned int dimension, double position = 0.0);

      /** @brief Initialise state with vector and covariance matrix
       */
      State(TMatrixD vector, TMatrixD covariance, double position = 0.0);

      /** @brief Copy Constructor */
      State(const State& st);

      /** @brief Assignment Operator */
      State& operator=(const State& st);

      /** @brief Shallow Copy function. Maintains previous ID and Position
       */
      State& copy(State state);

      /** @brief Destructor
       */
      virtual ~State() {}


      /** @brief Get ID of state
       */
      int GetId() const { return _id; }

      /** @brief Set ID of state. Should be unique!
       */
      void SetId(int new_id) { _id = new_id; }

      /** @brief Return the position of the state
       */
      double GetPosition() const { return _position; }

      /** @brief Set the position of the state
       */
      void SetPosition(double new_pos) { _position = new_pos; }

      /** @brief Return the state vector
       */
      TMatrixD& GetVector() { return _vector; }
      const TMatrixD& GetVector() const { return _vector; }

      /** @brief Set the state vector
       */
      void SetVector(TMatrixD vec);

      /** @brief Return a reference to the covariance matrix
       */
      TMatrixD& GetCovariance() { return _covariance; }
      const TMatrixD& GetCovariance() const { return _covariance; }

      /** @brief Set the covariance matrix
       */
      void SetCovariance(TMatrixD cov);


      /** @brief Returns true if there is a stored value.
       */
      bool HasValue() const { return _has_value; }

      /** @brief Set whether or not there is a stored value.
       *
       *  This is useful in track fitting if a measurement plane has not 
       *  actually recorded a measurement. With this option set to false 
       *  the track fit will continue as expected and return an estimate
       *  of the state vector for this state, without using any of the 
       *  stored information.
       */
      void SetHasValue(bool HV) { _has_value = HV; }

      /** @brief Return the dimension of the state vector
       */
      unsigned int GetDimension() const { return _dimension; }

      /** @brief Boolean operator, copies HasValue() function
       */
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
      Track(unsigned int dimension, unsigned int length = 0);

      /** @brief Return the state found at specified index in state array
       */
      State GetState(unsigned int index) const { return _track_vector[index]; }

      /** @brief Set the state at the specified index in the state array
       */
      void SetState(unsigned int index, State state);

      /** @brief Get a reference to the state at the specified index 
       */
      State& operator[] (unsigned int index) { return _track_vector[index]; }

      /** @brief Get a const reference to the state at the specified index 
       */
      const State& operator[] (unsigned int index) const { return _track_vector[index]; }

      /** @brief Return the number of states in the state array
       */
      unsigned int GetLength() const { return _track_vector.size(); }

      /** @brief Append a state to the state array
       */
      void Append(State state);

      /** @brief Remove a state from the specified index in the state array
       */
      void DeleteState(unsigned int index);

      /** @brief (Re)initialise the track using the structure of a similar track
       */
      void Reset(const Track& similar_track);

      /** @brief Return the dimension of the state vectors
       */
      unsigned int GetDimension() const { return _dimension; }

    protected:

    private:
      unsigned int _dimension;

      StateArray _track_vector;
  };
} // namespace Kalman
} // namespace MAUS

#endif
