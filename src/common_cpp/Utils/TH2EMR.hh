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

#ifndef _TH2EMR_H_
#define _TH2EMR_H_

#include <iostream>
#include <vector>
#include <algorithm>

#include "TStyle.h"
#include "TString.h"
#include "TColor.h"
#include "TCanvas.h"
#include "TH2Poly.h"
#include "TF1.h"

namespace MAUS {

class TH2EMR {
  public:

    /** Default constructor - initialises to 0/NULL */
    TH2EMR();

    /** Copy constructor - any pointers are deep copied */
    TH2EMR(const TH2EMR& _th2emr);

    /** Equality operator - any pointers are deep copied */
    TH2EMR& operator=(const TH2EMR& _th2emr);

    /** Constructor with name and title - Define the TH2Poly with the EMR structure */
    TH2EMR(const char* name, const char* title);

    /** Destructor - any member pointers are deleted */
    virtual ~TH2EMR();

    /** Fills the bin corresponding to bar j of plane i an additional hit */
    void Fill(int i, int j);

    /** Fills the bin corresponding to bar j of plane i with tot w */
    void Fill(int i, int j, double w);

    /** Fills the i plane charge with charge w */
    void FillPlane(int i, double w);

    /** Draws histograms in COLZ by default */
    void Draw();

    /** Gets the bin content corresponding to bar j of plane i an additional hit */
    double GetBinContent(int i, int j);

    /** Sets the bin content corresponding to bar j of plane i an additional hit */
    void SetBinContent(int i, int j, double w);

    /** Save the two histograms to a root file **/
    void Write();

  private:

    TH2Poly *_h;
    std::vector<int> _v;
};
} // namespace MAUS

#endif
