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

/* Iterates through events of a spill and passes filled EVEvent container for output */


#include <string>

#include "DataStructure/Spill.hh"
#include "DataStructure/TOFEvent.hh"
#include "DataStructure/SciFiEvent.hh"
#include "src/utilities/event-viewer/EVHepRepExporter.hh"
#include "src/utilities/event-viewer/EVEvent.hh"

namespace EventViewer {

class EVExporter {

 public:
  explicit EVExporter(MAUS::Spill *sp);
  virtual ~EVExporter();

  EVEvent evEvent;

  /** @brief Iterates through all events of a spill
   */
  void ReadAllEvents(int eventSelection, bool verbose);

  /** @brief Processes a particular event of a spill
   */
  int ReadOneEvent(int eventNumber, std::string opt, int eventSelection, int display);

 private:
  int detectorsHit;
  MAUS::Spill *spill;
  MAUS::TOFEvent *tof_event;
  MAUS::SciFiEvent *scifi_event;

  // - private methods
  void ProcessTOFEvent();
  void ProcessTOF0();
  void ProcessTOF1();
  void ProcessTOF2();
  void ProcessScifiEvent();
};

} // ~namespace EventViewer
