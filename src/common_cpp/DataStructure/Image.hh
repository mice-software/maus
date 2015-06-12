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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_IMAGE_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_IMAGE_HH_

#include <string>
#include <vector>

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/DateTime.hh"
#include "src/common_cpp/DataStructure/CanvasWrapper.hh"

namespace MAUS {

/** ImageData is the root of the MAUS image structure
 *
 *  ImageData class is the root class for the MAUS image structure. It
 *  just holds a pointer to the image. This is to handle stupid way in which
 *  ROOT does data structure stuff.
 */
class Image {
  public:
    /** Default constructor initialises everything to NULL */
    Image();

    /** Copy constructor (deepcopy) */
    Image(const Image& data);

    /** Deepcopy from data to *this */
    Image& operator=(const Image& data);

    /** Deletes the image data */
    virtual ~Image();

    /** Set the run number */
    void SetRunNumber(int run_number) {_run_number = run_number;}

    /** Get the run number */
    int GetRunNumber() const {return _run_number;}

    /** Set the spill number */
    void SetSpillNumber(int spill_number) {_spill_number = spill_number;}

    /** Get the spill number */
    int GetSpillNumber() const {return _spill_number;}

    /** Set the input time */
    void SetInputTime(DateTime time_stamp) {_input_time = time_stamp;}

    /** Get the input time */
    DateTime GetInputTime() const {return _input_time;}

    /** Set the output time */
    void SetOutputTime(DateTime time_stamp) {_output_time = time_stamp;}

    /** Get the output time */
    DateTime GetOutputTime() const {return _output_time;}

    /** Set the std::vector of canvas wrappers
     *
     *  Image takes ownership of the memory pointed to by the vector
     */
    void SetCanvasWrappers(std::vector<MAUS::CanvasWrapper*> wrappers);

    /** Get the std::vector of canvas wrappers
     *
     *  Image owns the memory pointed to by the vector
     */
    std::vector<CanvasWrapper*> GetCanvasWrappers() const;

    /** Push back new member of canvas wrappers 
     *
     *  Helper function for PyRoot
     */
    void CanvasWrappersPushBack(MAUS::CanvasWrapper* wrap) {
        _canvas_wrappers.push_back(wrap);
    }

  private:
    int _run_number;
    int _spill_number;
    DateTime _input_time;
    DateTime _output_time;
    std::vector<MAUS::CanvasWrapper*> _canvas_wrappers;

    MAUS_VERSIONED_CLASS_DEF(Image);
};

typedef std::vector<MAUS::CanvasWrapper*> CanvasWrapperArray;
}

#endif
