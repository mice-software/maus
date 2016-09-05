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


#include "JsonCppStreamer/RStream.hh"
#include "Utils/Exception.hh"

rstream::rstream(const char* filename, const char* mode)
  : m_tree(0),
    m_evtCount(0) {
  if ( !strcmp(filename, "") || !strcmp(mode, "") ) {
    MAUS::Squeak::mout(MAUS::Squeak::fatal)
      << "Couldn't open ROOT TFile as no filename or open mode given"
      << std::endl;
    throw MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable,
		 "rstream object not correctly initialised as null \"\" string passed as filename or open mode.",
		 "rstream::rstream(const char*, const char*)");
  }

  m_file = new TFile(filename, mode);
  if (!m_file) {
    MAUS::Squeak::mout(MAUS::Squeak::fatal)
      << "ROOT TFile opened incorrectly"
      << std::endl;
    throw MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable,
		 "rstream object not correctly initialised as TFile not opened properly",
		 "rstream::rstream(const char*, const char*)");
  }


  m_branchName = new char[40]();
  strcpy(m_branchName, "");
}

rstream::~rstream() {
  m_pointers.clear();
  if (m_branchName) {
    delete [] m_branchName;
    m_branchName = 0;
  }
  if (m_file) {
    m_file->Close();
    delete m_file;
    m_file = 0;
  }
  if (m_tree) {
    // delete m_tree; //ROOT does this automatically on closing file.
    m_tree = 0;
  }
}

bool rstream::is_open() {
    if (m_file == NULL)
        return false;
    return m_file->IsOpen();
}

rstream& rstream::setBranch(rstream& rs, const char* name) {
  strcpy(rs.m_branchName, name);
  return rs;
}
