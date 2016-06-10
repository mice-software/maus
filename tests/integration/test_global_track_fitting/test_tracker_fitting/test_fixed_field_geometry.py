import os
import subprocess
import unittest

import ROOT
import numpy
import xboa.common
import libMausCpp


class TestAnalysis(object):
    def __init__(self, file_name, target_z):
        self.events = []
        self.target_z = target_z
        self.root_objects = []
        self.n_std = 5
        self.verbose = True

        self.load_data(file_name)
        if len(self.events) == 0:
            raise RuntimeError("Failed to load any data")


    def plot_1d(self, var_1, el_1):
        name = var_1+" "+el_1
        values_1 = [event[var_1][el_1] for event in self.events]
        mean = numpy.mean(values_1)
        std = numpy.std(values_1)
        axis_1 = var_1+" "+el_1
        min_var = mean-self.n_std*std
        max_var = mean+self.n_std*std
        canvas = xboa.common.make_root_canvas(name)
        hist = xboa.common.make_root_histogram(name, values_1, axis_1, 100, xmin=min_var, xmax=max_var)
        canvas.Draw()
        hist.Draw()
        fit = ROOT.TF1("fit", "gaus")
        hist.Fit(fit)
        text_box = ROOT.TPaveText(0.6, 0.7, 0.89, 0.89, "NDC")
        text_box.SetFillColor(0)
        text_box.SetBorderSize(0)
        text_box.SetTextSize(0.04)
        text_box.SetTextAlign(12)
        text_box.SetTextSize(0.03)
        text_box.AddText("Number: "+str(len(values_1)))
        text_box.AddText("Mean:   "+str(fit.GetParameter(1)))
        text_box.AddText("Std:    "+str(fit.GetParameter(2)))
        text_box.Draw()
        self.root_objects.append(fit)
        self.root_objects.append(text_box)
        canvas.Update()
        for format in "root", "png", "pdf":
            name = name.replace(' ', '_')
            canvas.Print("plots/"+name+"."+format)

    def plot_2d(self, var_1, el_1, var_2, el_2):
        name = var_1+" "+el_1+" vs "+var_2+" "+el_2
        values_1 = [event[var_1][el_1] for event in self.events]
        values_2 = [event[var_2][el_2] for event in self.events]
        axis_1 = var_1+" "+el_1
        axis_2 = var_2+" "+el_2
        canvas = xboa.common.make_root_canvas(name)
        hist, graph = xboa.common.make_root_graph(name, values_1, axis_1, values_2, axis_2)
        canvas.Draw()
        hist.Draw()
        graph.SetMarkerStyle(6)
        graph.Draw("psame")
        canvas.Update()
        for format in "root", "png", "pdf":
            name = name.replace(' ', '_')
            canvas.Print("plots/"+name+"."+format)

    def load_data(self, file_name):
        print "Loading ROOT file", file_name
        if not os.path.exists(file_name):
            raise OSError("Could not find file "+str(file_name))
        root_file = ROOT.TFile(file_name, "READ") # pylint: disable = E1101

        print "Setting up data tree"
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = root_file.Get("Spill")
        tree.SetBranchAddress("data", data)

        print "Getting some data"
        for spill_number in range(tree.GetEntries())[:]:
            tree.GetEntry(spill_number)
            spill = data.GetSpill()
            if spill.GetDaqEventType() == "physics_event":
                print "spill", spill_number
                for ev_number, reco_event in enumerate(spill.GetReconEvents()):
                    scifi_event = reco_event.GetSciFiEvent()
                    global_event = reco_event.GetGlobalEvent()
                    sf_hit = self.load_scifi_event(scifi_event)
                    glob_hit = self.load_global_event(global_event)
                    mc_event = spill.GetMCEvents()[ev_number]
                    vhit = self.load_virtual_hits(mc_event)
                    if sf_hit == None or vhit == None or glob_hit == None:
                        print "    Failed for spill", spill_number, "event", ev_number, "sf ok?", sf_hit != None, "glob ok?", glob_hit != None, "vhit ok?", vhit != None
                        continue
                    sf_residual = [(key, sf_hit[key] - vhit[key]) for key in vhit.keys()]
                    sf_residual = dict(sf_residual)
                    glob_residual = [(key, glob_hit[key] - vhit[key]) for key in vhit.keys()]
                    glob_residual = dict(glob_residual)
                    self.events.append({"virtual_hit":vhit, "sf_hit":sf_hit, "sf_residual":sf_residual, "glob_residual":glob_residual})
                    if self.verbose or (spill_number == 0 and ev_number == 0):
                        print "virtual hit",
                        for key in sorted(vhit.keys()):
                            print key, str(round(vhit[key], 4)).rjust(8),
                        print "\nscifi hit",
                        for key in sorted(sf_hit.keys()):
                            print key, str(round(sf_hit[key], 4)).rjust(8),
                        print "\nscifi residual hit",
                        for key in sorted(sf_residual.keys()):
                            print key, str(round(sf_residual[key], 4)).rjust(8),
                        print "\nglobal residual hit",
                        for key in sorted(glob_residual.keys()):
                            print key, str(round(glob_residual[key], 4)).rjust(8),
                        print

    def load_global_event(self, global_event):
        z_list = []
        for i, track_point in enumerate(global_event.get_track_points()):
            if self.verbose:
                print "global track"
            pos = track_point.get_position()
            z_list.append([abs(pos.Z()-self.target_z), track_point])
            if self.verbose:
                print "global hit", pos.X(), pos.Y(), pos.Z()
        z_list = sorted(z_list)
        if len(z_list) > 0 and z_list[0][0] < 1.:
            track_point = z_list[0][1]
            pos = track_point.get_position()
            mom = track_point.get_momentum()
            return {"x":pos.X(), "y":pos.Y(), "z":pos.Z(), "px":mom.X(), "py":mom.Y(), "pz":mom.Z()}


    def load_scifi_event(self, scifi_event):
        z_list = []
        for i, track in enumerate(scifi_event.scifitracks()):
            for j, track_point in enumerate(track.scifitrackpoints()):
                pos = track_point.pos()
                z_list.append([abs(pos.z()-self.target_z), track_point])
                if self.verbose:
                    pass #print "sf hit", pos.x(), pos.y(), pos.z()
        z_list = sorted(z_list)
        if len(z_list) > 0 and z_list[0][0] < 1.:
            track_point = z_list[0][1]
            pos = track_point.pos()
            mom = track_point.mom()
            return {"x":pos.x(), "y":pos.y(), "z":pos.z(), "px":mom.x(), "py":mom.y(), "pz":mom.z()}

    def load_virtual_hits(self, mc_event):
        z_list = []
        for i, vhit in enumerate(mc_event.GetVirtualHits()):
            pos = vhit.GetPosition()
            if self.verbose:
                pass #print "vhit", pos.x(), pos.y(), pos.z()
            z_list.append([abs(pos.z()-self.target_z), vhit])
        z_list = sorted(z_list)
        if len(z_list) > 0 and z_list[0][0] < 1.:
            vhit = z_list[0][1]
            pos = vhit.GetPosition()
            mom = vhit.GetMomentum()
            return {"x":pos.x(), "y":pos.y(), "z":pos.z(), "px":mom.x(), "py":mom.y(), "pz":mom.z()}

class FixedFieldGeometryTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.here = os.getcwd()
        cls.test_dir = os.path.expandvars("${MAUS_ROOT_DIR}/tests/"+\
                   "integration/test_global_track_fitting/test_tracker_fitting")
        cls.sim = os.path.join(cls.test_dir, "fixed_field_mc.py")
        cls.out = os.path.expandvars("${MAUS_TMP_DIR}/tmp/test_fixed_field_geometry.root")
        cls.log = os.path.expandvars("${MAUS_TMP_DIR}/tmp/test_fixed_field_geometry.log")
        os.chdir(cls.test_dir)

    def tearDownClass(cls):
        os.chdir(here)

    def generate_dataset(self):
        log = open(self.log, "w")
        config = os.path.join("fixed_field_config.py")
        geometry = os.path.join("geometry_07469/ParentGeometryFile.dat")
        proc = subprocess.Popen(["python", self.sim,
                                    "--configuration_file", config,
                                    "--simulation_geometry_filename", geometry,
                                    "--output_root_file_name", self.out],
                      #stdout=log, stderr = subprocess.STDOUT
                )
        proc.wait()
        


    def test_fixed_field(self):
        print "Generating data"
        self.generate_dataset()
        print "Generated data"
        analysis = TestAnalysis(self.out, 15062.)
        analysis.plot_1d("sf_residual", "x")
        analysis.plot_1d("sf_residual", "y")
        analysis.plot_1d("sf_residual", "px")
        analysis.plot_1d("sf_residual", "py")
        analysis.plot_1d("sf_residual", "pz")
        analysis.plot_1d("glob_residual", "x")
        analysis.plot_1d("glob_residual", "y")
        analysis.plot_1d("glob_residual", "px")
        analysis.plot_1d("glob_residual", "py")
        analysis.plot_1d("glob_residual", "pz")
        raw_input()

if __name__ == "__main__":
    unittest.main()
