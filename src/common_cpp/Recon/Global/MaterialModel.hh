
#ifndef recon_global_materialmodel_hh_
#define recon_global_materialmodel_hh_

class G4Material;

namespace MAUS {

class MaterialModel {
  public:
    MaterialModel() {;}
    MaterialModel(const G4Material* material);
    MaterialModel(const MaterialModel& mat);
    double dEdx(double energy, double mass, double charge);
    double d2EdxdE(double energy, double mass, double charge);
    double dtheta2dx(double energy, double mass, double charge);
    double estrag2(double energy, double mass, double charge);

    void SetMaterial(const G4Material* material);
    const G4Material* GetMaterial() const {return _material;}

  private:
    // _material is a borrowed reference; data is owned by G4MaterialTable
    const G4Material* _material = NULL;
    double _n_e = 0.;
    double _I = 0.;
    double _x_0 = 0.;
    double _x_1 = 0.;
    double _C = 0.;
    double _a = 0.;
    double _k = 0.;
    double _rad_len_ratio = 0.;
    double _density = 0.;
    double _z_over_a = 0.;
    
    static constexpr double _d2EdxdE_delta_const = 0.1; //2.1047291091867513e-13; // 4 pi (r_e m_e c^2)^2
    static constexpr double _estrag_const = 0.157; //2.1047291091867513e-13; // 4 pi (r_e m_e c^2)^2
    static constexpr double _dedx_constant = 0.307075;
    static constexpr double _m_e = 0.510998928;
    static constexpr double c_l = 299.792458; // mm*ns^{-1}
};

}

#endif

