/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

#include <map>

#include "TMatrixD.h"  // ROOT

#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"
#include "src/common_cpp/Recon/Kalman/KalmanMeasurementBase.hh"

#ifndef MAUS_KALMAN_GLOBAL_MEASUREMENT_HH_
#define MAUS_KALMAN_GLOBAL_MEASUREMENT_HH_

namespace MAUS {
namespace Kalman {
namespace Global {
/** Handles the transformation into the measurement coordinate system.
 *
 *  The mapping from the measurement coordinate system to the global coordinate
 *  system is handled by a TMatrixD (I really hate ROOT matrices).
 *
 *  Also holds a static mapping which converts between a measurement type (e.g.
 *  scifi space point, tof space point, etc) and a measurement matrix. Aim is to
 *  setup "measurements" for every detector at initialisation time, so we don't
 *  have to do it on-the-fly. Maybe a bit of a CPU saving, more that it has to
 *  be done sometime, initialisation is as good a time as any.
 *
 *  For Global track fit, all I am really doing is applying a preset error on
 *  each measurement. Probably there is something in the data structure and the
 *  Global data import that does this anyway... I will defer integration with
 *  Global and deal with it at that point. So this whole area needs some tidying.
 */
class Measurement: public Measurement_base {
  public:
    typedef DataStructure::Global::DetectorPoint DetType;

    /** Default constructor */
    Measurement();

    /** Copy constructor */
    Measurement(const Measurement& meas);

    /** Constructor taking a measurement matrix as an argument */
    explicit Measurement(TMatrixD measurement);

    /** Destructor does nothing */
    ~Measurement() {}

    /** Inheritable copy constructor */
    Measurement* Clone() const;

    /** @return the measurement matrix */
    TMatrixD CalculateMeasurementMatrix(const State& state) {return _measurement;}

    /** Set the measurement matrix
     *
     *  @param measurement: the measurement matrix
     *
     *  Disabled for now; I will reimplement with pointer to TMatrixD so we can
     *  implement change of size of TMatrixD (Fing ROOT, fancy having a major
     *  analysis package with such a weak matrix object).
     */
    // void SetMeasurementMatrix(TMatrixD measurement);

    /** Call during initialisation of the global recon to initialise the mapping
     *  of detector type to measurement map
     */
    static void SetupDetectorToMeasurementMap();

    /** @returns the detector type to measurement map
     */
    static std::map<DetType, Measurement_base*>& GetDetectorToMeasurementMap() {
      return _detector_to_measurement_map;
    }

  private:
    static std::map<DetType, Measurement_base*> _detector_to_measurement_map;

    TMatrixD _measurement;
};
}
}
}

#endif
