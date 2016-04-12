import sys
import os
import site
import json

import numpy
import ROOT
analysis_path = os.environ["MAUS_ROOT_DIR"]+"/bin/user/fit_fields/scripts/"
site.addsitedir(analysis_path)
import xboa.common
import Configuration
import maus_cpp.global_error_tracking
import maus_cpp.globals
import maus_cpp.field

import scripts.data_loader
import scripts.config_extrapolate_through_detectors

class ExtrapolateTrackPoints(object):
    def __init__(self, config):
        self.config = config
        self.current_space_points = [] # list of space points in current event
        self.fitted_track_points = [] # list of fitted space points in current event
        self.best_guess = None
        self.iteration = 0
        self.mu_mass = xboa.common.pdg_pid_to_mass[13]
        self.setup_maus_config()
        self.residual_dicts = {}
        self.weighted_residual_dicts = {}
        self.in_cut_residual_dicts = {}
        self.in_cut_weighted_residual_dicts = {}
       
    def setup_maus_config(self):
        self.tracking = maus_cpp.global_error_tracking.GlobalErrorTracking()
        self.tracking.set_step_size(10.)
          
        maus_cpp.global_error_tracking.enable_material("AIR")
        maus_cpp.global_error_tracking.enable_material("POLYSTYRENE")
        maus_cpp.global_error_tracking.enable_material("POLYCARBONATE")
        maus_cpp.global_error_tracking.enable_material("BC600")
        # "Al", "STEEL", "lXe", "ALUMINUM" [sic], "Galactic"

    def get_point(self, event, detector):
        if len(event) == 0:
            raise ValueError("Event was empty")
        for rec_point in event:
            if rec_point["detector"] == detector:
                break
        if rec_point["detector"] != detector:
            raise ValueError("Failed to find rec point for "+str(detector))
        return rec_point

    def make_rec(self, value, ellipse, detector):
        rec_point = {
            "t":value[0],
            "x":value[1],
            "y":value[2],
            "z":value[3],
            "px":value[5],
            "py":value[6],
            "pz":value[7],
            "detector":self.global_key(detector),
            "covariance":ellipse,
        }
        return rec_point

    def global_key(self, detector):
        return "global_"+detector

    def extrapolate_event(self, event):
        event = sorted(event, key = lambda tp: tp["z"]) # check that it is sorted
        tku_rec = self.get_point(event, "tku")
        tof1_rec = self.get_point(event, "tof1")
        tof0_rec = self.get_point(event, "tof0")
        energy = (tku_rec["px"]**2 + tku_rec["py"]**2 + tku_rec["pz"]**2 + self.mu_mass**2)**0.5
        seed = [0.,     tku_rec["x"],  tku_rec["y"],  tku_rec["z"],
                energy, tku_rec["px"], tku_rec["py"], tku_rec["pz"],]
        ellipse = tku_rec["covariance"]
        self.tracking.set_energy_loss_model("bethe_bloch_backwards")
        self.tracking.set_scattering_model("moliere_backwards")
        try:
            tof1_point, tof1_error = self.tracking.propagate_errors(seed, ellipse, tof1_rec["z"])
            seed[0] = tof1_rec["t"]-tof1_point[0]
        except RuntimeError:
            pass
        try:
            tof1_point, tof1_error = self.tracking.propagate_errors(seed, ellipse, tof1_rec["z"])
            event.append(self.make_rec(tof1_point, tof1_error, "tof1"))
        except RuntimeError:
            pass
        try:
            tof0_point, tof0_error = self.tracking.propagate_errors(seed, ellipse, tof0_rec["z"])
            event.append(self.make_rec(tof0_point, tof0_error, "tof0"))
        except RuntimeError:
            pass
        return event

    def alt_min_max(self, float_list, n_sigma):
        length = len(float_list)+1
        while len(float_list) < length:
            mean = numpy.mean(float_list)
            sigma = numpy.std(float_list)
            min_max = [
                mean-n_sigma*sigma,
                mean+n_sigma*sigma,
            ]
            length = len(float_list)
            float_list = [x for x in float_list if x < min_max[1] and x > min_max[0]]
        return min_max

    def append_residual(self, event, detector, axis):
        if event["passed_cuts"]:
            passed_cuts = True
        else:
            passed_cuts = False
        event = event["data"]
        if detector not in self.residual_dicts:
            self.residual_dicts[detector] = {}
            self.weighted_residual_dicts[detector] = {}
            self.in_cut_residual_dicts[detector] = {}
            self.in_cut_weighted_residual_dicts[detector] = {}
        if axis not in self.residual_dicts[detector]:
            self.residual_dicts[detector][axis] = []
            self.weighted_residual_dicts[detector][axis] = []
            self.in_cut_residual_dicts[detector][axis] = []
            self.in_cut_weighted_residual_dicts[detector][axis] = []
        
        residual_list = self.residual_dicts[detector][axis]
        weighted_residual_list = self.weighted_residual_dicts[detector][axis]
        in_cut_residual_list = self.in_cut_residual_dicts[detector][axis]
        in_cut_weighted_residual_list = self.in_cut_weighted_residual_dicts[detector][axis]
        try:
            det_rec = self.get_point(event, detector)
            glob_rec = self.get_point(event, self.global_key(detector))
            residual = det_rec[axis]-glob_rec[axis]
            residual_list.append(residual)
            if passed_cuts:
                in_cut_residual_list.append(residual)
            if axis in self.cov_key_list:
                key = self.cov_key_list.index(axis)
                sigma = (glob_rec["covariance"][key][key]+det_rec["covariance"][key][key])**0.5
                weighted_residual = residual/sigma
                weighted_residual_list.append(weighted_residual)
                if passed_cuts:
                    in_cut_weighted_residual_list.append(weighted_residual)
        except ValueError:
            pass

    def print_canvas(self, canvas, name):
        plot_name = "plots/"+name.replace(' ', '_')
        for format in "png", "pdf", "root":
            canvas.Print(plot_name+"."+format)

    text_boxes = []
    def get_text_box(self, residual_list, in_cut_residual_list):
        text_box = ROOT.TPaveText(0.6, 0.5, 0.9, 0.9, "NDC")
        text_box.SetFillColor(0)
        text_box.SetBorderSize(0)
        text_box.SetTextSize(0.04)
        text_box.SetTextAlign(12)
        text_box.SetTextSize(0.03)
        text_box.AddText("All events (black)")
        text_box.AddText("Number: "+str(len(residual_list)))
        text_box.AddText("Mean:   "+str(numpy.mean(residual_list)))
        text_box.AddText("Std:    "+str(numpy.std(residual_list)))
        text_box.AddText("In Cut (blue)")
        text_box.AddText("Number: "+str(len(in_cut_residual_list)))
        text_box.AddText("Mean:   "+str(numpy.mean(in_cut_residual_list)))
        text_box.AddText("Std:    "+str(numpy.std(in_cut_residual_list)))
        text_box.SetBorderSize(1)
        text_box.Draw()
        self.text_boxes.append(text_box)
        return text_box

    def plot_residuals(self, detector, axis):
        residual_list = self.residual_dicts[detector][axis]
        weighted_residual_list = self.weighted_residual_dicts[detector][axis]
        in_cut_residual_list = self.in_cut_residual_dicts[detector][axis]
        in_cut_weighted_residual_list = self.in_cut_weighted_residual_dicts[detector][axis]
       
        nbins = self.config.residuals_plots_nbins
        name = "residuals - "+detector+" "+axis
        label = "Res("+axis+") ["+self.units[axis]+"]"
        min_max = self.alt_min_max(residual_list, 10)
        canvas = xboa.common.make_root_canvas(name)
        hist = xboa.common.make_root_histogram(name, residual_list, label, nbins, xmin = min_max[0], xmax = min_max[1])
        hist.SetTitle(detector+": "+axis)
        hist.Draw()
        hist = xboa.common.make_root_histogram(name, in_cut_residual_list, label, nbins, xmin = min_max[0], xmax = min_max[1])
        hist.SetTitle(detector+": "+axis)
        hist.SetLineColor(4)
        hist.Draw("SAME")
        self.get_text_box(residual_list, in_cut_residual_list)
        canvas.Update()
        self.print_canvas(canvas, name)
        name = "normalised "+name
        canvas = xboa.common.make_root_canvas(name)
        min_max = self.alt_min_max(weighted_residual_list, 10)
        hist = xboa.common.make_root_histogram(name, weighted_residual_list, "Res("+axis+")/#sigma("+axis+")", nbins, xmin = min_max[0], xmax = min_max[1])
        hist.SetTitle(detector+": "+axis)
        hist.Draw()
        hist = xboa.common.make_root_histogram(name, in_cut_weighted_residual_list, "Res("+axis+")/#sigma("+axis+")", nbins, xmin = min_max[0], xmax = min_max[1])
        hist.SetTitle(detector+": "+axis)
        hist.SetLineColor(4)
        hist.Draw("SAME")
        self.get_text_box(weighted_residual_list, in_cut_weighted_residual_list)
        canvas.Update()


        self.print_canvas(canvas, name)
        

    units = {"t":"ns", "x":"mm", "y":"mm", "energy":"MeV", "px":"MeV/c", "py":"MeV/c", "pz":"MeV/c"}
    cov_key_list = ["t", "x", "y", "energy", "px", "py"]
    mu_mass = xboa.common.pdg_pid_to_mass[13]

def maus_globals(config):
    str_conf = Configuration.Configuration().\
                                      getConfigJSON(command_line_args=False)
    json_conf = json.loads(str_conf)
    json_conf["simulation_geometry_filename"] = config.geometry
    maus_conf = json.dumps(json_conf)
    maus_cpp.globals.birth(maus_conf)
    print maus_cpp.field.str(True)


def main():
    config = scripts.config_extrapolate_through_detectors.Config()
    maus_globals(config)
    extrapolate = ExtrapolateTrackPoints(config)
    min_spill = 0
    all_event_count = 0
    all_event_in_cut_count = 0
    for max_spill in range(100, config.number_of_spills, 100):
        data_loader = scripts.data_loader.DataLoader(config)
        if max_spill + 100 > config.number_of_spills:
            max_spill = config.number_of_spills
        data_loader.load_data(min_spill, max_spill)
        progress = 0.1
        for i, event in enumerate(data_loader.events):
            all_event_count += 1
            if i/float(len(data_loader.events)) > progress:
                print "Done", i, "of", len(data_loader.events)
                progress += 0.1
            if event["passed_cuts"]:
                all_event_in_cut_count += 1
            try:
                event["data"] = extrapolate.extrapolate_event(event["data"])     
            except ValueError:
                pass
            for detector in "tof0", "tof1":
                for axis in "x", "y", "t":
                    extrapolate.append_residual(event, detector, axis)
        print "all events:", all_event_count, \
              "all events in cuts:", all_event_in_cut_count, \
              "extrapolated:", len(extrapolate.residual_dicts["tof1"]["t"]), \
              "extrapolated in cuts:", len(extrapolate.in_cut_residual_dicts["tof1"]["t"])
        xboa.common.clear_root()
        min_spill = max_spill
        for detector in "tof0", "tof1":
            for axis in "x", "y", "t":
                extrapolate.plot_residuals(detector, axis)      

if __name__ == "__main__":
    main()
    print "Done - press <Enter> to finish"
    raw_input()

