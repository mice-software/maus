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

#ifndef _MAUS_SRC_INPUT_INPUTCPPROOT_INPUTCPPROOT_HH__
#define _MAUS_SRC_INPUT_INPUTCPPROOT_INPUTCPPROOT_HH__

#include <string>

#include "json/json.h"

#include "src/legacy/Interface/Squeal.hh"
#include "src/common_cpp/API/InputBase.hh"

class irstream;

namespace MAUS {

/** @class InputCppRoot
 *
 *  InputCppRoot enables us to read in ROOT files and emit them as strings
 *  containing json formatted data. InputCppRoot uses irstream from
 *  JsonCppStreamer to read ROOT files and JsonCppConverter to convert from
 *  ROOT to json.
 */
class InputCppRoot : public InputBase<std::string> {
  public:
    /** Constructor for InputCppRoot, initialises all members to NULL
     *
     *  @param filename if set forces the Inputter to use the filename rather than
     *         pulling a filename from the datacards at birth
     */
    explicit InputCppRoot(std::string filename = "");

    /** Destructor for InputCppRoot - calls death()
     */
    ~InputCppRoot();

    void birth(const std::string& json_datacards) {
        InputBase<std::string>::birth(json_datacards);
    }

    void death() {
        InputBase<std::string>::death();
    }


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
     *  First attempts to find all JobHeaders, then the first
     *  RunHeader, then spill until the Spill number changes; then RunFooter,
     *  then RunHeader, then spill until the Spill number changes, etc, then
     *  finally all JobFooters. If there are no events of a particular type left,
     *  then that event type is skipped and we move on to the next event type
     *  until all events have been fired and we have no more data.
     */
    std::string _emitter_cpp();

    std::string emitter() {throw(Squeal(Squeal::recoverable, "Test Exception", "Test::Exception"));}


    /** Gets an event from the root file.
     *
     *  If there are no more events of the given type, or the event could not be
     *  resolved (data inconsistencies?) return ""
     *
     *  Note that if we are accessing a different branch from last time, we have
     *  to reopen _infile with the new branch name (that's how irstream works). 
     */
    template <class ConverterT, class DataT>
    std::string _load_event(std::string branch_name);

    /** _irstream holds root TFile.
     */
    irstream* _infile;
    std::string _infile_tree;
    std::string _filename;
    std::string _classname;

    InputBase::event_type _next_event_type;
};
}

#endif

