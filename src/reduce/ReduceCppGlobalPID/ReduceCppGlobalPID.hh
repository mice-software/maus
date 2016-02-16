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

/** @class ReduceCppGlobalPID
 *  @author Celeste Pidcott, University of Warwick
 *  Produce PDFs from MC data for use in global PID
 *
 */

#ifndef _REDUCECPPGLOBALPID_H
#define _REDUCECPPGLOBALPID_H

#include <string>
#include <vector>
#include <map>

#include "TH2.h"
#include "TFile.h"

#include "json/json.h"

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/Recon/Global/PIDBase.hh"
#include "src/common_cpp/Recon/Global/PIDBase1D.hh"
#include "src/common_cpp/Recon/Global/PIDBase2D.hh"
#include "src/common_cpp/Recon/Global/PIDVarA.hh"
#include "src/common_cpp/Recon/Global/PIDVarB.hh"
#include "src/common_cpp/Recon/Global/PIDVarC.hh"
#include "src/common_cpp/Recon/Global/PIDVarD.hh"
#include "src/common_cpp/Recon/Global/PIDVarE.hh"
#include "src/common_cpp/Recon/Global/PIDVarF.hh"
#include "src/common_cpp/Recon/Global/PIDVarG.hh"
#include "src/common_cpp/Recon/Global/PIDVarH.hh"
#include "src/common_cpp/Recon/Global/PIDVarI.hh"
#include "src/common_cpp/Recon/Global/PIDVarJ.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarA.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarB.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarC.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarD.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarE.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarF.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarG.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarH.hh"
#include "src/common_cpp/Recon/Global/ComPIDVarI.hh"

#include "src/common_cpp/API/ReduceBase.hh"
#include "src/common_cpp/API/PyWrapReduceBase.hh"


namespace MAUS {

class Data;
class ImageData;

class ReduceCppGlobalPID : public ReduceBase<Data, Data> {

  public:
    ReduceCppGlobalPID() : ReduceBase<Data, Data>("ReduceCppGlobalPID"), _configCheck(false) {}
  ~ReduceCppGlobalPID();

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
     *   
     *
     *  @param document Receive a document with spill data
     *  
     */
    void _process(MAUS::Data* data);

    /** @brief find pid of MC particle at US tracker ref. plane
     *
     *   
     *
     *  @param mc_event MC event
     *  
     */
    int _mc_pid_US_tracker_ref(MAUS::MCEvent* mc_event);

    /** @brief find pid of MC particle at DS tracker ref. plane
     *
     *   
     *
     *  @param mc_event MC event
     *  
     */
    int _mc_pid_DS_tracker_ref(MAUS::MCEvent* mc_event);

    /// Check that a valid configuration is passed to the process
    bool _configCheck;

    /// Hypotheses to create PDFs for
    std::vector<std::string> _hypotheses;

    /// MICE configuration for PID
    std::string _pid_config;

    /// PID beamline polarity- set in ConfigurationDefaults to select 
    /// positive or negative particles
    std::string _pid_beamline_polarity;

    /// PID beam setting- set in ConfigurationDefaults to select 
    /// beam setting, i.e. emittance and momentum
    std::string _pid_beam_setting;

    /// Mapper name, useful for tracking results...
    std::string _classname;

    /// Unique identifier for naming root files holding PDFs when reducer is run
    std::string _unique_identifier;

    /// PIDs to perform
    std::vector<MAUS::recon::global::PIDBase*> _mu_pid_vars;

    /// PIDs to perform
    std::vector<MAUS::recon::global::PIDBase*> _e_pid_vars;

    /// PIDs to perform
    std::vector<MAUS::recon::global::PIDBase*> _pi_pid_vars;

    /// The current spill
    const MAUS::Spill* _spill;
};

} // ~namespace MAUS

#endif
