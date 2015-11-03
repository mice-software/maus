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

#include "Utils/ReduceCppTools.hh"

namespace MAUS {

CanvasWrapper* ReduceCppTools::get_canvas_wrapper(TCanvas *canv,
                                                  TH1* hist,
                                                  std::string name,
                                                  std::string description,
                                                  std::string text_box_str) {

  CanvasWrapper *wrap = new CanvasWrapper();
  wrap->SetDescription(description);
  wrap->SetFileTag(name);
  wrap->SetCanvas(canv);

  canv->cd();
  hist->Draw();
  gPad->Update();
  TPaveStats *st = static_cast<TPaveStats*>(hist->FindObject("stats"));
  st->SetOptStat(10);
//   hist->SetStats(false);
//   TPaveText* text_box = new TPaveText(0.6, 0.5, 1.0, 0.9, "NDC");
//   text_box->AddText(description.c_str());

  return wrap;
}

CanvasWrapper* ReduceCppTools::get_canvas_multi_wrapper(TCanvas *canv,
                                 	       	        std::vector<TObject*> objarray,
					          	Option_t *draw_option,
                                                  	std::string name,
                                                  	std::string description) {

  CanvasWrapper *wrap = new CanvasWrapper();
  wrap->SetDescription(description);
  wrap->SetFileTag(name);
  wrap->SetCanvas(canv);

  canv->cd();
  objarray[0]->Draw(draw_option);
  for (size_t i = 1; i < objarray.size(); i++)
    objarray[i]->Draw("SAME");

  return wrap;
}

CanvasWrapper* ReduceCppTools::get_canvas_emr_wrapper(TCanvas *canv,
                                 	              TH2EMR *hemr,
                                                      std::string name,
                                                      std::string description) {

  CanvasWrapper *wrap = new CanvasWrapper();
  wrap->SetDescription(description);
  wrap->SetFileTag(name);
  wrap->SetCanvas(canv);

  canv->cd();
  hemr->GetHistogram()->Draw("COLZ L");

  return wrap;
}
}
