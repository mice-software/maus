#include "Simulation/MAUSGeant4Manager.hh"

#include "Simulation/MICEPhysicsList.hh"

namespace MAUS {

MAUSGeant4Manager* MAUSGeant4Manager::GetInstance() {
  static MAUSGeant4Manager* _instance = new MAUSGeant4Manager();
  return _instance;
}

MAUSGeant4Manager::MAUSGeant4Manager() : _storeTracks(false) {
    _runManager = new G4RunManager;
    _detector   = new MICEDetectorConstruction(*MICERun::getInstance());
    _runManager->SetUserInitialization(_detector);

    _physList = MICEPhysicsList::GetMICEPhysicsList();
    _runManager->SetUserInitialization(_physList);

    _primary  = new MAUSPrimaryGeneratorAction;
    _stepAct  = new MAUSSteppingAction;
    _trackAct = new MAUSTrackingAction;
    _runManager->SetUserAction(_primary);
    _runManager->SetUserAction(_trackAct);
    _runManager->SetUserAction(_stepAct);
    _runManager->SetUserAction(new MICERunAction);

    _runManager->Initialize();

    G4UImanager* UI = G4UImanager::GetUIpointer();

    if (_storeTracks) {
      UI->ApplyCommand("/tracking/storeTrajectory 1");
    } else {
      UI->ApplyCommand("/tracking/storeTrajectory 0");
    }
}

MAUSGeant4Manager::~MAUSGeant4Manager() {
    delete _runManager;
}

} // namespace MAUS

