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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TOFSLABHIT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TOFSLABHIT_

#include <string>

#include "src/common_cpp/Utils/VersionNumber.hh"

#include "src/common_cpp/DataStructure/Pmt1.hh"
#include "src/common_cpp/DataStructure/Pmt0.hh"

namespace MAUS {

/** @class TOFSlabHit comment
 *
 *  @var slab  <--description-->
 *  @var phys_event_number  <--description-->
 *  @var raw_time  <--description-->
 *  @var charge  <--description-->
 *  @var plane  <--description-->
 *  @var charge_product  <--description-->
 *  @var time  <--description-->
 *  @var station  <--description-->
 *  @var detector  <--description-->
 *  @var part_event_number  <--description-->
 *  @var pmt1  <--description-->
 *  @var pmt0  <--description-->
 */

class TOFSlabHit {
  public:
    /** Default constructor - initialises to 0/NULL */
    TOFSlabHit();

    /** Copy constructor - any pointers are deep copied */
    TOFSlabHit(const TOFSlabHit& _tofslabhit);

    /** Equality operator - any pointers are deep copied */
    TOFSlabHit& operator=(const TOFSlabHit& _tofslabhit);

    /** Destructor - any member pointers are deleted */
    virtual ~TOFSlabHit();


    /** Returns Slab */
    int GetSlab() const;

    /** Sets Slab */
    void SetSlab(int slab);

    /** Get the Global X, Y, Z positions of the slab hit*/
    double GetGlobalPosX() const;
    double GetGlobalPosY() const;
    double GetGlobalPosZ() const;
    /** Get the error in the X,Y,Z measurements
     * these are the sqrt(12) errors in the slab dimension */
    double GetGlobalPosXErr() const;
    double GetGlobalPosYErr() const;
    double GetGlobalPosZErr() const;
    /** Set the global X,Y,Z of the slab hit */
    void SetGlobalPosX(double global_x);
    void SetGlobalPosY(double global_y);
    void SetGlobalPosZ(double global_z);
    /** Set the error in the global x,y,z position */
    void SetGlobalPosXErr(double global_x_err);
    void SetGlobalPosYErr(double global_y_err);
    void SetGlobalPosZErr(double global_z_err);

    /** Returns PhysEventNumber */
    int GetPhysEventNumber() const;

    /** Sets PhysEventNumber */
    void SetPhysEventNumber(int phys_event_number);

    /** Returns RawTime */
    double GetRawTime() const;

    /** Sets RawTime */
    void SetRawTime(double raw_time);

    /** Returns Charge */
    int GetCharge() const;

    /** Sets Charge */
    void SetCharge(int charge);

    /** Returns Plane */
    int GetPlane() const;

    /** Sets Plane */
    void SetPlane(int plane);

    /** Returns ChargeProduct */
    int GetChargeProduct() const;

    /** Sets ChargeProduct */
    void SetChargeProduct(int charge_product);

    /** Returns Time */
    double GetTime() const;

    /** Sets Time */
    void SetTime(double time);

    /** Returns Station */
    int GetStation() const;

    /** Sets Station */
    void SetStation(int station);

    /** Returns Detector */
    std::string GetDetector() const;

    /** Sets Detector */
    void SetDetector(std::string detector);

    /** Returns PartEventNumber */
    int GetPartEventNumber() const;

    /** Sets PartEventNumber */
    void SetPartEventNumber(int part_event_number);

    /** Returns Pmt1 */
    Pmt1 GetPmt1() const;

    /** Returns Pmt1 */
    Pmt1* GetPmt1Ptr();

    /** Sets Pmt1 */
    void SetPmt1(Pmt1 pmt1);

    /** Returns Pmt0 */
    Pmt0 GetPmt0() const;

    /** Returns Pmt0 */
    Pmt0* GetPmt0Ptr();

    /** Sets Pmt0 */
    void SetPmt0(Pmt0 pmt0);

    /** the cabling convention is such that
     * horizontal slabs are in plane 0
     * vertical slabs are in plane 1
     * Set true if horizontal slab: plane 0 */
    void SetHorizontal(bool hSlab);

    /** Set true if vertical slab: plane 1 */
    void SetVertical(bool vSlab);

    /** True if slab is a vertical slab, else false */
    bool IsVertical() const {return _vertical_slab;}

    /** True if slab is a horizontal slab, else false */
    bool IsHorizontal() const {return _horizontal_slab;}

  private:
    int _slab;
    int _phys_event_number;
    double _raw_time;
    int _charge;
    int _plane;
    int _charge_product;
    double _time;
    int _station;
    std::string _detector;
    int _part_event_number;
    Pmt1 _pmt1;
    Pmt0 _pmt0;

    double _global_x;
    double _global_y;
    double _global_z;
    double _global_x_err;
    double _global_y_err;
    double _global_z_err;

    bool _vertical_slab;
    bool _horizontal_slab;

    MAUS_VERSIONED_CLASS_DEF(TOFSlabHit)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_TOFSLABHIT_

