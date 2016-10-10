
#ifndef recon_global_materialmodeldynamic_hh_
#define recon_global_materialmodeldynamic_hh_

#include <set>
#include <iostream>

#include "src/common_cpp/Recon/Global/MaterialModel.hh"

class G4Material;

namespace MAUS {
class GeometryNavigator;

class MaterialModelDynamic : public MaterialModel {
  public:
    MaterialModelDynamic() {}
    MaterialModelDynamic(double x, double y, double z);
    MaterialModelDynamic(const MaterialModelDynamic& mat);
    virtual ~MaterialModelDynamic() {}
    MaterialModelDynamic& operator=(const MaterialModelDynamic& mat);
    MaterialModelDynamic* Clone();

    void SetMaterial(double x, double y, double z);
    const G4Material* GetMaterial() const {return _material;}

  private:
};

}

#endif

