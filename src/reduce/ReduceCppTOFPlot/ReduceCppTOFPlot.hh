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

#ifndef SRC_REDUCECPPTOFPLOT_HH
#define SRC_REDUCECPPTOFPLOT_HH 1

#include <string>
#include <iostream>
#include <vector>

#include "TROOT.h"
#include "TH1.h"
#include "TCanvas.h"

#include "src/common_cpp/API/ReduceBase.hh"
#include "src/common_cpp/API/PyWrapReduceBase.hh"
#include "src/common_cpp/DataStructure/ImageData/ImageData.hh"
#include "src/common_cpp/DataStructure/ImageData/Image.hh"
#include "src/common_cpp/DataStructure/ImageData/CanvasWrapper.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/Utils/ReduceCppTools.hh"

namespace MAUS {

class ReduceCppTOFPlot : public ReduceBase<Data, ImageData> {
 public:
  ReduceCppTOFPlot();
  ~ReduceCppTOFPlot();

  int  getRefreshRate() {return _refresh_rate;}

  void reset();

 private:

  void _birth(const std::string& argJsonConfigDocument);

  void _death();

  void _process(Data* data);

  void update();

  void update_tof_plots(TOFEvent* tof_event);

  int _refresh_rate;
  int _process_count;

  std::vector<TH1F*> _histos;
  std::vector<TCanvas*> _canvs;
  TH1F *_h_tof01, *_h_tof12, *_h_tof02;
  TH1F *_hspslabx_0, *_hspslabx_1, *_hspslabx_2;
  TH1F *_hspslaby_0, *_hspslaby_1, *_hspslaby_2;
  TH1F *_hnsp_0, *_hnsp_1, *_hnsp_2;
  TH2F *_hspxy[3];
  TH2F *_hspxy_0, *_hspxy_1, *_hspxy_2;
  TCanvas *_canv_tof01, *_canv_tof02, *_canv_tof12;
  TCanvas *_canv_tof_spslabx, *_canv_tof_spslaby;
  TCanvas *_canv_tof0_spxy, *_canv_tof1_spxy, *_canv_tof2_spxy;
  TCanvas *_canv_tof_nsp;
};
}

#endif // SRC_REDUCECPPTOFPLOT_HH
