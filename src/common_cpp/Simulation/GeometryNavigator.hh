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

#ifndef _SRC_COMMON_CPP_UTILS_MAUSGEOMETRYNAVIGATOR_HH_
#define _SRC_COMMON_CPP_UTILS_MAUSGEOMETRYNAVIGATOR_HH_

#include <string>

#include "Geant4/G4PVPlacement.hh"
#include "Geant4/G4Navigator.hh"
#include "Geant4/G4Material.hh"

#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Utils/Globals.hh"

class G4Material;

namespace MAUS {

  /** @brief Wrap the G4Navigator class to allow access to the MAUS MC geometry
   *
   *  In order to allow simplified access to the MAUS Geant4 geoemtry, as the
   *  MC sees it, this class wraps the G4Navigator class to control access.
   *
   *  It is by default an instance is created in the global initialisation. You
   *  can alternatively set your own world volume pointer to initialise a
   *  different geometry.
   *
   *  By convention only MAUS ThreeVectors are allowed as arguments. This 
   *  simplifies the interface. The user is not expected to understand anything 
   *  of Geant4 to use this class.
   */
  class GeometryNavigator {
    public:

      /** @brief Constructor - performs default initialisation
       */
      GeometryNavigator();

      /** @brief Destructor - Deletes owned memory
       */
      ~GeometryNavigator();

      /** @brief Initialise navigator with a different geometry
       */
      void Initialise(G4VPhysicalVolume* global_volume);

      /** @brief Set the current global position
       */
      void SetPoint(ThreeVector point);

      /** @brief Increment the current position by the supplied displacement
       * 
       *  Returns the new position. This allows us to use Geant4's intelligent
       *  stepping routines. NOT YET IMPLEMENTED.
       */
      ThreeVector Step(ThreeVector displacement);

      /** @brief Returns the current global position
       */
      ThreeVector GetCurrentPoint() const {return ToMAUSVec(_current_position);}

      /** @brief Gets the name of the material
       */
      std::string GetMaterialName() const;

      /** @brief Returns true if there is more than one element in the material
       */
      bool IsMixture() const;

      /** @brief Gets the Mass Number for a given element in the material
       *  - element: indexes the element in the material
       *
       *  Units: g/mole
       *
       *  WARNING: This function will throw an exception if the current material
       *  is a mixture
       */
      double GetA(size_t element = 0) const;

      /** @brief Gets the Atomic Number for a given element in the material
       *  - element: indexes the element in the material
       *
       *  Unitless
       *
       *  WARNING: This function will throw an exception if the current material
       *  is a mixture
       */
      double GetZ(size_t element = 0) const;

      /** Get the number of elements in the current material
       */
      double GetNumberOfElements() const;

      /** Get the mass fraction of a given element in the current material
       */
      double GetFraction(size_t element) const;

      /** @brief Gets the Nuclear interaction length
       *
       *  Units: cm
       */
      double GetNuclearInteractionLength() const;

      /** @brief Gets the photon attenuation length
       *
       *  Units: cm
       */
      double GetRadiationLength() const;

      /** @brief Gets the density
       *
       *  Units: g/cm^3
       */
      double GetDensity() const;

      /** @brief Get the Geant4 material
       */
      G4Material* GetMaterial() const;

    private:

      /** @brief Private copy constructor. No Copying!
       */
      GeometryNavigator(const GeometryNavigator&) {}

      /** @brief Privately set the current global position
       *
       *  The only function that can create the _touchable_history object.
       */
      void _setPoint(G4ThreeVector);

      /** @brief Increment the current position with the displacement provided.
       */
      G4ThreeVector _step(G4ThreeVector displacement);

      static G4ThreeVector ToG4Vec(ThreeVector v)
                                   {return G4ThreeVector(v.x(), v.y(), v.z());}
      static ThreeVector ToMAUSVec(G4ThreeVector v)
                                     {return ThreeVector(v.x(), v.y(), v.z());}


      G4VPhysicalVolume* _global_volume;

      G4Navigator* _navigator;

      G4TouchableHistory* _touchable_history;

      G4ThreeVector _current_position;

      G4VPhysicalVolume* _current_volume;

      G4Material* _current_material;
  };
}

#endif // _SRC_COMMON_CPP_UTILS_MAUSGEOMETRYNAVIGATOR_HH_

