#include <math.h>
#include "Geant4/G4Material.hh"

#include "src/common_cpp/Recon/Global/MaterialModel.hh"

namespace MAUS {

void MaterialModel::SetMaterial(const G4Material* material) {
    _material = material;
    _n_e = _material->GetElectronDensity();
    _I = _material->GetIonisation()->GetMeanExcitationEnergy();
    _x_0 = _material->GetIonisation()->GetX0density();
    _x_1 = _material->GetIonisation()->GetX1density();
    _C = _material->GetIonisation()->GetCdensity();
    _a = _material->GetIonisation()->GetAdensity();
    _k = _material->GetIonisation()->GetMdensity();
    _rad_len_ratio = 13.6*13.6/_material->GetRadlen();
    _density = _material->GetDensity()/g*cm3;
}

double MaterialModel::dEdx(double E, double m, double charge) {
    // bethe bloch formula, see PDG particle data book
    // assume density effect is 0
    double beta2 = (E*E-m*m)/E/E;
    double beta = sqrt(beta2);
    double gamma = E/m;
    double bg = beta*gamma;
    double bg2 = bg*bg;
    double mRatio = _m_e/m;
    double T_max = 2.0*_m_e*bg2/(1.0 + 2.0*gamma*mRatio + mRatio*mRatio);

    double delta = 0.;
    double logterm = std::log(2.0*_m_e*bg2*T_max/(_I*_I));

    double dEdx = -_dedx_constant*_density*charge*charge/beta2*
                                          (logterm/2. - beta2 - delta)/cm;
    std::cerr << "const " << _dedx_constant << " dens " << _density 
              << " charge " << charge << " beta2 " << beta2
              << " log " << logterm << " cm " << cm << " dedx " << dEdx << std::endl;
    return dEdx;
}

double MaterialModel::dtheta2dx(double E, double m, double charge) {
    // moliere scattering formula, see PDG particle data book
    // assume log term is 0
    double one_over_beta_c_p = E/(E*E+m*m)*c_l;
    double theta_0_sq = _rad_len_ratio*one_over_beta_c_p*one_over_beta_c_p;
    return theta_0_sq;
}

double MaterialModel::estrag2(double E, double m, double charge) {
    // NIM A 532, Neuffer, "Introduction to Ionisation Cooling", 2004
    // Scraped from Ann. Rev. Nucl. Sci. 13, Fano, 1963
    // Note that I ignore the units in the NIM paper as I think it is
    // wrong; I think the correct units are MeV^2/cm, not MeV^2 cm^2/g
    // (Rogers, 2016)
    double gamma2 = E*E/m/m;
    double beta2 = (E*E-m*m)/E/E;
    double estrag2 = _estrag_const*gamma2*(1-beta2/2.)/cm; // missing factor Z/A

    return estrag2;
}

}

