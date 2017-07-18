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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIHELICALPRTRACK_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIHELICALPRTRACK_HH_

// C++ headers
#include <vector>

// ROOT headers
#include "TRefArray.h"

// MAUS headers
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiBasePRTrack.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Recon/SciFi/SimpleHelix.hh"
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"

namespace MAUS {

/** @class SciFiHelicalPRTrack
 *  @author A. Dobbs
 *  @brief Class representing helical tracks in the tracker formed by pattern recognition
 */
class SciFiHelicalPRTrack : public SciFiBasePRTrack {
  public:

    /** Default constructor */
    SciFiHelicalPRTrack();

    /** @brief Constructor using a SimpleHelix */
    SciFiHelicalPRTrack(const SimpleHelix& helix, SciFiSpacePointPArray spoints);

    /** @brief Constructor from SimpleCircle and SimpleLine
     *
     *  Constructor from SimpleCircle and SimpleLine, without explicit chi squared setting
     *  @param tracker The tracker number
     *  @param charge The track charge
     *  @param pos0 Intercept of helix with tracker reference surface
     *  @param phi0 Initial turning angle
     *  @param circle SimpleCircle class used to construct track
     *  @param line_sz SimpleLine class used to construct track
     *  @param phi The spacepoint turning angles
     *  @param spoints Pointers to the associated spacepoints
     *  @param covariance Covariances of the fits
     */
    SciFiHelicalPRTrack(int tracker, int charge, SimpleCircle circle, SimpleLine line_sz,
                        SciFiSpacePointPArray spoints, const DoubleArray& covariance);

    /** Copy constructor - any pointers are deep copied */
    SciFiHelicalPRTrack(const SciFiHelicalPRTrack &_htrk);

    /** Destructors */
    virtual ~SciFiHelicalPRTrack();

    /** Assignment operator - any pointers are deep copied */
    SciFiHelicalPRTrack& operator=(const SciFiHelicalPRTrack &_htrk);

    /** Combine the different chi sqs into one final value */
    virtual void form_total_chi_squared();

    /** Get the tracker number */
    int get_tracker() const { return _tracker; }

    /** Set the tracker number */
    void set_tracker(int tracker) { _tracker = tracker; }

    /** Get dsdz, the slope in the s-z plane (s is path length) */
    double get_dsdz() const { return _dsdz; }

    /** Set dsdz, the slope in the s-z plane (s is path length) */
    void set_dsdz(double dsdz) { _dsdz = dsdz; }

    /** Get R the radius of the helix in the x-y plane */
    double get_R() const { return _R; }

    /** Set R the radius of the helix in the x-y plane */
    void set_R(double R) { _R = R; }

    /** Get the intercept of the straight line fit in s-z */
    double get_line_sz_c() const { return _line_sz_c; }

    /** Set the intercept of the straight line fit in s-z */
    void set_line_sz_c(double line_sz_c) { _line_sz_c = line_sz_c; }

    /** Get the chi^2 of the straight line fit in s-z */
    double get_line_sz_chisq() const { return _line_sz_chisq; }

    /** Set the chi^2 of the straight line fit in s-z */
    void set_line_sz_chisq(double line_sz_chisq) { _line_sz_chisq = line_sz_chisq; }

    /** Get the ndf of the straight line fit in s-z */
    int get_line_sz_ndf() const { return _line_sz_ndf; }

    /** Set the ndf of the straight line fit in s-z */
    void set_line_sz_ndf(int line_sz_ndf) { _line_sz_ndf = line_sz_ndf; }

    /** Get the x coordinate of the centre of the circle */
    double get_circle_x0() const { return _circle_x0; }

    /** Set the x coordinate of the centre of the circle */
    void set_circle_x0(double circle_x0) { _circle_x0 = circle_x0; }

    /** Get the y coordinate of the centre of the circle */
    double get_circle_y0() const { return _circle_y0; }

    /** Set the y coordinate of the centre of the circle */
    void set_circle_y0(double circle_y0) { _circle_y0 = circle_y0; }

    /** Get the chi^2 of the circle fit in x-y */
    double get_circle_chisq() const { return _circle_chisq; }

    /** Set the chi^2 of the circle fit in x-y */
    void set_circle_chisq(double circle_chisq) { _circle_chisq = circle_chisq; }

    /** Get the ndf of the circle fit in x-y */
    int get_circle_ndf() const { return _circle_ndf; }

    /** Set the ndf of the circle fit in x-y */
    void set_circle_ndf(int circle_ndf) { _circle_ndf = circle_ndf; }

    /** Get the track type, for straight, 1 for helical */
    int get_type() const { return _type; }

    /** Get the circle fit algorithm used  -1 = None, 0 - LSQ, 1 - MINUIT */
    int get_alg_used_circle() const { return _alg_used_circle; }

    /** Set the circle fit algorithm used  -1 = None, 0 - LSQ, 1 - MINUIT.
      * NB: this will NOT set the fit to come, only records what fit already happened! */
    void set_alg_used_circle(int alg_used_circle) { _alg_used_circle = alg_used_circle; }

    /** Get the longitudinal fit algorithm used  -1 = None, 0 - LSQ, 1 - MINUIT */
    int get_alg_used_longitudinal() const { return _alg_used_longitudinal; }

    /** Set the longitudinal fit algorithm used  -1 = None, 0 - LSQ, 1 - MINUIT.
      * NB: this will NOT set the fit to come, only records what fit already happened! */
    void set_alg_used_longitudinal(int alg_used_longitudinal) { \
      _alg_used_longitudinal = alg_used_longitudinal; }

    /** Get the full fit algorithm used  -1 = None, 0 - LSQ, 1 - MINUIT */
    int get_alg_used_full() const { return _alg_used_full; }

    /** Set the full fit algorithm used  -1 = None, 0 - LSQ, 1 - MINUIT.
      * NB: this will NOT set the fit to come, only records what fit already happened! */
    void set_alg_used_full(int alg_used_full) { _alg_used_full = alg_used_full; }

  private:
    int _tracker;                 /** Tracker number, 0 for upstream, 1 for downstream */
    static const int _type = 1;   /** Track type identifier, 0 for straight, 1 for helical */
    double _R;                    /** Radius of the track fit */
    double _dsdz;                 /** Gradient of longitudinal fit in tracker local coordinates */
    double _line_sz_c;            /** Intercept of longitudinal fit in tracker local coordinates */
    double _line_sz_chisq;        /** ChiSq of the longitudinal fit */
    int _line_sz_ndf;             /** Number of degrees of freedom of the longitudunal fit */
    double _circle_x0;            /** x coord of the circle centre in tracker local coordinates */
    double _circle_y0;            /** y coord of the circle centre in tracker local coordinates */
    double _circle_chisq;         /** ChiSq of the circle (x-y) fit */
    int _circle_ndf;              /** Number of degrees of freedom of the circle fit */
    int _alg_used_circle;         /** Circle fit algorithm used, -1 = None, 0 - LSQ, 1 - MINUIT */
    int _alg_used_longitudinal;   /** Longitudinal algorithm used, -1 = None, 0 - LSQ, 1 - MINUIT */
    int _alg_used_full;           /** Full 3D fit algorithm used, -1 = None, 0 - LSQ, 1 - MINUIT */

    MAUS_VERSIONED_CLASS_DEF(SciFiHelicalPRTrack)
};

typedef std::vector<SciFiHelicalPRTrack*> SciFiHelicalPRTrackPArray;
typedef std::vector<SciFiHelicalPRTrack> SciFiHelicalPRTrackArray;

} // ~namespace MAUS

#endif
