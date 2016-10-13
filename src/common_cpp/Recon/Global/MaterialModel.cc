#include <math.h>
#include "Geant4/G4Material.hh"

#include "src/common_cpp/Simulation/GeometryNavigator.hh"
#include "src/legacy/Interface/Squeak.hh"

#include "src/common_cpp/Recon/Global/MaterialModel.hh"

namespace MAUS {

std::set<std::string> MaterialModel::_enabled_materials;
std::set<std::string> MaterialModel::_disabled_materials;


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
    if (_material == NULL) {
        return 0.;
    }
    double beta2 = (E*E-m*m)/E/E; // beta = p^2/E^2
    double gamma = E/m;
    double bg2 = beta2*gamma*gamma;
    double mRatio = _m_e/m;
    double T_max = 2.0*_m_e*bg2/(1.0 + 2.0*gamma*mRatio + mRatio*mRatio);

    double coefficient = std::log(2.0*_m_e*bg2*T_max/(_I*_I)) - beta2;
    coefficient *= 0.307075*charge*charge/2/beta2*_density/cm; // 0.307075 is cm^2 per mol; _density is g/cm^3

    double dEdx = 0.;
    //std::cerr << "dEdX " << _material << std::endl;
    for (size_t i = 0; i < _material->GetNumberOfElements(); ++i) {
        //std::cerr << "element " << i << std::flush;
        double frac = _material->GetFractionVector()[i];
        //std::cerr << " fraction " << frac << std::flush;
        double a_el = _material->GetElement(i)->GetA()/(g/mole);
        //std::cerr << " a " << a_el << std::flush;
        double z_el = _material->GetElement(i)->GetZ();
        //std::cerr << " z " << z_el << std::endl;
        dEdx += frac*z_el/a_el*coefficient;
    }
    return -dEdx;
}

double MaterialModel::d2EdxdE(double E, double m, double charge) {
    double delta_energy_pos = dEdx(E+_d2EdxdE_delta_const, m, charge);
    double delta_energy_neg = dEdx(E-_d2EdxdE_delta_const, m, charge);
    double deriv = (delta_energy_pos-delta_energy_neg)/2./_d2EdxdE_delta_const;
    return deriv;
}

double MaterialModel::dtheta2dx(double E, double m, double charge) {
    // moliere scattering formula, see PDG particle data book
    // assume log term is 0
    double one_over_beta_c_p = E/(E*E-m*m); // E/p^2
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



bool MaterialModel::IsEnabledMaterial(std::string material_name) {
    bool found = _enabled_materials.find(material_name) !=
                                                      _enabled_materials.end();
    if (!found && material_name.size() > 2 && material_name.substr(0, 3) == std::string("G4_")) {
        material_name = material_name.substr(3);
    }
    found = _enabled_materials.find(material_name) != _enabled_materials.end();

    return found;
}

void MaterialModel::EnableMaterial(std::string material_name) {
    _enabled_materials.insert(material_name);
}

void MaterialModel::DisableAllMaterials() {
    _enabled_materials = std::set<std::string>();
}

void MaterialModel::DisableMaterial(std::string material_name) {
    std::set<std::string>::iterator it = _enabled_materials.find(material_name);
    if (it != _enabled_materials.end())
        _enabled_materials.erase(it);
}

std::ostream& MaterialModel::PrintMaterials(std::ostream& out) {
    out << "Enabled materials:\n";
    for (std::set<std::string>::iterator it = _enabled_materials.begin();
         it != _enabled_materials.end(); ++it) {
        out << "    " << *it << "\n";
    }
    out << "Disabled materials:\n";
    for (std::set<std::string>::iterator it = _disabled_materials.begin();
         it != _disabled_materials.end(); ++it) {
        out << "    " << *it << std::endl;
    }
    return out;
}

} // namespace MAUS

