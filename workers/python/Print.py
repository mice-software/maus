# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>                                                                                                                     
#
# March 04, 2009
#   
from core.GenericProcessor import GenericProcessor

LOG_FILENAME = 'logging_rotatingfile_example.out'

# Set up a specific logger with our desired output level
log = logging.getLogger('workers.print')

class Print(GenericProcessor):

    def __init__(self,verbose=False,type='readout'):
        log.debug("Welcome to __init__ in Print()")
        pass
