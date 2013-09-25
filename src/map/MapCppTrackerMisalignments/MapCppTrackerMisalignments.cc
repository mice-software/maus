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

#include "src/map/MapCppTrackerMisalignments/MapCppTrackerMisalignments.hh"

namespace MAUS {

// Ascending station number.
bool SortByStation(const SciFiSpacePoint *a, const SciFiSpacePoint *b) {
  return ( a->get_station() < b->get_station() );
}

MapCppTrackerMisalignments::MapCppTrackerMisalignments()
                           :_classname("MapCppTrackerMisalignments"),
                            _spill_json(NULL),
                            _spill_cpp(NULL),
                            _root_file(NULL),
                            _tracker0_graphs(NULL),
                            _t0s2_x(NULL),
                            _t0s3_x(NULL),
                            _t0s4_x(NULL),
                            _t0s2_y(NULL),
                            _t0s3_y(NULL),
                            _t0s4_y(NULL),
                            _tracker1_graphs(NULL),
                            _t1s2_x(NULL),
                            _t1s3_x(NULL),
                            _t1s4_x(NULL),
                            _t1s2_y(NULL),
                            _t1s3_y(NULL),
                            _t1s4_y(NULL),
                            t1st2residual(NULL),
                            t1st3residual(NULL),
                            t1st4residual(NULL), //{//,
                            _jointPDF() {
  _root_file = new TFile("misalignments.root", "RECREATE");

  // Tracker 0 Graphs -------------
  _tracker0_graphs = new TMultiGraph("tracker0", "tracker0");
  _t0s2_x = new TGraph();
  _t0s2_x->SetName("t0s2_x");
  _t0s2_x->SetTitle("tracker 0, station 2, #Delta x");
  _t0s2_x->SetLineColor(kBlue);
  _t0s3_x = new TGraph();
  _t0s3_x->SetName("t0s3_x");
  _t0s3_x->SetTitle("tracker 0, station 3, #Delta x");
  _t0s3_x->SetLineColor(kRed);
  _t0s4_x = new TGraph();
  _t0s4_x->SetName("t0s4_x");
  _t0s4_x->SetTitle("tracker 0, station 4, #Delta x");
  _t0s4_x->SetLineColor(kGreen);

  _t0s2_y = new TGraph();
  _t0s2_y->SetName("t0s2_y");
  _t0s2_y->SetTitle("tracker 0, station 2, #Delta y");
  _t0s2_y->SetLineColor(kBlue);
  _t0s2_y->SetLineStyle(2);
  _t0s3_y = new TGraph();
  _t0s3_y->SetName("t0s3_y");
  _t0s3_y->SetTitle("tracker 0, station 3, #Delta y");
  _t0s3_y->SetLineColor(kRed);
  _t0s3_y->SetLineStyle(2);
  _t0s4_y = new TGraph();
  _t0s4_y->SetName("t0s4_y");
  _t0s4_y->SetTitle("tracker 0, station 4, #Delta y");
  _t0s4_y->SetLineColor(kGreen);
  _t0s4_y->SetLineStyle(2);
  // Tracker 1 Graphs -------------
  _tracker1_graphs = new TMultiGraph("tracker1", "tracker1");
  _t1s2_x = new TGraph();
  _t1s2_x->SetName("t1s2_x");
  _t1s2_x->SetTitle("tracker 1, station 2, #Delta x");
  _t1s2_x->SetLineColor(kBlue);
  _t1s3_x = new TGraph();
  _t1s3_x->SetName("t1s3_x");
  _t1s3_x->SetTitle("tracker 1, station 3, #Delta x");
  _t1s3_x->SetLineColor(kRed);
  _t1s4_x = new TGraph();
  _t1s4_x->SetName("t1s4_x");
  _t1s4_x->SetTitle("tracker 1, station 4, #Delta x");
  _t1s4_x->SetLineColor(kGreen);

  _t1s2_y = new TGraph();
  _t1s2_y->SetName("t1s2_y");
  _t1s2_y->SetTitle("tracker 1, station 2, #Delta y");
  _t1s2_y->SetLineColor(kBlue);
  _t1s2_y->SetLineStyle(2);
  _t1s3_y = new TGraph();
  _t1s3_y->SetName("t1s3_y");
  _t1s3_y->SetTitle("tracker 1, station 3, #Delta y");
  _t1s3_y->SetLineColor(kRed);
  _t1s3_y->SetLineStyle(2);
  _t1s4_y = new TGraph();
  _t1s4_y->SetName("t1s4_y");
  _t1s4_y->SetTitle("tracker 1, station 4, #Delta y");
  _t1s4_y->SetLineColor(kGreen);
  _t1s4_y->SetLineStyle(2);

  std::vector<TGraph*> tracker0_graphs;
  std::vector<TGraph*> tracker1_graphs;
  _graph_map.push_back(tracker0_graphs);
  _graph_map.push_back(tracker1_graphs);

  _graph_map[0].push_back(_t0s2_x);
  _graph_map[0].push_back(_t0s3_x);
  _graph_map[0].push_back(_t0s4_x);
  _graph_map[1].push_back(_t1s2_x);
  _graph_map[1].push_back(_t1s3_x);
  _graph_map[1].push_back(_t1s4_x);

  t1st2residual = new TH1D("t1st2residual", "t1st2residual", 70, -2, 2);
  t1st3residual = new TH1D("t1st3residual", "t1st3residual", 70, -2, 2);
  t1st4residual = new TH1D("t1st4residual", "t1st4residual", 70, -2, 2);
}

MapCppTrackerMisalignments::~MapCppTrackerMisalignments() {
  if (_spill_json != NULL) {
      delete _spill_json;
  }
  if (_spill_cpp != NULL) {
      delete _spill_cpp;
  }

  // _x_shift_pdfs
  // _y_shift_pdfs
}

bool MapCppTrackerMisalignments::birth(std::string argJsonConfigDocument) {
  std::string lname("joint");
  std::string pname("prob_station3");
  double shift_min = -3.;
  double shift_max = 3.;
  double bin_width = 0.001;

  _jointPDF = JointPDF(lname, bin_width, shift_min, shift_max);
  double sigma = 0.43; // mm

  int number_of_tosses = 700000000;
  _jointPDF.Build("gaussian", sigma, number_of_tosses);

  std::vector<PDF> tracker0_x_PDFS;
  std::vector<PDF> tracker1_x_PDFS;
  _x_shift_pdfs.push_back(tracker0_x_PDFS);
  _x_shift_pdfs.push_back(tracker1_x_PDFS);
  std::vector<PDF> tracker0_y_PDFS;
  std::vector<PDF> tracker1_y_PDFS;
  _y_shift_pdfs.push_back(tracker0_y_PDFS);
  _y_shift_pdfs.push_back(tracker1_y_PDFS);
  for ( int tracker = 0; tracker < 2; tracker++ ) {
    for ( int station = 0; station < 6; station++ ) {
      std::string pname_x("x_station_");
      std::string pname_y("y_station_");
      std::ostringstream station_n;
      station_n << station;
      std::ostringstream tracker_n;
      tracker_n << tracker;
      pname_x = pname_x + tracker_n.str()+ station_n.str();
      pname_y = pname_y + tracker_n.str()+ station_n.str();
      //PDF *probability_x = new PDF(pname_x, bin_width, shift_min, shift_max);
      //PDF *probability_y = new PDF(pname_y, bin_width, shift_min, shift_max);
      PDF probability_x(pname_x, bin_width, shift_min, shift_max);
      PDF probability_y(pname_y, bin_width, shift_min, shift_max);
      _x_shift_pdfs[tracker].push_back(probability_x);
      _y_shift_pdfs[tracker].push_back(probability_y);
    }
  }
  try {
    if (!Globals::HasInstance()) {
      GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
    }
    // Json::Value *json = Globals::GetConfigurationCards();
    // _helical_pr_on  = (*json)["SciFiPRHelicalOn"].asBool();
    return true;
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }
  return false;
}

bool MapCppTrackerMisalignments::death() {
  _tracker0_graphs->Add(_t0s2_x);
  _tracker0_graphs->Add(_t0s3_x);
  _tracker0_graphs->Add(_t0s4_x);
  _tracker0_graphs->Add(_t0s2_y);
  _tracker0_graphs->Add(_t0s3_y);
  _tracker0_graphs->Add(_t0s4_y);

  _tracker1_graphs->Add(_t1s2_x);
  _tracker1_graphs->Add(_t1s3_x);
  _tracker1_graphs->Add(_t1s4_x);
  _tracker1_graphs->Add(_t1s2_y);
  _tracker1_graphs->Add(_t1s3_y);
  _tracker1_graphs->Add(_t1s4_y);

  _root_file->cd();
  _tracker0_graphs->Write("", TObject::kOverwrite);
  _tracker1_graphs->Write("", TObject::kOverwrite);
  // Tracker 0 final PDF's.
  TH1D *final_probability02 = reinterpret_cast<TH1D*>
                              (_x_shift_pdfs[0][2].probability()->Clone("x_t0st2"));
  TH1D *final_probability03 = reinterpret_cast<TH1D*>
                              (_x_shift_pdfs[0][3].probability()->Clone("x_t0st3"));
  TH1D *final_probability04 = reinterpret_cast<TH1D*>
                              (_x_shift_pdfs[0][4].probability()->Clone("x_t0st4"));

  TH1D *final_probability02y = reinterpret_cast<TH1D*>
                              (_y_shift_pdfs[0][2].probability()->Clone("y_t0st2"));
  TH1D *final_probability03y = reinterpret_cast<TH1D*>
                              (_y_shift_pdfs[0][3].probability()->Clone("y_t0st3"));
  TH1D *final_probability04y = reinterpret_cast<TH1D*>
                              (_y_shift_pdfs[0][4].probability()->Clone("y_t0st4"));

  // Tracker 1 final PDF's.
  TH1D *final_probability2 = reinterpret_cast<TH1D*>
                              (_x_shift_pdfs[1][2].probability()->Clone("x_t1st2"));
  TH1D *final_probability3 = reinterpret_cast<TH1D*>
                              (_x_shift_pdfs[1][3].probability()->Clone("x_t1st3"));
  TH1D *final_probability4 = reinterpret_cast<TH1D*>
                              (_x_shift_pdfs[1][4].probability()->Clone("x_t1st4"));

  TH1D *final_probability2y = reinterpret_cast<TH1D*>
                              (_y_shift_pdfs[1][2].probability()->Clone("y_t1st2"));
  TH1D *final_probability3y = reinterpret_cast<TH1D*>
                              (_y_shift_pdfs[1][3].probability()->Clone("y_t1st3"));
  TH1D *final_probability4y = reinterpret_cast<TH1D*>
                              (_y_shift_pdfs[1][4].probability()->Clone("y_t1st4"));

  TH1D *likelihood = reinterpret_cast<TH1D*>
                     ((_jointPDF.GetLikelihood(1.2)).Clone("likelihood"));

  TH2D *joint = reinterpret_cast<TH2D*>
                           (_jointPDF.GetJointPDF()->Clone("joint"));

  _root_file->Write();
  _root_file->Close();

  return true;
}

std::vector< std::vector<MAUS::SciFiSpacePoint*> >
           MapCppTrackerMisalignments::setup(SciFiEvent *event) {
  // Get all spacepoints in the event.
  SpacePointArray all_spacepoints = event->spacepoints();
  // We need to separate them by tracker...
  SpacePointArray tracker0_spacepoints;
  SpacePointArray tracker1_spacepoints;
  // ... and store them in a container.
  std::vector<SpacePointArray> container;
  container.push_back(tracker0_spacepoints);
  container.push_back(tracker1_spacepoints);

  // Iterate over all the spacepoints and fill the container.
  std::vector<SciFiSpacePoint*>::iterator spacepoint;
  for( spacepoint = all_spacepoints.begin();
       spacepoint != all_spacepoints.end(); spacepoint++ ) {
    int tracker = (*spacepoint)->get_tracker();
    container[tracker].push_back(*spacepoint);
  }
  // If tracker0 or tracker1 have less than 5 spacepoints, remove them.
  for ( int tracker = 1; tracker > -1; tracker-- ) {
    if ( container[tracker].size() != 5 ) {
      container.erase(container.begin()+tracker);
    }
  }
  // Sort the vectors left in the container.
  for ( size_t i = 0; i < container.size(); i++ ) {
    std::sort(container[i].begin(), container[i].end(), SortByStation);
  }
  return container;
}

std::string MapCppTrackerMisalignments::process(std::string document) {
  Json::FastWriter writer;
  // Read in json data
  read_in_json(document);
  Spill& spill = *_spill_cpp;

  try { // ================= Reconstruction =========================
    if ( spill.GetReconEvents() ) {
      for ( unsigned int k = 0; k < spill.GetReconEvents()->size(); k++ ) {
        SciFiEvent *event = spill.GetReconEvents()->at(k)->GetSciFiEvent();
        // Skip event if it has < 5 spacepoints.
        if ( event->spacepoints().size() < 5 ) continue;
        // Otherwise, setup the container.
        std::vector<SpacePointArray> container = setup(event);
        // Now, process the spacepoints.
        for ( size_t i = 0; i < container.size(); i++ ) {
          process(container[i]);
        }
      }
    } else {
      std::cerr << "No recon events found\n";
    }
    save_to_json(spill);
  } catch(Squeal& squee) {
    squee.Print();
    // _spill_json = MAUS::CppErrorHandler::getInstance()
    //                                   ->HandleSqueal(_spill_json, squee, _classname);
  } catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["recon_failed"] = ss.str();
    (*_spill_json)["errors"] = errors;
    return writer.write(_spill_json);
  }
  return writer.write(*_spill_json);
}

void MapCppTrackerMisalignments::read_in_json(std::string json_data) {
  Json::Reader reader;
  Json::Value json_root;
  Json::FastWriter writer;

  if (_spill_cpp != NULL) {
    delete _spill_cpp;
    _spill_cpp = NULL;
  }

  try {
    json_root = JsonWrapper::StringToJson(json_data);
    SpillProcessor spill_proc;
    _spill_cpp = spill_proc.JsonToCpp(json_root);
  } catch(...) {
    std::cerr << "Bad json document" << std::endl;
    _spill_cpp = new Spill();
    MAUS::ErrorsMap errors = _spill_cpp->GetErrors();
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    _spill_cpp->GetErrors();
  }
}

void MapCppTrackerMisalignments::save_to_json(Spill &spill) {
    SpillProcessor spill_proc;
    if (_spill_json != NULL) {
        delete _spill_json;
        _spill_json = NULL;
    }
    _spill_json = spill_proc.CppToJson(spill, "");
}

void MapCppTrackerMisalignments::process(SpacePointArray spacepoints) {
  int tracker = spacepoints.front()->get_tracker();

  double x0, mx, y0, my;
  linear_fit(spacepoints, x0, mx, y0, my);
  for ( int station = 2; station < 5; station++ ) {
    // Now that the fit is done, we can calculate the residual.
    ThreeVector position = spacepoints.at(station-1)->get_position();
    double projected_x = x0 + mx * position.z();
    double projected_y = y0 + my * position.z();
    double x_residual = projected_x - (position.x() + _x_shift_pdfs[tracker][station].GetMean());
    double y_residual = projected_y - (position.y() + _y_shift_pdfs[tracker][station].GetMean());
    if ( x_residual != x_residual || y_residual != y_residual ) return;
    if ( fabs(x_residual) > 2. || fabs(y_residual) > 2. ) return;

    double old_x = _x_shift_pdfs[tracker][station].GetMean();
    double old_y = _y_shift_pdfs[tracker][station].GetMean();
    double new_x_shift = old_x+x_residual;
    double new_y_shift = old_y+y_residual;
    _x_shift_pdfs[tracker][station].
                  ComputeNewPosterior(_jointPDF.GetLikelihood(new_x_shift));
    _y_shift_pdfs[tracker][station].
                  ComputeNewPosterior(_jointPDF.GetLikelihood(new_y_shift));

    // _residuals[tracker][station]->Fill(x_residual);
    if ( tracker == 1 && station == 3 ) {
      _iteraction++;
      t1st3residual->Fill(x_residual);
      std::string name("st3_iter_");
      std::ostringstream number;
      number << _iteraction;
      name = name + number.str();
      const char *c_name = name.c_str();
      TH1D *probability3 = reinterpret_cast<TH1D*>
                             (_x_shift_pdfs[tracker][station].probability()->Clone(c_name));
      _root_file->cd();
      probability3->Write();
    } else if ( station == 2 ) {
      t1st2residual->Fill(x_residual);
    } else if ( station == 4 ) {
      t1st4residual->Fill(x_residual);
    }
    std::cerr << "Tracker " << tracker << ", station " << station << ": "
              << _x_shift_pdfs[tracker][station].GetMean() << std::endl;
    // Update TGraph.
    int n_points = _graph_map[tracker][station-2]->GetN();
    _graph_map[tracker][station-2]->SetPoint(n_points, n_points, _x_shift_pdfs[tracker][station].GetMean());
  }
}

void MapCppTrackerMisalignments::linear_fit(SpacePointArray spacepoints,
                                            double &x0,
                                            double &mx,
                                            double &y0,
                                            double &my) {
  int tracker = spacepoints.at(0)->get_tracker();
  const Int_t nrVar  = 2;
  const Int_t nrPnts = 5;

  Double_t ax[] = {spacepoints.at(0)->get_position().x(),
                   spacepoints.at(1)->get_position().x() + _x_shift_pdfs[tracker][2].GetMean(),
                   spacepoints.at(2)->get_position().x() + _x_shift_pdfs[tracker][3].GetMean(),
                   spacepoints.at(3)->get_position().x() + _x_shift_pdfs[tracker][4].GetMean(),
                   spacepoints.at(4)->get_position().x()};

  Double_t ay[] = {spacepoints.at(0)->get_position().y(),
                   spacepoints.at(1)->get_position().y() + _y_shift_pdfs[tracker][2].GetMean(),
                   spacepoints.at(2)->get_position().y() + _y_shift_pdfs[tracker][3].GetMean(),
                   spacepoints.at(3)->get_position().y() + _y_shift_pdfs[tracker][4].GetMean(),
                   spacepoints.at(4)->get_position().y()};

  Double_t az[] = {spacepoints.at(0)->get_position().z(),
                   spacepoints.at(1)->get_position().z(),
                   spacepoints.at(2)->get_position().z(),
                   spacepoints.at(3)->get_position().z(),
                   spacepoints.at(4)->get_position().z()};

  Double_t ae[] = {.01, 1., 1., 1., .01};

  TVectorD x;
  x.Use(nrPnts, ax);
  TVectorD y;
  y.Use(nrPnts, ay);
  TVectorD z;
  z.Use(nrPnts, az);
  TVectorD e;
  e.Use(nrPnts, ae);

  // Solve x = a+bz
  TMatrixD Axz(nrPnts, nrVar);
  TMatrixDColumn(Axz, 0) = 1.0;
  TMatrixDColumn(Axz, 1) = z;
  const TVectorD param_xz = NormalEqn(Axz, x, e);
  // Solve y = a+bz
  TMatrixD Ayz(nrPnts, nrVar);
  TMatrixDColumn(Ayz, 0) = 1.0;
  TMatrixDColumn(Ayz, 1) = z;
  const TVectorD param_yz = NormalEqn(Ayz, y, e);
  // y = a + b z

  x0 = param_xz[0];
  mx = param_xz[1];

  y0 = param_yz[0];
  my = param_yz[1];
}

} // ~namespace MAUS
