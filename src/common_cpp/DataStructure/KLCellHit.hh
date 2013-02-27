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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_KLCELLHIT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_KLCELLHIT_

#include <string>

#include "Rtypes.h"  // ROOT

namespace MAUS {

/** @class KLCellHit comment
 *
 *  @var cell  <which cell is flashed, possible values from 0 to 21>
 *  @var phys_event_number  <number of event in the spill>
 *  @var charge  <left charge + right charge in a given cell>
 *  @var charge_product  <2*leftcharge*rightcharge/(leftcharge+rightcharge)>
 *  @var detector  <kl>
 *  @var part_event_number  <number of particle in the phys_event>
 */

class KLCellHit {
  public:
    /** Default constructor - initialises to 0/NULL */
    KLCellHit();

    /** Constructor - initialises member variables from arguments */
  KLCellHit(int cell, int phys_event_number, int charge,  int charge_product,
            std::string detector, int part_event_number);

    /** Copy constructor - any pointers are deep copied */
    KLCellHit(const KLCellHit& _klcellhit);

    /** Equality operator - any pointers are deep copied */
    KLCellHit& operator=(const KLCellHit& _klcellhit);

    /** Destructor - any member pointers are deleted */
    virtual ~KLCellHit();


    /** Returns Cell */
    int GetCell() const;

    /** Sets Cell */
    void SetCell(int cell);

    /** Returns PhysEventNumber */
    int GetPhysEventNumber() const;

    /** Sets PhysEventNumber */
    void SetPhysEventNumber(int phys_event_number);

    /** Returns Charge */
    int GetCharge() const;

    /** Sets Charge */
    void SetCharge(int charge);

    /** Returns ChargeProduct */
    int GetChargeProduct() const;

    /** Septs ChargeProduct */
    void SetChargeProduct(int charge_product);

    /** Returns Detector */
    std::string GetDetector() const;

    /** Sets Detector */
    void SetDetector(std::string detector);

    /** Returns PartEventNumber */
    int GetPartEventNumber() const;

    /** Sets PartEventNumber */
    void SetPartEventNumber(int part_event_number);


  private:
    int _cell;
    int _phys_event_number;
    int _charge;
    int _charge_product;
    std::string _detector;
    int _part_event_number;

    ClassDef(KLCellHit, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_KLSLABHIT_

