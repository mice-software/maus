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

#ifndef _SRC_OUTPUT_OUTPUTCPPROOT_OUTPUTCPPROOT_HH_
#define _SRC_OUTPUT_OUTPUTCPPROOT_OUTPUTCPPROOT_HH_

#include <fstream>
#include <string>
#include <vector>

#include "src/common_cpp/DataStructure/MAUSEvent.hh"
#include "src/common_cpp/API/OutputBase.hh"

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/JobHeaderData.hh"
#include "src/common_cpp/DataStructure/JobFooterData.hh"
#include "src/common_cpp/DataStructure/RunHeaderData.hh"
#include "src/common_cpp/DataStructure/RunFooterData.hh"

class orstream;

namespace MAUS {

/** @class OutputCppRoot
 *  
 *  OutputCppRoot writes the data structure out as a ROOT tree
 */

class OutputCppRoot : public OutputBase<std::string> {
 public:
  /** OutputCppRoot constructor - initialise to NULL
   */
  OutputCppRoot();

  /** OutputCppRoot destructor - calls death()
   */
  ~OutputCppRoot() {death();}

  /** Initialise member data using datacards
   *
   *  @param json_datacards Json document containing datacards. Only card used
   *         is root_output_file [string], the root file name
   *
   *  Calls, in a roundabout way, _death(); required to force SWIG to see the
   *  base class method
   */
  inline void birth(const std::string& json_datacards) {
      ModuleBase::birth(json_datacards);
  }

  /** Delete member data
   *
   *  Calls, in a roundabout way, _death(); required to force SWIG to see the
   *  base class method
   */
  inline void death() {
      ModuleBase::death();
  }

  enum output_mode {
      one_big_file = 0,
      one_file_per_run = 1,
      end_of_run_file_per_run = 2
  };

 private:
  void _birth(const std::string& json_datacards);

  void _death();

  /** Write a generic event based on type specifications

   *  The lowest level of
   *  the branch has to be a MAUSEvent<DataT> and ConverterT has to be of type
   *  Converter<std::string, DataT>.
   *
   *  Reopens the _outfile if the DataT type has changed since the last read (we
   *  put one Event type per TTree in MAUS, open() required to change the name
   *  of _outfile).
   */
  template <class DataT>
  bool _save(MAUSEvent<DataT>* data_cpp);

  template <class DataT>
  bool write_event(MAUSEvent<DataT>* data_cpp);
  template <class DataT>
  std::string branch_name(MAUSEvent<DataT>* data_cpp);


  event_type get_event_type(const Json::Value& data_json);

  std::string file_name(int run_number);

  std::string dir_name(int run_number);

  template <class DataT>
  void check_file_exists(DataT data_cpp);

  template <class DataT>
  int run_number(DataT* data_cpp);



  orstream* _outfile;

  std::string _fname;
  std::string _end_of_run_dir;
  std::string _outfile_branch;
  JobHeaderData* _job_header_cpp;
  RunHeaderData* _run_header_cpp;
  Data* _spill_cpp;
  RunFooterData* _run_footer_cpp;
  JobFooterData* _job_footer_cpp;
  std::ofstream log;
  std::vector<int> _run_numbers;
  output_mode _mode;
};
}

#endif


