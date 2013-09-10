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

MapCppTrackerMisalignments::MapCppTrackerMisalignments() : _spill_json(NULL),
                                                           _spill_cpp(NULL),
                                                           _root_file(NULL),
                                                           _iteraction(0),
                                                           _tracker0_graphs(NULL),
                                                           _t0s2(NULL),
                                                           _t0s3(NULL),
                                                           _t0s4(NULL),
                                                           _tracker1_graphs(NULL),
                                                           _t1s2(NULL),
                                                           _t1s3(NULL),
                                                           _t1s4(NULL),
                                                           t1st2residual(NULL),
                                                           t1st3residual(NULL),
                                                           t1st4residual(NULL),
                                                           _jointPDF(NULL) {
  _root_file = new TFile("misalignments.root", "RECREATE");

  // Tracker 0 Graphs -------------
  _tracker0_graphs = new TMultiGraph("tracker0", "tracker0");
  _t0s2 = new TGraph();
  _t0s2->SetName("t0s2");
  _t0s2->SetLineColor(kBlue);

  _t0s3 = new TGraph();
  _t0s3->SetName("t0s3");
  _t0s3->SetLineColor(kRed);

  _t0s4 = new TGraph();
  _t0s4->SetName("t0s4");
  _t0s4->SetLineColor(kGreen);

  // Tracker 1 Graphs -------------
  _tracker1_graphs = new TMultiGraph("tracker1", "tracker1");

  _t1s2 = new TGraph();
  _t1s2->SetName("t1s2");
  _t1s2->SetLineColor(kBlue);

  _t1s3 = new TGraph();
  _t1s3->SetName("t1s3");
  _t1s3->SetLineColor(kRed);

  _t1s4 = new TGraph();
  _t1s4->SetName("t1s4");
  _t1s4->SetLineColor(kGreen);

  t1st2residual = new TH1D("t1st2residual", "t1st2residual", 100, -10, 10);
  t1st3residual = new TH1D("t1st3residual", "t1st3residual", 100, -10, 10);
  t1st4residual = new TH1D("t1st4residual", "t1st4residual", 100, -10, 10);
}

MapCppTrackerMisalignments::~MapCppTrackerMisalignments() {
  if (_spill_json != NULL) {
      delete _spill_json;
  }
  if (_spill_cpp != NULL) {
      delete _spill_cpp;
  }
}

bool MapCppTrackerMisalignments::birth(std::string argJsonConfigDocument) {
  _classname = "MapCppTrackerMisalignments";

  for ( int i = 0; i < 2; i++ ) {
    for ( int j = 0; j < 5; j++ ) {
      _x_shifts[i][j] = 0.;
      _y_shifts[i][j] = 0.;
    }
  }

/*
  _tree = new TTree("misaligments","misaligments");
  _tree->Branch("tracker", &_tracker, "tracker/I");
  _tree->Branch("station", &_station, "station/I");
  _tree->Branch("x_res", &_x_residual, "x_res/D");
  _tree->Branch("y_res", &_y_residual, "y_res/D");
  _tree->Branch("iter", &_iteraction, "iter/I");
*/
  std::string lname("joint");
  std::string pname("prob_station3");
  double shift_min = -10.;
  double shift_max = 10.;
  double bin_width = 0.1;

  _jointPDF = new JointPDF(lname, bin_width, shift_min, shift_max);
  double sigma = 1.5; // mm
  int number_of_tosses = 2000000;
  _jointPDF->Build("gaussian", sigma, number_of_tosses);

  for ( int station_index = 0; station_index < 5; station_index++ ) {
    std::string pname("prob_station");
    std::ostringstream number;
    number << station_index+1;
    pname = pname + number.str();
    PDF *_probability = new PDF(pname, bin_width, shift_min, shift_max);
    _x_shift_pdfs.push_back(_probability);
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
  _tracker0_graphs->Add(_t0s2);
  _tracker0_graphs->Add(_t0s3);
  _tracker0_graphs->Add(_t0s4);

  _tracker1_graphs->Add(_t1s2);
  _tracker1_graphs->Add(_t1s3);
  _tracker1_graphs->Add(_t1s4);

  _root_file->cd();
  _tracker0_graphs->Write("", TObject::kOverwrite);
  _tracker1_graphs->Write("", TObject::kOverwrite);

  TH1D *final_probability2 = reinterpret_cast<TH1D*>
                              (_x_shift_pdfs.at(1)->GetHistogram()->Clone("pdf_st2"));
  TH1D *final_probability3 = reinterpret_cast<TH1D*>
                              (_x_shift_pdfs.at(2)->GetHistogram()->Clone("pdf_st3"));
  TH1D *final_probability4 = reinterpret_cast<TH1D*>
                              (_x_shift_pdfs.at(3)->GetHistogram()->Clone("pdf_st4"));

  TH1D *likelihood = reinterpret_cast<TH1D*>
                     ((_jointPDF->GetLikelihood(1.2)).Clone("likelihood"));

  TH2D *joint = reinterpret_cast<TH2D*>
                           (_jointPDF->GetJointPDF()->Clone("joint"));

  _root_file->Write();
  _root_file->Close();

  return true;
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
        // Simple linear fit over spacepoints x,y.
        if ( event->spacepoints().size() == 5 ) {
          process(event);
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

void MapCppTrackerMisalignments::process(SciFiEvent *evt) {
  // Check the quality of the track.
  size_t number_scifitracks = evt->scifitracks().size();
  double P_value = 0;
  if ( number_scifitracks ) {
    P_value = evt->scifitracks()[0]->P_value();
  }
  if ( P_value < 0.1 ) return;

  SpacePointArray spacepoints = evt->spacepoints();
  std::sort(spacepoints.begin(), spacepoints.end(), SortByStation);

  _iteraction++;
  _tracker = spacepoints.front()->get_tracker();

  double x0, mx, y0, my;
  linear_fit(spacepoints, x0, mx, y0, my);

  for ( int station_i = 2; station_i < 5; station_i++ ) {
    int station_array_index = station_i-1;
    // Now that the fit is done, we can calculate the residual.
    ThreeVector position = spacepoints.at(station_array_index)->get_position();
    double projected_x = x0 + mx * position.z();
    double projected_y = y0 + my * position.z();

    double x_residual = projected_x - position.x();
    double y_residual = projected_y - position.y();

    std::cerr << "Residual found: " << x_residual << ", " << y_residual << std::endl;

    // _history[_tracker][station_i-1].push_back(residuals.x());
    // double updated_value = mean_value_update(_history[_tracker][station_i-1]);
    // _x_shifts[_tracker][station_i-1] = updated_value;
    double old_mean = _x_shift_pdfs.at(station_array_index)->GetMean();
    // double suggested_new_shift = old_mean + residuals.x();
    double suggested_new_shift = old_mean+x_residual;
    _x_shift_pdfs.at(station_array_index)->
                  ComputeNewPosterior(_jointPDF->GetLikelihood(suggested_new_shift));
    if ( _tracker == 1 && station_i == 3 ) {
      t1st3residual->Fill(x_residual);
      std::string name("st3_iter_");
      std::ostringstream number;
      number << _iteraction;
      name = name + number.str();
      const char *c_name = name.c_str();
      TH1D *probability3 = reinterpret_cast<TH1D*>
                             (_x_shift_pdfs.at(2)->GetHistogram()->Clone(c_name));
      _root_file->cd();
      probability3->Write();
    } else if ( station_i == 2 ) {
      t1st2residual->Fill(x_residual);
    } else if ( station_i == 4 ) {
      t1st4residual->Fill(x_residual);
    }
    std::cerr << "Station " << station_i << ", old mean: "
              << old_mean << "; new residual: " << x_residual << "\n"
              << "suggested_new_shift: " << suggested_new_shift << " , new mean: "
              << _x_shift_pdfs.at(station_array_index)->GetMean() << std::endl;
  }

  int n_points = _t1s3->GetN();
  std::cerr << "n_points: " << n_points << std::endl;
  // _t1s2->SetPoint(n_points, n_points, _x_shifts[_tracker][1]);
  // _t1s3->SetPoint(n_points, n_points, _x_shifts[_tracker][2]);
  // _t1s4->SetPoint(n_points, n_points, _x_shifts[_tracker][3]);
  _t1s2->SetPoint(n_points, n_points, _x_shift_pdfs.at(1)->GetMean());
  _t1s3->SetPoint(n_points, n_points, _x_shift_pdfs.at(2)->GetMean());
  _t1s4->SetPoint(n_points, n_points, _x_shift_pdfs.at(3)->GetMean());
}

void MapCppTrackerMisalignments::linear_fit(SpacePointArray spacepoints,
                                            double &x0,
                                            double &mx,
                                            double &y0,
                                            double &my) {
  const Int_t nrVar  = 2;
  const Int_t nrPnts = 5;

  Double_t ax[] = {spacepoints.at(0)->get_position().x(),
                   spacepoints.at(1)->get_position().x(),// - _x_shift_pdfs.at(1)->GetMean(),
                   spacepoints.at(2)->get_position().x(),// - _x_shift_pdfs.at(2)->GetMean(),
                   spacepoints.at(3)->get_position().x(),// - _x_shift_pdfs.at(3)->GetMean(),
                   spacepoints.at(4)->get_position().x()};

  Double_t ay[] = {spacepoints.at(0)->get_position().y(),
                   spacepoints.at(1)->get_position().y(),
                   spacepoints.at(2)->get_position().y(),
                   spacepoints.at(3)->get_position().y(),
                   spacepoints.at(4)->get_position().y()};

  Double_t az[] = {spacepoints.at(0)->get_position().z(),
                   spacepoints.at(1)->get_position().z(),
                   spacepoints.at(2)->get_position().z(),
                   spacepoints.at(3)->get_position().z(),
                   spacepoints.at(4)->get_position().z()};

  Double_t ae[] = {1., 1., 1., 1., 1.};

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
