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

#include <string>

#include "Geant4/G4VRML1File.hh"
#include "Geant4/G4VRML2File.hh"
#include "Geant4/G4HepRepFile.hh"
#include "Geant4/G4DAWNFILE.hh"
#include "Geant4/G4TrajectoryDrawByParticleID.hh"

#ifdef G4VIS_USE_DAWN
#include "Geant4/G4FukuiRenderer.hh"
#endif

#ifdef G4VIS_USE_OPACS
#include "Geant4/G4Wo.hh"
#include "Geant4/G4Xo.hh"
#endif

#ifdef G4VIS_USE_OPENGLX
#include "Geant4/G4OpenGLImmediateX.hh"
#include "Geant4/G4OpenGLStoredX.hh"
#endif

#ifdef G4VIS_USE_OPENGLWIN32
#include "Geant4/G4OpenGLImmediateWin32.hh"
#include "Geant4/G4OpenGLStoredWin32.hh"
#endif

#ifdef G4VIS_USE_OPENGLXM
#include "Geant4/G4OpenGLImmediateXm.hh"
#include "Geant4/G4OpenGLStoredXm.hh"
#endif

#ifdef G4VIS_USE_OIX
#include "Geant4/G4OpenInventorX.hh"
#endif

#ifdef G4VIS_USE_OIWIN32
#include "Geant4/G4OpenInventorWin32.hh"
#endif

#ifdef G4VIS_USE_VRML
#include "Geant4/G4VRML1.hh"
#include "Geant4/G4VRML2.hh"
#endif

#include "src/legacy/Interface/MICERun.hh"
#include "Utils/Squeak.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Simulation/MAUSVisManager.hh"


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

G4ThreeVector GetColour(std::string colour_name) {
  Json::Value& conf = *MICERun::getInstance()->jsonConfiguration;
  Json::Value json_rgb = JsonWrapper::GetProperty(conf, colour_name, JsonWrapper::objectValue);
  G4ThreeVector rgb;
  rgb[0] = JsonWrapper::GetProperty(json_rgb, "red", JsonWrapper::realValue).asDouble();
  rgb[1] = JsonWrapper::GetProperty(json_rgb, "green", JsonWrapper::realValue).asDouble();
  rgb[2] = JsonWrapper::GetProperty(json_rgb, "blue", JsonWrapper::realValue).asDouble();
  return rgb;
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
  int accumulate_tracks = JsonWrapper::GetProperty
           (conf, "accumulate_tracks", JsonWrapper::intValue).asInt();
  G4ThreeVector default_rgb = GetColour("default_vis_colour");
  G4ThreeVector pi_plus_rgb = GetColour("pi_plus_vis_colour");
  G4ThreeVector pi_minus_rgb = GetColour("pi_minus_vis_colour");
  G4ThreeVector mu_plus_rgb = GetColour("mu_plus_vis_colour");
  G4ThreeVector mu_minus_rgb = GetColour("mu_minus_vis_colour");
  G4ThreeVector e_plus_rgb = GetColour("e_plus_vis_colour");
  G4ThreeVector e_minus_rgb = GetColour("e_minus_vis_colour");
  G4ThreeVector gamma_rgb = GetColour("gamma_vis_colour");
  G4ThreeVector neutron_rgb = GetColour("neutron_vis_colour");
  G4ThreeVector photon_rgb = GetColour("photon_vis_colour");

  // This sets up a geant4 model which changes the colours of the particles
  const std::string model_name = "Particle Colour Model";
  const G4VTrajectoryModel * current_model = CurrentTrajDrawModel();
  if (model_name != current_model->Name()) {
    G4TrajectoryDrawByParticleID* model
      = new G4TrajectoryDrawByParticleID(model_name);
    model->SetDefault(default_rgb);
    model->Set("pi+", pi_plus_rgb);
    model->Set("pi-", pi_minus_rgb);
    model->Set("mu+", mu_plus_rgb);
    model->Set("mu-", mu_minus_rgb);
    model->Set("e+", e_plus_rgb);
    model->Set("e-", e_minus_rgb);
    model->Set("gamma", gamma_rgb);
    model->Set("neutron", neutron_rgb);
    model->Set("photon", photon_rgb);
    RegisterModel(model);
  }

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
  if (accumulate_tracks == 1) {
      ApplyCommand("/vis/scene/endOfEventAction accumulate");
      ApplyCommand("/vis/scene/endOfRunAction accumulate");
  }
  ApplyCommand("/vis/modeling/trajectories/list");
}

void MAUSVisManager::TearDownRun() {
  ApplyCommand("/vis/viewer/flush");
  // make eps here?
}
}
