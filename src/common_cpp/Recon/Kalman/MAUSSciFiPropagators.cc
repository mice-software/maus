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


#include "src/common_cpp/Recon/Kalman/MAUSSciFiPropagators.hh"

#include "src/common_cpp/Utils/Exception.hh"

namespace MAUS {

////////////////////////////////////////////////////////////////////////////////
  // USEFUL FUCNTIONS
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
  // STRAIGHT
////////////////////////////////////////////////////////////////////////////////

  StraightPropagator::StraightPropagator(SciFiGeometryHelper* geo) :
    Kalman::Propagator_base(4),
    _geometry_helper(geo),
    _include_mcs(true),
    _muon_mass(Recon::Constants::MuonMass),
    _muon_mass_sq(_muon_mass * _muon_mass) {
  }


  TMatrixD StraightPropagator::CalculatePropagator(const Kalman::TrackPoint& start,
                                                                   const Kalman::TrackPoint& end) {
    static TMatrixD prop(4, 4);

    // Find dz between sites.
    double deltaZ = end.GetPosition() - start.GetPosition();
    prop(0, 0) = 1.0;
    prop(1, 1) = 1.0;
    prop(2, 2) = 1.0;
    prop(3, 3) = 1.0;

    prop(0, 1) = deltaZ;
    prop(2, 3) = deltaZ;

    return prop;
  }


  TMatrixD StraightPropagator::CalculateProcessNoise(const Kalman::TrackPoint& start,
                                                                   const Kalman::TrackPoint& end) {
    TMatrixD new_noise(GetDimension(), GetDimension());
    new_noise.Zero();

    if (_include_mcs) {
      Kalman::State temp_state = start.GetFiltered();
      SciFiMaterialsList materials;
      _geometry_helper->FillMaterialsList(start.GetId(), end.GetId(), materials);
      int n_steps = materials.size();
      double L = 0.0;
      double width_sum = 0.0;

      for (int i = 0; i < n_steps; i++) {
        double width = materials.at(i).second;
        width_sum += width;
        L = materials.at(i).first->L(width);
        new_noise += BuildQ(temp_state, L, width);
      }
//        new_noise = BuildQ(temp_state, L, width_sum);
    } else {
      // Pass
    }
    return new_noise;
  }



//  TMatrixD StraightPropagator::BuildQ(const Kalman::State& state, double radLen, double width) {
//    double deltaZ = width;
//    double deltaZ_squared = deltaZ*deltaZ;
//
//    TMatrixD state_vector(4, 1);
//    state_vector = state.GetVector();
//
//    double mx    = state_vector(1, 0);
//    double my    = state_vector(3, 0);
//    double p     = 200.0; // TODO: Extract a more accurate value, somehow...
//    double p2    = p*p;
//
//    double E = TMath::Sqrt(_muon_mass_sq+p2);
//    double beta = p/E;
//
//    // Calculate the planar RMS scattering angle
//    double C = _geometry_helper->HighlandFormula(radLen, beta, p);
//
//    double C2 = C*C;
//
//    double c_mx_mx = C2 * (1. + mx*mx) *
//                          (1.+ mx*mx + my*my);
//
//    double c_my_my = C2 * (1. + my*my) *
//                          (1.+ mx*mx + my*my);
//
//    double c_mx_my = C2 * mx*my * (1.+ mx*mx + my*my);
//
//    TMatrixD Q(4, 4);
//    Q(0, 0) = deltaZ_squared*c_mx_mx;
//    Q(0, 1) = -deltaZ*c_mx_mx;
//    Q(0, 2) = deltaZ_squared*c_mx_my;
//    Q(0, 3) = -deltaZ*c_mx_my;
//
//    Q(1, 0) = -deltaZ*c_mx_mx;
//    Q(1, 1) = c_mx_mx;
//    Q(1, 2) = -deltaZ*c_mx_my;
//    Q(1, 3) = c_mx_my;
//
//    Q(2, 0) = deltaZ_squared*c_mx_my;
//    Q(2, 1) = -deltaZ*c_mx_my;
//    Q(2, 2) = deltaZ_squared*c_my_my;
//    Q(2, 3) = -deltaZ*c_my_my;
//
//    Q(3, 0) = -deltaZ*c_mx_my;
//    Q(3, 1) = c_mx_my;
//    Q(3, 2) = -deltaZ*c_my_my;
//    Q(3, 3) = c_my_my;
//
//    return Q;
//  }




  TMatrixD StraightPropagator::BuildQ(const Kalman::State& state, double radLen, double width) {
    double deltaZ = width;
    double deltaZ_squared = deltaZ*deltaZ;

    TMatrixD state_vector(4, 1);
    state_vector = state.GetVector();

//    double mx    = state_vector(1, 0);
//    double my    = state_vector(3, 0);
    double p     = 200.0; // TODO: Extract a more accurate value, somehow...
    double p2    = p*p;

    double E = TMath::Sqrt(_muon_mass_sq+p2);
    double beta = p/E;

    // Calculate the planar RMS scattering angle
    double C = _geometry_helper->HighlandFormula(radLen, beta, p);
    double C2 = C*C;

    // Delta x = Delta y = Length*C/sqrt(3) #PDG
    // Delta mx approx = C
    // Theta and x correlated with const = sqrt(3)/2

    TMatrixD Q(4, 4);
    Q(0, 0) = deltaZ_squared * C2 / 3;
    Q(0, 1) = deltaZ * C2 * 0.5;
    Q(0, 2) = 0.0;
    Q(0, 3) = 0.0;

    Q(1, 0) = deltaZ * C2 * 0.5;
    Q(1, 1) = C2;
    Q(1, 2) = 0.0;
    Q(1, 3) = 0.0;

    Q(2, 0) = 0.0;
    Q(2, 1) = 0.0;
    Q(2, 2) = deltaZ_squared * C2 / 3;
    Q(2, 3) = deltaZ * C2 * 0.5;

    Q(3, 0) = 0.0;
    Q(3, 1) = 0.0;
    Q(3, 2) = deltaZ * C2 * 0.5;
    Q(3, 3) = C2;

    return Q;
  }


////////////////////////////////////////////////////////////////////////////////
  // HELICAL
////////////////////////////////////////////////////////////////////////////////


  HelicalPropagator::HelicalPropagator(SciFiGeometryHelper* helper) :
    Kalman::Propagator_base(5),
    _Bz(0.0),
    _geometry_helper(helper),
    _subtract_eloss(true),
    _include_mcs(true),
    _correct_Pz(true),
    _muon_mass(Recon::Constants::MuonMass),
    _muon_mass_sq(_muon_mass * _muon_mass),
    _delta_z(0.0),
    _delta_theta(0.0),
    _u_const(0.0),
    _cosine_term(0.0),
    _sine_term(0.0),
    _momentum_reduction_factor(0.0) {
  }


  void HelicalPropagator::Propagate(const Kalman::TrackPoint& start, Kalman::TrackPoint& end) {
    Kalman::State old_filtered = start.GetFiltered();
    TMatrixD start_vector(5, 1);
    TMatrixD end_vector(5, 1);
    TMatrixD start_cov(5, 5);
    TMatrixD end_cov(5, 5);
    TMatrixD propagator(5, 5);

    start_vector = old_filtered.GetVector();
    start_cov = old_filtered.GetCovariance();

    _calculateBasePropagator(start, end, propagator);

    end_vector = propagator * start_vector;

    if (_correct_Pz) {
      _applyPzCorrections(propagator, start_vector);
    }

    PropagatorMatrix() = propagator;
    NoiseMatrix() = CalculateProcessNoise(start, end);

    TMatrixD propT(TMatrixD::kTransposed, PropagatorMatrix());
    end_cov = PropagatorMatrix() * start_cov * propT + NoiseMatrix();

    end.SetPredicted(Kalman::State(end_vector, end_cov));
  }

  TMatrixD HelicalPropagator::CalculatePropagator(const Kalman::TrackPoint& start,
                                                                   const Kalman::TrackPoint& end) {
    Kalman::State old_filtered = start.GetFiltered();
    TMatrixD start_vector(5, 1);
    TMatrixD propagator(5, 5);

    start_vector = old_filtered.GetVector();

    try {
      _calculateBasePropagator(start, end, propagator);
    }  catch (Exceptions::Exception& e) {
      std::cerr << e.what();
    }
    if (_correct_Pz) {
      _applyPzCorrections(propagator, start_vector);
    }

    return propagator;
  }


  TMatrixD HelicalPropagator::CalculateProcessNoise(const Kalman::TrackPoint& start,
                                                                   const Kalman::TrackPoint& end) {
    TMatrixD new_noise(5, 5);
    new_noise.Zero();

    if (_include_mcs) {
      SciFiMaterialsList materials;
      _geometry_helper->FillMaterialsList(start.GetId(), end.GetId(), materials);
      int n_steps = materials.size();
      double L = 0.0;
      double width_sum = 0.0;
      Kalman::State temp_state = start.GetFiltered();

      for (int i = 0; i < n_steps; i++) {
        double width = materials.at(i).second;
        width_sum += width;
        L = materials.at(i).first->L(width);
        new_noise += BuildQ(temp_state, L, width);
      }
//      new_noise = BuildQ(temp_state, L, width_sum);
    } else {
      // Pass...
    }
    return new_noise;
  }

  void HelicalPropagator::_calculateBasePropagator(const Kalman::TrackPoint& start,
                                               const Kalman::TrackPoint& end, TMatrixD& new_prop) {
    TMatrixD old_vec(5, 1);
    old_vec           = start.GetFiltered().GetVector();
    double old_px     = old_vec(1, 0);
    double old_py     = old_vec(3, 0);
    double old_kappa  = old_vec(4, 0);
    double old_pz     = fabs(1.0 / old_kappa);
    double charge = old_kappa/fabs(old_kappa);
    double old_momentum = sqrt(old_px*old_px + old_py*old_py + old_pz*old_pz);

    double gradient = sqrt(old_px*old_px + old_py*old_py) / old_pz;
    double distance_factor = sqrt(1.0 + gradient*gradient );

    _momentum_reduction_factor = 1.0;

    if (_subtract_eloss) {
      SciFiMaterialsList materials;
      _geometry_helper->FillMaterialsList(start.GetId(), end.GetId(), materials);

      // Reduce/increase momentum vector accordingly.
      double e_loss_sign = 1.0;
      if (end.GetId() > 0) {
        e_loss_sign = -1.0;
      }

      double delta_energy = 0.0;
      double energy = sqrt(old_momentum*old_momentum + _muon_mass_sq);
      double momentum = old_momentum;
      int n_steps = materials.size();

      for (int i = 0; i < n_steps; i++) {  // In mm => times by 0.1;
        double width = materials.at(i).second;
        double path_length = 0.1 * width * distance_factor;
        double SP = _geometry_helper->BetheBlochStoppingPower(momentum, materials.at(i).first);
        delta_energy = e_loss_sign*SP*path_length;
//        double SP = _geometry_helper->LandauVavilovStoppingPower(momentum,
//                                                            materials.at(i).first, path_length);
//        delta_energy = e_loss_sign*SP;

        energy = energy - delta_energy;
        if (energy < _muon_mass) {
          throw Exceptions::Exception(Exceptions::recoverable,
              "Mass Shell condition failed",
              "HelicalPropagator::_calculateBasePropagator()");
        }
        momentum = sqrt(energy*energy - _muon_mass_sq);
      }
      _momentum_reduction_factor = momentum/old_momentum;
    }


    _Bz = _geometry_helper->GetFieldValue((start.GetId() > 0 ? 1 : 0));

    _delta_z       = end.GetPosition() - start.GetPosition();
    _u_const       = - charge*_speed_of_light*_Bz;
    _delta_theta   = - _Bz*_speed_of_light*_delta_z*old_kappa;
    _sine_term     = sin(_delta_theta);
    _cosine_term   = cos(_delta_theta);

    if (start.GetId() < 0) _u_const = - _u_const;

    new_prop(0, 0) = 1.;
    new_prop(0, 1) = (_sine_term/_u_const);
    new_prop(0, 2) = 0.;
    new_prop(0, 3) = ((_cosine_term-1.)/_u_const);
    new_prop(0, 4) = 0.0;

    new_prop(1, 0) = 0.;
    new_prop(1, 1) = _cosine_term*_momentum_reduction_factor;
    new_prop(1, 2) = 0.;
    new_prop(1, 3) = -_sine_term*_momentum_reduction_factor;
    new_prop(1, 4) = 0.0;

    new_prop(2, 0) = 0.;
    new_prop(2, 1) = ((1.-_cosine_term)/_u_const);
    new_prop(2, 2) = 1.;
    new_prop(2, 3) = (_sine_term/_u_const);
    new_prop(2, 4) = 0.0;

    new_prop(3, 0) = 0.;
    new_prop(3, 1) = _sine_term*_momentum_reduction_factor;
    new_prop(3, 2) = 0.;
    new_prop(3, 3) = _cosine_term*_momentum_reduction_factor;
    new_prop(3, 4) = 0.0;

    new_prop(4, 0) = 0.;
    new_prop(4, 1) = 0.;
    new_prop(4, 2) = 0.;
    new_prop(4, 3) = 0.;
    new_prop(4, 4) = 1./_momentum_reduction_factor;
  }

  void HelicalPropagator::_applyPzCorrections(TMatrixD& propagator,
                                                              const TMatrixD& start_vector) const {
    propagator(0, 4) = _delta_z*(start_vector(1, 0)*_cosine_term - start_vector(3, 0)*_sine_term);

    propagator(1, 4) = -_u_const*_delta_z*(start_vector(1, 0)*_sine_term +
                                                                  start_vector(3, 0)*_cosine_term);
    propagator(2, 4) = _delta_z*(start_vector(1, 0)*_sine_term + start_vector(3, 0)*_cosine_term);

    propagator(3, 4) = _u_const*_delta_z*(start_vector(1, 0)*_cosine_term -
                                                                    start_vector(3, 0)*_sine_term);
  }

  TMatrixD HelicalPropagator::BuildQ(const Kalman::State& state, double L, double material_w) {
    TMatrixD vec = state.GetVector();
    double px    = vec(1, 0);
    double py    = vec(3, 0);
    double kappa = vec(4, 0);
    double pz    = fabs(1./kappa);
    double p     = sqrt(px*px+py*py+pz*pz);
    double p2    = p*p;

    double E    = TMath::Sqrt(_muon_mass_sq+p2);
    double beta = p/E;

    double theta_mcs  = _geometry_helper->HighlandFormula(L, beta, p);
    double theta_mcs2 = theta_mcs*theta_mcs;


    // Calculate covariance matrix in delta_x, theta_x, delta_y, theta_y coodinates.

    TMatrixD scat(4, 4);

    scat(1, 1) = 1.0; // Var(theta_x, theta_x)
    scat(3, 3) = 1.0; // Var(theta_y, theta_y)
    scat(0, 0) = material_w*material_w/3.0; // Var(delta_x, delta_x)
    scat(2, 2) = material_w*material_w/3.0; // Var(delta_y, delta_y)

    scat(0, 1) = material_w/2.0; // Cov(delta_x, theta_x)
    scat(1, 0) = material_w/2.0; // Cov(delta_x, theta_x)

    scat(2, 3) = material_w/2.0; // Cov(delta_y, theta_y)
    scat(3, 2) = material_w/2.0; // Cov(delta_y, theta_y)

    scat *= theta_mcs2;


    // Calculate the Jacbian with the state vector space
    //  (Some HEAVY use of the paraxial approximation here...)

    TMatrixD Jacob(5, 4);

    Jacob(0, 0) = 1.0;
    Jacob(1, 1) = pz;
    Jacob(2, 2) = 1.0;
    Jacob(3, 3) = pz;

    TMatrixD JacobT(TMatrixD::kTransposed, Jacob);

    TMatrixD Q = Jacob * scat * JacobT;

    return Q;
  }


//  TMatrixD HelicalPropagator::BuildQ(const Kalman::State& state, double L, double material_w) {
//    TMatrixD vec = state.GetVector();
//    double px    = vec(1, 0);
//    double py    = vec(3, 0);
//    double kappa = vec(4, 0);
//    double pz    = fabs(1./kappa);
//    double p     = sqrt(px*px+py*py+pz*pz);
//    double p2    = p*p;
//
//    double E    = TMath::Sqrt(_muon_mass_sq+p2);
//    double beta = p/E;
//
//    double theta_mcs  = _geometry_helper->HighlandFormula(L, beta, p);
//    double theta_mcs2 = theta_mcs*theta_mcs;
//
//    double charge = kappa/fabs(kappa);
//    double c      = CLHEP::c_light;
//    double u      = charge*c*_Bz;
//    double delta_theta = u*material_w*fabs(kappa);
//    double sine   = sin(delta_theta);
//    double cosine = cos(delta_theta);
//
//    double dtheta_dpz = -delta_theta/pz;
//    // ------------------------------------------------------------
//    TMatrixD dalpha_dp(GetDimension(), 3);
//    // dx, dpx
//    dalpha_dp(0, 0) = sine/u;
//    // dx, dpy
//    dalpha_dp(0, 1) = (cosine-1.)/u;
//    // dx, dpz
//    dalpha_dp(0, 2) = px*dtheta_dpz*cosine/u - py*dtheta_dpz*sine/u;
//
//    // dpx, dpx
//    dalpha_dp(1, 0) = cosine;
//    // dpx, dpy
//    dalpha_dp(1, 1) = -sine;
//    // dpx, dpz
//    dalpha_dp(1, 2) = -px*dtheta_dpz*sine - py*dtheta_dpz*cosine;
//
//    // dy, dpx
//    dalpha_dp(2, 0) = (1.-cosine)/u;
//    // dy, dpy
//    dalpha_dp(2, 1) = sine/u;
//    // dy, dpz
//    dalpha_dp(2, 2) = py*dtheta_dpz*cosine/u + px*dtheta_dpz*sine/u;
//
//    // dpy, dpx
//    dalpha_dp(3, 0) = sine;
//    // dpy, dpy
//    dalpha_dp(3, 1) = cosine;
//    // dpy, dpz
//    dalpha_dp(3, 2) = -py*dtheta_dpz*sine + px*dtheta_dpz*cosine;
//
//    // dkappa, dpx
//    dalpha_dp(4, 0) = 0.;
//    // dkappa, dpy
//    dalpha_dp(4, 1) = 0.;
//    // dkappa, dpz
//    dalpha_dp(4, 2) = -charge/(pz*pz);
//
//    TMatrixD dalpha_dp_transposed(3, GetDimension());
//    dalpha_dp_transposed.Transpose(dalpha_dp);
//
//    // ------------------------------------------------------------
//    TMatrixD dalpha_dx(GetDimension(), 3);
//    dalpha_dx.Zero();
//    dalpha_dx(0, 0) = 1.;
//    dalpha_dx(2, 1) = 1.;
//
//    TMatrixD dalpha_dx_transposed(3, GetDimension());
//    dalpha_dx_transposed.Transpose(dalpha_dx);
//
//    // ------------------------------------------------------------
//    TMatrixD P(3, 3);
//    // Diagonal terms.
//    P(0, 0) = 1.-px*px/p2;
//    P(1, 1) = 1.-py*py/p2;
//    P(2, 2) = 1.-pz*pz/p2;
//    // Off diagonal, symmetric.
//    P(0, 1) = px*py/p2;
//    P(1, 0) = px*py/p2;
//    P(0, 2) = px*pz/p2;
//    P(2, 0) = px*pz/p2;
//    P(1, 2) = py*pz/p2;
//    P(2, 1) = py*pz/p2;
//
//
//    TMatrixD term_1(GetDimension(), GetDimension());
//    term_1 = dalpha_dp*P*dalpha_dp_transposed;
//    term_1 = p2*term_1;
//
//    TMatrixD term_2(GetDimension(), GetDimension());
//    term_2 = dalpha_dp*P*dalpha_dx_transposed;
//    term_2 = p*material_w*0.5*term_2;
//
//    TMatrixD term_3(GetDimension(), GetDimension());
//    term_3 = dalpha_dx*P*dalpha_dp_transposed;
//    term_3 = p*material_w*0.5*term_3;
//
//    TMatrixD term_4(GetDimension(), GetDimension());
//    term_4 = dalpha_dx*P*dalpha_dx_transposed;
//    term_4 = (1./3.)*material_w*material_w*term_4;
//
//    TMatrixD Q(GetDimension(), GetDimension());
//    Q = theta_mcs2*(term_1 + term_2 + term_3 + term_4);
//
//    return Q;
//  }

} // namespace MAUS

