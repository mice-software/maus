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

#ifndef SRC_ReduceCppSciFiPLOT_HH
#define SRC_ReduceCppSciFiPLOT_HH 1

#include <string>
#include <iostream>
#include <vector>

#include "TROOT.h"
#include "TH1.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TLegend.h"

#include "src/common_cpp/API/ReduceBase.hh"
#include "src/common_cpp/API/PyWrapReduceBase.hh"
#include "src/common_cpp/DataStructure/ImageData/ImageData.hh"
#include "src/common_cpp/DataStructure/ImageData/Image.hh"
#include "src/common_cpp/DataStructure/ImageData/CanvasWrapper.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/Utils/ReduceCppTools.hh"

namespace MAUS {

class ReduceCppSciFiPlot : public ReduceBase<Data, ImageData> {
 public:
  ReduceCppSciFiPlot();
  ~ReduceCppSciFiPlot();

  int  getRefreshRate() {return _refresh_rate;}

  void reset();

 private:

  void _birth(const std::string& argJsonConfigDocument);

  void _death();

  void _process(Data* data);

  void update();

  void update_scifi_plots(SciFiEvent* scifi_event, int runNum);

  int _refresh_rate;
  int _process_count;

  std::vector<TH1*> _histos;
  std::vector<TCanvas*> _canvs;
  std::vector<TH1*> tku_digs, tkd_digs;

  TH1F *_eff_plot, *_track1_2Clus, *_track2_2Clus, *_track1_3Clus, *_track2_3Clus;
  TH1F *_trp_hist[2][6][3], *_dig_ch_hist_up[6][3], *_dig_ch_hist_dn[6][3];
  TH1F *_dig_st_hist[2], *_sp_st_hist[2], *_kuno_hist[2];

  TH2F *_spt_hist[2][6], *_spd_hist[2][6];
};
}

#endif // SRC_ReduceCppSciFiPLOT_HH
