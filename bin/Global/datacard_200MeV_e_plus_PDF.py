"""
Datacard to run create PDF using simulated 200MeV/c positrons
"""
# pylint: disable=C0103, W0611

import os
import datetime

# Use the current time and date as a unique identifier when creating files to
# contain PDFs. A unique_identifier is required by the reducer, and PDF 
# production will fail without one.
now = datetime.datetime.now()
unique_identifier = now.strftime("%Y_%m_%dT%H_%M_%S_%f")

# A json document containing global tracks from MC data
input_json_file_name = "200MeV_e_plus_hypothesis_Global_Recon.json"
input_json_file_type = "text"

# The particle hypothesis that the PDF is being created for. A 
# global_pid_hypothesis is required by the reducer, and PDF production will
# fail without one.
global_pid_hypothesis = "200MeV_e_plus"
