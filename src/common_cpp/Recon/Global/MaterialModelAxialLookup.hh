
#ifndef recon_global_materialmodelaxiallookup_hh_
#define recon_global_materialmodelaxiallookup_hh_

#include <vector>

#include "src/common_cpp/Recon/Global/MaterialModel.hh"


class G4Material;

namespace MAUS {
class GeometryNavigator;

/** Material model that builds a lookup table to determine materials at a given
 *  z position; assumes cylinders
 */

class MaterialModelAxialLookup : public MaterialModel {
  public:
    MaterialModelAxialLookup() {}
    MaterialModelAxialLookup(const G4Material* material);
    MaterialModelAxialLookup(double x, double y, double z);
    MaterialModelAxialLookup(const MaterialModelAxialLookup& mat);
    virtual ~MaterialModelAxialLookup() {}
    MaterialModelAxialLookup& operator=(const MaterialModelAxialLookup& mat);
    MaterialModelAxialLookup* Clone();

    void SetMaterial(double x, double y, double z);

    static void BuildLookupTable(double z_start, double z_end);
    static void GetBounds(double z_pos, double& lower_bound, double& upper_bound);
    static void PrintLookupTable(std::ostream& out);
    static const std::vector<std::pair<double, G4Material*> >* GetLookupTable() {return &_lookup;}

  private:

    static constexpr double _z_tolerance = 1e-3;

    static std::vector<std::pair<double, G4Material*> > _lookup; // references start position of the volume and material
};

}

#endif

