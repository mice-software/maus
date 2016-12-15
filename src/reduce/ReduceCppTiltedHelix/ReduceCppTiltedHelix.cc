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
#include <iostream>
//#include <ofstream>

#include "TCanvas.h"
#include "TH1D.h"
#include "TMinuit.h"
#include "TFitResult.h"
#include "TF1.h"
#include "TLatex.h"
#include "TPaveText.h"

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/TOFEvent.hh"
#include "src/common_cpp/DataStructure/TOFEventSpacePoint.hh"
#include "src/common_cpp/DataStructure/TOFSpacePoint.hh"


#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"

#include "src/common_cpp/DataStructure/ImageData/ImageData.hh"
#include "src/common_cpp/DataStructure/ImageData/Image.hh"
#include "src/common_cpp/DataStructure/ImageData/CanvasWrapper.hh"

#include "src/common_cpp/ReduceCppTiltedHelix_NS/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/ReduceCppTiltedHelix_NS/PatternRecognition.hh"

#include "src/legacy/Interface/STLUtils.hh"
#include "src/common_cpp/Utils/Squeak.hh"

#include "src/reduce/ReduceCppTiltedHelix/ReduceCppTiltedHelix.hh"

namespace MAUS {
namespace ReduceCppTiltedHelix_NS {
const size_t ReduceCppTiltedHelix::error_level = 0;
const size_t ReduceCppTiltedHelix::n_stations = 5;
const size_t ReduceCppTiltedHelix::n_trackers = 2;
const double ReduceCppTiltedHelix::residuals_cut = 5;
TMinuit* ReduceCppTiltedHelix::minimiser = NULL;
std::vector<std::vector<SciFiSpacePoint*> > ReduceCppTiltedHelix::space_points_by_station_;

ReduceCppTiltedHelix::ReduceCppTiltedHelix()
  : ReduceBase<Data, ImageData>("ReduceCppTiltedHelix") {
    clear_image_data();
    for (size_t i = 0; i < n_trackers; ++i) {
        std::string i_str = STLUtils::ToString(i);
        hist_canvas_ = new TCanvas("tracker_residuals", "tracker_residuals");
        hist_canvas_->Divide(5, 1);
        for (size_t j = 0; j < n_stations; ++j) {
            std::string j_str = STLUtils::ToString(j);
            std::string name_x = "tracker_"+i_str+"_station_"+j_str;
            std::string title_x = name_x+";#kappa - 1";
            hist_vector_.push_back(
                    new TH1D(name_x.c_str(), title_x.c_str(), 100, -2., 2.)
            );
            hist_vector_.back()->SetLineWidth(2);
        }
        tof_canvas_ = new TCanvas("tof12", "tof12");
        tof_hist_ = new TH1D("tof12", "TOF between 1 and 2;tof 2-tof1 [ns]", 100, 25., 50.);
        tof_hist_->SetLineWidth(2);
    }
}

ReduceCppTiltedHelix::~ReduceCppTiltedHelix() {}

void ReduceCppTiltedHelix::_birth(const std::string& str_config) {
    // 
    Json::Value config = JsonWrapper::StringToJson(str_config);
    _fit_range = JsonWrapper::GetProperty(config, "fit_range", JsonWrapper::realValue).asDouble(); // fit histograms in range +/- fit_range [rad]
    _will_do_cuts = JsonWrapper::GetProperty(config, "fit_do_cuts", JsonWrapper::booleanValue).asBool(); // if true, require exactly one TOF1 and one TOF2 event; 15 clusters per tracker; 1 space point per tracker station in a given tracker 
    _fit_tx = JsonWrapper::GetProperty(config, "fit_tx", JsonWrapper::booleanValue).asBool(); // float theta x as a free parameter
    _fit_ty = JsonWrapper::GetProperty(config, "fit_ty", JsonWrapper::booleanValue).asBool(); // float theta y as a free parameter
    _nbins = JsonWrapper::GetProperty(config, "fit_nbins", JsonWrapper::intValue).asInt(); // number of bins in the histograms
    _fit_refresh_rate = JsonWrapper::GetProperty(config, "fit_refresh_rate", JsonWrapper::intValue).asInt(); // histogram refresh rate
    _fit_file = std::string(JsonWrapper::GetProperty(config, "fit_file", JsonWrapper::stringValue).asString()); // file to write data to
    _fit_w_tx_seed = JsonWrapper::GetProperty(config, "fit_w_tx_seed", JsonWrapper::realValue).asDouble(); // put in a seed value; all things being equal, the fit should converge on 0
    _fit_w_ty_seed = JsonWrapper::GetProperty(config, "fit_w_ty_seed", JsonWrapper::realValue).asDouble(); // put in a seed value; all things being equal, the fit should converge on 0
    

    th_canvas_vector_.push_back(new TCanvas("theta0_x", "theta0_x"));
    th_canvas_vector_.push_back(new TCanvas("theta0_y", "theta0_y"));
    th_canvas_vector_.push_back(new TCanvas("theta1_x", "theta1_x"));
    th_canvas_vector_.push_back(new TCanvas("theta1_y", "theta1_y"));

    th_hist_vector_.push_back(new TH1D("station_0_theta_x", "TKU #theta_{x}; #theta_{x} = a_{4}/(2a_{2}) [rad]", _nbins, -0.2, +0.2));
    th_hist_vector_.push_back(new TH1D("station_0_theta_y", "TKU #theta_{y}; #theta_{y} = a_{5}/(2a_{2}) [rad]", _nbins, -0.2, +0.2));
    th_hist_vector_.push_back(new TH1D("station_1_theta_x", "TKD uw #theta_{x}; a_{4}/2 = #delta#theta_{x} [rad]", _nbins, -0.2, +0.2));
    th_hist_vector_.push_back(new TH1D("station_1_theta_y", "TKD vw #theta_{y}; a_{5}/2 = #delta#theta_{y} [rad]", _nbins, -0.2, +0.2));

    w_coeff_canvas_.push_back(new TCanvas("w_coeff_TKU", "w_coeff_TKU"));
    w_coeff_canvas_.push_back(new TCanvas("w_coeff_TKD", "w_coeff_TKD"));

    w_coeff_hist_.push_back(new TH1D("w_coeff", "TKU w coefficient;a_{3}/(a_{0}a_{4}+a_{1}a_{5})", _nbins, -100, 100));
    w_coeff_hist_.push_back(new TH1D("w_coeff", "TKD w coefficient;a_{3}/(a_{0}a_{4}+a_{1}a_{5})", _nbins, -100, 100));

    w_tx_canvas_.push_back(new TCanvas("TKU_w_tx", "TKU_w_tx"));
    w_tx_canvas_.push_back(new TCanvas("TKD_w_tx", "TKD_w_tx"));
    w_tx_hist_.push_back(new TH1D("w_tx", "TKU w #theta_{x};#theta_{x} = a_{3}/a_{0} [rad]", _nbins, -0.2, +0.2));
    w_tx_hist_.push_back(new TH1D("w_tx", "TKD w #theta_{x};#theta_{x} = a_{3}/a_{0} [rad]", _nbins, -0.2, +0.2));

    w_ty_canvas_.push_back(new TCanvas("TKU_w_ty", "TKU_w_ty"));
    w_ty_canvas_.push_back(new TCanvas("TKD_w_ty", "TKD_w_ty"));
    w_ty_hist_.push_back(new TH1D("w_ty", "TKU w #theta_{y};#theta_{y} = a_{3}/a_{0} [rad]", _nbins, -0.2, +0.2));
    w_ty_hist_.push_back(new TH1D("w_ty", "TKD w #theta_{y};#theta_{y} = a_{3}/a_{0} [rad]", _nbins, -0.2, +0.2));
}

void ReduceCppTiltedHelix::_death() {}

bool ReduceCppTiltedHelix::will_cut_tof(ReconEvent* event) {
    std::vector<TOFSpacePoint>* tof1_sps = event->GetTOFEvent()->GetTOFEventSpacePointPtr()->GetTOF1SpacePointArrayPtr();
    std::vector<TOFSpacePoint>* tof2_sps = event->GetTOFEvent()->GetTOFEventSpacePointPtr()->GetTOF2SpacePointArrayPtr();
    if (tof2_sps->size() != 1 || tof1_sps->size() != 1) {
        return true; // require exactly one space point in tof1 and tof2
    }
    double delta_t = tof2_sps->at(0).GetTime() - tof1_sps->at(0).GetTime();
    tof_hist_->Fill(delta_t);
    return false;
}

bool ReduceCppTiltedHelix::will_cut_sps(std::vector<SciFiSpacePoint*> space_points, size_t tracker) {
    std::vector<size_t> n_space_points_per_station(n_stations, 0);
    size_t n_clusters = 0;
    for (size_t i = 0; i < space_points.size(); ++i) {
        if (space_points[i]->get_tracker() == int(tracker)) {
            n_space_points_per_station[space_points[i]->get_station()-1] += 1;
            n_clusters += space_points[i]->get_channels_pointers().size();
        }
    }
    // cut if we don't have exactly 3 clusters per (5) space points
    if (n_clusters != 15) {
        return true;
    }
    // cut if we don't have exactly one space point per station
    for (size_t i = 0; i < n_stations; ++i) {
        if (n_space_points_per_station[i] != 1) {
            return true;
        }
    }
    for (size_t i = 0; i < space_points.size(); ++i) {
          if (space_points[i] == NULL)
              throw(Exceptions::Exception(Exceptions::recoverable,
                                          "Space points were NULL",
                                          "ReduceCppTiltedHelix::cut"));
    }
    return false;
}

size_t ReduceCppTiltedHelix::get_hist_index(size_t tracker, size_t station) {
    return (tracker*n_stations + station-1);
}

void ReduceCppTiltedHelix::update_canvas(TCanvas* canvas, TH1* hist, std::string name, std::string description, std::string text_box_str, TF1* fit) {
    std::vector<std::string> lines;
    canvas->cd();
    std::cerr << "ReduceCppTiltedHelix::new_canvas_wrapper 29 " << hist << std::endl;
    hist->Fill(0.);
    std::cerr << "ReduceCppTiltedHelix::new_canvas_wrapper 35 " << hist << std::endl;
    hist->Draw();
    std::cerr << "ReduceCppTiltedHelix::new_canvas_wrapper 39" << std::endl;
    if (fit != NULL) {
        std::cerr << "ReduceCppTiltedHelix::new_canvas_wrapper 49" << std::endl;
        hist->Fit(fit, "", "", -_fit_range, _fit_range);
        text_box_str = fit_caption(hist, fit);
        hist->SetStats(false);
        TPaveText* text_box = new TPaveText(0.6, 0.5, 1.0, 0.9, "NDC");
        text_box->SetFillColor(0);
        text_box->SetBorderSize(0);
        text_box->SetTextSize(0.04);
        text_box->SetTextAlign(12);
        text_box->SetTextSize(0.03);
        std::cerr << "ReduceCppTiltedHelix::new_canvas_wrapper 69" << std::endl;

        size_t pos = 0;
        while (pos < std::string::npos && pos < 1000) {
            pos = text_box_str.find('\n');
            lines.push_back(text_box_str.substr(0, pos+1));
            text_box_str = text_box_str.substr(pos+1, std::string::npos);
            text_box->AddText(lines.back().c_str());
        }
        text_box->Draw();
    }
    std::cerr << "ReduceCppTiltedHelix::new_canvas_wrapper 89" << std::endl;
    canvas->Update();
    std::cerr << "ReduceCppTiltedHelix::new_canvas_wrapper 99" << std::endl;
}

std::string ReduceCppTiltedHelix::fit_caption(TH1* hist, TF1* result) {
    double fit_content = 0;
    for (int j = 0; j < _nbins+2; ++j) {
        double center = hist->GetBinCenter(j);
        if (center > -_fit_range && center < _fit_range) {
            fit_content += hist->GetBinContent(j);
        }
    }
    double hist_content = 0;
    for (int j = 0; j < _nbins+2; ++j) {
        hist_content += hist->GetBinContent(j);
    }
    std::stringstream strstr;
    strstr << "Run " << run << "\n" << "N in hist " << hist_content
           << "\nN in fit " << fit_content << "\n"
           << "p_{0}/(1+p_{1}(x-p_{2})^{2})\n"
           << "p_{0} " << result->GetParameter(0) << " +/- " << result->GetParError(0) << "\n"
           << "p_{1} " << result->GetParameter(1) << " +/- " << result->GetParError(1) << "\n"
           << "p_{2} " << result->GetParameter(2) << " +/- " << result->GetParError(2) << "\n"
           << "#chi^{2} " << result->GetChisquare() << " Ndf " << result->GetNDF() << "\n"
           << " P-Value " << result->GetProb() << std::endl;
    return strstr.str();
}

void ReduceCppTiltedHelix::get_image_data() {
    std::cerr << "get_image_data tof hist" << std::endl;
    tof_hist_->Draw();
    std::cerr << "ASDS" << std::endl;
    std::cerr << "ASDSasdas" << std::endl;
    tof_hist_->Draw();
    std::cerr << "ASDSpingping" << std::endl;
    std::cerr << "get_image_data tof hist new wrap" << std::endl;
    update_canvas(tof_canvas_, tof_hist_, "tof12", "TOF", "", NULL);
    std::string formula = "[0]/(1+[1]*(x-[2])**2)";

    std::ofstream fout(_fit_file.c_str());

    for (size_t i = 0; i < n_trackers; ++i) {
        std::string tracker_str = "_tracker_"+STLUtils::ToString(i);
        std::string name = "residuals"+tracker_str;
        for (size_t station = 1; station <= n_stations; ++station) {
            hist_canvas_->cd(station);
            hist_vector_[get_hist_index(i, station)]->Draw();
        }
        hist_canvas_->Update();
        fout << "{\"tracker\" : " << i << ", \"run\" : " << run;
        TF1* fit_x = new TF1("fit_x", formula.c_str(), -_fit_range, _fit_range);
        std::cerr << "get_image_data " << i*2 << std::endl;
        update_canvas(
              th_canvas_vector_[i*2],
              th_hist_vector_[i*2],
              "tx"+tracker_str,
              "Fitted theta residuals for trackers",
              "",
              fit_x
        );

        std::cerr << "get_image_data " << i*2+1 << std::endl;
        TF1* fit_y = new TF1("fit_y", formula.c_str(), -_fit_range, _fit_range);
        update_canvas(
              th_canvas_vector_[i*2+1],
              th_hist_vector_[i*2+1],
              "ty"+tracker_str,
              "Fitted theta residuals for trackers",
              "",
              fit_y
        );
        fout << ", \"w_tx\" : " << fit_x->GetParameter(2)
             << ", \"sw_tx\" : " << fit_x->GetParError(2)
             << ", \"w_ty\" : " << fit_y->GetParameter(2)
             << ", \"sw_ty\" : " << fit_y->GetParError(2)
             << ", \"n_entries\" : " << w_coeff_hist_[i]->GetSumOfWeights()
             << "}" << std::endl;

        std::cerr << "get_image_data coeffs" << std::endl;
        update_canvas(w_coeff_canvas_[i], w_coeff_hist_[i], "w_coeff"+tracker_str, "TKD w coefficient", "", NULL);
        TF1* fit_wx = new TF1("fit_wx", formula.c_str(), -_fit_range, _fit_range);
        update_canvas(w_tx_canvas_[i], w_tx_hist_[i], "w_tx"+tracker_str, "TKD w coefficient #theta_{x}; only valid if #theta_{y} reconstruction is disabled", "", fit_wx);

        TF1* fit_wy = new TF1("fit_wy", formula.c_str(), -_fit_range, _fit_range);
        update_canvas(w_ty_canvas_[i], w_ty_hist_[i], "w_ty"+tracker_str, "TKD w coefficient #theta_{y}; only valid if #theta_{x} reconstruction is disabled", "", fit_wy);
    }
    //_output->GetImage()->SetCanvasWrappers(canvas_wrappers);

    fout.close();
}

double ReduceCppTiltedHelix::calculate_residual_tilted(SciFiSpacePoint* point, SimpleCircle circle) {
    std::vector<double> fit_pars = circle.get_fit_parameters();
    double u_i = point->get_position().x();
    double v_i = point->get_position().y();
    double w_i = point->get_position().z();
    double residual = fit_pars[0]*u_i + fit_pars[1]*v_i + fit_pars[3]*w_i +
                      fit_pars[2]*(u_i*u_i+v_i*v_i) + fit_pars[4]*u_i*w_i +
                      fit_pars[5]*v_i*w_i - 1;
    return residual;
}

void ReduceCppTiltedHelix::rotate_space_point(SciFiSpacePoint* data) {
    // x = u + tx*w; u = x - tx * w
    // y = v + ty*w; v = y - ty * w
    ThreeVector pos = data->get_position();
    pos.setX(pos.x() + _fit_w_tx_seed*pos.z());
    pos.setY(pos.y() + _fit_w_ty_seed*pos.z());
    data->set_position(pos);
}

void ReduceCppTiltedHelix::do_fit_tilted_circle(size_t tracker, std::vector<std::vector<SciFiSpacePoint*> > space_points_by_station) {
    double sd1to4 = 1.;
    double sd5 = 1.;
    std::vector<SciFiSpacePoint*> space_points;
    for (size_t i = 0; i < space_points_by_station.size(); ++i) {
        if (space_points_by_station[i].size() == 0)
            return;
        for (size_t j = 0; j < space_points_by_station[i].size(); ++j) {
            space_points.push_back(space_points_by_station[i][j]);
            rotate_space_point(space_points.back());
        }
    }
    SimpleCircle circle;
    TMatrixD covariance;
    LeastSquaresFitter::tilted_circle_fit(sd1to4, sd5, 1e9,
                  space_points, circle, covariance, _fit_tx, _fit_ty);
    std::vector<double> fit_pars = circle.get_fit_parameters();
    double theta_x = fit_pars[4]/fit_pars[2]/2.;
    double theta_y = fit_pars[5]/fit_pars[2]/2.;
    for (size_t i = 0; i < space_points_by_station.size(); ++i) {
        size_t station = space_points_by_station[i][0]->get_station();
        size_t index = 0;
        if (tracker == 1) {
            index = i;
        } else {
            index = space_points_by_station.size() - i - 1;
        }
        double residual = calculate_residual_tilted(space_points_by_station[i][0], circle);
        std::cout << "Calculate residual tr: " << tracker << " st: " << station << " ind: " << index << " res: " << residual   << " th_x: " << theta_x << " th_y: " <<  theta_y << std::endl;
        hist_vector_[get_hist_index(tracker, station)]->Fill(residual);
    }
    th_hist_vector_[2*tracker]->Fill(theta_x);
    th_hist_vector_[2*tracker+1]->Fill(theta_y);
    double w_coeff = fit_pars[3]/(fit_pars[0]*fit_pars[4]+fit_pars[1]*fit_pars[5]);
    w_coeff_hist_[tracker]->Fill(w_coeff);
    std::cout << "w coefficient " << fit_pars[3] << "/(" << fit_pars[0] << "*" << fit_pars[4] << " + " << fit_pars[0] << "*" << fit_pars[4] << ") = " << w_coeff << std::endl;
    w_tx_hist_[tracker]->Fill(fit_pars[3]/fit_pars[0]);
    w_ty_hist_[tracker]->Fill(fit_pars[3]/fit_pars[1]);
}

void PrintEventData(MAUS::Data* data) {
    if (true);
}

void ReduceCppTiltedHelix::clear_image_data() {
    if (_output != NULL) {
        delete _output;
    }
    _output = new ImageData();
    _output->SetImage(new Image());
}

void ReduceCppTiltedHelix::_process(MAUS::Data* data) {
    if (data == NULL)
        throw Exceptions::Exception(Exceptions::recoverable, "Data was NULL",
                        "ReduceCppTiltedHelix::_process");
    if (data->GetSpill() == NULL)
        throw Exceptions::Exception(Exceptions::recoverable, "Spill was NULL",
                        "ReduceCppTiltedHelix::_process");
    if (data->GetSpill()->GetDaqEventType() != "physics_event")
        return;
    PrintEventData(data);
    if (data->GetSpill()->GetReconEvents() == NULL)
        throw Exceptions::Exception(Exceptions::recoverable, "ReconEvents were NULL",
                        "ReduceCppTiltedHelix::_process");
    std::vector<ReconEvent*>* recon_events = data->GetSpill()->GetReconEvents();
    run = data->GetSpill()->GetRunNumber();
    for (size_t i = 0; i < recon_events->size(); ++i) {
        if (recon_events->at(i)->GetSciFiEvent() == NULL) {
            throw Exceptions::Exception(Exceptions::recoverable,
                            "SciFiEvent was NULL",
                            "ReduceCppTiltedHelix::_process");
        }
        std::vector<SciFiSpacePoint*> space_points =
                            recon_events->at(i)->GetSciFiEvent()->spacepoints();
        // cut space points if they don't conform to criteria
        std::vector<bool> will_cut_tracker(n_trackers, false);
        for (size_t j = 0; j < n_trackers; ++j) {
            will_cut_tracker[j] = _will_do_cuts &&
                                  (will_cut_sps(space_points, j) ||
                                             will_cut_tof(recon_events->at(i)));
        }
        do_fit(space_points, will_cut_tracker);
    }

    if (data->GetSpill()->GetSpillNumber() % _fit_refresh_rate == 0) {
        get_image_data();
    } else {
        return;
    }
}

void ReduceCppTiltedHelix::do_fit(std::vector<SciFiSpacePoint*> space_points,
                                  std::vector<bool> will_cut_tracker) {
    for (size_t tracker = 0; tracker < n_trackers; ++tracker) {
        if (will_cut_tracker[tracker])
            continue;
        std::vector<std::vector<SciFiSpacePoint*> > space_points_by_station(n_stations, std::vector<SciFiSpacePoint*>());
        for (size_t j = 0; j < space_points.size(); ++j) {
            if (space_points[j]->get_tracker() == int(tracker)) {
               int station = space_points[j]->get_station();
               space_points_by_station[station-1].push_back(space_points[j]);
            }
        }
        do_fit_tilted_circle(tracker, space_points_by_station);
    }
}
}
}

