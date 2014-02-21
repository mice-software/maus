"""
Datacard to create reconstructed global tracks from 200MeV/c pions
"""
# pylint: disable=C0103, W0611

import os

# A json document containing spills from MC data
input_json_file_name = "200MeV_pi_plus_hypothesis.json"
input_json_file_type = "text"

# The json document that the global tracks will be written to
output_json_file_name = "200MeV_pi_plus_hypothesis_Global_Recon.json"
output_json_file_type = "text"
