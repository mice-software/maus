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

#ifndef MAUS_PROPAGATORS_HH
#define MAUS_PROPAGATORS_HH

#include "src/common_cpp/Recon/SciFi/SciFiGeometryHelper.hh"
#include "src/common_cpp/Utils/Constants.hh"

#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanPropagatorBase.hh"

namespace MAUS {


  class StraightPropagator : public Kalman::Propagator_base {
    public :
      StraightPropagator(SciFiGeometryHelper* helper);

      virtual TMatrixD CalculatePropagator(const Kalman::State& start, const Kalman::State& end);

      virtual TMatrixD CalculateProcessNoise(const Kalman::State& start, const Kalman::State& end);

      TMatrixD BuildQ(const Kalman::State& state, double radLen, double width);

      void SetIncludeMCS(bool inc) { _include_mcs = inc; }

      virtual ~StraightPropagator() {}

    protected :

    private :
      SciFiGeometryHelper* _geometry_helper;
      bool _include_mcs;
  };


  class HelicalPropagator : public Kalman::Propagator_base {
    public :
      HelicalPropagator(SciFiGeometryHelper* helper);

      virtual void Propagate(const Kalman::State& start, Kalman::State& end);

      virtual TMatrixD CalculatePropagator(const Kalman::State& start, const Kalman::State& end);

      virtual TMatrixD CalculateProcessNoise(const Kalman::State& start, const Kalman::State& end);

      TMatrixD BuildQ(const Kalman::State& state, double radLen, double width);

      void SetSubtractELoss(bool sub) { _subtract_eloss = sub; }

      void SetIncludeMCS(bool inc) { _include_mcs = inc; }

      virtual ~HelicalPropagator() {}

    protected :

    private :
      double _Bz;
      SciFiGeometryHelper* _geometry_helper;
      bool _subtract_eloss;
      bool _include_mcs;
  };
}

#endif // MAUS_PROPAGATORS_HH

