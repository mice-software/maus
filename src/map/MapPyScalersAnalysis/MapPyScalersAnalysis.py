
#  MapPyScalersAnalysis class dumps some scaler information.
#  This a first prototype and for the moment it dumps only Triggers,
#  Trigger Requests, GVA, TOF0 and TOF1. It has to be extended and to
#  include also the other channels of the scaler.


import json
import types
import os
import io


class MapPyScalersAnalysis:
    def birth(self, configJSON):
        self._hits = {}
        self._triggers         = []
        self._trigger_requests = []
        self._GVA              = []
        self._TOF0             = []
        self._TOF1             = []
        return True

    def process(self, str):
        spill = json.loads(str)

        if 'daq_data' not in spill:
            return str

        if spill['daq_data'] is None:
            return str

        daq_event = spill['daq_data']
        if 'V830' not in daq_event:
            return str

        scalers = daq_event['V830']
        #print scalers
        if 'channels' not in scalers:
            return str

        self._hits = scalers['channels']
        event = scalers['phys_event_number']
        time = scalers['time_stamp']
        print 'Spill Num  : ', event, '  time : ', time, '\n'
        if self.add(self._hits) :
            self.dump()

        return str

    def death(self):
        return True


    def dump(self):
        if len(self._triggers):
            print 'triggers ....... : ', self._triggers[-1],
            print ' (', sum(self._triggers)/len(self._triggers), ')'

        if len(self._trigger_requests):
            print 'trigger requests : ', self._trigger_requests[-1],
            print ' (', sum(self._trigger_requests)/len(self._trigger_requests), ')'

        if len(self._GVA):
            print 'GVA ............ : ', self._GVA[-1],
            print ' (', sum(self._GVA)/len(self._GVA), ')'

        if len(self._TOF0):
            print 'TOF0 ........... : ', self._TOF0[-1],
            print ' (', sum(self._TOF0)/len(self._TOF0), ')'

        if len(self._TOF1):
            print 'TOF1 ........... : ', self._TOF1[-1],
            print ' (', sum(self._TOF1)/len(self._TOF1), ')' , '\n\n'

        return True

    def add(self, hits=None):
        if  'ch0' not in hits:
            return False

        self._triggers.append( hits['ch0'] );
        if len(self._triggers) == 11 :
            self._triggers.pop(0)

        self._trigger_requests.append( hits['ch1'] );
        if len(self._trigger_requests) == 11 :
            self._trigger_requests.pop(0)

        self._GVA.append( hits['ch2'] );
        if len(self._GVA) == 11 :
            self._GVA.pop(0)

        self._TOF0.append( hits['ch3'] );
        if len(self._TOF0) == 11 :
            self._TOF0.pop(0)

        self._TOF1.append( hits['ch4'] );
        if len(self._TOF1) == 11 :
            self._TOF1.pop(0)

        return True



