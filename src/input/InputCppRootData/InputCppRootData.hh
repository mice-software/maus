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

#ifndef _MAUS_SRC_INPUT_INPUTCPPROOTDATA_INPUTCPPROOTDATA_HH__
#define _MAUS_SRC_INPUT_INPUTCPPROOTDATA_INPUTCPPROOTDATA_HH__

#include <string>
#include <set>
#include <map>

#include "Rtypes.h"  // ROOT

#include "json/json.h"

#include "Utils/Exception.hh"
#include "src/common_cpp/API/InputBase.hh"

class irstream;

namespace MAUS {

/** @class InputCppRootData
 *
 *  InputCppRootData enables us to read in ROOT files and emit them as strings
 *  containing json formatted data. InputCppRootData uses irstream from
 *  JsonCppStreamer to read ROOT files and JsonCppConverter to convert from
 *  ROOT to json.
 */
class InputCppRootData : public InputBase<MAUS::Data> {
  public:
    /** Constructor for InputCppRootData, initialises all members to NULL
     *
     *  @param filename if set forces the Inputter to use the filename rather than
     *         pulling a filename from the datacards at birth
     */
    explicit InputCppRootData(std::string filename = "");

    /** Destructor for InputCppRootData - calls death()
     */
    ~InputCppRootData();

    void birth(const std::string& json_datacards) {
        InputBase<MAUS::Data>::birth(json_datacards);
    }

    void death() {
        InputBase<MAUS::Data>::death();
    }

    /** Return true if only loading spills specified in data cards
      */
    bool useSelectedSpills() const { return _select_spills; }

  private:
    /** Initialise the inputter
     *
     *  @param json_datacards json formatted string containing the json datacards
     *  - takes root file from "root_input_filename" parameter
     */
    void _birth(const std::string& json_datacards);

    /** Deletes inputter member data
     */
    void _death();

    /** Gets the next event from the root file. If there are no more events,
     *  returns an empty string ("")
     *
     *  This will cycle through different event types as follows:
     *  First attempts to find all JobHeaders, then RunHeaders until run_number
     *  changes, then spill until the run_number changes; then RunFooter until
     *  run_number changes. Then starts back on RunHeader.
     *
     *  If there are no events of a particular type left, then that event type
     *  is skipped and we move on to the next event type until all events have
     *  been fired and we have no more data.
     */
    MAUS::Data _emit_cpp();

    /** Gets an event from the root file.
     *
     *  If there are no more events of the given type, or the event could not be
     *  resolved (data inconsistencies?) return ""
     *
     *  Note that if we are accessing a different branch from last time, we have
     *  to reopen _infile with the new branch name (that's how irstream works). 
     */
    template <class DataT>
    bool load_event(std::string branch_name, DataT&);

    /** Returns true if we are only loading selected spill numbers and
     *  the spill number is found in the data card, or if it is an invalid
     *  spill - there are already methods to handle those cases.
     *
     *  This is a help function to see if the set of loaded spill numbers
     *  contains the spill \"spillNum\".
     */
    bool is_selected_spill(int spillNum) const;

    /** _irstream holds root TFile.
     */
    irstream* _infile;
    std::string _infile_tree;
    std::string _filename;
    std::string _classname;

    event_type _event_type;
    bool _select_spills;
    std::set< int > _selected_spill_numbers;
    int _highest_spill_number;
};
}

#endif

