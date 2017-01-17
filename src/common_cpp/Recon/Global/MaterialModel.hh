/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

#ifndef recon_global_materialmodel_hh_
#define recon_global_materialmodel_hh_

#include <string>
#include <set>
#include <iostream>

class G4Material;

namespace MAUS {
class GeometryNavigator;

/** @class MaterialModel provides mean process model for a G4Material
 *
 *  MaterialModel calculates the mean energy loss, scatter and straggle for a
 *  given G4Material.
 *
 *  MaterialModel class holds sets of materials (stored by string name) that
 *  are explicitly enabled or disabled. Materials that are disabled will be
 *  ignored. The ignoring logic is handled by the children. By default all
 *  materials are disabled.
 */
class MaterialModel {
  public:
    /** Constructor (does nothing)
     */
    MaterialModel() {}
    /** Copy constructor not implemented */
    MaterialModel& operator=(const MaterialModel& mat);
    /** Destructor does nothing; note _material is a borrowed pointer */
    virtual ~MaterialModel() {}
    /** Inheritable copy constructor */
    virtual MaterialModel* Clone() = 0;

    /** Set the material to be modelled
     *
     *  @param material: the material to be modelled; it is a borrowed reference
     *                   (Geant4 usually will own this memory)
     */
    virtual void SetMaterial(const G4Material* material);

    /** Calculate the energy loss using Bethe Bloch formula
     *
     *  @param energy: energy of the incident particle
     *  @param mass: mass of the incident particle
     *  @param charge: charge of the incident particle
     *
     *  Takes the sum over elements in the target material; energy loss is given
     *  by the sum of energy loss due to each element according to Bethe Bloch.
     */
    virtual double dEdx(double energy, double mass, double charge);

    /** Calculate the energy growth due to curvature of Bethe Bloch
     *
     *  @param energy: energy of the incident particle
     *  @param mass: mass of the incident particle
     *  @param charge: charge of the incident particle
     *
     *  Performs numerical derivative of the Bethe Bloch curve by doing
     *  d2E/dEdz = [dE/dz(E+dE) - dE/dz(E-dE)]/2/dE with dE given by the
     *  d2EdxdE_delta_const.
     */
    virtual double d2EdxdE(double energy, double mass, double charge);

    /** Calculate the mean scattering angle squared per unit length.
     *
     *  @param energy: energy of the incident particle
     *  @param mass: mass of the incident particle
     *  @param charge: charge of the incident particle
     *
     *  Use theta_0 ~ const / beta / p sqrt(x/x0) so dtheta^2/dz ~ const / beta / p / x0
     */
    virtual double dtheta2dx(double energy, double mass, double charge);

    /** Calculate the mean energy straggling
     *
     *  @param energy: energy of the incident particle
     *  @param mass: mass of the incident particle
     *  @param charge: charge of the incident particle
     *
     *  Use estrag2 = const * gamma^2*(1-beta^2/2.)
     */
    virtual double estrag2(double energy, double mass, double charge);

    /** Set the material to be one corresponding to a given position x, y, z
     */
    virtual void SetMaterial(double x, double y, double z) = 0;

    /** Get the stored material
     *
     *  @returns the stored material; note this is a borrowed pointer. Geant4
     *           usually owns this memory.
     */
    virtual const G4Material* GetMaterial() const {return _material;}

    /** Return true if the material is enabled
     *
     *  @param material_name: the name of the material to be checked
     */
    static bool IsEnabledMaterial(std::string material_name);

    /** Enable a given material. Default is disabled.
     *
     *  @param material_name: the name of the material to be enabled
     */
    static void EnableMaterial(std::string material_name);

    /** Disable a given material.
     *
     *  @param material_name: the name of the material to be disabled
     */
    static void DisableMaterial(std::string material_name);

    /** Disable all materials.
     */
    static void DisableAllMaterials();

    /** Print a list of all enabled materials
     *
     *  @param out: prints the materials to the ostream out
     */
    static std::ostream& PrintMaterials(std::ostream& out);

  protected:
    // _material is a borrowed reference; data is owned by G4MaterialTable
    const G4Material* _material = NULL;
    GeometryNavigator* _navigator = NULL;
    double _n_e = 0.;
    double _I = 1.;
    double _x_0 = 0.;
    double _x_1 = 0.;
    double _C = 0.;
    double _a = 0.;
    double _k = 0.;
    double _rad_len_ratio = 0.;
    double _density = 0.;
    double _z_over_a = 0.;

    static std::set<std::string> _enabled_materials;

    static constexpr double _d2EdxdE_delta_const = 0.1;
    // 2.1047291091867513e-13; // 4 pi (r_e m_e c^2)^2
    static constexpr double _estrag_const = 0.157;
    static constexpr double _dedx_constant = 0.307075;
    static constexpr double _m_e = 0.510998928;
    static constexpr double c_l = 299.792458; // mm*ns^{-1}
};
}

#endif

