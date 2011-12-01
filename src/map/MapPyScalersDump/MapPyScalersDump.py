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

##  MapPyScalersDump class dumps some scaler information.
#  This a first prototype and for the moment it dumps only Triggers,
#  Trigger Requests, GVA, TOF0 and TOF1. It has to be extended and to
#  include also the other channels of the scaler.

""" This class dump the scaler information"""

import json
import ErrorHandler
from datetime import datetime

class MapPyScalersDump:
    """ This class dump the scaler information"""
    def __init__(self):
        """ Constructor """
        self._hits = {}
        self._triggers         = []
        self._trigger_requests = []
        self._gva              = []
        self._tof0             = []
        self._tof1             = []
        self._clock            = []

    def birth(self, json_configuration):
        """ Do nothing here """
        try:
            config_doc = json.loads(json_configuration)
            if config_doc:
                return True
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)

        return False

    def process(self, json_spill_doc):
        """Process the JSON data"""
        spill = json.loads(json_spill_doc)

        if 'daq_data' not in spill:
            return json_spill_doc

        if spill['daq_data'] is None:
            return json_spill_doc

        daq_event = spill['daq_data']
        if 'V830' not in daq_event:
            return json_spill_doc

        scalers = daq_event['V830']
        #print scalers
        if 'channels' not in scalers:
            return json_spill_doc

        self._hits = scalers['channels']
        event = scalers['phys_event_number']
        time = scalers['time_stamp']
        print 'Spill Num  : ', event, '  time : ', datetime.fromtimestamp(time)
        if self.add(self._hits) :
            self.dump()

        return json_spill_doc

    def death(self): #pylint: disable=R0201
        """ Do nothing here """
        return True


    def dump(self):
        """ Dump the information """
        if len(self._triggers):
            print 'triggers .......... : ', self._triggers[-1], \
            ' (', sum(self._triggers)/len(self._triggers), ')'

        if len(self._trigger_requests):
            print 'trigger requests .. : ', self._trigger_requests[-1], \
            ' (', sum(self._trigger_requests)/len(self._trigger_requests), ')'

        if len(self._gva):
            print 'GVA ............... : ', self._gva[-1], \
            ' (', sum(self._gva)/len(self._gva), ')'

        if len(self._tof0):
            print 'TOF0 .............. : ', self._tof0[-1], \
            ' (', sum(self._tof0)/len(self._tof0), ')'

        if len(self._tof1):
            print 'TOF1 .............. : ', self._tof1[-1], \
            ' (', sum(self._tof1)/len(self._tof1), ')'

        if len(self._clock):
            print '10 MHz clock ...... : ', self._clock[-1], \
            ' (', sum(self._clock)/len(self._clock), ')' , '\n\n'

        return True

    def add(self, hits=None):
        """ add the information from this spill """
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

        self._clock.append( hits['ch12'] )
        if len(self._clock) == 11 :
            self._clock.pop(0)

        return True



