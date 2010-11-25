// @(#) $Id: MICEDetectorConstruction.hh,v 1.28 2009-08-25 13:32:08 rogers Exp $  $Name:  $
//
//  Super-FOFO channel based on Neutrino Factory Feasibility Study II
//  for the International Muon Ionization Cooling Experiment
//
//  Based on FSII code by Paul Lebrun
//
//  Yagmur Torun

#ifndef MICEDETECTORCONSTRUCTION_H
#define MICEDETECTORCONSTRUCTION_H 1

#include <string>
#include <vector>
#include <json/json.h>
#include "G4VUserDetectorConstruction.hh"
#include "G4UniformMagField.hh"

#include "DetModel/MAUSSD.hh"  

#include "Interface/MICERun.hh"

#include "BeamTools/BTFieldConstructor.hh"
#include "EngModel/MiceMagneticField.hh"
#include "EngModel/MiceElectroMagneticField.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class G4VSolid;

class MICEEvent;
class TofSD;

class MICEDetectorConstruction : public G4VUserDetectorConstruction
{

public:

  MICEDetectorConstruction( MICERun&, MICEEvent* );

  ~MICEDetectorConstruction();

  G4VPhysicalVolume* Construct();

  //Set the magnetic fields from BeamTools using MiceModule
  void    setBTMagneticField(MiceModule* rootModule);

  std::vector<Json::Value> GetSDHits(int i);

  int GetSDSize() { return _SDs.size(); }

private:

  std::vector<MAUSSD*> _SDs; // todo: add get/set 
  
  void		addDaughter( MiceModule*, G4VPhysicalVolume* );


  void		setUserLimits( G4LogicalVolume*, MiceModule* );

  void		setMagneticField( G4LogicalVolume*, MiceModule* );

  //Set G4 Stepping Accuracy parameters
  void		setSteppingAccuracy();
  //Set G4 Stepping Algorithm
  void		setSteppingAlgorithm();
  //Set up a special virtual for phasing RF cavities if mod is an RF cavity
  void SetPhasingVolume(MiceModule * cavityModule, G4VPhysicalVolume* cavityVolume);
  //Build a Q35 using Q35.hh methods
  G4LogicalVolume* BuildQ35(MiceModule * mod);

  MICEEvent*			_event;

  MiceModule*			_model;
  MiceMaterials*		_materials;
  G4LogicalVolume*		MICEExpHallLog;
  BTFieldConstructor *		_btField;
  MiceMagneticField *		_miceMagneticField;
  MiceElectroMagneticField *	_miceElectroMagneticField;
  MICERun&                      _simRun;
  bool _hasBTFields;

  static const double		_pillBoxSpecialVirtualLength;
  G4UniformMagField* 		magField;
  TofSD*			_tofSD;
  bool				_checkVolumes;
};

#endif

