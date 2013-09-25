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

/** @class MapCppTrackerMisalignments
 *  Digitize events by running Tracker electronics simulation.
 *
 */

#ifndef _SRC_MAP_MAPCPPKALMANMISALIGNMENTSEARCH_H_
#define _SRC_MAP_MAPCPPKALMANMISALIGNMENTSEARCH_H_

// C headers
#include <assert.h>
#include <json/json.h>
#include <CLHEP/Units/PhysicalConstants.h>

// C++ headers
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include "TMath.h"
#include "TMultiGraph.h"
#include "TGraph.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TDecompChol.h"
#include "TTree.h"
#include "TFile.h"


// Other headers
#include "Interface/Squeal.hh"
#include "Interface/Squeak.hh"
#include "Config/MiceModule.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"

#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"

#include "src/common_cpp/Recon/Bayes/PDF.hh"
#include "src/common_cpp/Recon/Bayes/JointPDF.hh"

namespace MAUS {

class MapCppTrackerMisalignments {

 public:
  /** Constructor - initialises pointers to NULL */
  MapCppTrackerMisalignments();

  /** Constructor - deletes any allocated memory */
  ~MapCppTrackerMisalignments();

  /** Sets up the worker
   *
   *  \param argJsonConfigDocument a JSON document with
   *         the configuration.
   */
  bool birth(std::string argJsonConfigDocument);

  /** Shutdowns the worker
   *
   *  This takes no arguments and does nothing
   */
  bool death();

  /** Process JSON document
   *
   *  Receive a document with digits (either MC or real) and then call the higher level
   *  reconstruction algorithms
   * 
   * \param document a line/spill from the JSON input
   */
  std::string process(std::string document);

  std::vector< std::vector<MAUS::SciFiSpacePoint*> > setup(MAUS::SciFiEvent *event);

  void process(std::vector<MAUS::SciFiSpacePoint*> spacepoints);

  void linear_fit(std::vector<MAUS::SciFiSpacePoint*> spacepoints,
                  double &x0,
                  double &mx,
                  double &y0,
                  double &my);

  /** Takes json data and returns a Sc
   *
   *  Track fit takes the spacepoints from Pattern Recognition and, going back to the clusters
   *  which formed the spacepoints, fits the tracks more acurately using a Kalman filter
   *
   *  \param evt the current SciFiEvent
   */
  void read_in_json(std::string json_data);

  void save_to_json(MAUS::Spill &spill);

 private:
  typedef std::vector<MAUS::SciFiSpacePoint*> SpacePointArray;

  /// This should be the classname
  std::string _classname;
  /// This will contain the configuration
  Json::Value _configJSON;
  /// This will contain the root value after parsing
  Json::Value* _spill_json;
  Spill* _spill_cpp;
  ///  JsonCpp setup
  Json::Reader reader;

  /// This will store
  TFile *_root_file;

  std::vector< std::vector<TGraph*> > _graph_map;
  TMultiGraph *_tracker0_graphs;
  TGraph *_t0s2_x;
  TGraph *_t0s3_x;
  TGraph *_t0s4_x;
  TGraph *_t0s2_y;
  TGraph *_t0s3_y;
  TGraph *_t0s4_y;
  TMultiGraph *_tracker1_graphs;
  TGraph *_t1s2_x;
  TGraph *_t1s3_x;
  TGraph *_t1s4_x;
  TGraph *_t1s2_y;
  TGraph *_t1s3_y;
  TGraph *_t1s4_y;

  TH1D *t1st2residual;
  TH1D *t1st3residual;
  TH1D *t1st4residual;

  int _iteraction;

  JointPDF _jointPDF;

  std::vector< std::vector<PDF> > _x_shift_pdfs;
  std::vector< std::vector<PDF> > _y_shift_pdfs;

  int SciFiRunRecon;
}; // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
