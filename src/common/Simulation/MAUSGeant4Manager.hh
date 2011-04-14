// Copyright 2011 Chris Rogers
//
// This file is a part of MAUS
//
// MAUS is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MAUS is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MAUS in the doc folder.  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef _SRC_COMMON_MAUSGEANT4MANAGER_HH_
#define _SRC_COMMON_MAUSGEANT4MANAGER_HH_

#include <G4RunManager.hh>
#include <G4SDManager.hh>

// should all be forward declarations?
#include "Simulation/MICERunAction.hh"
#include "Simulation/MICESteppingAction.hh"
#include "Simulation/FillMaterials.hh"
#include "Simulation/MICEDetectorConstruction.hh"
#include "Simulation/MAUSPrimaryGeneratorAction.hh"
#include "Simulation/MAUSSteppingAction.hh"
#include "Simulation/MAUSTrackingAction.hh"

class MICEPhysicsList;

namespace MAUS {

class MAUSGeant4Manager {
public:
    static MAUSGeant4Manager* GetInstance();

    G4RunManager* GetRunManager() const {return _runManager;}
    MAUSSteppingAction* GetStepping() const {return _stepAct;}
    MAUSTrackingAction* GetTracking() const {return _trackAct;}
    MAUSPrimaryGeneratorAction* GetPrimaryGenerator() const {return _primary;}
    MICEDetectorConstruction* GetGeometry() const {return _detector;}

    void SetStoreTracks(bool willStoreTracks) {_storeTracks = willStoreTracks;}
    bool GetStoreTracks() {return _storeTracks;}

private:
    MAUSGeant4Manager();
    ~MAUSGeant4Manager(); 

    G4RunManager* _runManager;
    MICEPhysicsList* _physList;
    MAUSPrimaryGeneratorAction* _primary;
    MAUSSteppingAction*          _stepAct;
    MAUSTrackingAction*          _trackAct;
    MICEDetectorConstruction*    _detector;
    bool                         _storeTracks;
};

} // namespace MAUS

#endif

