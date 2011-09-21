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

#include "src/common_cpp/Simulation/MAUSVisManager.hh"

#include <G4VRML1File.hh>
#include <G4VRML2File.hh>
#include <G4HepRepFile.hh>
#include <G4DAWNFILE.hh>

#ifdef G4VIS_USE_DAWN
#include <G4FukuiRenderer.hh>
#endif

#ifdef G4VIS_USE_OPACS
#include <G4Wo.hh>
#include <G4Xo.hh>
#endif

#ifdef G4VIS_USE_OPENGLX
#include <G4OpenGLImmediateX.hh>
#include <G4OpenGLStoredX.hh>
#endif

#ifdef G4VIS_USE_OPENGLWIN32
#include <G4OpenGLImmediateWin32.hh>
#include <G4OpenGLStoredWin32.hh>
#endif

#ifdef G4VIS_USE_OPENGLXM
#include <G4OpenGLImmediateXm.hh>
#include <G4OpenGLStoredXm.hh>
#endif

#ifdef G4VIS_USE_OIX
#include <G4OpenInventorX.hh>
#endif

#ifdef G4VIS_USE_OIWIN32
#include <G4OpenInventorWin32.hh>
#endif

#ifdef G4VIS_USE_VRML
#include <G4VRML1.hh>
#include <G4VRML2.hh>
#endif

#include <string>

#include "src/legacy/Interface/MICERun.hh"
#include "src/legacy/Interface/Squeak.hh"

namespace MAUS {

MAUSVisManager::MAUSVisManager() {
}

MAUSVisManager::~MAUSVisManager() {
}

void MAUSVisManager::RegisterGraphicsSystems() {
  RegisterGraphicsSystem(new G4HepRepFile);
  RegisterGraphicsSystem(new G4VRML1File);
  RegisterGraphicsSystem(new G4VRML2File);
  RegisterGraphicsSystem(new G4DAWNFILE);

#ifdef G4VIS_USE_DAWN
  RegisterGraphicsSystem(new G4FukuiRenderer);
#endif

#ifdef G4VIS_USE_OPACS
  RegisterGraphicsSystem(new G4Wo);
  RegisterGraphicsSystem(new G4Xo);
#endif

#ifdef G4VIS_USE_OPENGLX
  RegisterGraphicsSystem(new G4OpenGLImmediateX);
  RegisterGraphicsSystem(new G4OpenGLStoredX);
#endif

#ifdef G4VIS_USE_OPENGLWIN32
  RegisterGraphicsSystem(new G4OpenGLImmediateWin32);
  RegisterGraphicsSystem(new G4OpenGLStoredWin32);
#endif

#ifdef G4VIS_USE_OPENGLXM
  RegisterGraphicsSystem(new G4OpenGLImmediateXm);
  RegisterGraphicsSystem(new G4OpenGLStoredXm);
#endif

#ifdef G4VIS_USE_OIX
  RegisterGraphicsSystem(new G4OpenInventorX);
#endif

#ifdef G4VIS_USE_OIWIN32
  RegisterGraphicsSystem(new G4OpenInventorWin32);
#endif

#ifdef G4VIS_USE_VRML
  RegisterGraphicsSystem(new G4VRML1);
  RegisterGraphicsSystem(new G4VRML2);
#endif

  Squeak::mout(Squeak::debug)
      << "\nYou have successfully chosen to use the following graphics systems."
      << std::endl;
  PrintAvailableGraphicsSystems();
}

// needs error handling
void MAUSVisManager::SetupRun() {
  Json::Value& conf = *MICERun::getInstance()->jsonConfiguration;
  std::string viewer = JsonWrapper::GetProperty
           (conf, "visualisation_viewer", JsonWrapper::stringValue).asString();
  double theta = JsonWrapper::GetProperty
           (conf, "visualisation_theta", JsonWrapper::realValue).asDouble();
  double phi = JsonWrapper::GetProperty
           (conf, "visualisation_phi", JsonWrapper::realValue).asDouble();
  double zoom = JsonWrapper::GetProperty
           (conf, "visualisation_zoom", JsonWrapper::realValue).asDouble();
  int verbose = JsonWrapper::GetProperty
           (conf, "verbose_level", JsonWrapper::intValue).asInt();

  if (verbose > 0) {
      // doesnt seem to work - writes to std::cerr regardless... set
      // verbose_level > 4 (which quietens std::cerr) instead
      ApplyCommand("/vis/verbose 0");
  }
  ApplyCommand("/vis/open "+viewer);
  ApplyCommand("/vis/viewer/reset");
  ApplyCommand("/vis/viewer/zoomTo "+STLUtils::ToString(zoom));
  ApplyCommand("/vis/viewer/set/viewpointThetaPhi "+
                   STLUtils::ToString(theta)+" "+STLUtils::ToString(phi));
  ApplyCommand("/vis/drawVolume");
  ApplyCommand("/tracking/storeTrajectory 1");
  ApplyCommand("/vis/scene/add/trajectories");
}

void MAUSVisManager::TearDownRun() {
  ApplyCommand("/vis/viewer/flush");
  // make eps here?
}
}
