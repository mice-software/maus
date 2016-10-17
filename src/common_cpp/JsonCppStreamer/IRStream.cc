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

#include <stdio.h>

#include "JsonCppStreamer/IRStream.hh"
#include "Utils/Squeak.hh"
#include "Utils/Exception.hh"

irstream::irstream(const char* fileName,
		   const char* treeName,
		   const char* mode)
  : rstream(fileName, mode) {

  m_tree = reinterpret_cast<TTree*>(m_file->GetObjectChecked(treeName, "TTree"));
  if (!m_tree) {
    MAUS::Squeak::mout(MAUS::Squeak::fatal)
      << "The requested tree '"
      << treeName
      << "' was not found in the tree."
      <<std::endl;
    throw MAUS::Exceptions::Exception(MAUS::Exceptions::nonRecoverable,
		 "irstream object not correctly initialised as couldn;t find requested TTree.",
		 "irstream::irstream(const char*, const char*, const char*)");
  }
}


void irstream::open(const char* fileName,
		    const char* treeName,
		    const char* mode) {
  if ( !strcmp(fileName, "") ) {
    MAUS::Squeak::mout(MAUS::Squeak::error)
      << "Couldn't open ROOT TFile as no filename given"
      << std::endl;
    throw MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
		 "Cannot open file as null \"\" string passed as filename",
		 "void irstream::open(const char*, const char*, const char*)");
  }

  m_file = new TFile(fileName, mode);
  if (!m_file) {
    MAUS::Squeak::mout(MAUS::Squeak::error)
      << "ROOT TFile opened incorrectly"
      << std::endl;
    throw MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
		 "TFile object not opened properly",
		 "void irstream::open(const char*, const char*, const char*)");
  }

  m_tree = reinterpret_cast<TTree*>(m_file->GetObjectChecked(treeName, "TTree"));
  if (!m_tree) {
    MAUS::Squeak::mout(MAUS::Squeak::error)
      << "The requested tree '"
      << treeName
      << "' was not found in the tree."
      << std::endl;
    throw MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
		 "Could not find requested TTree.",
		 "void irstream::open(const char*, const char*, const char*)");
  }

  snprintf(m_branchName, sizeof(m_branchName), "%s", "");
  m_evtCount = 0;
}

void irstream::close() {
  m_file->Close();
  snprintf(m_branchName, sizeof(m_branchName), "%s", "");
  if (m_file) {
    delete m_file;
    m_file = 0;
  }
  if (m_tree) {
    // delete m_tree; //ROOT does this automatically on closing file.
    m_tree = 0;
  }
}

void irstream::set_current_event(Long64_t event) {
  m_evtCount = event;
}

irstream* irstream::operator>>(irstream* (*manip_pointer)(irstream&)) {
  return manip_pointer(*this);
}

irstream* readEvent(irstream& irs) {
  if (irs.m_evtCount >= irs.m_tree->GetEntries()) {
    MAUS::Squeak::mout(MAUS::Squeak::info)
      << "Failed to find event "
      << irs.m_evtCount
      << " in file."
      << std::endl;
    return 0;
  }
  irs.m_tree->GetEntry(irs.m_evtCount);
  ++irs.m_evtCount;

  return &irs;
}
