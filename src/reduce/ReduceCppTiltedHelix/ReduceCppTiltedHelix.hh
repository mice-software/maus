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

#ifndef SRC_REDUCE_REDUCECPPTILTEDHELIX_HH
#define SRC_REDUCE_REDUCECPPTILTEDHELIX_HH

#include <string>
#include <iostream>

#include "src/common_cpp/API/ReduceBase.hh"
#include "src/common_cpp/API/PyWrapReduceBase.hh"

namespace MAUS {

class ImageData;
class Data;

namespace ReduceCppTiltedHelix_NS {

class SciFiHelicalPRTrack;

void reduce_cpp_tilted_helix_minuit_function(int& n_pars, double* pars, double& score, double* buff, int err);

class ReduceCppTiltedHelix : public ReduceBase<Data, ImageData> {
  public:
    ReduceCppTiltedHelix();
    ~ReduceCppTiltedHelix();

  private:

    void _birth(const std::string& config);

    void _death();

    void _process(Data* data);

    bool will_cut_sps(std::vector<SciFiSpacePoint*> space_points, size_t tracker);

    bool will_cut_tof(ReconEvent* event);

    void do_fit(std::vector<SciFiSpacePoint*> space_points, std::vector<bool> will_cut_tracker);

    void do_fit_pattern_recognition(size_t tracker, std::vector<std::vector<SciFiSpacePoint*> > space_points_by_station);

    void do_fit_tilted_circle(size_t tracker, std::vector<std::vector<SciFiSpacePoint*> > space_points_by_station);

    void do_fit_circle(size_t tracker, std::vector<std::vector<SciFiSpacePoint*> > space_points_by_station);

    double calculate_residual_tilted(SciFiSpacePoint* point, SimpleCircle circle);

    double calculate_residual_circle(SciFiSpacePoint* point, SimpleCircle circle);

    std::vector<double> calculate_residual(size_t i, SciFiSpacePoint* space_point, MAUS::ReduceCppTiltedHelix_NS::SciFiHelicalPRTrack* pr_track);

    void get_image_data();
    void clear_image_data();

    std::string fit_caption(TH1* hist, TF1* result);

    void update_canvas(TCanvas* canvas, TH1* hist, std::string name, std::string description, std::string text_box, TF1* fit);

    size_t get_hist_index(size_t tracker, size_t station);

    void rotate_space_point(SciFiSpacePoint* data);

    void print_canvas(TCanvas* canvas);

    int run;
    TCanvas* hist_canvas_;
    std::vector<TH1D*> hist_vector_;
    std::vector< std::vector<double> > hist_data_;
    std::vector<TCanvas*> th_canvas_vector_;
    std::vector<TH1D*> th_hist_vector_;
    std::vector< std::vector<double> > th_hist_data_;
    std::vector<TCanvas*> w_coeff_canvas_;
    std::vector<TH1D*> w_coeff_hist_;
    std::vector<TCanvas*> w_tx_canvas_;
    std::vector<TH1D*> w_tx_hist_;
    std::vector<TCanvas*> w_ty_canvas_;
    std::vector<TH1D*> w_ty_hist_;
    TCanvas* tof_canvas_;
    TH1D* tof_hist_;
    TH1D* tof_hist_all_;
    static std::vector<std::vector<SciFiSpacePoint*> > space_points_by_station_;
    static TMinuit* minimiser;
    static const size_t n_stations;
    static const size_t n_trackers;
    static const double residuals_cut;
    static const size_t error_level;
    friend void reduce_cpp_tilted_helix_minuit_function(int& n_pars, double* pars, double& score, double* buff, int err);

    double _fit_range;
    double _tof12_upper;
    double _tof12_lower;
    bool _fit_tx;
    bool _fit_ty;
    bool _will_do_cuts;
    int _nbins;
    int _fit_refresh_rate;
    std::string _fit_file;
    double _fit_w_tx_seed;
    double _fit_w_ty_seed;
    double _track_count[2];
};

PyMODINIT_FUNC init_ReduceCppTiltedHelix(void) {
    PyWrapReduceBase<ReduceCppTiltedHelix>::PyWrapReduceBaseModInit(
                                  "ReduceCppTiltedHelix", "", "", "", "");
}
}
}
#endif // SRC_REDUCE_REDUCECPPTILTEDHELIX_HH

