import os
import unittest
import ROOT

from MAUS import ReduceCppTiltedHelix

def generate_scifi_data():
    data = ROOT.MAUS.Data()
    spill = ROOT.MAUS.Spill()
    recon_events = ROOT.MAUS.ReconEventPArray()        
    recon_events.push_back(ROOT.MAUS.ReconEvent())
    scifi_space_points = ROOT.MAUS.SciFiSpacePointPArray()
    for i in range(5):
        sp = ROOT.MAUS.SciFiSpacePoint()
        scifi_space_points.push_back(sp)
    scifi_event = ROOT.MAUS.SciFiEvent()
    scifi_event.set_spacepoints(scifi_space_points)
    recon_events[0].SetSciFiEvent(scifi_event)        
    spill.SetReconEvents(recon_events)
    data.SetSpill(spill)
    return data

class TestReduceCppTiltedHelix(unittest.TestCase):
    def setUp(self):
        self.image_path = "${MAUS_TMP_DIR}/test_reduce_cpp_tilted_helix.png"
        self.image_path = os.path.expandvars(self.image_path)

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
        data = generate_scifi_data()
        image = reducer.process(data)
        canvas_wrappers = image.GetImage().GetCanvasWrappers()
        for wrap in canvas_wrappers:
            print wrap.GetFileTag()
            wrap.GetCanvas().Print(self.image_path)

if __name__ == "__main__":
    unittest.main()

