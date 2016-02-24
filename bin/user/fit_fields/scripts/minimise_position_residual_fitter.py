import json
import os

import numpy
import ROOT

from xboa.hit import Hit
import xboa.common as common
import Configuration
import maus_cpp.error_propagation
import maus_cpp.globals
import maus_cpp.field


class MinimisePositionResidualFitter(object):
    def __init__(self, config):
        self.config = config
        self.maus_conf = ""
        self.current_space_points = [] # list of space points in current event
        self.fitted_track_points = [] # list of fitted space points in current event
        self.best_guess = None
        self.iteration = 0
        self.setup_maus_config()
        self.setup_minuit()
       
    def make_seed(self, event):
        tof1 = [sp for sp in event if sp["detector"] == "tof1"][0]
        tof2 = [sp for sp in event if sp["detector"] == "tof2"][0]
        dt = (tof2["t"] - tof1["t"])
        dz = (tof2["z"] - tof1["z"])
        beta = dz/dt/self.c_light
        gamma = 1/(1-beta**2)**0.5
        seed_pz = self.mu_mass*beta*gamma

        scifi0 = [sp for sp in event if sp["detector"] == "tku"][0]
        scifi1 = [sp for sp in event if sp["detector"] == "tku"][1]
        dx = scifi0["x"]-scifi1["x"]
        dy = scifi0["y"]-scifi1["y"]
        dz = scifi0["z"]-scifi1["z"]
        seed_px = dx/dz*seed_pz
        seed_py = dy/dz*seed_pz
        energy = (seed_px**2+seed_py**2+seed_pz**2+self.mu_mass**2)**0.5
        time = (tof2["t"] - tof1["t"])/(tof2["z"]-tof1["z"])*(scifi0["z"]-tof1["z"])+tof1["t"]
        hit_dict = {
          "x":scifi0["x"], "y":scifi0["y"], "z":scifi0["z"], "t":time,
          "px":seed_px, "py":seed_py, "pz":seed_pz, "energy":energy,
          "mass":self.mu_mass, "pid":-13, 
        }
        self.best_guess = Hit.new_from_dict(hit_dict)

    def setup_maus_config(self):
        str_conf = Configuration.Configuration().\
                                          getConfigJSON(command_line_args=False)
        json_conf = json.loads(str_conf)
        json_conf["simulation_geometry_filename"] = self.config.geometry
        self.maus_conf = json.dumps(json_conf)
        maus_cpp.globals.birth(self.maus_conf)
        print maus_cpp.field.str(True)

    def setup_minuit(self):
        self.minuit = ROOT.TMinuit(6)
        self.fitter = self
        self.minuit.SetPrintLevel(2) # -1 to silence
        self.minuit.SetFCN(self.run_function)


    def to_string(self, value):
          if type(value) != type(1.):
              return str(value).ljust(10)
          else:
              return str(round(value, 3)).ljust(10)


    def print_fit(self):
        print "detector".ljust(10),
        for key in ["x", "y", "z", "t"]:
            print (key+" meas").ljust(10), (key+" fit").ljust(10), "|",
        print
        for i, space_point in enumerate(self.current_space_points):
            track = self.fitted_track_points[i]
            if track == None:
                continue
            print space_point["detector"].ljust(10),
            for key in ["x", "y", "z", "t"]:
                print self.to_string(space_point[key]), self.to_string(track[key]), "|",
            print


    def fit_track(self, event, detectors):
        self.current_space_points = event
        self.current_space_points = sorted(self.current_space_points, key = lambda sp: sp["z"])
        self.fitted_track_points = [None]*len(self.current_space_points)
        self.make_seed(event)
        self.iteration = 0
        MinimisePositionResidualFitter.fitter = self
        ierr = ROOT.Long()
        self.minuit.mnparm(0, "t",self.best_guess["t"], 1., 0., 50., ierr)
        self.minuit.mnparm(1, "x",self.best_guess["x"], 10., -150., 150., ierr)
        self.minuit.mnparm(2, "y",self.best_guess["y"], 10., -150., 150., ierr)
        self.minuit.mnparm(3, "px",self.best_guess["px"], 10., 0., 0., ierr)
        self.minuit.mnparm(4, "py",self.best_guess["py"], 10., 0., 0., ierr)
        self.minuit.mnparm(5, "pz",self.best_guess["pz"], 10., 0., 0., ierr)
        args = numpy.array([self.config.fitter_max_iterations, 1e-6], dtype=numpy.float64)
        print "Running simplex"
        self.minuit.mnexcm("SIMPLEX", args, 2, ierr)
        MinimisePositionResidualFitter.fitter = None

    def plot_bz(self, min_z = None, max_z = None):
        sp_z_list = [point["z"] for point in self.current_space_points if None not in (point["x"], point["y"])]
        tp_z_list = [point["z"] for point in self.fitted_track_points]
        z_min_max = common.min_max(sp_z_list+tp_z_list)
        if min_z == None:
            min_z = z_min_max[0]
        if max_z == None:
            max_z = z_min_max[1]

        z_list = [i*(max_z-min_z)/1000.+min_z for i in range(1001)]
        bz_list = []
        for z in z_list:
            dummy, dummy, bz, dummy, dummy, dummy = maus_cpp.field.get_field_value(0., 0., z, 0.)
            bz_list.append(bz*1e3)
        canvas = common.make_root_canvas("Bz")
        hist, graph = common.make_root_graph("Bz", z_list, "z [mm]", bz_list, "B_{z} [T]", xmin = min_z, xmax = max_z)
        hist.Draw()
        graph.Draw("lsame")
        canvas.Update()
        canvas.Print(os.path.join(self.config.plot_dir, "bz.png"))
        return canvas, hist, graph

    def display_current_event(self, plot_name, detectors = None, plot_error = False):
        if detectors == None or len(detectors) == 0:
            detectors = [point["detector"] for point in self.current_space_points]
            detectors += [point["detector"] for point in self.fitted_track_points]
        detectors = set(detectors) # remove duplicates

        sp_x_list = [point["x"] for point in self.current_space_points if None not in (point["x"], point["y"]) and point["detector"] in detectors]
        sp_y_list = [point["y"] for point in self.current_space_points if None not in (point["x"], point["y"]) and point["detector"] in detectors]
        sp_z_list = [point["z"] for point in self.current_space_points if None not in (point["x"], point["y"]) and point["detector"] in detectors]

        tp_x_list = [point["x"] for point in self.fitted_track_points if "x" in self.config.fit_detectors[point["detector"]] and point["detector"] in detectors]
        tp_xz_list = [point["z"] for point in self.fitted_track_points if "x" in self.config.fit_detectors[point["detector"]] and point["detector"] in detectors]
        tp_y_list = [point["y"] for point in self.fitted_track_points if "y" in self.config.fit_detectors[point["detector"]] and point["detector"] in detectors]
        tp_yz_list = [point["z"] for point in self.fitted_track_points if "y" in self.config.fit_detectors[point["detector"]] and point["detector"] in detectors]

        z_min_max = common.min_max(sp_z_list)

        keys = ["t", "x", "y", "z", "energy", "px", "py", "pz"]
        values = [self.best_guess[key] for key in keys]
        ellipse = [[0. for i in range(6)] for j in range(6)]
        for i in range(6):
            ellipse[i][i] = 1.

        track_list = [[values[1], values[2], values[3]]]

        z = self.best_guess["z"]
        while z < z_min_max[1]:
            values, ellipse = maus_cpp.error_propagation.propagate_errors(values, ellipse, z, 1.)
            z += (z_min_max[1] - z_min_max[0])/1000.
            track_list.append([values[1], values[2], values[3]])

        values = [self.best_guess[key] for key in keys]
        ellipse = [[0. for i in range(6)] for j in range(6)]
        for i in range(6):
            ellipse[i][i] = 1.

        z = self.best_guess["z"]-1.
        while z > z_min_max[0]:
            values, ellipse = maus_cpp.error_propagation.propagate_errors(values, ellipse, z, -1.)
            z -= (z_min_max[1] - z_min_max[0])/1000.
            track_list.append([values[1], values[2], values[3]])
        track_list = sorted(track_list, key = lambda x: x[2])
        track_x_list = [track[0] for track in track_list]
        track_y_list = [track[1] for track in track_list]
        track_z_list = [track[2] for track in track_list]

        x_min_max = common.min_max(sp_x_list+tp_x_list+track_x_list)
        y_min_max = common.min_max(sp_y_list+tp_y_list+track_y_list)

        canvas = common.make_root_canvas("Global fit - x")
        hist, graph = common.make_root_graph("Measured space points",
                      sp_z_list, "z [mm]", sp_x_list, "x [mm]",
                      xmin = z_min_max[0], xmax = z_min_max[1],
                      ymin = x_min_max[0], ymax = x_min_max[1])
        hist.Draw()
        graph.SetMarkerStyle(24)
        graph.Draw('p')
        hist, graph = common.make_root_graph("Fitted track points",
                      tp_xz_list, "z [mm]", tp_x_list, "x [mm]")
        graph.SetMarkerColor(4)
        graph.SetMarkerStyle(26)
        graph.Draw('psame')
        hist, graph = common.make_root_graph("Fitted track",
                      track_z_list, "z [mm]", track_x_list, "x [mm]")
        graph.SetLineColor(4)
        graph.Draw('lsame')
        canvas.Update()
        canvas.Print(plot_name+"_x.png")
        canvas.Print(plot_name+"_x.root")


        canvas = common.make_root_canvas("Global fit - y")
        hist, graph = common.make_root_graph("Measured space points",
                      sp_z_list, "z [mm]", sp_y_list, "y [mm]",
                      xmin = z_min_max[0], xmax = z_min_max[1],
                      ymin = y_min_max[0], ymax = y_min_max[1])
        hist.Draw()
        graph.SetMarkerStyle(24)
        graph.Draw('p')
        hist, graph = common.make_root_graph("Fitted track points",
                      tp_yz_list, "z [mm]", tp_y_list, "y [mm]")
        graph.SetMarkerColor(4)
        graph.SetMarkerStyle(26)
        graph.Draw('psame')
        hist, graph = common.make_root_graph("Fitted track",
                      track_z_list, "z [mm]", track_y_list, "y [mm]")
        graph.SetLineColor(4)
        graph.Draw('lsame')
        canvas.Update()
        canvas.Print(plot_name+"_y.png")
        canvas.Print(plot_name+"_y.root")


    @staticmethod
    def run_function(n_pars, pars, score, buff, err):
        fitter = MinimisePositionResidualFitter.fitter
        fitter.iteration += 1
        score[0] = 0.
        parameter = ROOT.Double()
        error = ROOT.Double()
        for i, key in enumerate(["t", "x", "y", "px", "py", "pz"]):
            fitter.minuit.GetParameter(i, parameter, error)
            fitter.best_guess[key] = parameter
        fitter.best_guess.mass_shell_condition('energy')
        keys = ["t", "x", "y", "z", "energy", "px", "py", "pz"]
        print "    ", fitter.iteration,
        for i, key in enumerate(keys):
            print key+":", fitter.to_string(fitter.best_guess[key]),
        for sp_index, sp in enumerate(fitter.current_space_points):
            values = [fitter.best_guess[key] for key in keys]
            ellipse = [[0. for i in range(6)] for j in range(6)]
            for i in range(6):
                ellipse[i][i] = 1.
            if sp["z"] == fitter.best_guess["z"]:
                pass
            elif sp["z"] > fitter.best_guess["z"]:
                values, ellipse = maus_cpp.error_propagation.propagate_errors(values, ellipse, sp["z"], 10.)
            elif sp["z"] < fitter.best_guess["z"]:
                values, ellipse = maus_cpp.error_propagation.propagate_errors(values, ellipse, sp["z"], -10.)
            fitter.fitted_track_points[sp_index] = dict(zip(keys, values))
            fitter.fitted_track_points[sp_index]["detector"] = sp["detector"]
            chi2 = 0.
            for key, i, err in [("x", 1, 0.4), ("y", 2, 0.4), ("t", 0, 0.07)]:
                det = sp["detector"]
                if sp[key] != None and key in fitter.config.fit_detectors[det]:
                    chi2 += (values[i] - sp[key])**2/err**2
            score[0] += chi2
        print "score:", fitter.to_string(score[0])

    mu_mass = common.pdg_pid_to_mass[13]
    c_light = common.constants["c_light"]
    fitter = None

