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


#include "src/utilities/event-viewer/EVHepRepExporter.hh"

#include <unistd.h>
#include <string>
#include <sstream>
#include <vector>

namespace EventViewer {

EVHepRepExporter::EVHepRepExporter(EVEvent evEvent) {
  event = evEvent;
}

void EVHepRepExporter::Export(int display) {

  HepRepXMLWriter* writer = new HepRepXMLWriter();
  std::stringstream ssOutFileName;

  // - retrieve destination directory
  char *destDir = getenv("EV_DEST_DIR");
  if (destDir != NULL)
    ssOutFileName << destDir << "/";
  else
    std::cerr << "INFO: No destination directory set for HepRep files! "
              << "Output files will be saved to current working directory!" << std::endl;

  // - open output heprep file
  ssOutFileName << "MICEEvent_" << event.runNumber << "_" << event.spillNumber
                << "_" << event.eventNumber << ".heprep";
  std::string dataFileName = "data.heprep";
  writer->open(dataFileName.c_str());

  // - define tof points
  writer->addType("TOFPoints", 0);
  writer->addInstance();
  writer->addAttValue("DrawAs", "Point");
  writer->addAttValue("MarkName", "Box");
  writer->addAttValue("MarkColor", 0., 1., 1.);
  for (int point = 0; point < 3; ++point) {
    double x = event.tofPoints[point][0];
    double y = event.tofPoints[point][1];
    double z = event.tofPoints[point][2];
    if (z != 5673390) {
      writer->addPrimitive();
      writer->addPoint(x, y, z);
    }
  }

  // - define track points
  writer->addType("TrackPoints", 0);
  writer->addInstance();
  writer->addAttValue("DrawAs", "Point");

  // - plot TKU track points in HepRApp
  writer->addType("TKUTrackPoints", 1);
  writer->addInstance();
  writer->addAttValue("MarkName", "Cross");
  writer->addAttValue("MarkColor", 1., 1., 0.);
  for (int point = 0; point < 5; ++point) {
    double x = event.scifiUSTrackerPoints[point][0];
    double y = event.scifiUSTrackerPoints[point][1];
    double z = event.scifiUSTrackerPoints[point][2];
    if (z != 5673390) {
      writer->addPrimitive();
      writer->addPoint(x, y, z);
    }
  }

  // - plot TKD track points in HepRApp
  writer->addType("TKDTrackPoints", 1);
  writer->addInstance();
  writer->addAttValue("MarkName", "Cross");
  writer->addAttValue("MarkColor", 1., 1., 0.);
  for (int point = 0; point < 5; ++point) {
    double x = event.scifiDSTrackerPoints[point][0];
    double y = event.scifiDSTrackerPoints[point][1];
    double z = event.scifiDSTrackerPoints[point][2];
    if (z != 5673390) {
      writer->addPrimitive();
      writer->addPoint(x, y, z);
    }
  }

  // - define space points
  writer->addType("SpacePoints", 0);
  writer->addInstance();
  writer->addAttValue("DrawAs", "Point");

  // - plot TKU space points in HepRApp
  writer->addType("TKUSpacePoints", 1);
  writer->addInstance();
  writer->addAttValue("MarkName", "Cross");
  writer->addAttValue("MarkColor", 1., 0.3, 0.);
  for (std::vector<MAUS::ThreeVector>::iterator it = event.scifiUSTrackerSpacePoints.begin(),
       end = event.scifiUSTrackerSpacePoints.end(); it != end; ++it) {
    double x = it->x();
    double y = it->y();
    double z = it->z();
    if (z != 5673390) { // should be 5673390
      writer->addPrimitive();
      writer->addPoint(x, y, z);
    }
  }

  // - plot TKD space points in HepRApp
  writer->addType("TKDSpacePoints", 1);
  writer->addInstance();
  writer->addAttValue("MarkName", "Cross");
  writer->addAttValue("MarkColor", 1., 0.3, 0.);
  for (std::vector<MAUS::ThreeVector>::iterator it = event.scifiDSTrackerSpacePoints.begin(),
       end = event.scifiDSTrackerSpacePoints.end(); it != end; ++it) {
    double x = it->x();
    double y = it->y();
    double z = it->z();
    if (z != 5673390) { // should be 5673390
      writer->addPrimitive();
      writer->addPoint(x, y, z);
    }
  }

  // - define straight track
  writer->addType("StraightTrack", 0);
  writer->addInstance();
  writer->addAttValue("DrawAs", "Line");
  writer->addAttValue("LineColor", 1., 0., 0.);

  // - plot TKU straight track(s)
  writer->addType("TKUStraightTrack", 1);
  writer->addInstance();
  writer->addPrimitive();
  for (int point = 0; point < 2; ++point) {
    double x = event.scifiUSTrackerStraightTrackPoints[point][0];
    double y = event.scifiUSTrackerStraightTrackPoints[point][1];
    double z = event.scifiUSTrackerStraightTrackPoints[point][2];
    if (z != 5673390) {
      writer->addPoint(x, y, z);
    }
  }

  // - plot TKD straight track(s)
  writer->addType("TKDStraightTrack", 1);
  writer->addInstance();
  writer->addPrimitive();
  for (int point = 0; point < 2; ++point) {
    double x = event.scifiDSTrackerStraightTrackPoints[point][0];
    double y = event.scifiDSTrackerStraightTrackPoints[point][1];
    double z = event.scifiDSTrackerStraightTrackPoints[point][2];
    if (z != 5673390) {
      writer->addPoint(x, y, z);
    }
  }

  writer->close();

  char *detGeomFileName = getenv("EV_DETGEOM_FILE");
  if (detGeomFileName == NULL) { // disable writing to HepRep options if this is not selected
    std::cerr << "WARNING: Detector geometry file not defined! Modify and source configure.sh or "
              << "choose geometry file using selection button (if using GUI). "
              << "Not writting output heprep file!" << std::endl;
  } else {
    std::string sOutFileName = ssOutFileName.str();
    std::string sDetGeomFileName(detGeomFileName);
    Concatenate(sDetGeomFileName, dataFileName, sOutFileName);
  }

  // - remove MAUS data heprep file
  std::stringstream cmdRemove;
  cmdRemove << "rm " << dataFileName;
  system(cmdRemove.str().c_str());

  // - open the file for viewing
  if (display) {
    std::cout << display << std::endl;
    std::stringstream cmdDisplay;
    int displayCheck = 1;

    char *javaDir = getenv("EV_JAVA_DIR");
    if (javaDir != NULL) {
      cmdDisplay << javaDir << "/bin/java -jar ";
    } else {
      std::cerr << "WARNING: No java directory selected! HepRApp display unavailable! "
                << "Select directory in which your JRE can be found!" << std::endl;
      displayCheck = 0;
    }

    char *heprappDir = getenv("EV_HEPRAPP_DIR");
    if (heprappDir != NULL) {
      cmdDisplay << heprappDir << "/HepRApp.jar -file ";
    } else {
      std::cerr << "WARNING: No HepRapp directory selected! HepRApp display unavailable! "
                << "Select directory in which your HepRApp.jar can be found!" << std::endl;
      displayCheck = 0;
    }

    cmdDisplay << ssOutFileName.str();
    std::cout << cmdDisplay.str() << std::endl;
    if (displayCheck)
      system(cmdDisplay.str().c_str());
  }
}

int EVHepRepExporter::Concatenate(std::string& geometryFileName, std::string& dataFileName,
                                  std::string& outFileName) {
  std::ifstream geomFile;
  geomFile.open(geometryFileName.c_str(), std::ifstream::in);
  if (!geomFile.is_open()) {
    std::cerr << "ERROR: Could not open geometry file needed for output to heprep!" << std::endl;
    return 0;
  }

  std::ifstream dataFile;
  dataFile.open(dataFileName.c_str(), std::fstream::in);
  if (!dataFile.is_open()) {
    std::cerr << "ERROR: Could not open data file needed for output to heprep!" << std::endl;
    return 0;
  }

  std::ofstream outFile;
  outFile.open(outFileName.c_str(), std::ifstream::out);
  if (!outFile.is_open()) {
    std::cerr << "ERROR: Could not open temporary file needed for output to heprep!" << std::endl;
    return 0;
  }


  while (!geomFile.eof()) {
    std::string line, target;
    std::getline(geomFile, line);
    target = line.substr(0, 16);
    if (target != "</heprep:heprep>")
      outFile << line << std::endl;
  }

  int counter = 0;
  while (!dataFile.eof()) {
    ++counter;
    std::string line;
    std::getline(dataFile, line);
    if (counter > 3)
      outFile << line << std::endl;
  }

  return 1;
}

}  // ~namespace EventViewer

