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

#include "src/common_cpp/Recon/EMR/TrackRange.hh"

namespace TrackRange {

double range_piecewise(std::vector<MAUS::ThreeVector> points) {

  if ( !points.size() )
      return -1.;

  // Loop over the points, add a piece of track for each pair
  double range(0.);
  for (size_t i = 0; i < points.size()-1; i++) {
    MAUS::ThreeVector Pi = points[i];
    MAUS::ThreeVector Pj = points[i+1];
    if ( Pi.z() == Pj.z() )
	continue;

    range += sqrt(pow(Pj.x()-Pi.x(), 2)+pow(Pj.y()-Pi.y(), 2)+pow(Pj.z()-Pi.z(), 2));
  }

  return range;
}

double range_piecewise_error(std::vector<MAUS::ThreeVector> points,
			     std::vector<MAUS::ThreeVector> errors) {

  if ( !points.size() )
      return -1.;

  // Loop over the points, add a piece of track for each pair
  double error2(0.);
  for (size_t i = 0; i < points.size()-1; i++) {
    MAUS::ThreeVector Pi = points[i];
    MAUS::ThreeVector Pj = points[i+1];
    MAUS::ThreeVector ei = errors[i];
    MAUS::ThreeVector ej = errors[i+1];

    double Rij = sqrt(pow(Pj.x()-Pi.x(), 2)+pow(Pj.y()-Pi.y(), 2)+pow(Pj.z()-Pi.z(), 2));
    if ( !Rij )
      continue;

    double e2x = pow((Pj.x()-Pi.x())/Rij, 2)*(pow(ei.x(), 2)+pow(ej.x(), 2));
    double e2y = pow((Pj.y()-Pi.y())/Rij, 2)*(pow(ei.y(), 2)+pow(ej.y(), 2));
    double e2z = pow((Pj.z()-Pi.z())/Rij, 2)*(pow(ei.z(), 2)+pow(ej.z(), 2));

    error2 += e2x + e2y + e2z;
  }

  return sqrt(error2);
}

TF1* f_path(0);

double range_integral(std::vector<double> parx,
		      std::vector<double> pary,
		      double zstart,
		      double zend) {

  if ( !parx.size() )
      return -1.;

  if ( parx.size() == 1 )
      return zend-zstart;

  size_t n = parx.size()-1;	// Order of the polynomial
  double par[2*n+1];
  par[0] = n; 			// Tells the function the order of the polynomial
  for (size_t p = 1; p < n+1; p++) {
    par[p] = p*parx[p];		// (n-1) parameters from the xz fit
    par[n+p] = p*pary[p];	// (n-1) parameters from the yz fit
  }

  // Path function of the 3D track, follows the variations in the two projections
  if (!f_path)
    f_path = new TF1("f_path", fpath, zstart, zend, 2*n+1);

  f_path->SetParameters(par); 			// Tells the function the size of the array
  double range = f_path->Integral(zstart, zend, f_path->GetParameters(), 1e-3);
//   delete f_path;

  return range;
}

double range_integral_error(std::vector<double> parx,
			    std::vector<double> pary,
			    std::vector<double> eparx,
			    std::vector<double> epary,
			    double zstart,
			    double zend,
			    double ezstart,
			    double ezend) {

  if ( !parx.size() )
      return -1.;

  if ( parx.size() == 1 )
      return sqrt(pow(ezstart, 2)+pow(ezend, 2));

  size_t n = parx.size()-1;	// Order of the polynomial
  double par[2*n+3];
  par[0] = n; 			// Tells the function the order of the polynomial
  for (size_t p = 1; p < n+1; p++) {
    par[p] = p*parx[p];		// (n-1) parameters from the xz fit
    par[n+p] = p*pary[p];	// (n-1) parameters from the yz fit
  }

  // Error function of the 3D track, follows the variations in the two projections
  TF1* f_error = new TF1("f_error", fpath_error, zstart, zend, 2*n+3);

  // Each derivative in a parameter contributes to the total uncertainty
  double error2(0.);
  for (size_t k = 1; k < n+1; k++) {
    par[2*n+1] = k;	    			// Sets the order of the parameter (1->n)
    par[2*n+2] = 0;				// Sets the orientation of the projection
    f_error->SetParameters(par); 		// Tells the function the size of the array
    double integralx = f_error->Integral(zstart, zend, f_error->GetParameters(), 1e-3);
    error2 += pow(k*integralx, 2)*eparx[k*(n+2)];

    par[2*n+2] = 1;				// Sets the orientation of the projection
    f_error->SetParameters(par); 		// Tells the function the size of the array
    double integraly = f_error->Integral(zstart, zend, f_error->GetParameters(), 1e-3);
    error2 += pow(k*integraly, 2)*epary[k*(n+2)];
  }
  delete f_error;

  // The uncertainty on the boundaries contributes to the total uncertainty as well
  double drdzstart = fpath(&zstart, par);
  double drdzend = fpath(&zend, par);

  error2 += pow(drdzstart*ezstart, 2);
  error2 += pow(drdzend*ezend, 2);

  return sqrt(error2);
}

double fpol(double* x, double* par) {

  double y(0.);
  for (size_t p = 0; p < par[0]; p++)
    y += par[p+1]*pow(x[0], p);

  return y;
}

double fpath(double* x, double* par) {

  size_t n = par[0]; 		// Number of derivative parameters
  double parx[n+1], pary[n+1]; 	// Parameter containers
  parx[0] = n;
  pary[0] = n;
  for (size_t i = 1; i < n+1; i++) {
    parx[i] = par[i];
    pary[i] = par[n+i];
  }
  double dpolx = fpol(x, parx);	// Derivative of the xz polynome in z
  double dpoly = fpol(x, pary);	// Derivative of the yz polynome in z

  return sqrt(1 + pow(dpolx, 2) + pow(dpoly, 2));
}

double fpath_error(double* x, double* par) {

  size_t n = par[0]; 		// Number of derivative parameters
  size_t k = par[2*n+1];	// Order of the parameter
  double p = fpath(x, par);  	// Path function in x
  double parq[n+1];
  parq[0] = n;
  for (size_t i = 1; i < n+1; i++) {
    if ( !par[2*n+2] ) {
      parq[i] = par[i];
    } else {
      parq[i] = par[n+i];
    }
  }
  double dpol = fpol(x, parq);

  return dpol*pow(x[0], k-1)/p;	// p > 1, safe
}
} // namespace TrackRange
