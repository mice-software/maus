"""
ReducePyScalers is a class for extracting scaler data from binary DATE files,
and outputting as ascii.
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
#

import json
import ErrorHandler
from datetime import datetime

# Too many instance attributes # pylint: disable=R0902
# pylint: disable=R0903

class ReducePyScalers:
    """ Class to extract scaler output from json and write to ascii """

    def __init__(self):
        """ Constructor """
        self._hits = {}
        self._triggers         = []
        self._trigger_requests = []
        self._gva              = []
        self._tof0             = []
        self._tof1             = []
        self._lm12             = []
        self._lm34             = []
        self._lm1234           = []
        self._clock            = []

    # Attribute defined outside __init__ # pylint: disable=W0201
    def birth(self, json_configuration):
        """ Set up the configuration and output file header """
        try:
            config_doc = json.loads(json_configuration)
            if config_doc:
                key = 'output_scalers_file_name'
                if ( key in config_doc ):
                    self._output_file_name = config_doc[key]
                else:
                    self._output_file_name = 'scalers.dat'

                self._fout = open(self._output_file_name, 'w')
                header = 'Timestamp\tSpill\tTrigs\tTReqs\tGVA1\tTOF0\t'
                header = header + 'TOF1\tLM12\tLM34\tLM1234\tClock\n'
                self._fout.write(header)
                return True
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)
            return False

    def process(self, json_spill_doc, screen_output=False):
        """ Process the JSON data """
        spill = json.loads(json_spill_doc)

        if 'daq_data' not in spill:
            return json_spill_doc

        if spill['daq_data'] is None:
            return json_spill_doc

        daq_event = spill['daq_data']
        if 'V830' not in daq_event:
            return json_spill_doc

        scalers = daq_event['V830']

        # Print scalers
        if 'channels' not in scalers:
            return json_spill_doc

        self._hits = scalers['channels']
        event = scalers['phys_event_number']
        time = scalers['time_stamp']

        if screen_output:
            print 'Spill Num  : ', event, '  time : ', \
                  datetime.fromtimestamp(time)

        self._fout.write( str(time) + '\t')
        self._fout.write( str(event) + '\t')

        if self.add(self._hits) :
            self.dump(screen_output)
            return json_spill_doc

    # Too many instance attributes #pylint: disable=R0201
    # Too many branches #pylint: disable=R0912
    # Too many statements #pylint: disable=R0915
    def death(self): 
        """ Close the output file """
        self._fout.close()
        return True

    def dump(self, screen_output=False):
        """ Write the information to file and screen """
        if len(self._triggers):
            if screen_output:
                print 'triggers .......... : ', self._triggers[-1], \
                ' (', sum(self._triggers)/len(self._triggers), ')'
            self._fout.write(str(self._triggers[-1]) + '\t' )
        else:
            self._fout.write( '0\t' )

        if len(self._trigger_requests):
            if screen_output:
                print 'trigger requests .. : ', self._trigger_requests[-1], \
                ' (', sum(self._trigger_requests)/len(self._trigger_requests), \
                ')'
            self._fout.write(str(self._trigger_requests[-1]) + '\t')
        else:
            self._fout.write( '0\t')

        if len(self._gva):
            if screen_output:
                print 'GVA ............... : ', self._gva[-1], \
                ' (', sum(self._gva)/len(self._gva), ')'
            self._fout.write(str(self._gva[-1]) + '\t')
        else:
            self._fout.write( '0\t')

        if len(self._tof0):
            if screen_output:
                print 'TOF0 .............. : ', self._tof0[-1], \
                ' (', sum(self._tof0)/len(self._tof0), ')'
            self._fout.write(str(self._tof0[-1]) + '\t')
        else:
            self._fout.write( '0\t')

        if len(self._tof1):
            if screen_output:
                print 'TOF1 .............. : ', self._tof1[-1], \
                ' (', sum(self._tof1)/len(self._tof1), ')'
            self._fout.write(str(self._tof1[-1]) + '\t')
        else:
            self._fout.write( '0\t')

        if len(self._lm12):
            if screen_output:
                print 'LM12 .............. : ', self._lm12[-1], \
                ' (', sum(self._lm12)/len(self._lm12), ')'
            self._fout.write(str(self._lm12[-1]) + '\t')
        else:
            self._fout.write( '0\t')

        if len(self._lm34):
            if screen_output:
                print 'LM34 .............. : ', self._lm34[-1], \
                ' (', sum(self._lm34)/len(self._lm34), ')'
            self._fout.write(str( self._lm34[-1]) + '\t')
        else:
            self._fout.write( '0\t')

        if len(self._lm1234):
            if screen_output:
                print 'LM1234 ............ : ', self._lm1234[-1], \
                ' (', sum(self._lm1234)/len(self._lm1234), ')'
            self._fout.write(str(self._lm1234[-1]) + '\t')
        else:
            self._fout.write( '0\t')

        if len(self._clock):
            if screen_output:
                print '10 MHz clock ...... : ', self._clock[-1], \
                ' (', sum(self._clock)/len(self._clock), ')' , '\n\n'
            self._fout.write(str(self._clock[-1]) + '\n')
        else:
            self._fout.write( '0\n')

        return True

    def add(self, hits=None):
        """ Add the information from this spill """
        if  'ch0' not in hits:
            return False

        self._triggers.append( hits['ch0'] )
        if len(self._triggers) == 11 :
            self._triggers.pop(0)

        self._trigger_requests.append( hits['ch1'] )
        if len(self._trigger_requests) == 11 :
            self._trigger_requests.pop(0)

        self._gva.append( hits['ch2'] )
        if len(self._gva) == 11 :
            self._gva.pop(0)

        self._tof0.append( hits['ch3'] )
        if len(self._tof0) == 11 :
            self._tof0.pop(0)

        self._tof1.append( hits['ch4'] )
        if len(self._tof1) == 11 :
            self._tof1.pop(0)

        self._lm12.append( hits['ch9'] )
        if len(self._lm12) == 11 :
            self._lm12.pop(0)

        self._lm34.append( hits['ch10'] )
        if len(self._lm34) == 11 :
            self._lm34.pop(0)

        self._lm1234.append( hits['ch11'] )
        if len(self._lm1234) == 11 :
            self._lm1234.pop(0)

        self._clock.append( hits['ch12'] )
        if len(self._clock) == 11 :
            self._clock.pop(0)

        return True

