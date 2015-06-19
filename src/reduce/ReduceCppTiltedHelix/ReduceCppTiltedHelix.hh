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

#include "src/common_cpp/API/ReduceBase.hh"
#include "src/common_cpp/API/PyWrapReduceBase.hh"

namespace MAUS {
class ImageData;
class Data;

void reduce_cpp_tilted_helix_minuit_function(int& n_pars, double* pars, double& score, double* buff, int err);

class ReduceCppTiltedHelix : public ReduceBase<Data, ImageData> {
  public:
    ReduceCppTiltedHelix();
    ~ReduceCppTiltedHelix();

  private:

    void _birth(const std::string& config);

    void _death();

    ImageData* _process(Data* data);

    bool will_cut(std::vector<SciFiSpacePoint*> space_points, size_t tracker);

    void do_fit(std::vector<SciFiSpacePoint*> space_points, std::vector<bool> will_cut_tracker);

    void do_fit_pattern_recognition(size_t tracker, std::vector<std::vector<SciFiSpacePoint*> > space_points_by_station);

    void do_fit_minuit(size_t tracker, std::vector<std::vector<SciFiSpacePoint*> > space_points_by_station);

    void fill_residuals_minuit(size_t tracker, std::vector<std::vector<SciFiSpacePoint*> > space_points_by_station);

    std::vector<double> calculate_residual(size_t i, SciFiSpacePoint* space_point, SciFiHelicalPRTrack* pr_track);

    ImageData* get_image_data();

    size_t get_hist_index(size_t tracker, size_t station, size_t x_or_y);

    std::vector<TH1D> hist_vector_;
    static std::vector<std::vector<SciFiSpacePoint*> > space_points_by_station_;
    static TMinuit* minimiser;
    static const size_t n_stations;
    static const size_t n_trackers;
    static const double residuals_cut;
    static const size_t error_level;
    friend void reduce_cpp_tilted_helix_minuit_function(int& n_pars, double* pars, double& score, double* buff, int err);
};

PyMODINIT_FUNC init_ReduceCppTiltedHelix(void) {
    PyWrapReduceBase<ReduceCppTiltedHelix>::PyWrapReduceBaseModInit(
                                  "ReduceCppTiltedHelix", "", "", "", "");
}
}

#endif // SRC_REDUCE_REDUCECPPTILTEDHELIX_HH

