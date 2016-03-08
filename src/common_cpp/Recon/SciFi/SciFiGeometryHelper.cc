/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

#include "src/common_cpp/Recon/SciFi/SciFiGeometryHelper.hh"

namespace MAUS {

SciFiGeometryHelper::SciFiGeometryHelper() {}

SciFiGeometryHelper::SciFiGeometryHelper(const std::vector<const MiceModule*>& modules)
                                        : _modules(modules) {
  Json::Value *json = Globals::GetConfigurationCards();

  // Set Fibre Parameters.
  FibreParameters.Z                     = (*json)["SciFiParams_Z"].asDouble();
  FibreParameters.Plane_Width           = (*json)["SciFiParams_Plane_Width"].asDouble();
  FibreParameters.Radiation_Length      = (*json)["SciFiParams_Radiation_Length"].asDouble();
  FibreParameters.Density               = (*json)["SciFiParams_Density"].asDouble();
  FibreParameters.Mean_Excitation_Energy = (*json)["SciFiParams_Mean_Excitation_Energy"].asDouble();
  FibreParameters.A                     = (*json)["SciFiParams_A"].asDouble();
  FibreParameters.Pitch                 = (*json)["SciFiParams_Pitch"].asDouble();
  FibreParameters.Station_Radius        = (*json)["SciFiParams_Station_Radius"].asDouble();
  FibreParameters.Density_Correction    = (*json)["SciFiParams_Density_Correction"].asDouble();

  MylarParameters.Z                     = (*json)["MylarParams_Z"].asDouble();
  MylarParameters.Plane_Width           = (*json)["MylarParams_Plane_Width"].asDouble();
  MylarParameters.Radiation_Length      = (*json)["MylarParams_Radiation_Length"].asDouble();
  MylarParameters.Density               = (*json)["MylarParams_Density"].asDouble();
  MylarParameters.Mean_Excitation_Energy = (*json)["MylarParams_Mean_Excitation_Energy"].asDouble();
  MylarParameters.A                     = (*json)["MylarParams_A"].asDouble();
  MylarParameters.Density_Correction    = (*json)["MylarParams_Density_Correction"].asDouble();

  GasParameters.Z                       = (*json)["GasParams_Z"].asDouble();
  GasParameters.Radiation_Length        = (*json)["GasParams_Radiation_Length"].asDouble();
  GasParameters.Density                 = (*json)["GasParams_Density"].asDouble();
  GasParameters.Mean_Excitation_Energy  = (*json)["GasParams_Mean_Excitation_Energy"].asDouble();
  GasParameters.A                       = (*json)["GasParams_A"].asDouble();
  GasParameters.Density_Correction      = (*json)["GasParams_Density_Correction"].asDouble();

  _default_momentum  = (*json)["SciFiDefaultMomentum"].asDouble();
}

SciFiGeometryHelper::~SciFiGeometryHelper() {}


void SciFiGeometryHelper::Build() {
  // Iterate over existing modules, adding planes to the map.
  std::vector<const MiceModule*>::iterator iter;
  for ( iter = _modules.begin(); iter != _modules.end(); iter++ ) {
    const MiceModule* module = (*iter);
    if ( module->propertyExists("Tracker", "int") &&
         module->propertyExists("Station", "int") &&
         module->propertyExists("Plane", "int") ) {
      int tracker_n = module->propertyInt("Tracker");
      int station_n = module->propertyInt("Station");
      int plane_n   = module->propertyInt("Plane");

      double pitch        = module->propertyDouble("Pitch");
      double centralfibre = module->propertyDouble("CentralFibre");
      ThreeVector direction(0., 1., 0.);

      // G4RotationMatrix global_fibre_rotation = G4RotationMatrix(module->globalRotation());
      const MiceModule* plane = module->mother();
      HepRotation internal_fibre_rotation(module->relativeRotation(module->mother() // plane
                                               ->mother()));  // tracker/ station??

      direction     *= internal_fibre_rotation;

      // The plane rotation wrt to the solenoid. Identity matrix for tracker 1,
      // [ -1, 0, 0],[ 0, 1, 0],[ 0, 0, -1] for tracker 0 (180 degrees rot. around y).
      // const MiceModule* plane = module->mother();
      HepRotation plane_rotation(plane->relativeRotation(plane->mother()  // tracker
                                                              ->mother()));    // solenoid

      ThreeVector position  = clhep_to_root(module->globalPosition());
      ThreeVector reference = FindReferenceFramePosition(tracker_n);

      ThreeVector tracker_ref_frame_pos = position-reference;
      tracker_ref_frame_pos *= plane_rotation;

      int plane_id =  3*(station_n-1) + (plane_n+1);
//      plane_id     = ( tracker_n == 0 ? -plane_id : plane_id );

      const MiceModule* trackerModule = plane->mother(); // tracker
      ThreeVector trackerPos = clhep_to_root(trackerModule->globalPosition());


      SciFiPlaneGeometry this_plane;
      this_plane.Direction      = direction.Unit();
      this_plane.Position       = tracker_ref_frame_pos;
      this_plane.GlobalPosition = position;
      this_plane.CentralFibre   = centralfibre;
      this_plane.Pitch          = pitch;

      SciFiTrackerGeometry trackerGeo = _geometry_map[tracker_n];
      trackerGeo.Position = reference;
      trackerGeo.Rotation = (trackerModule->globalRotation()).inverse();
      trackerGeo.Field = FieldValue(trackerModule);
      trackerGeo.Planes[plane_id] = this_plane;

      _geometry_map[tracker_n] = trackerGeo;
    }
  }
}

double SciFiGeometryHelper::FieldValue(ThreeVector global_position,
                                       HepRotation plane_rotation) {
  double EMfield[6]  = {0., 0., 0., 0., 0., 0.};
  double position[4] = {global_position.x(), global_position.y(), global_position.z(), 0.};
  BTFieldConstructor* field = Globals::GetReconFieldConstructor();
  field->GetElectroMagneticField()->GetFieldValue(position, EMfield);
  ThreeVector B_field(EMfield[0], EMfield[1], EMfield[2]);
  B_field *= plane_rotation;
  double Tracker_Bz = B_field.z();
  return Tracker_Bz;
}

double SciFiGeometryHelper::FieldValue(const MiceModule* trackerModule ) {
//  Hep3Vector hepGlobalPos = trackerModule->globalPosition();
  if (trackerModule == NULL) {
    std::cerr << "No tracker module inited!\n";
    throw Exception(Exception::nonRecoverable,
                    "Invalid MiceModule provided",
                    "SciFiGeometryHelper::FieldValue()");
  }
  Hep3Vector globalPos = trackerModule->globalPosition();
  Hep3Vector relativePos(0., 0., 0.);
  HepRotation trackerRotation = (trackerModule->globalRotation()).inverse();
  double EMfield[6]  = {0., 0., 0., 0., 0., 0.};
  double position[4] = {0., 0., 0., 0.};
  BTFieldConstructor* field = Globals::GetReconFieldConstructor();

  Hep3Vector dims = trackerModule->dimensions();
  double stepSize = 0.1; // mm
  double halfLength = 0.5*dims[1];
  double z_pos = - halfLength;
  double sumBz = 0.0;
  int numSteps = static_cast<int>(2.0 * halfLength / stepSize);

  do {
    relativePos.setZ(z_pos);

    Hep3Vector testPosition = ( trackerRotation*relativePos ) + globalPos;
    position[0] = testPosition[0];
    position[1] = testPosition[1];
    position[2] = testPosition[2];
    field->GetElectroMagneticField()->GetFieldValue(position, EMfield);

    ThreeVector B_field(EMfield[0], EMfield[1], EMfield[2]);
    B_field *= trackerRotation;
    sumBz += B_field[2];
//    sumBz += EMfield[2];

    z_pos += stepSize;
  } while (z_pos < halfLength);

  return sumBz / numSteps;
}

const MiceModule* SciFiGeometryHelper::FindPlane(int tracker, int station, int plane) const {
  const MiceModule* this_plane = NULL;
  for ( unsigned int j = 0; !this_plane && j < _modules.size(); ++j ) {
    // Find the right module
    if ( _modules[j]->propertyExists("Tracker", "int") &&
         _modules[j]->propertyExists("Station", "int") &&
         _modules[j]->propertyExists("Plane", "int")  &&
         _modules[j]->propertyInt("Tracker") ==
         tracker &&
         _modules[j]->propertyInt("Station") ==
         station &&
         _modules[j]->propertyInt("Plane") ==
         plane ) {
         // Save the module
      this_plane = _modules[j];
    }
  }
  if ( this_plane == NULL ) {
    throw(Exception(Exception::nonRecoverable,
    "Failed to find tracker plane.",
    "SciFiGeometryHelper::find_plane"));
  }
  return this_plane;
}

double SciFiGeometryHelper::GetPlanePosition(int tracker, int station, int plane) const {
  int id = ( ((station-1)*3) + plane + 1 );
  return _geometry_map.find(tracker)->second.Planes.find(id)->second.Position.z();
}

ThreeVector SciFiGeometryHelper::FindReferenceFramePosition(int tracker) const {
  int station = 1;
  int plane   = 0;
  const MiceModule* reference_plane = NULL;
  reference_plane = FindPlane(tracker, station, plane);

  assert(reference_plane != NULL);
  ThreeVector reference_pos = clhep_to_root(reference_plane->globalPosition());
  return reference_pos;
}

double SciFiGeometryHelper::HighlandFormula(double L, double beta, double p) {
  static double HighlandConstant = Recon::Constants::HighlandConstant;
  // Note that the z factor (charge of the incoming particle) is omitted.
  // We don't need to consider |z| > 1.
  double result = HighlandConstant*TMath::Sqrt(L)*(1.+0.038*TMath::Log(L))/(beta*p);
  return result;
}

double SciFiGeometryHelper::BetheBlochStoppingPower(double p, const SciFiMaterialParams* material) {
  double muon_mass      = Recon::Constants::MuonMass;
  double electron_mass  = Recon::Constants::ElectronMass;
  double muon_mass2     = muon_mass*muon_mass;

  double E = TMath::Sqrt(muon_mass2+p*p);

  double beta   = p/E;
  double beta2  = beta*beta;
  double gamma  = E/muon_mass;
  double gamma2 = gamma*gamma;

  double K = Recon::Constants::BetheBlochParameters::K();
  double A = material->A;
  double I = material->Mean_Excitation_Energy;
  double I2= I*I;
  double Z = material->Z;
  double density = material->Density;
  double density_correction = material->Density_Correction;

  double outer_term = K*Z/(A*beta2);
  double Tmax = 2.*electron_mass*beta2*gamma2/(1.+(2.*gamma*electron_mass/muon_mass) +
                (electron_mass*electron_mass/(muon_mass*muon_mass)));
  double log_term = TMath::Log(2.*electron_mass*beta2*gamma2*Tmax/(I2));
  double dEdx = outer_term*(0.5*log_term-beta2-density_correction/2.);
  return density*dEdx;
}


void SciFiGeometryHelper::FillMaterialsList(int start_id, int end_id,
                                                              SciFiMaterialsList& materials_list) {
  int increment;
  start_id = abs(start_id);
  end_id = abs(end_id);
  if (start_id < end_id) increment = 1;
  else                   increment = -1;

  for (int current_id = start_id; current_id != end_id; current_id += increment) {
    materials_list.push_back(std::make_pair(&MylarParameters, MylarParameters.Plane_Width));
    materials_list.push_back(std::make_pair(&FibreParameters, FibreParameters.Plane_Width));

    switch (current_id) {
      case 4:
        materials_list.push_back(std::make_pair(&GasParameters, 200.0));
        break;
      case 7:
        materials_list.push_back(std::make_pair(&GasParameters, 250.0));
        break;
      case 10:
        materials_list.push_back(std::make_pair(&GasParameters, 300.0));
        break;
      case 13:
        materials_list.push_back(std::make_pair(&GasParameters, 350.0));
        break;
      default:
        break;
    }
  }
}

ThreeVector SciFiGeometryHelper::TransformPositionToGlobal(const ThreeVector& pos,
                                                           int tracker) const {
  ThreeVector reference_position = GetReferencePosition(tracker);
  CLHEP::HepRotation reference_rotation = GetReferenceRotation(tracker);

  ThreeVector global_position = pos;
  global_position *= reference_rotation;
  global_position += reference_position;

  return global_position;
}

std::vector<double> SciFiGeometryHelper::TransformStraightParamsToGlobal(
    const std::vector<double>& params, int tracker) const {
  // Paramters of the fit in tracker local coordinates
  double x0 = params[0];
  double mzx = params[1];
  double y0 = params[2];
  double mzy = params[3];

  // Paramters of the fit in global coordinates
  double gx0 = 0.0;
  double gmzx = 0.0;
  double gy0 = 0.0;
  double gmzy = 0.0;

  // Calculate the global coordinates of the point at the ref. plane
  double z = GetPlanePosition(1, 1, 0);
  ThreeVector pos(x0, y0, z);
  ThreeVector global_pos = TransformPositionToGlobal(pos, tracker);

  // The gradients are rotated around the tracker centre
  ThreeVector grad(mzx, mzy, 1);
  CLHEP::HepRotation reference_rotation = GetReferenceRotation(tracker);
  grad *= reference_rotation;

  gmzx = grad.x()/grad.z();
  gmzy = grad.y()/grad.z();

  // Calculate the y-intercepts at the z=0 (D2 in global coordinates)
  gx0 = global_pos.x()-gmzx*global_pos.z();
  gy0 = global_pos.y()-gmzy*global_pos.z();

  // Return the result as a vector
  std::vector<double> global_params{ gx0, gmzx, gy0, gmzy }; // NOLINT
  return global_params;
}

} // ~namespace MAUS
