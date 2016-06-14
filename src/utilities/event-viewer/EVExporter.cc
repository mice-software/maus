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

#include "src/utilities/event-viewer/EVExporter.hh"

#include <vector>

#include "DataStructure/SciFiTrack.hh"
#include "DataStructure/SciFiTrackPoint.hh"

namespace EventViewer {

EVExporter::EVExporter(MAUS::Spill *sp) {
  spill = sp;
}

EVExporter::~EVExporter() {
  // Do nothing
}

void EVExporter::ReadAllEvents(int eventSelection, bool verbose) {
  // - iterate through events
  for (size_t i = 0; i < spill->GetReconEvents()->size(); ++i) {

    detectorsHit = 0;
    evEvent.Reset();
    evEvent.runNumber = spill->GetRunNumber();
    evEvent.spillNumber = spill->GetSpillNumber();
    evEvent.eventNumber = i;

    // tof_event = new MAUS::TOFEvent; // will this help?
    tof_event = (*spill->GetReconEvents())[i]->GetTOFEvent();
    if (tof_event != NULL)
      ProcessTOFEvent();

    // scifi_event = new MAUS::SciFiEvent; // will this help?
    scifi_event = (*spill->GetReconEvents())[i]->GetSciFiEvent();
    if (scifi_event != NULL)
      ProcessScifiEvent();

    // - export one EVEvent to HepRep
    // add selection of visualization output format at some point,
    // maybe as an argument to method (only HepRep supported for now)
    if (eventSelection == 0) {
      if (verbose) {
        std::cout << "EVExporter: processing spill " << evEvent.spillNumber
                  << "   event: " << evEvent.eventNumber << std::endl; // remove after checks
      }
      EVHepRepExporter exp(evEvent);
      exp.Export(0);
    } else if (eventSelection == 1 && detectorsHit == 0) {
      if (verbose) {
        std::cout << "EVExporter: processing spill " << evEvent.spillNumber
                  << "   event: " << evEvent.eventNumber << std::endl; // remove after checks
      }
      EVHepRepExporter exp(evEvent);
      exp.Export(0);
    } else if (detectorsHit == eventSelection) {
      if (verbose) {
        std::cout << "EVExporter: processing spill " << evEvent.spillNumber
                  << "   event: " << evEvent.eventNumber << std::endl; // remove after checks
      }
      EVHepRepExporter exp(evEvent);
      exp.Export(0);
    }
  }
}

int EVExporter::ReadOneEvent(int eventNumber, std::string opt, int eventSelection, int display) {
  detectorsHit = 0;
  evEvent.Reset();

  evEvent.runNumber = spill->GetRunNumber();
  evEvent.spillNumber = spill->GetSpillNumber();
  evEvent.eventNumber = eventNumber;

  if ((*spill->GetReconEvents())[evEvent.eventNumber] == NULL) {
      std::cerr << "Event " << evEvent.eventNumber << " not available in current spill!" << "\n";
      return 0;
  }

  tof_event = new MAUS::TOFEvent; // will this help?
  tof_event = (*spill->GetReconEvents())[eventNumber]->GetTOFEvent();
  if (tof_event != NULL)
    ProcessTOFEvent();

  scifi_event = new MAUS::SciFiEvent; // will this help?
  scifi_event = (*spill->GetReconEvents())[eventNumber]->GetSciFiEvent();
  if (scifi_event != NULL)
    ProcessScifiEvent();

  // add some verbosity option later?
  // std::cout << "EVExporter: processing spill " << evEvent.spillNumber
  //           << "   event: " << evEvent.eventNumber << std::endl; // remove after checks

  // - export one EVEvent to HepRep
  // add selection of visualization output format at some point,
  // maybe as an argument to method (only HepRep supported for now)
  if (opt == "exp" && eventSelection == 0) {
    std::cout << "EVExporter: processing spill " << evEvent.spillNumber
              << "   event: " << evEvent.eventNumber << std::endl; // add vebosity option?
    EVHepRepExporter exp(evEvent);
    exp.Export(display);
  } else if (opt == "exp" && eventSelection == 1 && detectorsHit == 0) {
    std::cout << "EVExporter: processing spill " << evEvent.spillNumber
              << "   event: " << evEvent.eventNumber << std::endl; // add vebosity option?
    EVHepRepExporter exp(evEvent);
    exp.Export(display);
  } else if (opt == "exp" && detectorsHit == eventSelection) {
    std::cout << "EVExporter: processing spill " << evEvent.spillNumber
              << "   event: " << evEvent.eventNumber << std::endl; // add vebosity option?
    EVHepRepExporter exp(evEvent);
    exp.Export(display);
  } else if (opt != "exp" && opt != "no_exp") {
    std::cerr << "Error! Wrong option passed to EVExporter::ReadEvent! "
              << "Options are 'exp' to export to HepRep or 'no_exp' not to"
              << std::endl; // revise later whether this is necessary?
    return 0;
  }

  if (detectorsHit > 0) {
    return detectorsHit;
  } else if (detectorsHit == 0) {
    return 1; // check whether this is working correctly for non existing events
  }
}


void EVExporter::ProcessTOFEvent() {
  // std::cout << "Processing TOF event" << std::endl; //remove after checks
  ProcessTOF0();
  ProcessTOF1();
  ProcessTOF2();
}

void EVExporter::ProcessTOF0() {

  MAUS::TOFEventSpacePoint space_points = tof_event->GetTOFEventSpacePoint();
  MAUS::TOFSpacePoint tof0_space_points;

  // - modify ReadOneEvent return value
  if (space_points.GetTOF0SpacePointArray().size() > 0)
    detectorsHit += 2;

  // 1. Loop over TOF0 space points:
  for (size_t i = 0; i < space_points.GetTOF0SpacePointArray().size(); ++i)  {

    tof0_space_points = space_points.GetTOF0SpacePointArray()[i];

    double TOF0_x = tof0_space_points.GetGlobalPosX();
    double TOF0_y = tof0_space_points.GetGlobalPosY();
    double TOF0_z = tof0_space_points.GetGlobalPosZ();

    MAUS::ThreeVector tof0Coordinates(TOF0_x, TOF0_y, TOF0_z);
    evEvent.tofPoints[0] = tof0Coordinates;
  }
}

void EVExporter::ProcessTOF1() {
  MAUS::TOFEventSpacePoint space_points = tof_event->GetTOFEventSpacePoint();
  MAUS::TOFSpacePoint tof1_space_points;

  // - modify ReadOneEvent return value
  if (space_points.GetTOF1SpacePointArray().size() > 0)
    detectorsHit += 4;

  for (size_t i = 0; i < space_points.GetTOF1SpacePointArray().size(); ++i) {

    tof1_space_points = space_points.GetTOF1SpacePointArray()[i];

    double TOF1_x = tof1_space_points.GetGlobalPosX();
    double TOF1_y = tof1_space_points.GetGlobalPosY();
    double TOF1_z = tof1_space_points.GetGlobalPosZ();

    MAUS::ThreeVector tof1Coordinates(TOF1_x, TOF1_y, TOF1_z);
    evEvent.tofPoints[1] = tof1Coordinates;
  }
}

void EVExporter::ProcessTOF2() {
  MAUS::TOFEventSpacePoint space_points = tof_event->GetTOFEventSpacePoint();
  MAUS::TOFSpacePoint tof2_space_points;

  // - modify ReadOneEvent return value
  if (space_points.GetTOF2SpacePointArray().size() > 0)
    detectorsHit += 32;

  for (size_t i = 0; i < space_points.GetTOF2SpacePointArray().size(); ++i) {

    tof2_space_points = space_points.GetTOF2SpacePointArray()[i];

    double TOF2_x = tof2_space_points.GetGlobalPosX();
    double TOF2_y = tof2_space_points.GetGlobalPosY();
    double TOF2_z = tof2_space_points.GetGlobalPosZ();

    MAUS::ThreeVector tof2Coordinates(TOF2_x, TOF2_y, TOF2_z);
    evEvent.tofPoints[2] = tof2Coordinates;
  }
}

void EVExporter::ProcessScifiEvent() {
  // Extract raw scifi cluster data
  std::vector<MAUS::SciFiCluster*> clusters = scifi_event->clusters();
  std::vector<MAUS::SciFiCluster*>::iterator clus_iter;

  for (clus_iter = clusters.begin(); clus_iter != clusters.end(); ++clus_iter) {
    MAUS::SciFiCluster* clus = (*clus_iter);
    int tracker = clus->get_tracker();
    int station = clus->get_station();
    int plane = clus->get_plane();
    MAUS::ThreeVector position = clus->get_position();
    double alpha = clus->get_alpha();

    if (tracker == 0) {
      evEvent.scifiUSTrackerClusters[plane][0].push_back(-alpha);
      evEvent.scifiUSTrackerClusters[plane][1].push_back(-position.z()); // add offset later
    } else if (tracker == 1) {
      evEvent.scifiDSTrackerClusters[plane][0].push_back(alpha);
      evEvent.scifiDSTrackerClusters[plane][1].push_back(position.z()); // add offset later
    }
  }

  // --- iterate through scifi tracks
  std::vector<MAUS::SciFiTrack*> tracks = scifi_event->scifitracks();
  std::vector<MAUS::SciFiTrack*>::iterator track_iter;

  int USPoints = 0, DSPoints = 0; // track point counters for US and DS tracker

  for (track_iter = tracks.begin(); track_iter != tracks.end(); ++track_iter) {
    std::vector<MAUS::SciFiTrackPoint*> track_points = (*track_iter)->scifitrackpoints();
    std::vector<MAUS::SciFiTrackPoint*>::iterator track_point_iter;

    // - iterate through track points
    for (track_point_iter = track_points.begin();
         track_point_iter != track_points.end(); ++track_point_iter) {

      MAUS::SciFiTrackPoint* point = (*track_point_iter);
      int tracker = point->tracker();
      int station = point->station();
      MAUS::ThreeVector position = point->pos();
      MAUS::ThreeVector momentum = point->mom();
      if (tracker == 0) {
        evEvent.scifiUSTrackerPoints[station-1] = position;
        evEvent.scifiUSTrackerPointsMomenta[station-1] = momentum;
        ++USPoints;
      } else if (tracker == 1) {
        evEvent.scifiDSTrackerPoints[station-1] = position;
        evEvent.scifiDSTrackerPointsMomenta[station-1] = momentum;
        ++DSPoints;
      }
    }
  }

  // - modify ReadOneEvent return value
  if (USPoints > 0)
    detectorsHit += 8;
  if (DSPoints > 0)
    detectorsHit += 16;

  // --- iterate through space points
  std::vector<MAUS::SciFiSpacePoint*> space_points = scifi_event->spacepoints();
  std::vector<MAUS::SciFiSpacePoint*>::iterator spoint_iter;

  for (spoint_iter = space_points.begin(); spoint_iter != space_points.end(); ++spoint_iter) {
    MAUS::SciFiSpacePoint* point = (*spoint_iter);
    int tracker = point->get_tracker();

    if (tracker == 0)
      evEvent.scifiUSTrackerSpacePoints.push_back(point->get_global_position());
    else if (tracker == 1)
      evEvent.scifiDSTrackerSpacePoints.push_back(point->get_global_position());
  }

  // --- iterate through straight tracks
  std::vector<MAUS::SciFiStraightPRTrack*> straight_tracks = scifi_event->straightprtracks();
  std::vector<MAUS::SciFiStraightPRTrack*>::iterator straight_track_iter;

  for (straight_track_iter = straight_tracks.begin();
       straight_track_iter != straight_tracks.end(); ++straight_track_iter) {
    MAUS::SciFiStraightPRTrack* strTrack = (*straight_track_iter);

    double x0 = strTrack->get_global_x0();
    double mx = strTrack->get_global_mx();
    double y0 = strTrack->get_global_y0();
    double my = strTrack->get_global_my();

    double what = strTrack->get_x0();

    double extra = 50;
    double zminUS = 13958 - extra;
    double zmaxUS = 15059 + extra;
    double zminDS = 18854 - extra;
    double zmaxDS = 19955 + extra;

    int tracker = strTrack->get_tracker();
    if (tracker == 0) {
      double xFirst = mx*zminUS + x0;
      double yFirst = my*zminUS + y0;
      MAUS::ThreeVector first(xFirst, yFirst, zminUS);
      evEvent.scifiUSTrackerStraightTrackPoints[0] = first;

      double xSecond = mx*zmaxUS + x0;
      double ySecond = my*zmaxUS + y0;
      MAUS::ThreeVector second(xSecond, ySecond, zmaxUS);
      evEvent.scifiUSTrackerStraightTrackPoints[1] = second;
    } else if (tracker == 1) {
      double xFirst = mx*zminDS + x0;
      double yFirst = my*zminDS + y0;
      MAUS::ThreeVector first(xFirst, yFirst, zminDS);
      evEvent.scifiDSTrackerStraightTrackPoints[0] = first;

      double xSecond = mx*zmaxDS + x0;
      double ySecond = my*zmaxDS + y0;
      MAUS::ThreeVector second(xSecond, ySecond, zmaxDS);
      evEvent.scifiDSTrackerStraightTrackPoints[1] = second;
    }
  }
}

} // ~namespace EventViewer
