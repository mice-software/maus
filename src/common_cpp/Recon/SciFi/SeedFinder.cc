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

#include <algorithm>

#include "CLHEP/Matrix/Matrix.h"
#include "src/common_cpp/Recon/SciFi/SeedFinder.hh"

SeedFinder::SeedFinder() {
  // _B    = 4.;
  _Q    = 1.;
  _sigx = 0.5;
  _sigy = 0.5;
  // the direction of rotation +1 = clockwise, -1 = anticlockwise
  _direction = _B * _Q / fabs(_B * _Q);
  // _pz = 0.0;
  value = 0;
  _p = 200;
}

SeedFinder::~SeedFinder() {}

bool descending_station_numb(const SciFiSpacePoint& pnt1, const SciFiSpacePoint& pnt2) {
      return pnt1.get_station() > pnt2.get_station();
}

bool ascending_station_numb(const SciFiSpacePoint& pnt1, const SciFiSpacePoint& pnt2) {
      return pnt1.get_station() < pnt2.get_station();
}

bool SeedFinder::process(std::vector<SciFiSpacePoint> spacepoints) {
  Hep3Vector pos1, pos2, pos3;
  SciFiSpacePoint sp1, sp2, sp3;
  int numb_spacepoints = spacepoints.size();
  sp1 = (spacepoints[0]);
  sp2 = (spacepoints[numb_spacepoints-2]);
  sp3 = (spacepoints[numb_spacepoints-1]);
  pos1 = sp1.get_position();
  pos2 = sp2.get_position();
  pos3 = sp3.get_position();

  _tracker = sp1.get_tracker();

  if ( _tracker == 0 ) {
    _B    = -4.;
  }
  if ( _tracker == 1 ) {
    _B    = 4.;
  }

  try {
    // order points by increazing z
    // order_points();
    // determine the center and radius of the circle
    std::cout << "Determining Centre..." << std::endl;
    determineCentre(pos1, pos2, pos3);

    // calculate Pt
    // from the radius of the circle and the magnetic field
    determinePtFromR();
    std::cout << "momentum from points." << std::endl;
    // determine the phi angles of each three points
    momentumFromPoints(sp1, sp2, sp3);

    // SciFiSeed seed = new SciFiSeed();
    print_event_info();
    print_to_file();
  } catch(char * str) {
    std::cerr << "Exception: " << str << std::endl;
  }
  std::cout << "Finished without errors" << std::endl;
  return true;
}

void SeedFinder::momentumFromPoints(SciFiSpacePoint pnt1,
                                    SciFiSpacePoint pnt2,
                                    SciFiSpacePoint pnt3) {
  // p is initialized with a guess value...
  // double p = _pz;

  std::vector<SciFiSpacePoint> spacepoints;
  spacepoints.push_back(pnt1);
  spacepoints.push_back(pnt2);
  spacepoints.push_back(pnt3);
  // ============================================
  // MUCH ATTENTION NEEDED HERE... coordinate reference frame...
  if ( pnt1.get_tracker() == 0 ) {
    sort(spacepoints.begin(), spacepoints.end(), descending_station_numb);
  } else if ( pnt1.get_tracker() == 1 ) {
    sort(spacepoints.begin(), spacepoints.end(), ascending_station_numb);
  }
/*
  if ( pnt1.get_tracker() == 0 ) {
    sort(spacepoints.begin(), spacepoints.end(), increazing_station_no);
  } else if ( pnt1.get_tracker() == 1 ) {
    sort(spacepoints.begin(), spacepoints.end(), increazing_z);
   }

  std::cout << spacepoints[0].get_position().z() << " " <<
               spacepoints[1].get_position().z() << " " <<
               spacepoints[2].get_position().z() << std::endl;

  std::cout << spacepoints[0].get_station() << " " <<
               spacepoints[1].get_station() << " " <<
               spacepoints[2].get_station() << std::endl;
  */
  if ( spacepoints[0].get_station() == spacepoints[1].get_station() ||
       spacepoints[1].get_station() == spacepoints[2].get_station() ||
       spacepoints[0].get_station() == spacepoints[2].get_station()) {
    throw "Spacepoints in same station.";
  }
  // --------------------------------------------
  // ============================================

  Hep3Vector pos1 = spacepoints[0].get_position();
  Hep3Vector pos2 = spacepoints[1].get_position();
  Hep3Vector pos3 = spacepoints[2].get_position();

  // determine the phi angles of each three points
  double phi1, phi2, phi3;
  determinePhi(pos1, phi1);
  determinePhi(pos2, phi2);
  determinePhi(pos3, phi3);

  _phi_0 = phi1;

  // determine PX and PY from PT and phi
  // determine_PxandPy_fromPt();
  _px =   _pt * cos(phi1);
  _py = - _pt * sin(phi1);

  // determine PZ from PT and the slope of the line in s vs z
  double dphi1 = _direction * (phi2 - phi1);

  if ( dphi1 < 0. )
     dphi1 += 2. * pi;

  double dphi2 = _direction * (phi3 - phi1);

  if ( dphi2 < 0. )
    dphi2 += 2. * pi;

  if ( dphi2 < dphi1 )
    dphi2 += 2. * pi;

  // R_z is the
  double R_z = ( pos3.z() - pos1.z() ) / ( pos2.z() - pos1.z() );

  double phirat = dphi2 / dphi1;

  value = (fabs(phirat-R_z)/R_z);
  // test if this ratio is consistent with the z spacing
  if ( ( fabs( phirat - R_z ) / R_z ) > 0.2 )
    throw "Inconsistent z spacing.";

  double dsdz = _r * dphi2 / ( pos3.z() - pos1.z() );

  if ( dsdz == 0. )
    throw "dsdz ==0";

  // if( use_p ) {
    // calculate pz knowing pt and the total momentum

  if ( _pt > _p ) {
    throw "pt > p";
  } else {
      _pz = sqrt(_p * _p - _pt * _pt);
  }
  // else {
  // tan(lambda) = dz/ds
  // and pz = pt x tan(lambda)
  // _pz = _pt / dsdz;
  // }

  // _pz = fabs( _pz );
  // tan(lamda)
  _tan_lambda = 1./dsdz;
  // determine sigpx, sigpy and sigpz
  _sigr = sqrt(( _sigx * _sigx ) + ( _sigy * _sigy ));
  _sigpt = _pt * (_sigr / _r);
  _sigpz = _pz * (_sigpt / _pt);
  _sigpx = _sigpt / sqrt(2.);
  _sigpy = _sigpt / sqrt(2.);
  // ME corrections from Monte Carlo
/*
  _sigpx *= 6.34; //3.17; 
  _sigpy *= 6.18; //3.09; 
  _sigpz *= 1.88; //0.94;
  // keep mcs always on
  _sigpx *= 0.42;
  _sigpy *= 0.44;
  _sigpz *= 0.36;
*/
  // return true;
}

void SeedFinder::determineCentre(Hep3Vector& p1, Hep3Vector& p2, Hep3Vector& p3) {
  CLHEP::HepMatrix a(3, 3);
  CLHEP::HepMatrix d(3, 3);
  CLHEP::HepMatrix e(3, 3);
  CLHEP::HepMatrix f(3, 3);

  a[0][0] = p1.x();
  a[0][1] = p1.y();
  a[0][2] = 1.;
  a[1][0] = p2.x();
  a[1][1] = p2.y();
  a[1][2] = 1.;
  a[2][0] = p3.x();
  a[2][1] = p3.y();
  a[2][2] = 1.;

  d[0][0] = p1.x() * p1.x() + p1.y() * p1.y();
  d[0][1] = p1.y();
  d[0][2] = 1.;
  d[1][0] = p2.x() * p2.x() + p2.y() * p2.y();
  d[1][1] = p2.y();
  d[1][2] = 1.;
  d[2][0] = p3.x() * p3.x() + p3.y() * p3.y();
  d[2][1] = p3.y();
  d[2][2] = 1.;

  e[0][0] = p1.x() * p1.x() + p1.y() * p1.y();
  e[0][1] = p1.x();
  e[0][2] = 1.;
  e[1][0] = p2.x() * p2.x() + p2.y() * p2.y();
  e[1][1] = p2.x();
  e[1][2] = 1.;
  e[2][0] = p3.x() * p3.x() + p3.y() * p3.y();
  e[2][1] = p3.x();
  e[2][2] = 1.;

  f[0][0] = p1.x() * p1.x() + p1.y() * p1.y();
  f[0][1] = p1.x();
  f[0][2] = p1.y();
  f[1][0] = p2.x() * p2.x() + p2.y() * p2.y();
  f[1][1] = p2.x();
  f[1][2] = p2.y();
  f[2][0] = p3.x() * p3.x() + p3.y() * p3.y();
  f[2][1] = p3.x();
  f[2][2] = p3.y();

  double detA = a.determinant();
  double detD = - d.determinant();
  double detE = e.determinant();
  double detF = - f.determinant();

  if ( detA == 0. )
    throw "detA == 0";

  _x0 = - detD / (2. * detA);
  _y0 = - detE / (2. * detA);


  double det = ( detD * detD + detE * detE ) / ( 4. * detA * detA ) - ( detF / detA );

  if ( det < 0. )
    throw "det < 0";

  _r = sqrt(det);
}

void SeedFinder::determinePtFromR() { // double& r, double& B, double& pt ) {
  // pt = r / CLHEP::m * B / CLHEP::tesla * 0.3 * CLHEP::GeV;
  _pt = _r * _B * 0.3; // pt in MeV
}

void SeedFinder::determinePhi(Hep3Vector& pos, double& phi ) {
  // phi = atan2(pos.x() - _x0, pos.y() - _y0);
  phi = atan2(pos.y() - _y0, pos.x() - _x0); // correction

  if ( phi < 0. )
    phi += 2. * pi;
}

void SeedFinder::print_event_info() {
  std::cout << " ******** Seed state: ******** " << "\n"
            << "Tracker " << _tracker << "\n"
            << "Radius " << _r << "+/-" << _sigr
            << ", Centre: (" << _x0 << ", " << _y0 << ") " << "\n"
            << "Momentum = (" << _px << "+/-" << _sigpx
            << ", " << _py << "+/-" << _sigpy << ", " << _pz
            << "+/-" << _sigpz << ") \n"
            << "Transverse Mom = " << _pt << "+/-" << _sigpt << "\n"
            << "|Momentum| = " << sqrt(_pt*_pt+_pz*_pz) << std::endl;
}

void SeedFinder::print_to_file() {
  std::ofstream myfile;
  myfile.open("tracks.txt", std::ios::app);
  myfile << _tracker << " " << _r << " " << _x0 << " " << _y0 << " "
         << _pt << " " << _pz << _phi_0 << " " << _tan_lambda << std::endl;
  myfile.close();
}
