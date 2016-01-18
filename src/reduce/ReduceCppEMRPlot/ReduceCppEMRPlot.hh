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

#ifndef SRC_REDUCECPPEMRPLOT_HH
#define SRC_REDUCECPPEMRPLOT_HH

#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>
#include <iostream>

#include "TROOT.h"
#include "TObject.h"
#include "TH1.h"
#include "TH2.h"
#include "TH2Poly.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TText.h"
#include "TLine.h"
#include "TString.h"

#include "TGraphErrors.h"

#include "API/ReduceBase.hh"
#include "API/PyWrapReduceBase.hh"
#include "DataStructure/ThreeVector.hh"
#include "DataStructure/ImageData/ImageData.hh"
#include "DataStructure/ImageData/Image.hh"
#include "DataStructure/ImageData/CanvasWrapper.hh"
#include "DataStructure/Data.hh"
#include "DataStructure/Spill.hh"
#include "DataStructure/ReconEvent.hh"
#include "DataStructure/EMREvent.hh"
#include "DataStructure/EMRSpillData.hh"
#include "Utils/ReduceCppTools.hh"
#include "Utils/TH2EMR.hh"

namespace MAUS {

class ReduceCppEMRPlot : public ReduceBase<Data, ImageData> {
 public:
  ReduceCppEMRPlot();
  ~ReduceCppEMRPlot();

  int GetRefreshRate() const {return _refresh_rate;}

  void reset();

 private:

  void _birth(const std::string& argJsonConfigDocument);

  void _death();

  void _process(Data* data);

  void update();

  void fill_emr_plots(EMREvent* emr_event);

  size_t _number_of_bars;
  int _process_count;
  int _refresh_rate;
  double _charge_threshold;
  double _density_cut;
  double _chi2_cut;

  std::vector<TCanvas*> _canvs;
  std::vector<TH1*> _histos;
  std::vector<TObject*> _objects;

  TH1F *_hrange_primary, *_hrange_secondary,
       *_htotal_charge_ma, *_htotal_charge_sa,
       *_hcharge_ratio_ma, *_hcharge_ratio_sa,
       *_hdepth_profile;
  TH2F *_hbeam_profile, *_hchi2_density;
  TH2EMR *_hoccupancy_xz, *_hoccupancy_yz;
  TLegend *_leg_range, *_leg_charge;
  TText *_text_match_eff, *_text_pid_stats;
  TLine *_line_density, *_line_chi2;
};
}

#endif // SRC_REDUCECPPEMRPLOT_HH
