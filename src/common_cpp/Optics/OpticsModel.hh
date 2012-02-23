#ifndef SRC_COMMON_CPP_OPTICS_OPTICS_MODEL_HH
#define SRC_COMMON_CPP_OPTICS_OPTICS_MODEL_HH


namespace MAUS {

class TransferMap;

class OpticsModel
{
 public:
  /** FIXME: I don't think this is right. I believe the least-squares alorithms
   *         determine the transfer matrix from particle tracking data
   *         without knowing what the fields and materials are.
   */
  virtual Build(BTField * electromagnetic_field,
                std::vector<Material> * materials) = 0;
  TransferMap const * transfer_map() {return transfer_map_;}
 protected:
  TransferMap * transfer_map_;
};

}

#endif
