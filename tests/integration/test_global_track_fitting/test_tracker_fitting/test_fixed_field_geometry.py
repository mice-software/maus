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
                    sf_hit = self.load_scifi_event(scifi_event)
                    mc_event = spill.GetMCEvents()[ev_number]
                    vhit = self.load_virtual_hits(mc_event)
                    if sf_hit == None or vhit == None:
                        print "    Failed for spill", spill_number, "event", ev_number, "sf ok?", sf_hit != None, "vhit ok?", vhit != None
                        continue
                    residual = [(key, sf_hit[key] - vhit[key]) for key in vhit.keys()]
                    residual = dict(residual)
                    self.events.append({"virtual_hit":vhit, "sf_hit":sf_hit, "residual":residual})
                    if self.verbose or (spill_number == 0 and ev_number == 0):
                        print "virtual hit",
                        for key in sorted(vhit.keys()):
                            print key, str(round(vhit[key], 4)).rjust(8),
                        print "\nscifi hit",
                        for key in sorted(sf_hit.keys()):
                            print key, str(round(sf_hit[key], 4)).rjust(8),
                        print "\nresidual hit",
                        for key in sorted(residual.keys()):
                            print key, str(round(residual[key], 4)).rjust(8),
                        print

    def load_global_event(self, global_event):
        z_list = []
        for i, track in enumerate(global_event.get_tracks()):
            for j, track_point in enumerate(track.GetTrackPoints()):
                pos = track_point.pos()
                z_list.append([abs(pos.z()-self.target_z), track_point])
                if self.verbose:
                    print "sf hit", pos.x(), pos.y(), pos.z()
        z_list = sorted(z_list)
        if len(z_list) > 0 and z_list[0][0] < 1.:
            track_point = z_list[0][1]
            pos = track_point.pos()
            mom = track_point.mom()
            return {"x":pos.x(), "y":pos.y(), "z":pos.z(), "xp":mom.x()/mom.z(), "yp":mom.y()/mom.z()}


    def load_scifi_event(self, scifi_event):
        z_list = []
        for i, digit in enumerate(scifi_event.digits()):
            print "sf digit", digit.get_tracker(), digit.get_station(), digit.get_plane()
        for i, cluster in enumerate(scifi_event.clusters()):
            print "sf cluster", cluster.get_tracker(), cluster.get_station(), cluster.get_plane()
        for i, spacepoint in enumerate(scifi_event.spacepoints()):
            pos = spacepoint.get_position()
            print "sf spacepoint hit", pos.x(), pos.y(), pos.z()
        for i, track in enumerate(scifi_event.scifitracks()):
            for j, track_point in enumerate(track.scifitrackpoints()):
                pos = track_point.pos()
                z_list.append([abs(pos.z()-self.target_z), track_point])
                if self.verbose:
                    print "sf hit", pos.x(), pos.y(), pos.z()
        z_list = sorted(z_list)
        if len(z_list) > 0 and z_list[0][0] < 1.:
            track_point = z_list[0][1]
            pos = track_point.pos()
            mom = track_point.mom()
            return {"x":pos.x(), "y":pos.y(), "z":pos.z(), "xp":mom.x()/mom.z(), "yp":mom.y()/mom.z()}

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
            return {"x":pos.x(), "y":pos.y(), "z":pos.z(), "xp":mom.x()/mom.z(), "yp":mom.y()/mom.z()}

class FixedFieldGeometryTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.test_dir = os.path.expandvars("${MAUS_ROOT_DIR}/tests/"+\
                   "integration/test_global_track_fitting/test_tracker_fitting")
        cls.sim = os.path.join(cls.test_dir, "fixed_field_mc.py")
        cls.out = os.path.expandvars("${MAUS_TMP_DIR}/tmp/test_fixed_field_geometry.root")

    def generate_dataset(self):
        config = os.path.join(self.test_dir, "fixed_field_config.py")
        geometry = os.path.join(self.test_dir, "fixed_field_geometry.dat")
        proc = subprocess.Popen(["python", self.sim,
                                    "--configuration_file", config,
                                    "--simulation_geometry_filename", geometry,
                                    "--output_root_file_name", self.out])
        proc.wait()
        


    def test_fixed_field(self):
        print "Generating data"
        self.generate_dataset()
        print "Generated data"
        raw_input()
        analysis = TestAnalysis(self.out, -550.)
        analysis.plot_1d("residual", "x")
        analysis.plot_1d("residual", "y")
        analysis.plot_1d("residual", "xp")
        analysis.plot_1d("residual", "yp")
        analysis.plot_2d("virtual_hit", "x", "residual", "x")
        analysis.plot_2d("virtual_hit", "y", "residual", "y")
        analysis.plot_2d("virtual_hit", "xp", "residual", "xp")
        analysis.plot_2d("virtual_hit", "yp", "residual", "yp")


if __name__ == "__main__":
    unittest.main()
