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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TOFSPACEPOINT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TOFSPACEPOINT_

#include <string>

#include "src/common_cpp/Utils/VersionNumber.hh"

namespace MAUS {

/** @class TOFSpacePoint comment
 *
 *  @var phys_event_number  <--description-->
 *  @var pixel_key  <--description-->
 *  @var charge  <--description-->
 *  @var station  <--description-->
 *  @var slabY  <--description-->
 *  @var slabX  <--description-->
 *  @var charge_product  <--description-->
 *  @var time  <--description-->
 *  @var detector  <--description-->
 *  @var part_event_number  <--description-->
 *  @var dt  <--description-->
 */

class TOFSpacePoint {
  public:
    /** Default constructor - initialises to 0/NULL */
    TOFSpacePoint();

    /** Copy constructor - any pointers are deep copied */
    TOFSpacePoint(const TOFSpacePoint& _tofspacepoint);

    /** Equality operator - any pointers are deep copied */
    TOFSpacePoint& operator=(const TOFSpacePoint& _tofspacepoint);

    /** Destructor - any member pointers are deleted */
    virtual ~TOFSpacePoint();


    /** Returns PhysEventNumber */
    int GetPhysEventNumber() const;

    /** Sets PhysEventNumber */
    void SetPhysEventNumber(int phys_event_number);

    /** Returns PixelKey */
    std::string GetPixelKey() const;

    /** Sets PixelKey */
    void SetPixelKey(std::string pixel_key);

    /** Returns Charge */
    int GetCharge() const;

    /** Sets Charge */
    void SetCharge(int charge);

    /** Returns Station */
    int GetStation() const;

    /** Sets Station */
    void SetStation(int station);

    /** Returns Slaby -- slabs oriented along Y, measure X*/
    int GetSlaby() const;

    /** Sets Slaby -- slabs oriented along Y, measure X*/
    void SetSlaby(int slabY);

    /** Returns Slabx -- slabs oriented along X, measure Y*/
    int GetSlabx() const;

    /** Sets Slabx -- slabs oriented along X, measure Y*/
    void SetSlabx(int slabX);

    /** Returns Vertical slab, measure X, equiv to GetSlabY*/
    int GetVertSlab() const;

    /** Sets Vertical slab - same as SetSlabY*/
    void SetVertSlab(int slabY);

    /** Returns Horizontal slab, measure Y, equiv to GetSlabX*/
    int GetHorizSlab() const;

    /** Sets Horizontal slab - same as SetSlabX*/
    void SetHorizSlab(int slabX);

    /** Get the Global X, Y, Z positions of the space point*/
    double GetGlobalPosX() const;
    double GetGlobalPosY() const;
    double GetGlobalPosZ() const;
    /** Get the error in the X,Y,Z measurements
     * these are the sqrt(12) errors in the slab dimension */
    double GetGlobalPosXErr() const;
    double GetGlobalPosYErr() const;
    double GetGlobalPosZErr() const;
    /** Set the global X,Y,Z of the space point */
    void SetGlobalPosX(double global_x);
    void SetGlobalPosY(double global_y);
    void SetGlobalPosZ(double global_z);
    /** Set the error in the global x,y,z position */
    void SetGlobalPosXErr(double global_x_err);
    void SetGlobalPosYErr(double global_y_err);
    void SetGlobalPosZErr(double global_z_err);

    /** Returns ChargeProduct */
    int GetChargeProduct() const;

    /** Sets ChargeProduct */
    void SetChargeProduct(int charge_product);

    /** Returns Time */
    double GetTime() const;

    /** Sets Time */
    void SetTime(double time);

    /** Returns Detector */
    std::string GetDetector() const;

    /** Sets Detector */
    void SetDetector(std::string detector);

    /** Returns PartEventNumber */
    int GetPartEventNumber() const;

    /** Sets PartEventNumber */
    void SetPartEventNumber(int part_event_number);

    /** Returns Dt */
    double GetDt() const;

    /** Sets Dt */
    void SetDt(double dt);

  private:
    int _phys_event_number;
    std::string _pixel_key;
    int _charge;
    int _station;
    int _slabY;
    int _slabX;
    int _charge_product;
    double _time;
    std::string _detector;
    int _part_event_number;
    double _dt;
    double _global_x;
    double _global_y;
    double _global_z;
    double _global_x_err;
    double _global_y_err;
    double _global_z_err;

    MAUS_VERSIONED_CLASS_DEF(TOFSpacePoint)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_TOFSPACEPOINT_

