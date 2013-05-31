/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

#include <stdlib.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>


#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#include "TTree.h"
#include "TArc.h"
#include "TF1.h"

#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/JsonCppStreamer/IRStream.hh"

/** Access Tracker data using ROOT
 *
 */


class TrackerData {
  /**
   * Class holding data for one tracker.
   * Uses arrays to make compatible with ROOT plotting.
   * May accumulate data over many events. 
   */
  public:
    TrackerData() {
      _trker_num = 0;
      _num_events = 0;
      _num_digits = 0;
      _num_clusters = 0;
      _num_spoints = 0;
      _num_noise_spoints = 0;
      _num_seeds = 0;
      _num_noise_seeds = 0;
      _num_stracks = 0;
      _num_htracks_5pt = 0;
      _num_htracks_4pt = 0;
    };

    ~TrackerData() {}

    void accumulate_data(MAUS::SciFiEvent *evt, int trker_num) {
      _trker_num = trker_num;
      ++_num_events;

      // Loop over digits
      for ( size_t i = 0; i < evt->digits().size(); ++i ) {
         if ( evt->digits()[i]->get_tracker() == trker_num ) {
           ++_num_digits;
         }
      }

      // Loop over clusters
      for ( size_t i = 0; i < evt->clusters().size(); ++i ) {
         if ( evt->clusters()[i]->get_tracker() == trker_num ) {
           ++_num_clusters;
         }
      }

      // Loop over all spacepoints and pull out data to arrays
      for ( size_t i = 0; i < evt->spacepoints().size(); ++i ) {
        MAUS::SciFiSpacePoint *sp = evt->spacepoints()[i];
        double x = sp->get_position().x();
        double y = sp->get_position().y();
        double z = sp->get_position().z();
        if ( sp->get_tracker() == trker_num ) {
          _spoints_x.push_back(x);
          _spoints_y.push_back(y);
          _spoints_z.push_back(z);
          MAUS::SciFiCluster *clus = sp->get_channels()[0];
          MAUS::ThreeVector mom = clus->get_true_momentum();
          if ( mom.z() < 10.0 ) ++_num_noise_spoints;
          ++_num_spoints;
        }
      }

      // Loop over straight tracks
      for ( size_t i = 0; i < evt->straightprtracks().size(); ++i ) {
        if ( evt->straightprtracks()[i]->get_tracker() == trker_num ) {
          ++_num_stracks;
        }
      }

      // Loop over helical tracks
      for ( size_t i = 0; i < evt->helicalprtracks().size(); ++i ) {
        MAUS::SciFiHelicalPRTrack *trk = evt->helicalprtracks()[i];
        if ( trk->get_tracker() == trker_num ) {
          if (trk->get_num_points() == 5) ++_num_htracks_5pt;
          if (trk->get_num_points() == 4) ++_num_htracks_4pt;
          double rad = trk->get_R();
          double pt = rad * _RtoPt;
          double pz = pt / trk->get_dsdz();

          // Print track info to screen
          std::cout << "Tracker " << trker_num << + ", ";
          std::cout << "Track " << i << ", ";
          std::cout << "Num points " << trk->get_num_points() << ", ";
          std::cout << "X0 = " << std::setprecision(4) << trk->get_circle_x0() << "mm, ";
          std::cout << "Y0 = " <<  std::setprecision(4) << trk->get_circle_y0() << "mm, ";
          std::cout << "dsdz " <<  std::setprecision(4) << trk->get_dsdz() << ", ";
          std::cout << "pt = " <<  std::setprecision(4) << pt << "MeV/c, ";
          std::cout << "pz = " <<  std::setprecision(4) << pz << "MeV/c, ";
          std::cout << "xy_chi2 = " << std::setprecision(4) << trk->get_circle_chisq() << ", ";
          std::cout << "sz_c = " << std::setprecision(4) << trk->get_line_sz_c() << ", ";
          std::cout << "sz_chi2 = " << std::setprecision(4) << trk->get_line_sz_chisq() << "\n";
          std::cout << "x\ty\tz\ttime\t\tphi\tpx_mc\tpy_mc\tpt_mc\tpz_mc\n";

          // Pull out turning angles for each track seed
          std::vector<double> phi_i;
          std::vector<double> s_i;
          for ( size_t j = 0; j < trk->get_phi().size(); ++j ) {
            phi_i.push_back(trk->get_phi()[j]);
            s_i.push_back(trk->get_phi()[j]*rad);
          }
          _seeds_phi.push_back(phi_i);
          _seeds_s.push_back(s_i);

          // Loop over track seed spacepoints
          for ( size_t j = 0; j < trk->get_spacepoints().size(); ++j ) {
            MAUS::ThreeVector pos = trk->get_spacepoints()[j]->get_position();
            MAUS::SciFiCluster *clus = trk->get_spacepoints()[j]->get_channels()[0];
            MAUS::ThreeVector mom = clus->get_true_momentum();
            double t = trk->get_spacepoints()[j]->get_time();
            double pt_mc = sqrt(mom.x()*mom.x()+mom.y()*mom.y());
            std::cout <<  std::setprecision(4) << pos.x() << "\t";
            std::cout <<  std::setprecision(4) << pos.y() << "\t";
            std::cout <<  std::setprecision(4) << pos.z() << "\t";
            std::cout <<  std::setprecision(12) << t << "\t";
            std::cout <<  std::setprecision(4) << phi_i[j] << "\t";
            std::cout <<  std::setprecision(4) << mom.x() << "\t";
            std::cout <<  std::setprecision(4) << mom.y() << "\t";
            std::cout <<  std::setprecision(4) << pt_mc << "\t";
            std::cout <<  std::setprecision(4) << mom.z() << "\n";
            ++_num_seeds;
            if ( mom.z() < 10.0 ) ++_num_noise_seeds;
          }
        }
      }
    };

    void clear() {
      _trker_num = 0;
      _num_events = 0;
      _num_digits = 0;
      _num_clusters = 0;
      _num_spoints = 0;
      _num_noise_spoints = 0;
      _num_seeds = 0;
      _num_noise_seeds = 0;
      _num_stracks = 0;
      _num_htracks_5pt = 0;
      _num_htracks_4pt = 0;
      _spoints_x.clear();
      _spoints_x.resize(0);
      _spoints_y.clear();
      _spoints_y.resize(0);
      _spoints_z.clear();
      _spoints_z.resize(0);
      _seeds_z.clear();
      _seeds_z.resize(0);
      _seeds_phi.clear();
      _seeds_phi.resize(0);
      _seeds_s.clear();
      _seeds_s.resize(0);
      _seeds_circles.clear();
      _seeds_circles.resize(0);
      _seeds_xz.clear();
      _seeds_xz.resize(0);
      _seeds_yz.clear();
      _seeds_yz.resize(0);
      _seeds_sz.clear();
      _seeds_sz.resize(0);
    };

    static const double _RtoPt = 1.2;
    int _trker_num;
    int _num_events;
    int _num_digits;
    int _num_clusters;
    int _num_spoints;
    int _num_noise_spoints;
    int _num_seeds;
    int _num_noise_seeds;
    int _num_stracks;
    int _num_htracks_5pt;
    int _num_htracks_4pt;
    std::vector<double> _spoints_x;
    std::vector<double> _spoints_y;
    std::vector<double> _spoints_z;
    std::vector< std::vector<double> > _seeds_z;
    std::vector< std::vector<double> > _seeds_phi;
    std::vector< std::vector<double> > _seeds_s;
    std::vector< std::vector<TArc> > _seeds_circles;
    std::vector< std::vector<TF1> > _seeds_xz;
    std::vector< std::vector<TF1> > _seeds_yz;
    std::vector< std::vector<TF1> > _seeds_sz;
};


int main(int argc, char *argv[]) {

  // First argument to code should be the input ROOT file name
  std::string filename = std::string(argv[1]);

  // Check if the user wants to pause between evts, indicated by making second argument to code "1"
  std::string str_pause;
  bool bool_pause = false;
  if (argc > 2) {
    std::string s1 = std::string(argv[2]);
    if (s1 == "1") bool_pause = true;
  }

  std::cout << "Opening file " << filename << std::endl;
  MAUS::Data data;
  ofstream of1("out.txt");

  int t1_5pt_trks = 0;
  int t2_5pt_trks = 0;
  int t1_4pt_trks = 0;
  int t2_4pt_trks = 0;
  int tot_t1_sp = 0;
  int tot_t2_sp = 0;
  int tot_t1_noise_sp = 0;
  int tot_t2_noise_sp = 0;
  int tot_t1_seeds = 0;
  int tot_t2_seeds = 0;
  int tot_t1_noise_seeds = 0;
  int tot_t2_noise_seeds = 0;

  irstream infile(filename.c_str(), "Spill");

  // Loop over all events
  while (infile >> readEvent != NULL) {
    infile >> branchName("data") >> data;
    MAUS::Spill* spill = data.GetSpill();
    TrackerData t1;
    TrackerData t2;
    if (spill != NULL && spill->GetDaqEventType() == "physics_event") {
      std::cout << "Opening spill " << spill->GetSpillNumber() << std::endl;
      // Loop over recon events
      for (size_t i = 0;  i < spill->GetReconEvents()->size(); ++i) {
        MAUS::SciFiEvent* evt = (*spill->GetReconEvents())[i]->GetSciFiEvent();
        t1.accumulate_data(evt, 0);
        std::cout << "\n";
        t2.accumulate_data(evt, 1);
      }
      std::cout << "\nFor T1 found spoints: " << t1._num_spoints << ", 5pt trks: ";
      std::cout << t1._num_htracks_5pt << ", 4pt trks: " <<  t1._num_htracks_4pt << "\n";
      std::cout << "For T2 found spoints: " << t2._num_spoints << ", 5pt trks: ";
      std::cout << t2._num_htracks_5pt << ", 4pt trks: " <<  t2._num_htracks_4pt << "\n\n";

      tot_t1_sp += t1._num_spoints;
      tot_t2_sp += t2._num_spoints;
      tot_t1_noise_sp += t1._num_noise_spoints;
      tot_t2_noise_sp += t2._num_noise_spoints;

      tot_t1_seeds += t1._num_seeds;
      tot_t2_seeds += t2._num_seeds;
      tot_t1_noise_seeds += t1._num_noise_seeds;
      tot_t2_noise_seeds += t2._num_noise_seeds;

      t1_5pt_trks += t1._num_htracks_5pt;
      t2_5pt_trks += t2._num_htracks_5pt;
      t1_4pt_trks += t1._num_htracks_4pt;
      t2_4pt_trks += t2._num_htracks_4pt;

      t1.clear();
      t2.clear();
      if (bool_pause) {
        std::cout << "Press Enter to Continue";
        std::cin.ignore();
      }
    }
  }
  infile.close();
  of1 << "T1 spacepoints: " <<  tot_t1_sp << std::endl;
  of1 << "T1 noise spacepoints: " <<  tot_t1_noise_sp << std::endl;
  of1 << "T2 spacepoints: " <<  tot_t2_sp << std::endl;
  of1 << "T2 noise spacepoints: " <<  tot_t2_noise_sp << std::endl;
  of1 << "T1 seeds: " <<  tot_t1_seeds << std::endl;
  of1 << "T1 noise seeds: " <<  tot_t1_noise_seeds << std::endl;
  of1 << "T2 seeds: " <<  tot_t2_seeds << std::endl;
  of1 << "T2 noise seeds: " <<  tot_t2_noise_seeds << std::endl;
  of1 << "T1 5 point tracks: " <<  t1_5pt_trks << std::endl;
  of1 << "T1 4 point tracks: " <<  t1_4pt_trks << std::endl;
  of1 << "T2 5 point tracks: " <<  t2_5pt_trks << std::endl;
  of1 << "T2 4 point tracks: " <<  t2_4pt_trks << std::endl;
}
