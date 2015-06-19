import os
import unittest
import ROOT
import math
import numpy
import json

import maus_cpp.globals
import Configuration
from MAUS import ReduceCppTiltedHelix

def generate_scifi_data(n_recon_events, radius, wave_length, centre, noise, dxdz, dydz):
    print "generate scifi data"
    z_positions = [0., 200., 450., 750., 1100.]
    data = ROOT.MAUS.Data()
    spill = ROOT.MAUS.Spill()
    recon_events = ROOT.MAUS.ReconEventPArray()        
    for i in range(n_recon_events):
        recon_events.push_back(ROOT.MAUS.ReconEvent())
        scifi_space_points = ROOT.MAUS.SciFiSpacePointPArray()
        for tracker in range(2):
            l = wave_length #numpy.random.normal(wave_length, wave_length/10.)
            r = radius #abs(numpy.random.normal(0., radius))
            x0 = numpy.random.normal(0., centre[0])
            y0 = numpy.random.normal(0., centre[1])
            z0 = numpy.random.uniform(0., 2.*math.pi) # centre[2]/wave_length*2.*math.pi
            for station, z_pos in enumerate(z_positions):
                sp = ROOT.MAUS.SciFiSpacePoint()
                sp.set_tracker(tracker)
                sp.set_station(station)
                pos = ROOT.MAUS.ThreeVector()
                x_pos = r*math.cos(2.*math.pi*(z_pos/l)+z0)+x0 + dxdz*z_pos
                x_pos += numpy.random.normal(0., noise)
                y_pos = r*math.sin(2.*math.pi*(z_pos/l)+z0)+y0 + dydz*z_pos
                y_pos += numpy.random.normal(0., noise)
                pos.setX(x_pos)
                pos.setY(y_pos)
                pos.setZ(z_pos)
                sp.set_position(pos)
                scifi_space_points.push_back(sp)
        scifi_event = ROOT.MAUS.SciFiEvent()
        scifi_event.set_spacepoints(scifi_space_points)
        recon_events[i].SetSciFiEvent(scifi_event)        
    spill.SetReconEvents(recon_events)
    data.SetSpill(spill)
    return data

class TestReduceCppTiltedHelix(unittest.TestCase):
    def setUp(self):
        self.image_path = "${MAUS_TMP_DIR}/test_reduce_cpp_tilted_helix.png"
        self.image_path = os.path.expandvars(self.image_path)
        config = Configuration.Configuration().getConfigJSON()
        config = json.loads(config)
        config["SciFiRadiusResCut"] = 1e6
        config["SciFiPatRecCircleChi2Cut"] = 1e6
        config["SciFiNTurnsCut"] = 1e6
        config["SciFiPatRecSZChi2Cut"] = 1e6
        config["SciFiMaxPt"] = 1e6
        config["SciFiMinPz"] = 1e6
        maus_cpp.globals.birth(json.dumps(config))

    def tearDown(self):
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()

    def test_init(self):
        reducer = ReduceCppTiltedHelix()
        reducer.birth("")
        reducer.death()
        reducer.death()

    def test_reduce_bad_event(self):
        reducer = ReduceCppTiltedHelix()
        reducer.birth("")
        try:
            image = reducer.process(None)
            self.assertTrue(False, msg="Should have thrown")
        except RuntimeError:
            pass

    def test_reduce_perfect_helix(self):
        reducer = ReduceCppTiltedHelix()
        reducer.birth("")
        for i in range(10):
            data = generate_scifi_data(1000, 20., 800., [30., 40., 200.], 0.5, 3.e-3, 0.)
            image = reducer.process(data)
        canvas_wrappers = image.GetImage().GetCanvasWrappers()
        for wrap in canvas_wrappers:
            print wrap.GetFileTag()
            wrap.GetCanvas().Print(self.image_path)
        raw_input()

if __name__ == "__main__":
    unittest.main()

