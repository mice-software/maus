
#include "src/common_cpp/Recon/Kalman/MAUSPropagators.hh"


namespace MAUS {

////////////////////////////////////////////////////////////////////////////////
  // USEFUL FUCNTIONS
////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////
  // STRAIGHT
////////////////////////////////////////////////////////////////////////////////

  StraightPropagator::StraightPropagator( SciFiGeometryHelper* geo) :
    Kalman::Propagator_base(4),
    _geometry_helper(geo),
    _include_mcs(true) {
  }


  TMatrixD StraightPropagator::CalculatePropagator(const State& start, const State& end) {
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


  TMatrixD StraightPropagator::CalculateProcessNoise(const State& start, const State& end) {
    static TMatrixD new_noise(GetDimension(), GetDimension());

    if ( _include_mcs ) {


    } else {
      new_noise.Zero();
      return new_noise;
    }
  }


  TMatrixD StraightPropagator::BuildQ(const State& state, double radLen, double width) {
    double deltaZ_squared = deltaZ*deltaZ;

    double mx    = a(1, 0);
    double my    = a(3, 0);
    double p   = GetTrackMomentum();
    double p2  = p*p;

    double muon_mass = Recon::Constants::MuonMass;
    double muon_mass2 = muon_mass*muon_mass;
    double E = TMath::Sqrt(muon_mass2+p2);
    double beta = p/E;

    double C = HighlandFormula(L0, beta, p);

    double C2 = C*C;

    double c_mx_mx = C2 * (1. + mx*mx) *
                          (1.+ mx*mx + my*my);

    double c_my_my = C2 * (1. + my*my) *
                          (1.+ mx*mx + my*my);

    double c_mx_my = C2 * mx*my * (1.+ mx*mx + my*my);

    TMatrixD Q(_n_parameters, _n_parameters);
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

  HelicalPropagator::HelicalPropagator(SciFiGeometryHelper* helper, double magnetic_field) :
    Kalman::Propagator_base(5),
    _Bz(magnetic_field),
    _geometry_helper(helper),
    _subtract_eloss(true),
    _include_mcs(true) {
  }


  void HelicalPropagator::Propagate(const State& start, State& end) {
    TMatrixD old_vec  = start.GetVector();
    double old_x      = old_vec(0, 0);
    double old_px     = old_vec(1, 0);
    double old_y      = old_vec(2, 0);
    double old_py     = old_vec(3, 0);
    double old_kappa  = old_vec(4, 0);
    double charge = old_kappa/fabs(old_kappa);

    double c      = CLHEP::c_light;
    double u      = charge*c*_Bz;
    double delta_theta = u*delta_z*fabs(old_kappa);
    double sine   = sin(delta_theta);
    double cosine = cos(delta_theta);

    // Calculate the new track parameters.
    double new_x  = old_x + old_px*sine/u
                    - old_py*(1.-cosine)/u;

    double new_px = old_px*cosine - old_py*sine;

    double new_y  = old_y + (old_py*sine)/u
                    + old_px*(1.-cosine)/u;

    double new_py = old_py*cosine + old_px*sine;

    double new_kapp = old_kappa;

    if ( _subtract_eloss ) {
      SciFiMaterialsList materials;
      _geometry_helper->FillMaterialsList(old_state.GetId(), new_state.GetId(), materials);

      // Reduce/increase momentum vector accordingly.
      double e_loss_sign = 1.;
      if ( new_site->id() > 0 ) {
        e_loss_sign = -1.;
      }

      double delta_p = 0;
      int n_steps = materials.size();
      double momentum = old_momentum;
      for ( int i = 0; i < n_steps; i++ ) {  // In mm => times by 0.1;
        delta_p  += _geometry_helper->BetheBlochStoppingPower(momentum, materials.at(i).first)*materials.at(i).second*0.1;
        momentum += e_loss_sign*delta_p;
      }

      double reduction_factor = momentum/old_momentum;
      TMatrixD a = new_site->a(KalmanState::Projected);
      new_px *= reduction_factor
      new_py *= reduction_factor
      new_kappa /= reduction_factor
    }

    TMatrixD end_vec(5, 1);
    end_vec(0, 0) = new_x;
    end_vec(1, 0) = new_px;
    end_vec(2, 0) = new_y;
    end_vec(3, 0) = new_py;
    end_vec(4, 0) = new_kappa;
    end.SetVector(end_vec);

    PropagatorMatrix() = CalculatePropagator(start, end);
    ProcessNoise() = CalculateProcessNoise(start, end);

    TMatrixD propT(5, 5); propT.Transpose(PropagatorMatrix());
    TMatrixD end_cov = PropagatorMatrix * end.GetCovariance() * proptT + ProcessNoise();
    end.SetCovariance(end_cov);
  }

  TMatrixD HelicalPropagator::CalculatePropagator(const State& start, const State& end) {
    TMatrixD old_vec  = start.GetVector();
    double old_x      = old_vec(0, 0);
    double old_px     = old_vec(1, 0);
    double old_y      = old_vec(2, 0);
    double old_py     = old_vec(3, 0);
    double old_kappa  = old_vec(4, 0);

    double charge = old_kappa/fabs(old_kappa);
    double c      = CLHEP::c_light;
    double u      = charge*c*_Bz;
    double delta_theta = u*delta_z*fabs(old_kappa);
    double sine   = sin(delta_theta);
    double cosine = cos(delta_theta);

    TMatrixD new_prop(5, 5);

    new_prop(0, 0) = 1.;
    new_prop(0, 1) = sine/u;
    new_prop(0, 2) = 0.;
    new_prop(0, 3) = (cosine-1.)/u;
    new_prop(0, 4) = delta_z*(old_px*cosine - old_py*sine);

    new_prop(1, 0) = 0.;
    new_prop(1, 1) = cosine;
    new_prop(1, 2) = 0.;
    new_prop(1, 3) = -sine;
    new_prop(1, 4) = -u*delta_z*(old_px*sine + old_py*cosine);

    new_prop(2, 0) = 0.;
    new_prop(2, 1) = (1.-cosine)/u;
    new_prop(2, 2) = 1.;
    new_prop(2, 3) = sine/u;
    new_prop(2, 4) = delta_z*(old_px*sine + old_py*cosine);

    new_prop(3, 0) = 0.;
    new_prop(3, 1) = sine;
    new_prop(3, 2) = 0.;
    new_prop(3, 3) = cosine;
    new_prop(3, 4) = u*delta_z*(old_px*cosine - old_py*sine);

    new_prop(4, 0) = 0.;
    new_prop(4, 1) = 0.;
    new_prop(4, 2) = 0.;
    new_prop(4, 3) = 0.;
    new_prop(4, 4) = 1.;

    return new_prop;
  }


  TMatrixD HelicalPropagator::CalculateProcessNoise(const State& start, const State& end) {
    static TMatrixD new_noise(GetDimension(), GetDimension());

    if ( _include_mcs ) {
      // TODO : Correct this for new system
      SciFiMaterialsList materials;
      _geometry_helper->FillMaterialsList(start.GetId(), end.GetId(), materials);

      int n_steps = materials.size();

      TMatrixD C = C_old;
      double delta_z = 0;
      for ( int i = 0; i < n_steps; i++ ) {
        delta_z += widths.at(i);

        TMatrixD F;
        TMatrixD a = GetIntermediateState(old_site, delta_z, F);

        double L = GetL(materials.at(i), widths.at(i));
        TMatrixD Q = BuildQ(a, L, widths.at(i));

        TMatrixD F_transposed(_n_parameters, _n_parameters);
        F_transposed.Transpose(F);

        C = F*C*F_transposed + Q;
      }
      return C;

    } else {
      new_noise.Zero();
      return new_noise;
    }
  }


  TMatrixD KalmanHelicalPropagator::BuildQ(const State& state, double L, double material_w) {
    TMatrixD vec = state.GetVector()
    double px    = vec(1, 0);
    double py    = vec(3, 0);
    double kappa = vec(4, 0);
    double pz    = fabs(1./kappa);
    double p     = sqrt(px*px+py*py+pz*pz);
    double p2    = p*p;

    double muon_mass  = Recon::Constants::MuonMass;
    double muon_mass2 = muon_mass*muon_mass;
    double E    = TMath::Sqrt(muon_mass2+p2);
    double beta = p/E;

    double theta_mcs  = HighlandFormula(L, beta, p);
    double theta_mcs2 = theta_mcs*theta_mcs;

    double charge = kappa/fabs(kappa);
    double c      = CLHEP::c_light;
    double u      = charge*c*_Bz;
    double delta_theta = u*material_w*fabs(kappa);
    double sine   = sin(delta_theta);
    double cosine = cos(delta_theta);

    double dtheta_dpz = -delta_theta/pz;
    // ------------------------------------------------------------
    TMatrixD dalpha_dp(_n_parameters, 3);
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

    TMatrixD dalpha_dp_transposed(3, _n_parameters);
    dalpha_dp_transposed.Transpose(dalpha_dp);

    // ------------------------------------------------------------
    TMatrixD dalpha_dx(_n_parameters, 3);
    dalpha_dx.Zero();
    dalpha_dx(0, 0) = 1.;
    dalpha_dx(2, 1) = 1.;

    TMatrixD dalpha_dx_transposed(3, _n_parameters);
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


    TMatrixD term_1(_n_parameters, _n_parameters);
    term_1 = dalpha_dp*P*dalpha_dp_transposed;
    term_1 = p2*term_1;

    TMatrixD term_2(_n_parameters, _n_parameters);
    term_2 = dalpha_dp*P*dalpha_dx_transposed;
    term_2 = p*material_w*0.5*term_2;

    TMatrixD term_3(_n_parameters, _n_parameters);
    term_3 = dalpha_dx*P*dalpha_dp_transposed;
    term_3 = p*material_w*0.5*term_3;

    TMatrixD term_4(_n_parameters, _n_parameters);
    term_4 = dalpha_dx*P*dalpha_dx_transposed;
    term_4 = (1./3.)*material_w*material_w*term_4;

    TMatrixD Q(_n_parameters, _n_parameters);
    Q = theta_mcs2*(term_1 + term_2 + term_3 + term_4);

    return Q;
  }


} // namespace MAUS

