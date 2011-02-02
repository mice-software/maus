#include "MAUSEventAction.h"

namespace MAUS {

void MAUSEventAction::BeginOfEventAction(const G4Event* currentEvent) {
  MAUSSteppingAction* stepAct = MAUSSteppingAction::GetSteppingAction();
  if (!stepAct)
    return;

  stepAct->_track = Json::Value();
}

}  //  ends MAUS namespace
