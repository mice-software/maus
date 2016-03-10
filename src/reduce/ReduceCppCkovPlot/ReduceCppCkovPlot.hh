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

#ifndef SRC_REDUCECPPCKOVPLOT_HH
#define SRC_REDUCECPPCKOVPLOT_HH 1

#include <iostream>
#include <string>
#include <vector>
#include <deque>

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TSystem.h"

#include "src/common_cpp/API/ReduceBase.hh"
#include "src/common_cpp/API/PyWrapReduceBase.hh"
#include "src/common_cpp/DataStructure/ImageData/ImageData.hh"
#include "src/common_cpp/DataStructure/ImageData/Image.hh"
#include "src/common_cpp/DataStructure/ImageData/CanvasWrapper.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/CkovEvent.hh"
#include "src/common_cpp/DataStructure/TOFEvent.hh"

namespace MAUS {

class ReduceCppCkovPlot : public ReduceBase<Data, ImageData> {
 public:
  ReduceCppCkovPlot();
  ~ReduceCppCkovPlot();

  int  getRefreshRate() {return _refresh_rate;}
  void reset();
  void update();

 private:

  void _birth(const std::string& config);

  void _death();

  void _process(Data* data);

  void fill_Ckov_plots(MAUS::CkovEvent* evt, MAUS::TOFEvent* tof_evt);
  void update_Ckov_plots();

  int _refresh_rate;
  int _process_count;

  std::vector<TH1*>     _histos;
  std::vector<TCanvas*> _canvs;

  TH1F *_htof;
  std::vector<TH1 *> _hcharge, _htime;
  TH2F *_htof_A, *_htof_B;

// //   TLegend *_legend;
//   TCanvas *_canvas_charge, *_canvas_time, *_canvas_tof, *_canvas_tof_A, *_canvas_tof_B;
//   CanvasWrapper *_cwrap_charge, *_cwrap_time, *_cwrap_tof, *_cwrap_tof_A, *_cwrap_tof_B;
};
}

#endif // SRC_REDUCECPPKLPLOT_HH
