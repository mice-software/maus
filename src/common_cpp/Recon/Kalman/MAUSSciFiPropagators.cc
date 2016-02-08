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


  TMatrixD StraightPropagator::CalculatePropagator(const Kalman::State& start,
                                                                        const Kalman::State& end) {
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


  TMatrixD StraightPropagator::CalculateProcessNoise(const Kalman::State& start,
                                                                        const Kalman::State& end) {
    static TMatrixD new_noise(GetDimension(), GetDimension());
    new_noise.Zero();

    if (_include_mcs) {
      // TODO : Should probably finish this one...
      Kalman::State temp_start(start);
      Kalman::State temp_end(start);

      SciFiMaterialsList materials;
      _geometry_helper->FillMaterialsList(start.GetId(), end.GetId(), materials);
      int n_steps = materials.size();

      for (int i = 0; i < n_steps; i++) {

        double width = materials.at(i).second;

        temp_end.SetPosition(temp_end.GetPosition() + width);

        TMatrixD F = CalculatePropagator(temp_start, temp_end);
        temp_end.SetVector(F * temp_start.GetVector());

        double L = materials.at(i).first->L(width);
        TMatrixD Q = BuildQ(temp_end, L, width);

        TMatrixD F_transposed(GetDimension(), GetDimension());
        F_transposed.Transpose(F);

        new_noise = F*new_noise*F_transposed + Q;
        temp_start = temp_end;
      }
    } else {
      // Pass
    }
    return new_noise;
  }


  TMatrixD StraightPropagator::BuildQ(const Kalman::State& state, double radLen, double width) {
    double deltaZ = width;
    double deltaZ_squared = deltaZ*deltaZ;

    TMatrixD state_vector(4, 1);
    state_vector = state.GetVector();

    double mx    = state_vector(1, 0);
    double my    = state_vector(3, 0);
    double p     = 200.0; // TODO: Extract a more accurate value, somehow...
    double p2    = p*p;

    double E = TMath::Sqrt(_muon_mass_sq+p2);
    double beta = p/E;

    double C = _geometry_helper->HighlandFormula(radLen, beta, p);

    double C2 = C*C;

    double c_mx_mx = C2 * (1. + mx*mx) *
                          (1.+ mx*mx + my*my);

    double c_my_my = C2 * (1. + my*my) *
                          (1.+ mx*mx + my*my);

    double c_mx_my = C2 * mx*my * (1.+ mx*mx + my*my);

    TMatrixD Q(4, 4);
    Q(0, 0) = deltaZ_squared*c_mx_mx;
    Q(0, 1) = -deltaZ*c_mx_mx;
    Q(0, 2) = deltaZ_squared*c_mx_my;
    Q(0, 3) = -deltaZ*c_mx_my;

    Q(1, 0) = -deltaZ*c_mx_mx;
    Q(1, 1) = c_mx_mx;
    Q(1, 2) = -deltaZ*c_mx_my;
    Q(1, 3) = c_mx_my;

    Q(2, 0) = deltaZ_squared*c_mx_my;
    Q(2, 1) = -deltaZ*c_mx_my;
    Q(2, 2) = deltaZ_squared*c_my_my;
    Q(2, 3) = -deltaZ*c_my_my;

    Q(3, 0) = -deltaZ*c_mx_my;
    Q(3, 1) = c_mx_my;
    Q(3, 2) = -deltaZ*c_my_my;
    Q(3, 3) = c_my_my;

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
    _muon_mass_sq(_muon_mass * _muon_mass) {
  }


  void HelicalPropagator::Propagate(const Kalman::State& start, Kalman::State& end) {
    TMatrixD old_vec(5, 1);
    old_vec             = start.GetVector();
    double old_x        = old_vec(0, 0);
    double old_px       = old_vec(1, 0);
    double old_y        = old_vec(2, 0);
    double old_py       = old_vec(3, 0);
    double old_kappa    = old_vec(4, 0);
    double old_pz       = fabs(1.0 / old_kappa);
    double charge       = old_kappa*old_pz;
    double old_momentum = sqrt(old_px*old_px + old_py*old_py + old_pz*old_pz);

    _Bz = _geometry_helper->GetFieldValue((start.GetId() > 0 ? 1 : 0));

    double c       = CLHEP::c_light;
    double u       = - charge*c*_Bz;
    double delta_z = end.GetPosition() - start.GetPosition();
    double delta_theta = - c*_Bz*delta_z*old_kappa;
    double sine    = sin(delta_theta);
    double cosine  = cos(delta_theta);

    if (start.GetId() < 0) u = - u;

    // Calculate the new track parameters.
    double new_x  = old_x + old_px*sine/u
                    - old_py*(1.-cosine)/u;

    double new_px = old_px*cosine - old_py*sine;

    double new_y  = old_y + (old_py*sine)/u
                    + old_px*(1.-cosine)/u;

    double new_py = old_py*cosine + old_px*sine;

    double new_kappa = old_kappa;

    double gradient = sqrt(old_px*old_px + old_py*old_py) / old_pz;
    double distance_factor = sqrt(1.0 + gradient*gradient );

    if (_subtract_eloss) {
      SciFiMaterialsList materials;
      _geometry_helper->FillMaterialsList(start.GetId(), end.GetId(), materials);

      // Reduce/increase momentum vector accordingly.
      double e_loss_sign = 1.0;
      if (end.GetId() > 0) {
        e_loss_sign = -1.0;
      }

      double delta_energy = 0.0;
      double energy = sqrt( old_momentum*old_momentum + _muon_mass_sq );
      double momentum = old_momentum;
      int n_steps = materials.size();
      for (int i = 0; i < n_steps; i++) {  // In mm => times by 0.1;
        double SP = _geometry_helper->BetheBlochStoppingPower(momentum, materials.at(i).first);
        delta_energy = e_loss_sign*SP*materials.at(i).second*0.1*distance_factor;
        energy = energy + delta_energy;
        momentum = sqrt( energy*energy - _muon_mass_sq );
      }

      double reduction_factor = momentum/old_momentum;
      new_px *= reduction_factor;
      new_py *= reduction_factor;
      new_kappa /= reduction_factor;
    }

    TMatrixD end_vec(5, 1);
    end_vec(0, 0) = new_x;
    end_vec(1, 0) = new_px;
    end_vec(2, 0) = new_y;
    end_vec(3, 0) = new_py;
    end_vec(4, 0) = new_kappa;

    PropagatorMatrix() = CalculatePropagator(start, end);
    NoiseMatrix() = CalculateProcessNoise(start, end);

    TMatrixD propT(TMatrixD::kTransposed, PropagatorMatrix());
    TMatrixD end_cov = PropagatorMatrix() * start.GetCovariance() * propT + NoiseMatrix();

    end.SetVector(end_vec);
    end.SetCovariance(end_cov);
  }


  TMatrixD HelicalPropagator::CalculatePropagator(const Kalman::State& start,
                                                                        const Kalman::State& end) {
    TMatrixD old_vec(5, 1);
    old_vec           = start.GetVector();
//  double old_x      = old_vec(0, 0);
    double old_px     = old_vec(1, 0);
//  double old_y      = old_vec(2, 0);
    double old_py     = old_vec(3, 0);
    double old_kappa  = old_vec(4, 0);
    double old_pz     = fabs(1.0 / old_kappa);
    double charge = old_kappa/fabs(old_kappa);
    double old_momentum = sqrt(old_px*old_px + old_py*old_py + old_pz*old_pz);

    _Bz = _geometry_helper->GetFieldValue((start.GetId() > 0 ? 1 : 0));

    double c      = CLHEP::c_light;
    double u      = - charge*c*_Bz;
    double delta_z = end.GetPosition() - start.GetPosition();
    double delta_theta = - _Bz*c*delta_z*old_kappa;
    double sine   = sin(delta_theta);
    double cosine = cos(delta_theta);

    double gradient = sqrt(old_px*old_px + old_py*old_py) / old_pz;
    double distance_factor = sqrt(1.0 + gradient*gradient );

    if (start.GetId() < 0) u = - u;

    double reduction_factor = 1.0;

    if (_subtract_eloss) {
      SciFiMaterialsList materials;
      _geometry_helper->FillMaterialsList(start.GetId(), end.GetId(), materials);

      // Reduce/increase momentum vector accordingly.
      double e_loss_sign = 1.0;
      if (end.GetId() > 0) {
        e_loss_sign = -1.0;
      }

      double delta_energy = 0.0;
      double energy = sqrt( old_momentum*old_momentum + _muon_mass_sq );
      double momentum = old_momentum;
      int n_steps = materials.size();
      for (int i = 0; i < n_steps; i++) {  // In mm => times by 0.1;
        double SP = _geometry_helper->BetheBlochStoppingPower(momentum, materials.at(i).first);
        delta_energy = e_loss_sign*SP*materials.at(i).second*0.1*distance_factor;
        energy = energy + delta_energy;
        momentum = sqrt( energy*energy - _muon_mass_sq );
      }

      reduction_factor = momentum/old_momentum;
    }

    TMatrixD new_prop(5, 5);

    new_prop(0, 0) = 1.;
    new_prop(0, 1) = (sine/u);
    new_prop(0, 2) = 0.;
    new_prop(0, 3) = ((cosine-1.)/u);

    new_prop(1, 0) = 0.;
    new_prop(1, 1) = cosine*reduction_factor;
    new_prop(1, 2) = 0.;
    new_prop(1, 3) = -sine*reduction_factor;

    new_prop(2, 0) = 0.;
    new_prop(2, 1) = ((1.-cosine)/u);
    new_prop(2, 2) = 1.;
    new_prop(2, 3) = (sine/u);

    new_prop(3, 0) = 0.;
    new_prop(3, 1) = sine*reduction_factor;
    new_prop(3, 2) = 0.;
    new_prop(3, 3) = cosine*reduction_factor;

    new_prop(4, 0) = 0.;
    new_prop(4, 1) = 0.;
    new_prop(4, 2) = 0.;
    new_prop(4, 3) = 0.;
    new_prop(4, 4) = 1./reduction_factor;

    if (_correct_Pz) {
      if (start.GetId() < 0) {
        new_prop(0, 4) = delta_z*(old_px*cosine - old_py*sine);
        new_prop(1, 4) = -u*delta_z*(old_px*sine + old_py*cosine)*reduction_factor;
        new_prop(2, 4) = delta_z*(old_px*sine + old_py*cosine);
        new_prop(3, 4) = u*delta_z*(old_px*cosine - old_py*sine)*reduction_factor;
      } else {
        new_prop(0, 4) = delta_z*(old_px*cosine - old_py*sine);
        new_prop(1, 4) = -u*delta_z*(old_px*sine + old_py*cosine)*reduction_factor;
        new_prop(2, 4) = delta_z*(old_px*sine + old_py*cosine);
        new_prop(3, 4) = u*delta_z*(old_px*cosine - old_py*sine)*reduction_factor;
      }
    } else {
      new_prop(0, 4) = 0.0;
      new_prop(1, 4) = 0.0;
      new_prop(2, 4) = 0.0;
      new_prop(3, 4) = 0.0;
    }

//    new_prop.Print();

    return new_prop;
  }


  TMatrixD HelicalPropagator::CalculateProcessNoise(const Kalman::State& start,
                                                                        const Kalman::State& end) {
    static TMatrixD new_noise(5, 5);
    new_noise.Zero();

    if (_include_mcs) {
      // TODO : Correct this for new system
      Kalman::State temp_start(start);
      Kalman::State temp_end(start);

      SciFiMaterialsList materials;
      _geometry_helper->FillMaterialsList(start.GetId(), end.GetId(), materials);
      int n_steps = materials.size();

      for (int i = 0; i < n_steps; i++) {

        double width = materials.at(i).second;

        temp_end.SetPosition(temp_end.GetPosition() + width);

        TMatrixD F = CalculatePropagator(temp_start, temp_end);
        temp_end.SetVector(F * temp_start.GetVector());

        double L = materials.at(i).first->L(width);
        TMatrixD Q = BuildQ(temp_end, L, width);

        TMatrixD F_transposed(GetDimension(), GetDimension());
        F_transposed.Transpose(F);

        new_noise = F*new_noise*F_transposed + Q;
        temp_start = temp_end;
      }
    } else {
      // Pass...
    }
    return new_noise;
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

    double charge = kappa/fabs(kappa);
    double c      = CLHEP::c_light;
    double u      = charge*c*_Bz;
    double delta_theta = u*material_w*fabs(kappa);
    double sine   = sin(delta_theta);
    double cosine = cos(delta_theta);

    double dtheta_dpz = -delta_theta/pz;
    // ------------------------------------------------------------
    TMatrixD dalpha_dp(GetDimension(), 3);
    // dx, dpx
    dalpha_dp(0, 0) = sine/u;
    // dx, dpy
    dalpha_dp(0, 1) = (cosine-1.)/u;
    // dx, dpz
    dalpha_dp(0, 2) = px*dtheta_dpz*cosine/u - py*dtheta_dpz*sine/u;

    // dpx, dpx
    dalpha_dp(1, 0) = cosine;
    // dpx, dpy
    dalpha_dp(1, 1) = -sine;
    // dpx, dpz
    dalpha_dp(1, 2) = -px*dtheta_dpz*sine - py*dtheta_dpz*cosine;

    // dy, dpx
    dalpha_dp(2, 0) = (1.-cosine)/u;
    // dy, dpy
    dalpha_dp(2, 1) = sine/u;
    // dy, dpz
    dalpha_dp(2, 2) = py*dtheta_dpz*cosine/u + px*dtheta_dpz*sine/u;

    // dpy, dpx
    dalpha_dp(3, 0) = sine;
    // dpy, dpy
    dalpha_dp(3, 1) = cosine;
    // dpy, dpz
    dalpha_dp(3, 2) = -py*dtheta_dpz*sine + px*dtheta_dpz*cosine;

    // dkappa, dpx
    dalpha_dp(4, 0) = 0.;
    // dkappa, dpy
    dalpha_dp(4, 1) = 0.;
    // dkappa, dpz
    dalpha_dp(4, 2) = -charge/(pz*pz);

    TMatrixD dalpha_dp_transposed(3, GetDimension());
    dalpha_dp_transposed.Transpose(dalpha_dp);

    // ------------------------------------------------------------
    TMatrixD dalpha_dx(GetDimension(), 3);
    dalpha_dx.Zero();
    dalpha_dx(0, 0) = 1.;
    dalpha_dx(2, 1) = 1.;

    TMatrixD dalpha_dx_transposed(3, GetDimension());
    dalpha_dx_transposed.Transpose(dalpha_dx);

    // ------------------------------------------------------------
    TMatrixD P(3, 3);
    // Diagonal terms.
    P(0, 0) = 1.-px*px/p2;
    P(1, 1) = 1.-py*py/p2;
    P(2, 2) = 1.-pz*pz/p2;
    // Off diagonal, symmetric.
    P(0, 1) = px*py/p2;
    P(1, 0) = px*py/p2;
    P(0, 2) = px*pz/p2;
    P(2, 0) = px*pz/p2;
    P(1, 2) = py*pz/p2;
    P(2, 1) = py*pz/p2;


    TMatrixD term_1(GetDimension(), GetDimension());
    term_1 = dalpha_dp*P*dalpha_dp_transposed;
    term_1 = p2*term_1;

    TMatrixD term_2(GetDimension(), GetDimension());
    term_2 = dalpha_dp*P*dalpha_dx_transposed;
    term_2 = p*material_w*0.5*term_2;

    TMatrixD term_3(GetDimension(), GetDimension());
    term_3 = dalpha_dx*P*dalpha_dp_transposed;
    term_3 = p*material_w*0.5*term_3;

    TMatrixD term_4(GetDimension(), GetDimension());
    term_4 = dalpha_dx*P*dalpha_dx_transposed;
    term_4 = (1./3.)*material_w*material_w*term_4;

    TMatrixD Q(GetDimension(), GetDimension());
    Q = theta_mcs2*(term_1 + term_2 + term_3 + term_4);

    return Q;
  }
} // namespace MAUS

