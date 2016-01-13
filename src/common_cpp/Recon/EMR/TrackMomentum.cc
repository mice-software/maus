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

#include "src/common_cpp/Recon/EMR/TrackMomentum.hh"

namespace TrackMomentum {

double csda_momentum(double range,
		     std::string particle) {

  if ( range < 0 )
    return -1;

  if ( !initialize(particle) )
    return -1;

  TF1* f = new TF1("f", rangefunction, .05*_m, 1000, 1);
  f->SetParameter(0, _m);
  double p = f->GetX(range, .05*_m, 1000, pow(10, -6));
  delete f;

  return p; // Total momentum [MeV/c]
}

double csda_momentum_error(double momentum,
			   double erange,
			   std::string particle) {

  if ( !initialize(particle) )
    return -1;

  if ( momentum < .05*_m ) // Limit of Bethe-Bloch validity
    return -1;

  TF1* f = new TF1("f", rangefunction, .05*_m, 1000, 1);
  f->SetParameter(0, _m);
  double dpdr = 1./(f->Derivative(momentum));
  delete f;

  return dpdr*erange; // [MeV/c]
}

bool initialize(std::string particle) {

  // !!!TODO!!! get the properties from G4
  // Electron mass
  _me = .511;

  // Mass of impinging particle
  if ( particle == "muon" ) {
    _m = 105.66;
    _z = 0.;
  } else if ( particle == "pion" ) {
    _m = 139.57;
    _z = 0.;
  } else if ( particle == "electron" ) {
    _m = .511;
    _z = -1.;
  } else if ( particle == "positron" ) {
    _m = .511;
    _z = 1.;
  } else {
    Squeak::mout(Squeak::warning)
    << "WARNING in csda_momentum. The particle type is not supported,"
    << " the momentum will not be reconstructed."
    << std::endl;
    return false;
  }

  // Properties of polystryrene (C8H8)
  double Cfrac = 48./56;
  double Hfrac = 8./56;
  _Z = pow(Cfrac*pow(6., 2.94)+Hfrac*pow(1., 2.94), 1./2.94);
  _A = _Z/0.53768;
  _rau = 1.060;
  _I = 12*_Z*pow(10, -6);

  _C = 3.2;
  _a = 0.161;
  _k = 3.24;
  _X0 = 0.1464;
  _X1 = 2.49;

  // Bethe-Bloch mean ionization energy loss constant
  _K = 0.307;

  return true;
}

double tmax(double beta, double gamma, double M) {

  return 2*_me*beta*beta*gamma*gamma/(1+2*gamma*_me/M+pow(_me/M, 2));
}

double tauFel(double beta, double tau) {

  return 1.-pow(beta, 2)+(pow(tau, 2)/8.-(2.*tau+1.)*log(2.))/pow(tau+1., 2);
}

double tauFpos(double beta, double tau) {

  return 2.*log(2.) - (pow(beta, 2)/12.)*(23. + 14./(tau+2.)
		    + 10./pow(tau+2., 2) + 4./pow(tau+2., 3));
}

double deltaF(double alpha) {

  double X = log(alpha)/log(10); // Base 10 logarithm of alpha

  if ( X < _X0 ) {
    return 0;
  } else if ( X >= _X0 && X < _X1 ) {
    return 4.605*X - _C + _a*pow(_X1-X, _k);
  } else {
    return 4.605*X - _C;
  }
}

double bethe(double *x, double *par) {

  double beta = x[0]/sqrt(pow(x[0], 2)+1);
  double gamma = sqrt(pow(x[0], 2)+1);
  double tau = gamma-1;

  double dedx(0.); // [MeV/mm]
  if ( _m > _me ) { 		// Heavy ion Bethe-Bloch
     dedx = 0.1*_K*_rau*_Z/(_A*beta*beta)
	    * (.5*log(2*_me*pow(x[0], 2)*tmax(beta, gamma, par[0])/pow(_I, 2))
	       - beta*beta - .5*deltaF(x[0]));
  } else if ( _z < 0 ) { 	// Electron Bethe-Bloch
     dedx = 0.1*_K*_rau*_Z/(_A*beta*beta)
	    * (log(_me*pow(x[0], 2)*tmax(beta, gamma, par[0])/(2*pow(_I, 2)))
	       + tauFel(beta, tau) - deltaF(x[0]));
  } else if ( _z > 0 ) {	// Positron Bethe-Bloch
     dedx = 0.1*_K*_rau*_Z/(_A*beta*beta)
	    * (log(_me*pow(x[0], 2)*tmax(beta, gamma, par[0])/(2*pow(_I, 2)))
	       + tauFpos(beta, tau) - deltaF(x[0]));
  }

  return dedx; // MeV/mm
}

double invbethe(double *x, double *par) {

  return 1./bethe(x, par); // mm/MeV
}

double invbethe_jac(double *x, double *par) {

  double beta = x[0]/sqrt(pow(x[0], 2)+1);
  double dxde = invbethe(x, par);

  return dxde*beta*par[0];
}

double rangefunction(double *x, double *par) {

  double alpha = x[0]/par[0];

  TF1 *f = new TF1("f", invbethe_jac, 0.05, 100, 1); // Scope of validity of Bethe-Bloch
  f->SetParameter(0, par[0]);
  double R = f->Integral(0.05, alpha);
  delete f;

  return R; // mm
}
} // namespace TrackMomentum
