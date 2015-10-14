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
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMREVENT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMREVENT_

#include <vector>

#include "Utils/VersionNumber.hh"
#include "DataStructure/EMRPlaneHit.hh"

namespace MAUS {

typedef std::vector<EMRPlaneHit*> EMRPlaneHitArray;

/** @class EMREvent comment
 *
 */

class EMREvent {
  public:
    /** Default constructor - initialises to 0/NULL */
    EMREvent();

    /** Copy constructor - any pointers are deep copied */
    EMREvent(const EMREvent& _emrevent);

    /** Equality operator - any pointers are deep copied */
    EMREvent& operator=(const EMREvent& _emrevent);

    /** Destructor - any member pointers are deleted */
    virtual ~EMREvent();

    /** Returns  */
    EMRPlaneHitArray GetEMRPlaneHitArray() const;

    /** Sets  */
    void SetEMRPlaneHitArray(EMRPlaneHitArray emrplanehitarray);

    /** Returns  */
    bool GetInitialTrigger() const;

    /** Sets  */
    void SetInitialTrigger(bool initial_trigger);

    /** Returns  */
    bool GetHasPrimary() const;

    /** Sets  */
    void SetHasPrimary(bool has_primary);

    /** Returns  */
    double GetRangePrimary() const;

    /** Sets  */
    void SetRangePrimary(double range_primary);

    /** Returns  */
    bool GetHasSecondary() const;

    /** Sets  */
    void SetHasSecondary(bool has_secondary);

    /** Returns  */
    double GetRangeSecondary() const;

    /** Sets  */
    void SetRangeSecondary(double range_secondary);

    /** Returns  */
    double GetSecondaryToPrimaryTrackDistance() const;

    /** Sets  */
    void SetSecondaryToPrimaryTrackDistance(double secondary_to_primary_track_distance);

    /** Returns  */
    double GetTotalChargeMA() const;

    /** Sets  */
    void SetTotalChargeMA(double total_charge_MA);

    /** Returns  */
    double GetTotalChargeSA() const;

    /** Sets  */
    void SetTotalChargeSA(double total_charge_SA);

    /** Returns  */
    double GetChargeRatioMA() const;

    /** Sets  */
    void SetChargeRatioMA(double charge_ratio_MA);

    /** Returns  */
    double GetChargeRatioSA() const;

    /** Sets  */
    void SetChargeRatioSA(double charge_ratio_SA);

    /** Returns  */
    double GetPlaneDensityMA() const;

    /** Sets  */
    void SetPlaneDensityMA(double plane_density_MA);

    /** Returns  */
    double GetPlaneDensitySA() const;

    /** Sets  */
    void SetPlaneDensitySA(double plane_density_SA);

    /** Returns  */
    double GetChi2() const;

    /** Sets  */
    void SetChi2(double chi2);

  private:
    EMRPlaneHitArray _emrplanehitarray;

    bool _initial_trigger;
    bool _has_primary;
    double _range_primary;
    bool _has_secondary;
    double _range_secondary;
    double _secondary_to_primary_track_distance;
    double _total_charge_MA;
    double _total_charge_SA;
    double _charge_ratio_MA;
    double _charge_ratio_SA;
    double _plane_density_MA;
    double _plane_density_SA;
    double _chi2;

    MAUS_VERSIONED_CLASS_DEF(EMREvent)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_EMREVENT_

