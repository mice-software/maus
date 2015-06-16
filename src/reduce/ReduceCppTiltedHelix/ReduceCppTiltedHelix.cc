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

#include <Python.h>
#include <stdlib.h>

#include <vector>

#include "TCanvas.h"
#include "TH1D.h"

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/ImageData.hh"
#include "src/common_cpp/DataStructure/Image.hh"
#include "src/common_cpp/DataStructure/CanvasWrapper.hh"

#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"

#include "src/legacy/Interface/STLUtils.hh"

#include "src/reduce/ReduceCppTiltedHelix/ReduceCppTiltedHelix.hh"

namespace MAUS {

const size_t ReduceCppTiltedHelix::n_stations = 5;
const size_t ReduceCppTiltedHelix::n_trackers = 2;

ReduceCppTiltedHelix::ReduceCppTiltedHelix()
  : ReduceBase<Data, ImageData>("ReduceCppTiltedHelix") {
    for (size_t i = 0; i < n_trackers; ++i) {
        std::string i_str = STLUtils::ToString(i);
        for (size_t j = 0; j < n_stations; ++j) {
            std::string j_str = STLUtils::ToString(j);
            hist_vector_.push_back(
                TH1D(("station_"+i_str+"_plane_"+j_str).c_str(),
                      ";#chi^{2}", 1000, -10, 10));
        }
    }
}

ReduceCppTiltedHelix::~ReduceCppTiltedHelix() {}

void ReduceCppTiltedHelix::_birth(const std::string& config) {}

void ReduceCppTiltedHelix::_death() {}

bool ReduceCppTiltedHelix::will_cut(std::vector<SciFiSpacePoint*> space_points, size_t tracker) {
    std::vector<size_t> n_space_points_per_station(n_stations, 0);
    for (size_t i = 0; i < space_points.size(); ++i) {
        if (space_points[i]->get_tracker() == int(tracker)) {
            n_space_points_per_station[space_points[i]->get_station()] += 1;
        }
    }
    // cut if we don't have exactly one space point per station
    for (size_t i = 0; i < n_stations; ++i)
        if (n_space_points_per_station[i] != 1)
            return true;
    for (size_t i = 0; i < space_points.size(); ++i) {
          if (space_points[i] == NULL)
              throw(Exception(Exception::recoverable,
                              "Space points were NULL",
                              "ReduceCppTiltedHelix::cut"));
    }
    return false;
}

ImageData* ReduceCppTiltedHelix::get_image_data() {
    ImageData* image_data = new ImageData();
    Image * image = new Image();
    std::vector<CanvasWrapper*> canvas_wrappers;
    for (size_t i = 0; i < hist_vector_.size(); ++i) {
        std::string i_str = STLUtils::ToString(i);
        CanvasWrapper* wrap = new CanvasWrapper();
        wrap->SetDescription("Space point residuals station "+i_str);
        wrap->SetFileTag("ReduceCppHelixStation_"+
                         std::string(hist_vector_[i].GetName()));
        TCanvas* canvas = new TCanvas();
        hist_vector_[i].Draw();
        canvas->Update();
        wrap->SetCanvas(canvas);
        canvas_wrappers.push_back(wrap);
    }
    image->SetCanvasWrappers(canvas_wrappers);
    image_data->SetImage(image);
    return image_data;
}

void ReduceCppTiltedHelix::do_fit(std::vector<SciFiSpacePoint*> space_points,
                                  std::vector<bool> will_cut_tracker) {
    for (size_t tracker = 0; tracker < n_trackers; ++tracker) {
        if (will_cut_tracker[tracker])
            continue;
        std::vector<std::vector<SciFiSpacePoint*> > space_points_by_station(n_stations, std::vector<SciFiSpacePoint*>());
        std::cerr << "Do fit tracker " << tracker << std::endl;
        std::cerr << "    Will cut " << tracker << " " <<will_cut_tracker[tracker] << std::endl;
        for (size_t j = 0; j < space_points.size(); ++j) {
            if (space_points[j]->get_tracker() == int(tracker)) {
               int station = space_points[j]->get_station();
               space_points_by_station[station].push_back(space_points[j]);
            }
        }
        std::vector<SciFiStraightPRTrack*> strks;
        std::vector<SciFiHelicalPRTrack*> htrks;
        std::cerr << "    htracks 1 " << htrks.size() << " " << tracker << std::endl;
        PatternRecognition().make_5tracks(true, tracker, space_points_by_station, strks, htrks);
        std::cerr << "    htracks 2 " << htrks.size() << " " << tracker << std::endl;
        if (htrks.size() == 1) {
        }
    }
}

MAUS::ImageData* ReduceCppTiltedHelix::_process(MAUS::Data* data) {
    if (data == NULL)
        throw Exception(Exception::recoverable, "Data was NULL",
                        "ReduceCppTiltedHelix::_process");
    if (data->GetSpill() == NULL)
        throw Exception(Exception::recoverable, "Spill was NULL",
                        "ReduceCppTiltedHelix::_process");
    if (data->GetSpill()->GetReconEvents() == NULL)
        throw Exception(Exception::recoverable, "ReconEvents were NULL",
                        "ReduceCppTiltedHelix::_process");
    std::vector<ReconEvent*>* recon_events = data->GetSpill()->GetReconEvents();
    for (size_t i = 0; i < recon_events->size(); ++i) {
        if (recon_events->at(i)->GetSciFiEvent() == NULL) {
            throw Exception(Exception::recoverable,
                            "ReconEvents were NULL",
                            "ReduceCppTiltedHelix::_process");
        }
        std::vector<SciFiSpacePoint*> space_points =
                            recon_events->at(i)->GetSciFiEvent()->spacepoints();
        // cut space points if they don't conform to criteria
        std::vector<bool> will_cut_tracker(n_trackers, false);
        for (size_t i = 0; i < n_trackers; ++i)
            will_cut_tracker[i] = will_cut(space_points, i);
        do_fit(space_points, will_cut_tracker);
    }
    return get_image_data();
}
}

