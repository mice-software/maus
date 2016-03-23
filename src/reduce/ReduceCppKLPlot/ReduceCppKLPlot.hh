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

#ifndef SRC_REDUCECPPKLPLOT_HH
#define SRC_REDUCECPPKLPLOT_HH 1

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
#include "src/common_cpp/DataStructure/KLEvent.hh"

namespace MAUS {


class ReduceCppKLPlot : public ReduceBase<Data, ImageData> {
 public:
  ReduceCppKLPlot();
  ~ReduceCppKLPlot();

  int  getRefreshRate() {return _refresh_rate;}
  void reset();
  void update();

 private:

  void _birth(const std::string& config);

  void _death();

  void _process(Data* data);

  void fill_kl_plots(MAUS::KLEvent* evt);
  void update_KL_plots();

  int _refresh_rate;
  int _process_count;

  std::vector<TH1*> _histos;
  TH1F *hadc, *hadc_product, *hprofile;
  TH2F *digitkl;

//   TLegend *_legend;
  TCanvas *_canv_kl;
  CanvasWrapper *_cwrap_kl;
};

TH1F&  operator<< (TH1F& h, std::deque<double> s);
}

#endif // SRC_REDUCECPPKLPLOT_HH
