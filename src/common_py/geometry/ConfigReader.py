"""
m. Littlefield
"""
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

from Configuration import Configuration
import json

class Configreader(): #pylint: disable = R0903, R0902
    """
    @class configreader, Configuration File Reader Class
    
    This class reads the information in the Configuration Text File and 
    translates this into variables which can be passed between the other 
    classes.
    """
    def __init__(self):
        """
        @method Class Constructor
        
        This sets up the blank variables ready to be filled by the information
        contained in the Configuration text file.
        """
        inputs = Configuration().getConfigJSON(command_line_args = True)
        config_dict = json.loads(inputs)
        self.cdb_upload_url = config_dict['cdb_upload_url']
        self.geometry_upload_wsdl  = config_dict['geometry_upload_wsdl']
        self.geometry_upload_directory = config_dict \
                                                   ['geometry_upload_directory']
        self.geometry_upload_note = config_dict['geometry_upload_note']
        self.geometry_upload_valid_from = config_dict \
                                                  ['geometry_upload_valid_from']
        self.geometry_upload_cleanup = config_dict['geometry_upload_cleanup']

        self.cdb_download_url = config_dict['cdb_download_url']
        self.cdb_cc_download_url = config_dict['cdb_cc_download_url']
        self.geometry_download_wsdl = config_dict['geometry_download_wsdl']
        self.geometry_download_directory = config_dict \
                                                 ['geometry_download_directory']
        self.geometry_download_by = config_dict['geometry_download_by']
        self.geometry_download_run_number = config_dict \
                                            ['geometry_download_run_number']
        self.geometry_download_id = config_dict['geometry_download_id']
        self.geometry_download_cleanup = config_dict \
                                                   ['geometry_download_cleanup']
        self.g4_step_max = config_dict['g4_step_max']
        self.download_beamline_for_run = \
                               config_dict['geometry_download_beamline_for_run']
        self.download_beamline_tag = \
                               config_dict['geometry_download_beamline_tag']
        self.download_coolingchannel_tag = \
                             config_dict['geometry_download_coolingchannel_tag']
        self.get_ids_start_time = config_dict['get_ids_start_time']
        self.get_ids_stop_time = config_dict['get_ids_stop_time']
        self.get_ids_create_file = config_dict['get_ids_create_file']
        self.get_beamline_by = config_dict['get_beamline_by']
        self.get_beamline_run_number = config_dict['get_beamline_run_number']
        self.get_beamline_start_time = config_dict['get_beamline_start_time']
        self.get_beamline_stop_time = config_dict['get_beamline_stop_time']
        self.tof_0_file_number = config_dict['tof_0_file_number']
        self.tof_1_file_number = config_dict['tof_1_file_number']
        self.tof_2_file_number = config_dict['tof_2_file_number']
        self.ckov1_file_number = config_dict['ckov1_file_number']
        self.ckov2_file_number = config_dict['ckov2_file_number']
        self.kl_file_number = config_dict['kl_file_number']
        self.emr_file_number = config_dict['emr_file_number']
        self.tracker0_file_number = config_dict['tracker0_file_number']
        self.tracker1_file_number = config_dict['tracker1_file_number']
        self.absorber0_file_number = config_dict['absorber0_file_number']
        self.absorber1_file_number = config_dict['absorber1_file_number']
        self.absorber2_file_number = config_dict['absorber2_file_number']

        self.survey_target_detectors = config_dict['survey_target_detectors']
        self.apply_corrections_by_run = config_dict['geometry_download_apply_corrections']
        
