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

#ifndef _SRC_OUTPUT_OUTPUTCPPSOCKET_OUTPUTCPPSOCKET_HH_
#define _SRC_OUTPUT_OUTPUTCPPSOCKET_OUTPUTCPPSOCKET_HH_

#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>

#include <fstream>
#include <string>
#include <vector>

#include "src/common_cpp/DataStructure/MAUSEvent.hh"
#include "src/common_cpp/API/OutputBase.hh"

namespace MAUS {

class ImageData;
class JobHeaderData;
class JobFooterData;
class RunHeaderData;
class RunFooterData;
class Data;

/** @class OutputCppSocket
 *  
 *  OutputCppSocket exports the data structure out as a ROOT TSocket
 */

class OutputCppSocket : public OutputBase<ImageData*> {
 public:
  /** OutputCppSocket constructor - initialise to NULL
   */
  OutputCppSocket();

  /** OutputCppSocket destructor - calls death()
   */
  ~OutputCppSocket() {death();}

  /** Initialise member data using datacards
   *
   *  @param json_datacards Json document containing datacards. Cards used are
   *         "output_cpp_socket_host" host to search for doc db
   *         "output_cpp_socket_port" port to search for doc db
   *         "output_cpp_socket_collection" collection name
   *         "root_document_store_timeout" timeout if don't get a response
   *         "root_document_store_poll_time" frequency to poll on e.g. connect
   */
  inline void birth(const std::string& json_datacards) {
      ModuleBase::birth(json_datacards);
  }

  /** Delete member data
   */
  inline void death() {
      ModuleBase::death();
  }

 private:
  void _birth(const std::string& json_datacards);

  void _death();

  /** Save a C++ Image in the docstore
   *
   *  @param Image* image to be saved
   *  @returns true if successful
   */
  void _save(ImageData* image);
  void _save(JobHeaderData* jh);
  void _save(JobFooterData* jf);
  void _save(RunHeaderData* rh);
  void _save(RunFooterData* rf);
  void _save(Data* spill);

  bool _save_py_object(PyObject* object);

  PyObject* _get_root_document_store(double timeout, double poll_time);
  void _connect(std::string host, int port);
  void _create_collection(std::string collection, int maxsize);

  PyObject* _string_to_py_json(std::string json_str);

  PyObject* _root_doc_store;
  int       _doc_id;
};
}

#endif

