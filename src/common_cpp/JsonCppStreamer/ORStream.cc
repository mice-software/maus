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
#include <string>

#include "JsonCppStreamer/ORStream.hh"
#include "Utils/Squeak.hh"
#include "Utils/Exception.hh"

  orstream::orstream(const char* fileName,
		     const char* treeName,
		     const char* treeTitle,
		     const char* mode)
    : rstream(fileName, mode) {
    m_tree = new TTree(treeName, treeTitle);
  }


  void orstream::open(const char* fileName,
		      const char* treeName,
		      const char* treeTitle,
		      const char* mode) {

    MAUS::Squeak::mout(MAUS::Squeak::debug)
                    << "Opening TFile " << fileName << std::endl;
    if ( !strcmp(fileName, "") ) {
      MAUS::Squeak::mout(MAUS::Squeak::error)
        << "Couldn't open ROOT TFile as no filename given"
        << std::endl;
      throw MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
		   "Cannot open file as null \"\" string passed as filename",
		   "void orstream::open(const char*, const char*, const char*, const char*)");
    }

    m_file = new TFile(fileName, mode);
    if (!m_file) {
      MAUS::Squeak::mout(MAUS::Squeak::error)
        << "ROOT TFile opened incorrectly"
        << std::endl;
      throw MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
		   "TFile object not opened properly",
		   "void orstream::open(const char*, const char*, const char*, const char*)");
    }
    TObject* tree_search = m_file->Get(treeName);
    if (tree_search != NULL) {
        if (strcmp(tree_search->ClassName(), "TTree") != 0)
            throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
                 "Attempt to open TTree with existing non-tree object in the way",
                 "void orstream::open(...)"));
        MAUS::Squeak::mout(MAUS::Squeak::debug)
                << "Opening TFile using existing tree " << treeName << std::endl;
        m_tree = static_cast<TTree*>(tree_search);
    } else {
        m_tree = new TTree(treeName, treeTitle);
        MAUS::Squeak::mout(MAUS::Squeak::debug)
                    << "Opening TFile using new tree " << treeName << std::endl;
    }
    snprintf(m_branchName, sizeof(m_branchName), "%s", "");
    m_evtCount = m_tree->GetEntries();
  }


  void orstream::close() {

    m_file->cd();
    m_tree->Write(m_tree->GetName(), TTree::kWriteDelete);
    m_file->Close();
    MAUS::Squeak::mout(MAUS::Squeak::info)
      << "Written " << m_evtCount << " event(s) to file." << std::endl;
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

  orstream& orstream::operator<<(orstream& (*manip_pointer)(orstream&)) {
    return manip_pointer(*this);
  }

  orstream& fillEvent(orstream& ors) {
    ors.m_tree->Fill();
    ++ors.m_evtCount;
    return ors;
  }


