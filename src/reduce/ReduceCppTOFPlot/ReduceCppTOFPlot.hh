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

#include "TH1.h"
#include "TCanvas.h"

#include "src/common_cpp/API/ReduceBase.hh"
#include "src/common_cpp/API/PyWrapReduceBase.hh"
#include "src/common_cpp/DataStructure/ImageData/ImageData.hh"
#include "src/common_cpp/DataStructure/ImageData/Image.hh"
#include "src/common_cpp/DataStructure/ImageData/CanvasWrapper.hh"

namespace MAUS {

class ImageData;
class Data;

class ReduceCppTools {
 public:
  static CanvasWrapper* get_canvas_wrapper(TCanvas *canv,
                                 TH1* hist,
                                 std::string name,
                                 std::string description,
                                 std::string text_box_str);
};

class ReduceCppTOFPlot : public ReduceBase<Data, ImageData> {
 public:
  ReduceCppTOFPlot();
  ~ReduceCppTOFPlot();

  int  getRefreshRate() {return _refresh_rate;}
  void reset();

 private:

  void _birth(const std::string& config);

  void _death();

  void _process(Data* data);

  void update_tof_plots(TOFEvent* tof_event);

  int _refresh_rate;

  std::vector<TH1F*> _histos;
  TH1F *_h_tof01, *_h_tof12, *_h_tof02;
  TCanvas *_canv_tof01, *_canv_tof02, *_canv_tof12;
  CanvasWrapper *_cwrap_tof01, *_cwrap_tof02, *_cwrap_tof12;
};
}

#endif // SRC_REDUCECPPTOFPLOT_HH
