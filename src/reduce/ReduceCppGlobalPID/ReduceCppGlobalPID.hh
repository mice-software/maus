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

#include "src/common_cpp/DataStructure/Spill.hh"
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

#include "src/common_cpp/API/ReduceBase.hh"
#include "src/common_cpp/API/PyWrapReduceBase.hh"


namespace MAUS {

  class ReduceCppGlobalPID {

  public:

    /** @brief Sets up the worker
     *
     *  @param argJsonConfigDocument a JSON document with
     *         the configuration.
     */
    bool birth(std::string argJsonConfigDocument);

    /** @brief Shutdowns the worker
     *
     *  This takes no arguments.
     */
    bool death();

    /** @brief process JSON document
     *
     *   
     *
     *  @param document Receive a document with spill data
     *  
     */
    std::string process(std::string document);

  private:
    /// Check that a valid configuration is passed to the process
    bool _configCheck;

    /// This will contain the root value after parsing
    Json::Value _root;

    /// Mapper name, useful for tracking results...
    std::string _classname;

    /// Particle hypothesis for which PDFs will be produced
    std::string _hypothesis_name;

    /// Unique identifier for naming root files holding PDFs when reducer is run
    std::string _unique_identifier;

    /// PIDs to perform
    std::vector<MAUS::recon::global::PIDBase*> _pid_vars;

    // The current spill
    Spill* _spill;
  };

} // ~namespace MAUS

#endif
