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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMRTRACK_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMRTRACK_HH_

// C++ headers
#include <vector>

// MAUS headers
#include "Utils/VersionNumber.hh"
#include "DataStructure/EMRTrackPoint.hh"
#include "DataStructure/ThreeVector.hh"

namespace MAUS {

typedef std::vector<EMRTrackPoint> EMRTrackPointArray;

/** @class EMRTrack comment
 *
 *  @var trackpoints	<-- array of points belonging to the fitted track -->
 *  @var par		<-- array of parameters of the polynomial fit in the iz plane -->
 *  @var epar		<-- error on the array of parameters -->
 *  @var origin		<-- (x,y,z) coordinates of the track at the entrance of the EMR -->
 *  @var eorigin	<-- (ex,ey,ez) errors on the coordinates of the origin -->
 *  @var polar		<-- global polar angle theta (inclination) -->
 *  @var epolar		<-- error on the global polar angle theta -->
 *  @var azimuth	<-- global azimuthal angle phi -->
 *  @var eazimuth	<-- error on the global azimuthal angle phi -->
 *  @var chi2		<-- total normalised chi squared of the fit -->
 *  @var range		<-- range of the track in the scintillating volume -->
 *  @var erange		<-- error on the range of the track -->
 *  @var mom		<-- momentum estimated from the CSDA range assumption -->
 *  @var mom		<-- error on the CSDA momentum -->
 */

class EMRTrack {
 public:
  /** @brief  Default constructor - initialises to 0/NULL */
  EMRTrack();

  /** @brief  Copy constructor - any pointers are deep copied */
  EMRTrack(const EMRTrack &emrt);

  /** @brief  Equality operator - any pointers are deep copied */
  EMRTrack& operator=(const EMRTrack &emrt);

  /** @brief  Destructor - any member pointers are deleted */
  virtual ~EMRTrack();

  /** @brief Return trackpoints vector */
  EMRTrackPointArray GetEMRTrackPointArray() const           { return _trackpoints; }

  /** @brief Sets trackpoints vector, shallow copy */
  void SetEMRTrackPointArray(EMRTrackPointArray trackpoints) { _trackpoints = trackpoints; }

  /** @brief Add trackpoint to the vector */
  void AddEMRTrackPoint(EMRTrackPoint tp)                    { _trackpoints.push_back(tp); }

  /** @brief Returns the number of points on the track */
  size_t GetEMRTrackPointArraySize() const                   { return _trackpoints.size(); }

  /** @brief Returns the parameters of the fit in the xz plane */
  std::vector<double> GetParametersX() const                 { return _parx; }

  /** @brief Sets the parameters of the fit in the xz plane */
  void SetParametersX(std::vector<double> parx)              { _parx = parx; }

  /** @brief Returns the parameters of the fit in the yz plane */
  std::vector<double> GetParametersY() const                 { return _pary; }

  /** @brief Sets the parameters of the fit in the yz plane */
  void SetParametersY(std::vector<double> pary)              { _pary = pary; }

  /** @brief Returns the errors on the parameters of the fit in the xz plane */
  std::vector<double> GetParametersErrorsX() const           { return _eparx; }

  /** @brief Sets the errors on the parameters of the fit in the xz plane */
  void SetParametersErrorsX(std::vector<double> eparx)       { _eparx = eparx; }

  /** @brief Returns the errors on the parameters of the fit in the yz plane */
  std::vector<double> GetParametersErrorsY() const           { return _epary; }

  /** @brief Sets the errors on the parameters of the fit in the yz plane */
  void SetParametersErrorsY(std::vector<double> epary)       { _epary = epary; }

  /** @brief Returns the position of the starting point of the track */
  ThreeVector GetOrigin() const                              { return _origin; }

  /** @brief Sets the position of the starting point of the track */
  void SetOrigin(ThreeVector origin)                         { _origin = origin; }

  /** @brief Returns the errors on the position of the starting point of the track */
  ThreeVector GetOriginErrors() const                        { return _eorigin; }

  /** @brief Sets the errors on the position of the starting point of the track */
  void SetOriginErrors(ThreeVector eorigin)                  { _eorigin = eorigin; }

  /** @brief Returns the global polar angle theta (inclination) */
  double GetPolar() const                                    { return _polar; }

  /** @brief Sets the global polar angle theta (inclination) */
  void SetPolar(double polar)                                { _polar = polar; }

  /** @brief Returns the error on the global polar angle theta (inclination) */
  double GetPolarError() const                               { return _epolar; }

  /** @brief Sets the error on the global polar angle theta (inclination) */
  void SetPolarError(double epolar)                          { _epolar = epolar; }

  /** @brief Returns the global azimuthal angle phi */
  double GetAzimuth() const                                  { return _azimuth; }

  /** @brief Sets the global azimuthal angle phi */
  void SetAzimuth(double azimuth)                            { _azimuth = azimuth; }

  /** @brief Returns the error on the global azimuthal angle phi */
  double GetAzimuthError() const                             { return _eazimuth; }

  /** @brief Sets the error on the global azimuthal angle phi */
  void SetAzimuthError(double eazimuth)                      { _eazimuth = eazimuth; }

  /** @brief Returns the total normalised chi2 SUM_i[SUM_q[(qi - q)^2]]/nu */
  double GetChi2() const                                     { return _chi2; }

  /** @brief Sets the total normalised chi2 */
  void SetChi2(double chi2)                                  { _chi2 = chi2; }

  /** @brief Returns the range of the particle track */
  double GetRange() const                                    { return _range; }

  /** @brief Sets the range of the particle track */
  void SetRange(double range)                                { _range = range; }

  /** @brief Returns the error on the range of the particle track */
  double GetRangeError() const                               { return _erange; }

  /** @brief Sets the error on the range of the particle track */
  void SetRangeError(double erange)                          { _erange = erange; }

  /** @brief Returns the CSDA momentum */
  double GetMomentum() const                                 { return _mom; }

  /** @brief Sets the CSDA momentum */
  void SetMomentum(double mom)                               { _mom = mom; }

  /** @brief Returns the error on the CSDA momentum */
  double GetMomentumError() const                            { return _emom; }

  /** @brief Sets the error on the CSDA momentum */
  void SetMomentumError(double emom)                         { _emom = emom; }

 private:
  EMRTrackPointArray	_trackpoints;
  std::vector<double>	_parx;
  std::vector<double>	_pary;
  std::vector<double>	_eparx;
  std::vector<double>	_epary;
  ThreeVector		_origin;
  ThreeVector		_eorigin;
  double		_polar;
  double		_epolar;
  double		_azimuth;
  double		_eazimuth;
  double		_chi2;
  double		_range;
  double		_erange;
  double		_mom;
  double		_emom;

  MAUS_VERSIONED_CLASS_DEF(EMRTrack)
};
} // namespace MAUS

#endif // #define _SRC_COMMON_CPP_DATASTRUCTURE_EMRTRACK_HH_
