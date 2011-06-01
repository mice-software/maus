#include "MICEStackingAction.hh"
 

MICEStackingAction::MICEStackingAction()
{ 
}

MICEStackingAction::~MICEStackingAction()
{  }

G4ClassificationOfNewTrack 
MICEStackingAction::ClassifyNewTrack(const G4Track * aTrack)
{
    return fWaiting;
}

void MICEStackingAction::NewStage()
{
}
    
void MICEStackingAction::PrepareNewEvent()
{ 
}


