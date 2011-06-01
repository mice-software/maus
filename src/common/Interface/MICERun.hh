// MAUS WARNING: THIS IS LEGACY CODE.
// Declaration of class MICERun
// Class to encapsulate static classes that are required for a single processing run

#ifndef MICE_RUN_HH
#define MICE_RUN_HH

// Standard C++ includes
#include <vector>
#include <map>

#include <json/json.h>

#include "Interface/dataCards.hh"

// Forward declarations
class MiceModule;
class MiceMaterials;
class VlpcCable;
class VlpcCalib;
class TofCable;
class TofCalib;
class TrajectoryManager;
class BTFieldConstructor;

class MICERun
{
public :

  static MICERun* getInstance()    {if(instance == NULL) instance=new MICERun(1); return instance;}
  static void     deleteInstance() {if(instance != NULL) delete instance; instance = NULL;}

  MiceModule*         miceModule;
  MiceMaterials*      miceMaterials;
  VlpcCable*          vlpcCable;
  TofCable*           tofCable;
  VlpcCalib*          vlpcCalib;
  TofCalib*           tofCalib;
  dataCards*          DataCards;
  TrajectoryManager*  trajectoryManager;
  BTFieldConstructor* btFieldConstructor;
  Json::Value*        jsonConfiguration;

  MICERun()
  {
    std::cerr << "WARNING - using deprecated method in MICERun. Please use static initialisation MICERun::getInstance() instead" << std::endl;
    miceModule = NULL;
    miceMaterials = NULL;
    vlpcCable = NULL;
    tofCable = NULL;
    tofCalib = NULL;
    vlpcCalib = NULL;
    DataCards = NULL;
    trajectoryManager = NULL;
    btFieldConstructor = NULL;
  };


  ~MICERun() {instance = NULL;}

private :
  MICERun(const MICERun &);
  MICERun(int dummy) //private initialisation without warning ...
  {
    miceModule = NULL;
    miceMaterials = NULL;
    vlpcCable = NULL;
    tofCable = NULL;
    tofCalib = NULL;
    vlpcCalib = NULL;
    DataCards = NULL;
    trajectoryManager = NULL;
    btFieldConstructor = NULL;
  };


  static MICERun*      instance;

};

#endif
