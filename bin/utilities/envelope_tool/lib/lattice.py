#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
Class for managing the lattice, tracking envelopes, etc
"""

import json
import numpy

from xboa.hit.factory import MausJsonHitFactory

import Configuration
import maus_cpp.field
import maus_cpp.globals
import maus_cpp.simulation
import maus_cpp.covariance_matrix
import maus_cpp.optics_model

class Lattice:
    """Class for managing the lattice, tracking envelopes, etc"""
    def __init__(self):
        """Lattice initialisation - mice modules"""
        self.lattice = None
        self.mice_modules = None
        self.optics_model = None
        self.config = None
        self.ref_list = []
        self.ellipse_list = []
        self._set_defaults()
        self.run_lattice()

    def run_lattice(self):
        """
        Setup the lattice for optics work
          - initial_momentum: beam momentum at lattice start
          - z_start: start position for tracking/beam propagation
        """
        self.config["simulation_reference_particle"] = \
                              self.ref_list[0].get_maus_dict('maus_primary')[0]
        self.config["simulation_reference_particle"]["random_seed"] = 0
        self.config["physics_processes"] = "mean_energy_loss"
        config_str = json.dumps(self.config)
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        maus_cpp.globals.birth(config_str)
        maus_cpp.globals.set_monte_carlo_mice_modules(self.mice_modules)
        self._tracking()

    def get_field_list(self):
        """
        Recursively interrogate the mice modules building up a list of fields

        Require each field module has field_name, field_type, scale_factor,
        aperture, outer, position, rotation
        """
        return self._get_fields_recursive(self.mice_modules, [])

    def set_fields(self, field_list):
        """
        Set field scalings from a list of dictionaries
        - field_list: list of fields with dict elements "field_name" and 
          "scale_factor" mapping to string and float respectively
        """
        field_dict = {}
        for field in field_list:
            field_dict[field["field_name"]] = field["scale_factor"]
        self.mice_modules = self._set_fields_recursive(self.mice_modules,
                                                       field_dict)
        
    def set_beam(self, reference, ellipse):
        """
        Set beam parameters following ellipse transport
        - reference: set the initial reference trajectory
        - ellipse: set the initial beam ellipse
        """
        self.ref_list = [reference]
        self.ellipse_list = [ellipse]

    def get_beam(self):
        """
        Get a tuple like (ref, ellipse) containing a reference particle and beam
        ellipse
        """
        return self.ref_list[0], self.ellipse_list[0]

    def _set_defaults(self):
        """
        Setup the lattice according to the configuration specified in the usual
        way
        """
        config_str = Configuration.Configuration().getConfigJSON(
                                                        command_line_args=True)
        self.config = json.loads(config_str)
        ref_json = self.config["simulation_reference_particle"]
        self.ref_list = [
                MausJsonHitFactory.hit_from_maus_object("primary", ref_json, 0)
        ]
        ell_data = [[float(i == j) for i in range(6)] for j in range(6)]
        np_diag = numpy.array(ell_data)
        self.ellipse_list = [
                         maus_cpp.covariance_matrix.create_from_matrix(np_diag)]
        self.lattice = self.config["simulation_geometry_filename"]
        self.mice_modules = maus_cpp.mice_module.MiceModule(self.lattice)
        children = self.mice_modules.get_children() #pylint: disable=E1103
        children.append(self._virtual_plane(self.ref_list[0]['z']))
        self.mice_modules.set_children(children) #pylint: disable=E1103

    def _set_fields_recursive(self, mice_mod, field_dict):
        """
        Recurse into the MiceModules tree setting ScaleFactor by FieldName
        """
        try:
            my_name = mice_mod.get_property("FieldName", "string")
            if my_name in field_dict:
                scale = field_dict[my_name]
                mice_mod.set_property("ScaleFactor", "double", scale)
        except KeyError:
            pass
        except ValueError:
            pass
        child_list = []
        for child in mice_mod.get_children():
            child_list.append(self._set_fields_recursive(child, field_dict))
        mice_mod.set_children(child_list)
        return mice_mod

    def _get_fields_recursive(self, mice_mod, field_list):
        """
        Recurse into the MiceModules tree setting ScaleFactor by FieldName
        """
        try:
            my_props = {}
            for key in self.property_dict:
                my_props[key] = mice_mod.get_property(*self.property_dict[key])
            field_list.append(my_props)
        except KeyError:
            pass
        except ValueError:
            pass
        for child in mice_mod.get_children():
            field_list = self._get_fields_recursive(child, field_list)
        return field_list

    @staticmethod
    def _virtual_plane(z_pos):
        """Build a virtual plane module with the given z position"""
        mod = maus_cpp.mice_module.MiceModule(
                        Lattice.share_dir+"/VirtualPlane.dat").get_children()[0]
        mod.set_property("Position", "Hep3Vector", {"x":0., "y":0., "z":z_pos})
        return mod

    def _tracking(self):
        """Do the tracking - push reference trajectory and beam ellipse"""
        mc_event = [{
                  "primary":self.ref_list[0].get_maus_dict('maus_primary')[0]}]
        mc_event[0]["primary"]["random_seed"] = 0
        mc_event = maus_cpp.simulation.track_particles(json.dumps(mc_event))
        mc_event = json.loads(mc_event)
        virtual_list = mc_event[0]["virtual_hits"]
        self.ref_list = [self.ref_list[0]]+[
            MausJsonHitFactory.hit_from_maus_object('virtual_hit', virtual, 0) \
                                                   for virtual in virtual_list]
        ellipse = self.ellipse_list[0]
        try:
            self.optics_model = maus_cpp.optics_model.OpticsModel()
            self.ellipse_list = [self.ellipse_list[0]]
            for ref in self.ref_list:
                ell = self.optics_model.transport_covariance_matrix(ellipse, 
                                                                    ref['z'])
                self.ellipse_list.append(ell)
        except RuntimeError:
            pass # beam transport failed
        return self.ref_list, self.ellipse_list

    property_dict = {
        "field_name":("FieldName", "string"),
        "field_type":("FieldType", "string"),
        "scale_factor":("ScaleFactor", "double"),
        "aperture":("NominalAperture", "hep3vector"),
        "outer":("NominalOuter", "hep3vector"),
        "position":("Position", "hep3vector"),
        "rotation":("Rotation", "hep3vector")
    }

    share_dir = ""

