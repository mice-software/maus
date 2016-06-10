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

#include <string>

#include "src/utilities/event-viewer/EVEvent.hh"
#include "third_party/install/heprep/HepRepXMLWriter.hh"

namespace EventViewer {

/** @class EVHepRepExporter
 *  @author M. Savic
 *  @brief Combines EVEvent data with detector geometry and expxorts it to HepRep file
 */
class EVHepRepExporter {
 public:
  explicit EVHepRepExporter(EVEvent evEvent);
  void Export(int display);

 private:
  EVEvent event;
  int Concatenate(std::string& geometryFileName, std::string& dataFileName,
                  std::string& outFileName);
};

}  // ~namespace EventViewer
