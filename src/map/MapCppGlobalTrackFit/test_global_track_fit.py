import json
import unittest
from xboa.hit import Hit
import ROOT 
import libMausCpp
import maus_cpp.globals

import Configuration
from MAUS import MapCppGlobalTrackFit


class TestMapCppGlobalTrackFit(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.config_str = \
            Configuration.Configuration().getConfigJSON(command_line_args=False)
        config_json = json.loads(cls.config_str)
        config_json["global_track_fit_will_require_triplet_space_points"] = False
        config_json["verbose_level"] = 0
        cls.config_str = json.dumps(config_json)
        if not maus_cpp.globals.has_instance():
            maus_cpp.globals.birth(cls.config_str)

    @classmethod
    def tearDownClass(cls):
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()

    def test_init(self):
        fit = MapCppGlobalTrackFit()
        fit.birth(self.config_str)

    def transport_straight_track(self, z_pos, seed):
        track = seed.deepcopy()
        track["t"] = seed["t'"]*(z_pos-seed["z"]) + seed["x"]
        track["x"] = seed["x'"]*(z_pos-seed["z"]) + seed["x"]
        track["y"] = seed["y'"]*(z_pos-seed["z"]) + seed["y"]
        track["z"] = z_pos
        return track

    def make_scifi_space_points(self, seed, scifi_event):
        space_point_array = scifi_event.spacepoints()
        cluster = ROOT.MAUS.SciFiCluster()
        array = ROOT.TRefArray()
        for i in range(3):
            array.Add(cluster)
        for i, z in enumerate([15000., 15100., 15300., 15600., 16100., 20000., 20100., 20300., 20600., 21100.]):
            station = i % 5 + 1
            if station > 5:
                station -= 5
            tracker = i/5
            hit = self.transport_straight_track(z, seed)
            pos = ROOT.MAUS.ThreeVector(hit["x"], hit["y"], hit["z"])

            space_point = ROOT.MAUS.SciFiSpacePoint()
            space_point.set_tracker(tracker)
            space_point.set_station(station)
            space_point.set_global_position(pos)
            space_point.set_channels(array)
            space_point_array.push_back(space_point)
            print "Triplet check", space_point_array[i].get_channels().GetEntries()
        scifi_event.set_spacepoints(space_point_array)            


    def make_tof_space_points(self, seed, tof_event):
        space_point_list = []
        for station, z in enumerate([4000., 12000., 22000.]):
            hit = self.transport_straight_track(4000., seed)
            space_point = ROOT.MAUS.TOFSpacePoint()
            space_point.SetStation(station)
            space_point.SetGlobalPosX(hit["x"])
            space_point.SetGlobalPosY(hit["y"])
            space_point.SetGlobalPosZ(hit["z"])
            space_point_list.append(space_point)
        tof_space_point = ROOT.MAUS.TOFEventSpacePoint()
        tof_space_point.GetTOF0SpacePointArrayPtr().push_back(space_point_list[0])
        tof_space_point.GetTOF1SpacePointArrayPtr().push_back(space_point_list[1])
        tof_space_point.GetTOF2SpacePointArrayPtr().push_back(space_point_list[2])
        tof_event.SetTOFEventSpacePoint(tof_space_point)

    def test_straight_track_fit(self):
        data = ROOT.MAUS.Data()
        spill = ROOT.MAUS.Spill()
        spill.SetDaqEventType("physics_event")
        recon_events = ROOT.MAUS.ReconEventPArray()
        for i in range(-2, 3):
            a_recon_event = ROOT.MAUS.ReconEvent()
            scifi_event = ROOT.MAUS.SciFiEvent()
            tof_event = ROOT.MAUS.TOFEvent()
            seed = Hit.new_from_dict({"x":0.1*i, "y":0.2*i, "z":12000.,
                                      "px":1.*i, "py":2.*i, "pz":200.+i,
                                      "mass":105.658},
                                      "energy")
            self.make_scifi_space_points(seed, scifi_event)
            a_recon_event.SetSciFiEvent(scifi_event)
            self.make_tof_space_points(seed, tof_event)
            a_recon_event.SetTOFEvent(tof_event)
            recon_events.push_back(a_recon_event)
        spill.SetReconEvents(recon_events)
        data.SetSpill(spill)

        fit = MapCppGlobalTrackFit()
        fit.birth(self.config_str)
        fit.process(data)
        print "Found ", data.GetSpill().GetReconEvents().size(), "recon events"

        for i, event in enumerate(data.GetSpill().GetReconEvents()):
            print event.GetGlobalEvent()
            print " ", "recon event", i, "found", event.GetGlobalEvent().get_tracks().size(), "global events"

                
    config_str = ""
    mu_mass = 105.658        

if __name__ == "__main__":
    unittest.main()
    raw_input()

