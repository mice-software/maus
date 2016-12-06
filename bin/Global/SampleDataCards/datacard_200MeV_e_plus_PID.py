"""
Datacard to use when performing PID on simulated 200MeV/c positrons
"""
# pylint: disable=C0103, W0611

import os

# A json document containing global tracks from MC data
input_json_file_name = "200MeV_e_plus_hypothesis_Global_Recon.json"
input_json_file_type = "text"

# Output json document with track pid information included
output_json_file_name = "200MeV_e_plus_Global_PID.json"
output_json_file_type = "text"
