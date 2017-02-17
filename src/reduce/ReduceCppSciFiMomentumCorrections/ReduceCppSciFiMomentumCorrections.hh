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

/** @class ReduceCppSciFiMomentumCorrections
 *  @author Christopher Hunt
 *  @brief
 *
 */

#ifndef _REDUCECPPSCIFIMOMCORRECTIONS_H
#define _REDUCECPPSCIFIMOMCORRECTIONS_H

#include <string>

#include "json/json.h"

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/VirtualHit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiGeometryHelper.hh"

#include "src/common_cpp/Simulation/VirtualPlanes.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"

#include "src/common_cpp/API/ReduceBase.hh"
#include "src/common_cpp/API/PyWrapReduceBase.hh"

#include "TProfile.h"
#include "TF1.h"
#include "TVectorD.h"
#include "TH2F.h"
#include "TFile.h"


namespace MAUS {

class Data;

class ReduceCppSciFiMomentumCorrections : public ReduceBase<Data, Data> {

  public:
    ReduceCppSciFiMomentumCorrections() :
      ReduceBase<Data, Data>("ReduceCppSciFiMomentumCorrections") {}

    ~ReduceCppSciFiMomentumCorrections();

  private:

    /** @brief Sets up the worker
     *
     *  @param argJsonConfigDocument a JSON document with
     *         the configuration.
     */
    void _birth(const std::string& argJsonConfigDocument);

    /** @brief Shutdowns the worker
     *
     *  This takes no arguments.
     */
    void _death();

    /** @brief process Data object
     *
     *  @param document Receive a document with spill data
     */
    void _process(MAUS::Data* data);

    /** @brief extract pattern recognition seeds
     *
     * @param the SciFi Event
     * @param upstream track pointer
     * @param downstream track pointer
     */
    bool _getHelicalTracks(SciFiEvent* event, SciFiHelicalPRTrack*& up, SciFiHelicalPRTrack*& down);

    /** @brief extract MC true hits
     *
     * @param the MC event
     * @param upstream VHit pointer
     * @param downstream VHit pointer
     */
    bool _getMCTracks(MCEvent* event, VirtualHit*& up, VirtualHit*& down);

    /** @brief return the station ID of the nearest virtual plane
     * 
     * @param z position
     */
    int _findVirtualPlane(double z);

    /// Mapper name, useful for tracking results...
    std::string _classname;

    /// Output File Name
    std::string _corrections_filename;

    /// The current spill
    const MAUS::Spill* _spill;

    SciFiGeometryHelper _geometry_helper;

    TH2F _up_residual_p;
    TH2F _down_residual_p;
    TH2F _up_residual_pz;
    TH2F _down_residual_pz;
    TH2F _up_p_residual_pt;
    TH2F _down_p_residual_pt;
    TH2F _up_p_residual_pz;
    TH2F _down_p_residual_pz;

    TProfile _up_profile_p;
    TProfile _down_profile_p;
    TProfile _up_profile_pz;
    TProfile _down_profile_pz;
    TProfile _up_profile_pt;
    TProfile _down_profile_pt;
    TProfile _up_p_profile_pt;
    TProfile _down_p_profile_pt;
    TProfile _up_p_profile_pz;
    TProfile _down_p_profile_pz;

    int _upstream_virtual_plane;
    int _downstream_virtual_plane;
};

} // ~namespace MAUS

#endif
