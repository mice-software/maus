#!/bin/env python
# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>

import os, sys, gc

from datetime import datetime

import logging

logger_core_fr = logging.getLogger('core.fetch_runs')

def FetchRuns(input_string):
    ## Iterate through the workers
    #
    # this processor handles all the workers
    runs = None

    logger_core_fr.info("Fetching runs that match: %s..." % input_string)

    # <main body>
    # </main body>

    logger_core_fr.info("Found:")
    logger_core_fr.info(runs)

    return runs
