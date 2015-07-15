#!/usr/bin/env python

#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

"""
Geometry Validation tool is used for building a 2D map of the materials that
particles see as they traverse the beamline and then plotting it. Assumes a 
straight geometry along z.

The algorithm is to:
* Force keep_steps = True so that MAUS reports position and material at every
  step
* User specified initial z position and a line of points in x, y
* Force physics_processes = None so no energy loss, scattering etc can occur
* Fire a very high energy gamma down the beamline for each initial (x, y, z) in
  the z direction
* Plot as a function of x, y, z the resultant map of materials in the beamline
** Colours are chosen automatically (this can sometimes be not great)
* Plot as a function of z a 1D graph of materials on the beamline axis

Uses the "geometry_validation" dictionary of datacards, with dictionary keys:
- "file_name", (string) name of the file that will be used to store the tracking
               data
- "will_plot", (bool) set to boolean true to make plots of the particles
- "will_track" (bool) set to boolean true to do tracking, else attempts to reuse
               old tracking
- "z_start" (double, mm) start z-position for tracking and plotting
- "z_end" (double, mm) end z-position for plotting (tracking will keep going
          until it reaches the edge of the geometry)
- "x_start" (double, mm) x-position of the first particle to be tracked
- "x_step" (double, mm) increment in x-position for each subsequent particle
- "y_start" (double, mm) y-position of the first particle to be tracked
- "y_step" (double, mm) increment in y-position for each subsequent particle
- "n_steps" (int) number of particles to track
- "plot_formats" (string) list of plot formats, e.g. ["png", "eps"]
            would plot in png and encapsulated postscript format
- "geometry_validation_materials_1d" (string) name of the 1D plot to be printed
            by MAUS. The appropriate suffix will be appended for each format.
- "geometry_validation_materials_2d" (string) name of the 2D plot to be printed
            by MAUS. The appropriate suffix will be appended for each format.
- "geometry_validation_volumes_1d" (string) name of the 1D plot to be printed
            by MAUS giving z positions of volumes. The appropriate suffix will 
            be appended for each format.
- "geometry_validation_volumes_2d" (string) name of the 2D plot to be printed
            by MAUS showing volume bounding rectangles and name labels. The
            appropriate suffix will be appended for each format.
- "2d_volume_plot_label_size" (float) between 0.0 and 1.0 that tells how big the
  labels should be in the 2d volume plot. Labels are stacked vertically, so if
  the label size is small more labels can be put on the plot
- "volume_bounding_box_dump" (string) file name for dumping bounding boxes to.
  Format is json, going something something like:
{
  <string volume name>: {
    "z_min": <float minimum z of steps in volume>,
    "z_max": <float maximum z of steps in volume>, 
    "r_min": <float minimum radius of steps in volume>,
    "r_max": <float maximum radius of steps in volume>, 
    "material": <string material name>
  },
  ...
}
e.g.
{
  "Vol14": {
    "z_max": 320.0, 
    "r_min": 0.0, 
    "material": "G4_Al", 
    "r_max": 2.23606806280132, 
    "z_min": 280.0
  },
  ...
}
"""

# pylint: disable=E1101

import os
import sys
import json
import ROOT
import xboa.common as common

import Configuration

import maus_cpp.simulation
import maus_cpp.globals


class Plotting(object): # pylint: disable=R0902
    """Plotting class handles reading and plotting of the tracking data"""
    def __init__(self, configuration):
        """
        Initialise the plotting class
        - configuration should be a valid datacard set in string representation
        """
        py_config = json.loads(configuration)["geometry_validation"]
        print json.dumps(py_config, indent=2)
        self.file_name = py_config["file_name"]
        self.z_start = py_config["z_start"]
        self.z_end = py_config["z_end"]
        self.will_plot = py_config["will_plot"]
        self.n_steps = py_config["n_steps"]
        self.formats = py_config["plot_formats"]
        self.mat_1d = os.path.expandvars(py_config["1d_material_plot"])
        self.mat_2d = os.path.expandvars(py_config["2d_material_plot"])
        self.vol_1d = os.path.expandvars(py_config["1d_volume_plot"])
        self.vol_2d = os.path.expandvars(py_config["2d_volume_plot"])
        self.vol_2d_label_size = py_config["2d_volume_plot_label_size"]
        self.volume_bb_dump = py_config["volume_bounding_box_dump"]
        self.material_data = {}
        self.volume_data = {}
        self.volume_label_index = 0

    def do_plotting(self):
        """Load the data and plot"""
        if not self.will_plot:
            return
        print "Doing plots"
        # step_data should be member
        step_data = self._load_data()
        for key in step_data:
            print "Found", len(step_data[key]), \
                  "steps for material '"+str(key)+"'"
        self._bb_dump()
        material_dict = self.get_material_dict(step_data)
        data_range = self.get_range(step_data)
        x_range = data_range[0]
        z_range = data_range[1]
        z_range[0] = max(z_range[0], self.z_start)
        z_range[1] = min(z_range[1], self.z_end)
        print "data range r:", x_range, "z:", z_range
        self._draw_materials_1d(step_data, z_range)
        self._draw_materials_2d(step_data, material_dict, x_range, z_range)
        self._draw_volumes_1d(z_range)
        self._draw_volumes_2d(x_range, z_range)

    def _readline_generator(self):
        """
        Generator that reads a file line-by-line, with a progress bar

        Python builtin readlines() reads the file in one go, making big memory
        usage for potentially large files
        """
        bar_length = 72
        sys.stdout.write("["+" "*(bar_length+1)+"]")
        sys.stdout.flush()
        sys.stdout.write("\b"*(bar_length+1))
        old_done = 0
        fin = open(self.file_name)  
        line = None
        i = 0
        while True:
            line = fin.readline()
            if line == "":
                break
            yield line
            done = float(i)/(self.n_steps-1)*bar_length
            while done-old_done > 1.:
                old_done += 1
                sys.stdout.write("=")
                sys.stdout.flush()
            i += 1
        fin.close()
        print
            

    def _load_data(self):
        """Load the data"""
        step_data = {}
        print "Loading data"
        for line in self._readline_generator():
            json_data = json.loads(line)
            for event in json_data:
                for track in event["tracks"]:
                    for step in track["steps"]:
                        z_pos = step["position"]["z"]
                        if z_pos < self.z_start or z_pos > self.z_end:
                            continue
                        r_pos = self._get_r(step["position"])
                        mat = step["material"]
                        if not mat in step_data.keys():
                            step_data[mat] = []
                        step_data[mat].append({"r":r_pos, "z":z_pos})
                        vol = step["volume"]
                        if not vol in self.volume_data.keys():
                            self.volume_data[vol] = {
                                "r_min":r_pos,
                                "r_max":r_pos,
                                "z_min":z_pos,
                                "z_max":z_pos,
                                "material":mat,
                            }
                        else:
                            if r_pos < self.volume_data[vol]["r_min"]:
                                self.volume_data[vol]["r_min"] = r_pos
                            elif  r_pos > self.volume_data[vol]["r_max"]:
                                self.volume_data[vol]["r_max"] = r_pos
                            if z_pos < self.volume_data[vol]["z_min"]:
                                self.volume_data[vol]["z_min"] = z_pos
                            elif  z_pos > self.volume_data[vol]["z_max"]:
                                self.volume_data[vol]["z_max"] = z_pos
        return step_data

    def _bb_dump(self):
        """Dump bounding box data to disk"""
        fout = open(self.volume_bb_dump, "w")
        print >> fout, json.dumps(self.volume_data, indent=2)

    def _draw_materials_1d(self, step_data, z_range):
        """Make a 1D plot of position vs material along the z axis"""
        # we plot G4_AIR first, then G4_GALACTIC?
        draw_list = step_data.keys()
        draw_list = self.move(draw_list, "G4_AIR", 0)
        draw_list = self.move(draw_list, "G4_Galactic", 1)
        black_x_list, black_y_list = [], []
        grey_x_list, grey_y_list = [], []
        canvas = common.make_root_canvas("Material vs z")
        hist = ROOT.TH2D("name", "Material vs z", 30000, z_range[0], z_range[1],
                         len(draw_list)+1, -0.5, len(draw_list)+0.5)
        common._common._hist_persistent.append(hist) #pylint: disable=W0212
        for material in draw_list:
            hist.Fill(z_range[0], material, 1.)
        hist.SetBit(ROOT.TH1.kCanRebin)
        hist.SetStats(0)
        hist.LabelsDeflate("Y")
        hist.LabelsOption("v")
        hist.Draw()
        for i, material in enumerate(draw_list):
            grey_x_list += [i+0.1 for step in step_data[material]]
            grey_y_list += [step["z"] for step in step_data[material]]
            black_x_list += [i for step in step_data[material] \
                                                        if step["r"] < 1e-9]
            black_y_list += [step["z"] for step in step_data[material] \
                                                        if step["r"] < 1e-9]

        if len(grey_x_list) == 0:
            return
        hist, graph = common.make_root_graph("", grey_y_list, "z [mm]",
                                               grey_x_list, "material index")
        graph.SetMarkerColor(ROOT.TColor.GetColor(0.5, 0.5, 0.5))
        graph.SetMarkerStyle(7)
        graph.Draw('p')
        if len(black_x_list) != 0:
            hist, graph = common.make_root_graph("", black_y_list, "z [mm]",
                                                 black_x_list, "material index")
            graph.SetMarkerStyle(7)
            graph.Draw('p')
        canvas.Update()
        for _format in self.formats:
            canvas.Print(self.mat_1d+"."+_format)

    def _draw_volumes_1d(self, z_range):
        """Make a 1D plot of position vs volume along the z axis"""
        draw_list = sorted(self.volume_data.keys(), self._sort_cmp)
        x_list = []
        canvas = common.make_root_canvas("Volume Name vs z")
        # set up axis labels
        print "VOLUME PLOT", z_range
        hist = ROOT.TH2D("name", "Volume Name vs z", 30000,
                         z_range[0], z_range[1],
                         len(draw_list)+1, -0.5, len(draw_list)+0.5)
        common._common._hist_persistent.append(hist) #pylint: disable=W0212
        for material in draw_list:
            hist.Fill(z_range[0], material, 1.)
        hist.SetBit(ROOT.TH1.kCanRebin)
        hist.SetStats(0)
        hist.LabelsDeflate("Y")
        hist.LabelsOption("v")
        hist.Draw()
        # now draw graphs
        for i, volume in enumerate(draw_list):
            x_list = [self.volume_data[volume]["z_min"],
                        self.volume_data[volume]["z_max"]]
            y_list = [i+0.1, i+0.1]
            hist, graph = common.make_root_graph("", x_list, "z [mm]",
                                                   y_list, "volume index")
            graph.SetLineColor(ROOT.TColor.GetColor(0.5, 0.5, 0.5))
            graph.Draw('l')
            if self.volume_data[volume]["r_min"] < 1e-9:
                y_list = [i, i]
                hist, graph = common.make_root_graph("", x_list, "z [mm]",
                                                       y_list, "material index")
                graph.Draw('l')
        canvas.Update()
        for _format in self.formats:
            canvas.Print(self.vol_1d+"."+_format)

    def _sort_cmp(self, vol_key_1, vol_key_2):
        """Compare for sorting volume keys by z_min"""
        return cmp(self.volume_data[vol_key_1]["z_min"],
                   self.volume_data[vol_key_2]["z_min"])

    def _draw_volume_label(self, x_position, volume, x_range, color):
        """Draw a volume label at x_position"""
        
        self.labels.append(ROOT.TLatex())
        delta = abs(x_range[1]-x_range[0])/5.
        n_vert = int(1./self.vol_2d_label_size)
        scale = min(self.vol_2d_label_size*4, 1.0)
        text = "#scale["+str(scale)+"]{#color["+str(color)+"]{"+volume+"}}"
        vertical = float(self.volume_label_index%n_vert)/n_vert
        vert = x_range[1]+delta*(1.0*vertical-0.4)
        self.labels[-1].DrawLatex(x_position, vert, text)
        self.volume_label_index += 1
        return self.labels[-1]

    def _draw_volumes_2d(self, x_range, z_range):
        """Make a 2D plot of position vs volume"""
        canvas = common.make_root_canvas("Volume Name vs position")
        hist, graph = common.make_root_graph("Volume Name vs position",
                                             [1e9], "z [mm]",
                                             [1e9], "x [mm]",
                                             xmin=z_range[0], xmax=z_range[1],
                                             ymin=x_range[0], ymax=x_range[1])
        hist.Draw()
        draw_list = sorted(self.volume_data.keys(), self._sort_cmp)
        n_colors = 5
        for i, vol in enumerate(draw_list):
            data = self.volume_data[vol]
            r_list = [data["r_min"], data["r_max"], data["r_max"],
                      data["r_min"], data["r_min"]]
            z_list = [data["z_min"], data["z_min"], data["z_max"],
                      data["z_max"], data["z_min"]]
            hist, graph = common.make_root_graph(vol,
                                                 z_list, "z [mm]",
                                                 r_list, "r [mm]", sort = False)
            color = self.color(i % n_colors, n_colors)
            graph.SetLineColor(color)
            graph.Draw('l')
            self._draw_volume_label(data["z_min"], vol, x_range, color)
        canvas.Update()
        for _format in self.formats:
            canvas.Print(self.vol_2d+"."+_format)


    def _draw_materials_2d(self, step_data, material_dict, x_range, z_range):
        """Make a 2D plot of position vs material in r, z"""
        # we plot G4_AIR first, then G4_GALACTIC?
        canvas = common.make_root_canvas("Material vs position")
        hist, graph = common.make_root_graph("Material vs position",
                                             [1e9], "z [mm]",
                                             [1e9], "r [mm]",
                                             xmin=z_range[0], xmax=z_range[1],
                                             ymin=x_range[0], ymax=x_range[1])
        hist.Draw()
        draw_list = step_data.keys()
        draw_list = self.move(draw_list, "G4_AIR", 0)
        draw_list = self.move(draw_list, "G4_Galactic", 1)
        graph_list = []
        for material in draw_list:
            x_list = [step["r"] for step in step_data[material]]
            z_list = [step["z"] for step in step_data[material]]
            hist, graph = common.make_root_graph(material,
                                                 z_list, "z [mm]",
                                                 x_list, "r [mm]")
            graph.SetMarkerColor(material_dict[material])
            graph.SetMarkerStyle(7)
            graph.SetLineColor(10)
            graph.Draw('p')
            graph_list.append(graph)
        common.make_root_legend(canvas, graph_list)
        canvas.Update()
        for _format in self.formats:
            canvas.Print(self.mat_2d+"."+_format)

    @classmethod
    def _get_r(cls, step):
        """Get r for step"""
        return (step["x"]**2+step["y"]**2)**0.5

    @classmethod
    def color(cls, ith_colour, n_colours):
        """Get the colour for the ith material of n total materials"""
        if n_colours == 0:
            return 0
        ith_colour = float(ith_colour)
        n_colours = float(n_colours)
        n_lim = n_colours/3.
        red, green, blue = 0., 0., 0.
        if ith_colour < n_lim:
            red = (n_lim-ith_colour)/n_lim
            green = ith_colour/n_lim
        if ith_colour >= n_lim and ith_colour < 2.*n_lim:
            ith_colour -= n_lim
            green = (n_lim-ith_colour)/n_lim
            blue = ith_colour/n_lim
        if ith_colour >= 2.*n_lim:
            ith_colour -= 2.*n_lim
            blue = (n_lim-ith_colour)/n_lim
            red = ith_colour/n_lim
        color = ROOT.TColor.GetColor(red, green, blue)
        return color

    @classmethod
    def get_material_dict(cls, step_data):
        """
        Get a dictionary that maps material to colour. Air and Galactic are
        always grey.
        """
        n_items = len(step_data)
        material_dict = {}
        for i, key in enumerate(step_data.keys()):
            material_dict[key] = cls.color(i, n_items)
        material_dict["G4_AIR"] = ROOT.TColor.GetColor(0.1, 0.1, 0.1)
        material_dict["G4_Galactic"] = ROOT.TColor.GetColor(0.3, 0.3, 0.3)
        return material_dict

    @classmethod
    def get_range(cls, step_data):
        """
        Get axis range in r, z
        """
        r_data, z_data = [], []
        for steps in step_data.values():
            r_data += [pos["r"] for pos in steps]
            z_data += [pos["z"] for pos in steps]
        r_data = sorted(r_data)
        z_data = sorted(z_data)
        
        data_range = []
        for data in r_data, z_data:
            delta = data[-1]-data[0]
            if delta < 1e-9:
                data_range.append([-1., 1.])
            else:
                data_range.append([data[0]-delta/10., data[-1]+delta/10.])
        return data_range

    @classmethod
    def move(cls, a_list, value, index):
        """
        Move item "value" in list "a_list" to position "index".

        If "value" is not in "a_list", well never mind
        """
        if value in a_list:
            a_list.remove(value)
            a_list.insert(index, value)
        return a_list

    labels = []

class Tracking(object): # pylint: disable=R0902
    """Plotting class handles generation of particles and tracking"""
    def __init__(self, configuration):
        """
        Initialise the tracking class
        - configuration should be a valid datacard set in string representation
        """
        self.config = json.loads(configuration)
        py_config = self.config["geometry_validation"]
        self.file_name = py_config["file_name"]
        self.n_steps = py_config["n_steps"]
        self.x_start = py_config["x_start"]
        self.x_step = py_config["x_step"]
        self.y_start = py_config["y_start"]
        self.y_step = py_config["y_step"]
        self.z_start = py_config["z_start"]
        self.will_track = py_config["will_track"]

    def do_tracking(self):
        """Do the tracking"""
        if not self.will_track:
            return
        print "Force stepping to verbose"
        output_file = open(self.file_name, "w")
        self._fiddle_configuration()
        print "Tracking", self.n_steps, "events"
        bar_length = 72
        sys.stdout.write("["+" "*(bar_length+1)+"]")
        sys.stdout.flush()
        sys.stdout.write("\b"*(bar_length+1))
        old_done = 0
        for i in range(self.n_steps):
            done = i/float(self.n_steps-1)*bar_length
            while done-old_done > 1.:
                old_done += 1
                sys.stdout.write("=")
                sys.stdout.flush()
            x_pos = self.x_start+i*self.x_step
            y_pos = self.y_start+i*self.y_step
            event = self.track_particle(x_pos, y_pos)
            print >> output_file, json.dumps(event)
        print

    def _fiddle_configuration(self):
        """Switch off physics processes and force keep steps to True"""
        self.config["keep_steps"] = True
        self.config["physics_processes"] = "none"
        maus_cpp.globals.birth(json.dumps(self.config))

    def _get_primary(self, x_pos, y_pos):
        """Build a primary with initial position (x, y, z_start) and large
        momentum in the z-direction"""
        return {'primary':
          {
            'position':{'x':x_pos, 'y':y_pos, 'z':self.z_start},
            'momentum':{'x':0., 'y':0., 'z':1.},
            'spin':{'x':0., 'y':0., 'z':1.},
            'particle_id':22,
            'energy':1e9,
            'time':0.,
            'random_seed':0
          }
        }

    def track_particle(self, x_pos, y_pos):
        """Track a particle with initial position x,y"""
        primary = json.dumps([self._get_primary(x_pos, y_pos)])
        event = json.loads(maus_cpp.simulation.track_particles(primary))
        return event

def main():
    """Set up the datacards and call tracking and plotting routines"""
    configuration = Configuration.Configuration().\
                                          getConfigJSON(command_line_args=True)
    tracking = Tracking(configuration)
    plotting = Plotting(configuration)
    tracking.do_tracking()
    plotting.do_plotting()
    print "Press <Enter> to finish"
    raw_input()

if __name__ == "__main__":
    main()


