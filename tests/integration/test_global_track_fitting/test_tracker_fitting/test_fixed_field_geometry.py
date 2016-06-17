import os
import subprocess
import unittest
import copy

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
        self.verbose = False
        self.event_display_events = [(0, 0)]
        self.do_event_display = False
        self.event_display_event = {}
        self.bounds_from_fit = False

        self.load_data(file_name)
        if len(self.events) == 0:
            raise RuntimeError("Failed to load any data")


    def plot_1d(self, var_1, el_1, canvas = None, color = 1):

        name = el_1
        values_1 = [event[var_1][el_1] for event in self.events]
        axis_1 = el_1
        fit = ROOT.TF1("fit", "gaus")

        if self.bounds_from_fit:
            hist = xboa.common.make_root_histogram(name, values_1, axis_1, 100)
            hist.Fit(fit, "N")
            mean = fit.GetParameter(1)
            std = fit.GetParameter(2)
        else:
            mean = numpy.mean(values_1)
            std = numpy.std(values_1)
        print "Axis range mean", mean, "std", std

        min_var = mean-self.n_std*std
        max_var = mean+self.n_std*std
        out_of_range = sum([1 for val in values_1 if val < min_var or val > max_var])

        hist = xboa.common.make_root_histogram(name, values_1, axis_1, 100, xmin=min_var, xmax=max_var)
        max_content = hist.GetMaximum()
        if abs(max_content) < 1e-9:
            max_content = 1.
        print name, max_content
        for bin in range(hist.GetNbinsX()+2):
            hist.SetBinContent(bin, hist.GetBinContent(bin)/max_content)
        hist.SetLineColor(color)
        if canvas == None:
            y_min = 0.6
            canvas = xboa.common.make_root_canvas(name)
            canvas.Draw()
            hist.Draw()
        else:
            y_min = 0.3
            canvas.cd()
            hist.Draw("SAME")
        y_max = y_min + 0.29
        fit = ROOT.TF1("fit", "gaus")
        fit.SetLineColor(color)
        hist.Fit(fit)
        text_box = ROOT.TPaveText(0.6, y_min, 0.89, y_max, "NDC")
        text_box.SetFillColor(0)
        text_box.SetBorderSize(0)
        text_box.SetTextSize(0.04)
        text_box.SetTextAlign(12)
        text_box.SetTextSize(0.03)
        text_box.SetTextColor(color)
        text_box.AddText(var_1)
        #text_box.SetTextColor(1)
        text_box.AddText("Number:         "+str(len(values_1)))
        text_box.AddText("Over+underflow: "+str(out_of_range))
        text_box.AddText("Mean:           "+str(fit.GetParameter(1)))
        text_box.AddText("Std:            "+str(fit.GetParameter(2)))
        text_box.Draw()
        self.root_objects.append(fit)
        self.root_objects.append(text_box)
        canvas.Update()
        for format in "root", "png", "pdf":
            name = name.replace(' ', '_')
            canvas.Print("plots/"+name+"."+format)
        return canvas

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
                print "spill", spill_number, "with", spill.GetReconEvents().size(), "events"
                for ev_number, reco_event in enumerate(spill.GetReconEvents()):
                    self.setup_event_display(spill_number, ev_number)
                    scifi_event = reco_event.GetSciFiEvent()
                    global_event = reco_event.GetGlobalEvent()
                    sf_hit = self.load_scifi_event(scifi_event)
                    glob_hit = self.load_global_event(global_event)
                    mc_event = spill.GetMCEvents()[ev_number]
                    vhit = self.load_virtual_hits(mc_event)
                    if self.do_event_display == True:
                        self.make_event_display()
                    if sf_hit == None or vhit == None or glob_hit == None:
                        print "    Failed for spill", spill_number, "event", ev_number, "sf ok?", sf_hit != None, "glob ok?", glob_hit != None, "vhit ok?", vhit != None
                        continue
                    sf_residual = [(key, sf_hit[key] - vhit[key]) for key in vhit.keys()]
                    sf_residual = dict(sf_residual)
                    glob_residual = [(key, glob_hit[key] - vhit[key]) for key in vhit.keys()]
                    glob_residual = dict(glob_residual)
                    self.events.append({"virtual_hit":vhit, "sf_hit":sf_hit, "glob_hit":glob_hit, "scifi":sf_residual, "global":glob_residual})
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

    def setup_event_display(self, spill_number, event_number):
        sample_event = {"x":[], "y":[], "z":[], "px":[], "py":[], "pz":[], "p":[]}
        if (spill_number, event_number) in self.event_display_events:
            self.event_display_event = {
                "glob_hit":copy.deepcopy(sample_event),
                "sf_hit":copy.deepcopy(sample_event),
                "virtual_hit":copy.deepcopy(sample_event)
            }
            self.do_event_display = True
        else:
            self.do_event_display = False

    def make_event_display(self):
        for x_axis, y_axis in [("z", "x"), ("z", "y"), ("z", "p")]:
            x_all_list = []
            y_all_list = []
            for i, hit_type in enumerate(["sf_hit", "glob_hit", "virtual_hit"]):
                x_all_list += self.event_display_event[hit_type][x_axis]
                y_all_list += self.event_display_event[hit_type][y_axis]
            canvas = xboa.common.make_root_canvas("Event display "+x_axis+" "+y_axis)
            hist, graph = xboa.common.make_root_graph(x_axis+" "+y_axis, x_all_list, x_axis, y_all_list, y_axis)
            canvas.Draw()
            hist.Draw()
            for i, hit_type in enumerate(["sf_hit", "glob_hit", "virtual_hit"]):
                name = hit_type+" "+x_axis+" "+y_axis
                x_list = self.event_display_event[hit_type][x_axis]
                y_list = self.event_display_event[hit_type][y_axis]
                if len(x_list) == 0:
                    continue # oops, maybe the event was not reconstructed?
                hist, graph = xboa.common.make_root_graph(name, x_list, x_axis, y_list, y_axis)
                graph.SetLineColor(2**(i+1))
                graph.SetMarkerStyle(24+i)
                graph.SetMarkerColor(2**(i+1))
                graph.Draw("SAMEPL")
                canvas.Update()
                for format in "png", "root", "pdf":
                    canvas.Print("plots/event-display_"+x_axis+"-"+y_axis+"."+format)

    def load_global_event(self, global_event):
        z_list = []
        for i, track_point in enumerate(global_event.get_track_points()):
            if self.verbose:
                print "global track"
            pos = track_point.get_position()
            z_list.append([abs(pos.Z()-self.target_z), track_point])
            if self.verbose:
                print "global hit", pos.X(), pos.Y(), pos.Z()
            if self.do_event_display:
                mom = track_point.get_momentum()
                event = {"x":pos.X(), "y":pos.Y(), "z":pos.Z(),
                         "px":mom.X(), "py":mom.Y(), "pz":mom.Z(),
                         "p":(mom.X()**2+mom.Y()**2+mom.Z()**2)**0.5}
                for key in event.keys():
                    self.event_display_event["glob_hit"][key].append(event[key])
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
                if self.do_event_display:
                    mom = track_point.mom()
                    event = {"x":pos.X(), "y":pos.Y(), "z":pos.Z(),
                             "px":mom.X(), "py":mom.Y(), "pz":mom.Z(),
                             "p":mom.Mag()}
                    for key in event.keys():
                        self.event_display_event["sf_hit"][key].append(event[key])
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
                print "vhit", pos.x(), pos.y(), pos.z()
            if self.do_event_display:
                mom = vhit.GetMomentum()
                event = {"x":pos.X(), "y":pos.Y(), "z":pos.Z(),
                         "px":mom.X(), "py":mom.Y(), "pz":mom.Z(),
                         "p":mom.Mag()}
                for key in event.keys():
                        self.event_display_event["virtual_hit"][key].append(event[key])
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

    @classmethod
    def tearDownClass(cls):
        os.chdir(cls.here)

    def generate_dataset(self, extra_datacards, verbose):
        log = open(self.log, "w")
        config = os.path.join("fixed_field_config.py")
        proc_list = ["python", self.sim,
                     "--configuration_file", config,
                     "--output_root_file_name", self.out]
        for key in extra_datacards:
            proc_list.append("--"+key)
            proc_list.append(str(extra_datacards[key]))
        if verbose:
            proc = subprocess.Popen(proc_list)
        else:
            proc = subprocess.Popen(proc_list,
                                    stdout = log,
                                    stderr = subprocess.STDOUT)
        proc.wait()

    def plots(self, analysis):
        x_canvas = analysis.plot_1d("scifi", "x", color = 4)
        y_canvas = analysis.plot_1d("scifi", "y", color = 4)
        px_canvas = analysis.plot_1d("scifi", "px", color = 4)
        py_canvas = analysis.plot_1d("scifi", "py", color = 4)
        pz_canvas = analysis.plot_1d("scifi", "pz", color = 4)
        analysis.plot_1d("global", "x", x_canvas, color = 8)
        analysis.plot_1d("global", "y", y_canvas, color = 8)
        analysis.plot_1d("global", "px", px_canvas, color = 8)
        analysis.plot_1d("global", "py", py_canvas, color = 8)
        analysis.plot_1d("global", "pz", pz_canvas, color = 8)

    def test_fixed_field(self):
        print "Generating data"
        #self.generate_dataset({}, True)
        print "Generated data"
        analysis = TestAnalysis(self.out, 15062.)
        self.plots(analysis)
        raw_input()

if __name__ == "__main__":
    unittest.main()
