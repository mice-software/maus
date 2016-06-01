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

/* Simple code used to open a reconstructed MAUS ROOT file,
 * iterate through spills and pass a spill to EVExporter 
 */

#include <iostream>
#include <vector>
#include <string>

#include "DataStructure/Spill.hh"
#include "DataStructure/Data.hh"
#include "JsonCppStreamer/IRStream.hh"

#include "src/utilities/event-viewer/EVExporter.hh"

int ParseArguments(int argc, char* argv[]);
void ShowUsage(std::string arg);

std::string FILE_NAME;
int SPILL_NUM;
int SPILL_RANGE[2];
int EVENT_SELECTION;
bool VERBOSE;

int main(int argc, char* argv[]) {
  SPILL_NUM = 0;
  SPILL_RANGE[0] = 0;
  SPILL_RANGE[1] = 0;
  EVENT_SELECTION = 0;
  VERBOSE = false;

  if (!ParseArguments(argc, argv))
    return 1;

  // - start reading data:
  MAUS::Data data;
  irstream infile(FILE_NAME.c_str(), "Spill");

  // - iterate over spills:
  while (infile >> readEvent != NULL) {
    infile >> branchName("data") >> data;
    MAUS::Spill *spill = data.GetSpill();

    if (spill != NULL && spill->GetDaqEventType() == "physics_event") {
      int spillNumber = spill->GetSpillNumber();

      // - instantiate EVExported and pass spill to it
      if (SPILL_NUM != 0) {
        if (spillNumber == SPILL_NUM) {
          EventViewer::EVExporter *exp = new EventViewer::EVExporter(spill);
          exp->ReadAllEvents(EVENT_SELECTION, VERBOSE);
          return 0;
        }
      } else if (SPILL_RANGE[1] != 0) { // can not pass both --spill and --spill_range arguments
        if (SPILL_RANGE[1] > SPILL_RANGE[0]) {
          if (spillNumber >= SPILL_RANGE[0] && spillNumber <= SPILL_RANGE[1]) {
            EventViewer::EVExporter *exp = new EventViewer::EVExporter(spill);
            exp->ReadAllEvents(EVENT_SELECTION, VERBOSE);
          } else if (spillNumber > SPILL_RANGE[1]) {
            return 0;
          }
        } else {
          std::cerr << std::endl << "ERROR: Invalid spill range!" << std::endl << std::endl;
          return 2;
        }
      } else if (SPILL_NUM == 0 && SPILL_RANGE[0] == 0 && SPILL_RANGE[1] == 0) {
        EventViewer::EVExporter *exp = new EventViewer::EVExporter(spill);
        exp->ReadAllEvents(EVENT_SELECTION, VERBOSE);
      }
    }
  }
  infile.close();
  return 0;
}

int ParseArguments(int argc, char* argv[]) {
  FILE_NAME = "";

  if (argc < 2) {
    ShowUsage(argv[0]);
    return 0;
  }

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-h" || arg == "--help") {
      ShowUsage(argv[0]);
      return 0;
    }
    if (arg == "-v" || arg == "--verbose") {
      VERBOSE = true;
    }
    if (arg == "-f" || arg == "--file")
      FILE_NAME = argv[i+1];
    if (arg == "-s" || arg == "--spill") {
      if (i+1 < argc) {
        SPILL_NUM = atoi(argv[i+1]);
      } else {
        std::cerr << "\n" << "WARNING: You must provide argument to --spill option" << "\n\n";
        return 0;
      }
    }
    if (arg == "-sr" || arg == "--spill_range") {
      if (i+1 < argc) {
        SPILL_RANGE[0] = atoi(argv[i+1]);
      } else {
        std::cerr << "\n" << "WARNING: You must provide argument to --spill_range option" << "\n\n";
        return 0;
      }
      if (i+2 < argc) {
        SPILL_RANGE[1] = atoi(argv[i+2]);
      } else {
        std::cerr << "\n" << "WARNING: You must provide second argument to --spill_range option"
                  << "\n\n";
        return 0;
      }
    }
    if (arg == "-es" || arg == "--event_selection")
      if (i+1 < argc) {
        EVENT_SELECTION = atoi(argv[i+1]);
      } else {
        std::cerr << "\n" << "WARNING: You must provide argument to --event_selection option"
                  << "\n\n";
        return 0;
      }
  }

  if (FILE_NAME == "") {
    std::cerr << "\n" << "ERROR! You must provide INPUT_FILE argument!" << "\n\n";
    ShowUsage(argv[0]);
    return 0;
  }

  return 1;
}

void ShowUsage(std::string progName) {
  std::cerr << "\n" << "USAGE: " << progName << " <option(s)> SOURCES\n"
    << " Options:\n"
    << "   -h, --help\t\t\t\t\tShow this message\n"
    << "   -v, --verbose\t\t\t\tverbose mode\n"
    << "   -f, --file INPUT_FILE\t\t\tSpecify ROOT input file (has to be provided)\n"
    << "   -s, --spill SPILL_NUMBER\t\t\tSpecify spill to export\n"
    << "   -sr, --spill_range FIRST_SPILL LAST_SPILL\tSpecify range of spills to be exported\n"
    << "   (if no spills are selected the whole run will be processed)\n\n"
    << "   -es, --event_selection EVENT_SELECTION\tNumber describing combination "
    << "of detectors hit in an event:\n"
    << "      \t0\t\texport all events (default value)\n"
    << "      \t1\t\tno detectors hit\n"
    << "      \t+2\t\tTOF0 hit\n"
    << "      \t+4\t\tTOF1 hit\n"
    << "      \t+8\t\tat least one hit in US tracker\n"
    << "      \t+16\t\tat least one hit in DS tracker\n"
    << "      \t+32\t\tTOF2 hit\n"
    << "   (only events that have this combination of detectors hit will be exported "
    << "(e.g. 62 for all detectors hit))\n\n"
    << "\n\n";
}
