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

#ifndef _MAUS_REDUCECPPTOOLS_HH_
#define _MAUS_REDUCECPPTOOLS_HH_


#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>
#include <iostream>

#include "TObject.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TPaveStats.h"

#include "API/ReduceBase.hh"
#include "API/PyWrapReduceBase.hh"
#include "DataStructure/ImageData/ImageData.hh"
#include "DataStructure/ImageData/Image.hh"
#include "DataStructure/ImageData/CanvasWrapper.hh"
#include "Utils/TH2EMR.hh"

namespace MAUS {

/** Set of usefull functions used by the Cpp reducers
 ** including wrapper constructors for the TCanvases
 */

class ReduceCppTools {
 public:
  static CanvasWrapper* get_canvas_wrapper(TCanvas *canv,
                                 	   TH1* hist,
                                 	   std::string name = "",
                                 	   std::string description = "",
					   Option_t *draw_option = "");

  static CanvasWrapper* get_canvas_multi_wrapper(TCanvas *canv,
                                 	   	 std::vector<TObject*> objarray,
                                 	   	 std::string name = "",
                                 	   	 std::string description = "",
					   	 Option_t *draw_option = "");

  static CanvasWrapper* get_canvas_emr_wrapper(TCanvas *canv,
                                 	       TH2EMR *hemr,
                                               std::string name = "",
                                               std::string description = "");

  static CanvasWrapper* get_canvas_divide_wrapper(TCanvas *canv, int div_x, int div_y, bool log,
                                                  std::vector<TH1F *> histos,
                                                  std::string name = "",
                                                  std::string description = "");
};
} // namespace MAUS

#endif // _MAUS_REDUCECPPTOOLS_HH_
