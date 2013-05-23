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
#include "src/common_cpp/Recon/Kalman/KalmanSeed.hh"
#include <iostream>
#include <fstream>

#include "TVirtualFitter.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

namespace MAUS {

// Ascending site number.
bool SortByID(const SciFiCluster *a, const SciFiCluster *b) {
  return ( a->get_id() < b->get_id() );
}

// Ascending station number.
bool SortByStation(const SciFiSpacePoint *a, const SciFiSpacePoint *b) {
  return ( a->get_station() < b->get_station() );
}

double KalmanSeed::tan_lambda_fit(const double *par) {
  double f = 0;
  for (Int_t i = 0; i < 4; i++) {
    Double_t ds = _s[i+1] - _s[i];
    Double_t dz = _z[i+1] - _z[i];
    Double_t dr = par[0]  - dz/ds;
    f += dr*dr;
  }
  return f;
}

double KalmanSeed::circle_fit(const double *par) {
  // minimisation function computing the sum of squares of residuals
  double f = 0;
  //Double_t *x = gr->GetX();
  //Double_t *y = gr->GetY();
  for (Int_t i = 0; i < 5; i++) {
    Double_t u = _x[i] - par[0];
    Double_t v = _y[i] - par[1];
    Double_t dr = par[2] - TMath::Sqrt(u*u+v*v);
    f += dr*dr;
  }
  return f;
}

double KalmanSeed::mx_fit(const double *par) {
  double f = 0;
  for (Int_t i = 0; i < 5; i++) {
    Double_t dx = _x[i] - _x[0];
    Double_t dz = _z[i] - _z[0];
    Double_t dmx = par[2] - dx/dz;
    f += dmx*dmx;
  }
  return f;
}

double KalmanSeed::my_fit(const double *par) {
  double f = 0;
  for (Int_t i = 0; i < 5; i++) {
    Double_t dy = _y[i] - _y[0];
    Double_t dz = _z[i] - _z[0];
    Double_t dmy = par[2] - dy/dz;
    f += dmy*dmy;
  }
  return f;
}

KalmanSeed::KalmanSeed(): _straight(false), _helical(false) {
}

KalmanSeed::~KalmanSeed() {}


void KalmanSeed::Build(const SciFiEvent &evt, bool field_on) {
  if ( field_on ) {
    _helical  = true;
    _n_parameters = 5;
  } else {
    _straight = true;
    _n_parameters = 4;
  }
  _a0.ResizeTo(_n_parameters, 1);
  _clusters    = evt.clusters();
  _spacepoints = evt.spacepoints();

  std::sort(_clusters.begin(), _clusters.end(), SortByID);
  std::sort(_spacepoints.begin(), _spacepoints.end(), SortByStation);

  for ( size_t i = 0; i < 5; ++i ) {
    _x[i]   = _spacepoints[i]->get_position().x();
    _y[i]   = _spacepoints[i]->get_position().y();
    _z[i]   = _spacepoints[i]->get_position().z();
    // _s[i]   = TMath::Sqrt(_x[i]*_x[i] + _y[i]*_y[i] + _z[i]*_z[i]);
    // _phi[i] = _spacepoints[0]->get_position().Phi();
  }

  if ( field_on ) {
    double radius, x0, y0;
    get_circle_param(radius, x0, y0);
    std::cerr << "Circle parameters: " << std::endl;
    std::cerr << radius << " " << x0 << " " << y0 << std::endl;
/*
    _s[0] = 0;
    for ( size_t i = 0; i < 5; ++i ) {
      _phi[i] = atan2(_y[i]-y0, _x[i] - x0);
    }
    for ( size_t i = 0; i < 4; ++i ) {
      double delta_phi = 
    }
    double tanlambda;
    get_tan_lambda(tanlambda);
*/
    // _a0 = ComputeInitialStateVector(pr_track);
  } else {
    // double mx, my;
    // get_gradients(mx, my);

    // _a0 = ComputeInitialStateVector(pr_track);
  }
}

void KalmanSeed::get_gradients(double &mx, double &my) {
/*
  TVirtualFitter::SetDefaultFitter("Minuit");
  TVirtualFitter *fitter = TVirtualFitter::Fitter(0, 3);
  fitter->SetFCN(mx_fit);
  fitter->SetParameter(0, "x0",   0, 0.1, 0,0);
  fitter->SetParameter(1, "mx",   0, 0.1, 0,0);
  Double_t arglist[1] = {0};
  fitter->ExecuteCommand("MIGRAD", arglist, 0);
  mx = fitter->GetParameter(1);

  TVirtualFitter::SetDefaultFitter("Minuit");
  TVirtualFitter *fitter2 = TVirtualFitter::Fitter(0, 3);
  fitter2->SetFCN(my_fit);
  fitter2->SetParameter(0, "y0",   0, 0.1, 0,0);
  fitter2->SetParameter(1, "my",   0, 0.1, 0,0);
  Double_t arglist2[1] = {0};
  fitter2->ExecuteCommand("MIGRAD", arglist2, 0);
  my = fitter->GetParameter(1);
*/
}

void KalmanSeed::get_circle_param(double &radius, double &x0, double &y0) {
  ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer("Minuit", "Migrad");
  min->SetMaxFunctionCalls(100000000);
  min->SetTolerance(0.001);
  min->SetPrintLevel(0);
  ROOT::Math::Functor f(this, &KalmanSeed::circle_fit, 3);

  double step[3]     = {0.1, 0.1, 0.02};
  // starting point
  double variable[3] = { -150., -150., 0.1};

  min->SetFunction(f);
  // Set the free variables to be minimized!
  min->SetLimitedVariable(0, "x0", variable[0], step[0], -150., 150.);
  min->SetLimitedVariable(1, "y0", variable[1], step[1], -150., 150.);
  min->SetLimitedVariable(2, "r",  variable[2], step[2], 0.1, 75.);
  // do the minimization
  min->Minimize();

  const double *par = min->X();
  std::cout << "Minimum: f(" << par[0] << "," << par[1] << " " << par[2] << "): " 
            << min->MinValue()  << std::endl;

  // expected minimum is 0
  std::cerr << min->MinValue() << " " << circle_fit(par) << std::endl;
  if ( min->MinValue()  < 1.E-1  && circle_fit(par) < 1.E-1) {
     std::cerr << "Seed fit converged." << std::endl;
  } else {
     std::cerr << "Seed fit failed to converge." << std::endl;
  }
  x0     = par[0];
  y0     = par[1];
  radius = par[2];
  /*
  TVirtualFitter::SetDefaultFitter("Minuit");
  TVirtualFitter *fitter = TVirtualFitter::Fitter(0, 3);
  fitter->SetFCN(circle_fit);
  fitter->SetParameter(0, "x0",   0, 0.1, 0,0);
  fitter->SetParameter(1, "y0",   0, 0.1, 0,0);
  fitter->SetParameter(2, "R",    1, 0.1, 0,0);

  Double_t arglist[1] = {0};
  fitter->ExecuteCommand("MIGRAD", arglist, 0);

  radius = fitter->GetParameter(0);
  x0     = fitter->GetParameter(1);
  y0     = fitter->GetParameter(2);
  */
}

void KalmanSeed::get_tan_lambda(double &tanlambda) {
  ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer("Minuit", "Migrad");
  min->SetMaxFunctionCalls(100000000);
  min->SetTolerance(0.001);
  min->SetPrintLevel(0);
  ROOT::Math::Functor f(this, &KalmanSeed::tan_lambda_fit, 1);

  double step[1]     = {0.001};
  double variable[1] = { 0.};

  min->SetFunction(f);
  min->SetLimitedVariable(0, "tanlambda", variable[0], step[0], 0., 2.);
  min->Minimize();

  const double *par = min->X();
  std::cout << "Minimum: f(" << par[0] << "," << par[1] << " " << par[2] << "): " 
            << min->MinValue()  << std::endl;

  // expected minimum is 0
  std::cerr << min->MinValue() << " " << circle_fit(par) << std::endl;
  if ( min->MinValue()  < 1.E-1  && circle_fit(par) < 1.E-1) {
     std::cerr << "Seed fit converged." << std::endl;
  } else {
     std::cerr << "Seed fit failed to converge." << std::endl;
  }

  tanlambda = par[0];
}

TMatrixD KalmanSeed::ComputeInitialStateVector(const SciFiHelicalPRTrack* seed) {
  // Get seed values.
  double r  = seed->get_R();
  double B  = -4.;
  double pt = -0.3*B*r;

  double dsdz  = seed->get_dsdz();
  double tan_lambda = 1./dsdz;
  // PR doesnt see Eloss, so overstimates pz.
  // Total Eloss = 2 MeV/c.
  double pz = pt*tan_lambda;

  double kappa = fabs(1./pz);

  double PI = acos(-1.);
  double phi_0 = seed->get_phi0();
  double phi = phi_0 + PI/2.;
  double px  = pt*cos(phi);
  double py  = pt*sin(phi);

  double x = _spacepoints[_first_station_hit-1]->get_position().x();
  double y = _spacepoints[_first_station_hit-1]->get_position().y();

  TMatrixD a(_n_parameters, 1);
  a(0, 0) = x;
  a(1, 0) = px*kappa;
  a(2, 0) = y;
  a(3, 0) = py*kappa;
  a(4, 0) = kappa;

  _momentum = TMath::Sqrt(px*px+py*py+pz*pz);

  return a;
}

TMatrixD KalmanSeed::ComputeInitialStateVector(const SciFiStraightPRTrack* seed) {
  double x = _spacepoints[_first_station_hit-1]->get_position().x();
  double y = _spacepoints[_first_station_hit-1]->get_position().y();

  double mx = seed->get_mx();
  double my = seed->get_my();
  double seed_pz = 226.;

  TMatrixD a(_n_parameters, 1);
  a(0, 0) = x;
  a(1, 0) = mx;
  a(2, 0) = y;
  a(3, 0) = my;

  _momentum = seed_pz;

  return a;
}

void KalmanSeed::RetrieveClusters(std::vector<SciFiSpacePoint*> &spacepoints,
                                  double &seed_pz) {
  size_t numb_spacepoints = spacepoints.size();

  for ( size_t i = 0; i < numb_spacepoints; ++i ) {
    SciFiSpacePoint *spacepoint = spacepoints[i];
    size_t num_clusters = spacepoint->get_channels().size();
    for ( size_t j = 0; j < num_clusters; ++j ) {
      SciFiCluster *cluster = spacepoint->get_channels()[j];
      _clusters.push_back(cluster);
    }
  }

  std::sort(_clusters.begin(), _clusters.end(), SortByID);
  std::sort(_spacepoints.begin(), _spacepoints.end(), SortByStation);
  /*
  // Compute pz from tracker timing.
  int last_cluster = clusters.size();
  double deltaT = clusters[0]->get_time() - clusters[last_cluster-1]->get_time();
  double deltaZ = clusters[0]->get_position().z() - clusters[last_cluster-1]->get_position().z();

  std::cerr << "dt and dz: " << deltaT << " " << deltaZ << "\n";
  deltaZ = deltaZ/1000.; // mm -> m
  deltaT = deltaT/1000000000.; // ns -> s

  deltaZ = fabs(deltaZ);
  deltaT = fabs(deltaT);

  double mass = 105.; // MeV/c2
  double c = 300000000.; // m/s2
  double gamma = 1./pow(1.-pow(deltaZ/(c*deltaT), 2.), 0.5);
  seed_pz = gamma*mass*deltaZ/deltaT;
  std::cerr << "Pz: " << gamma << " " << deltaZ << " "
            << deltaT << " " << seed_pz << " "
            << clusters[0]->get_true_momentum().z() << "\n";
  */
  // seed_pz = 200.;
}

} // ~namespace MAUS
