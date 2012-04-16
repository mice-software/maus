"""Finds Second Peaks in Ckov fADC output """

# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

#pylint: disable=C0301

import json

import ErrorHandler

class MapPyCkovSecondPeaks:
    """Class for Finding Second Peaks in Ckovs"""
    def __init__(self):
        """Constructor"""
        self.peak1_val = None
        self.peak1_pos = None
        self.peak2_pos = None
        self.peak2_val = None

    def birth(self, json_configuration):
        """birth: check json doc"""
        try:
            config_doc = json.loads(json_configuration)
            if config_doc:
                return True
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)
        return False
                               
    def process(self, doc):#pylint: disable=R0912,R0914,R0915
        """Ckov Second Peaks"""
        try:
            spill = json.loads(doc)
        except ValueError:
            spill = {"errors": {"bad_json_document":
                                "unable to do json.loads on input"} }
            return json.dumps(spill)
                                    
        spill = json.loads(doc)
        if 'daq_data' not in spill:
            return doc
        if spill['daq_data'] is None:
            return doc
        daq_event = spill['daq_data']
        if 'ckov' not in daq_event:
            return doc
        ckov = daq_event['ckov']

        digits = []
        ckov_digit = {}
        samples = []
        samples_reduce = []
        second_pulse_threshold = 1
        
        for i in range(len(ckov)):
            #print json.dumps(ckov[i], sort_keys=True, indent=4)
            if ckov[i] != None :
                ckov_digit['B2'] = {}
                ckov_digit['A2'] = {}
                ckov_digit['B2']['peak2_value']    = [0, 0, 0, 0]
                ckov_digit['B2']['peak2_position'] = [0, 0, 0, 0]
                ckov_digit['A2']['peak2_value']    = [0, 0, 0, 0]
                ckov_digit['A2']['peak2_position'] = [0, 0, 0, 0]
                
                #To find secondary peaks, a secons sample array, sample-reduce, is created with the
                #original samples, but with fADC values within the largest peak set to 200
                #(so as not to be counted as the minimum).
                #The minimum within the new array is found and bins from -10 to +20 are summed.
                
                for pmt in range(0, 8):#pylint: disable=C0103
                    for j in range(len(ckov[0]['V1731'][0]['samples'])):
                        samples.append(ckov[i]['V1731'][pmt]['samples'][j])
                    if pmt <= 3:
                        ckov_sta = 'A2'
                        slot = pmt
                    if pmt >= 4:
                        ckov_sta = 'B2'
                        slot = pmt -4
                    ckov_digit[ckov_sta]['part_event_number'] = ckov[i]['V1731'][pmt]['part_event_number']
                    pedestal = ckov[i]['V1731'][pmt]['pedestal']
                    self.peak1_val = ckov[i]['V1731'][pmt]['charge_mm']
                    self.peak1_pos = samples.index(min(samples))
                    
                    if (self.peak1_val > 0 and  self.peak1_pos > 10 and  self.peak1_pos < (len(samples) - 20)):
                        lower_bound = self.peak1_pos - 10
                        upper_bound = self.peak1_pos + 20
                        end = len(samples)

                   #Create new array excluding the inital peak
                   
                        for low in range(0, lower_bound):
                            samples_reduce.append(samples[low])
                            
                        for peak in range(lower_bound, upper_bound):#pylint: disable=W0612
                            samples_reduce.append(200)
                                
                        for high in range(upper_bound, end):
                            samples_reduce.append(samples[high])

                    #Find the new minimum using python's built-in "min" function.
                        if (pedestal - min(samples_reduce) )> second_pulse_threshold:
                            self.peak2_val = pedestal - min(samples_reduce)
                            self.peak2_pos = samples_reduce.index(min(samples_reduce))
                            ckov_digit[ckov_sta]['peak2_position'][slot] = self.peak2_pos
                            ckov_digit[ckov_sta]['peak2_value'][slot] = self.peak2_val

                    #If there is no initial peak, there will not be a second.
                    
                        else:
                            self.peak2_val = 0
                            self.peak2_pos = 0
                            ckov_digit[ckov_sta]['peak2_position'][slot] = self.peak2_pos
                            ckov_digit[ckov_sta]['peak2_value'][slot] = self.peak2_val

                    samples_reduce = []
                    samples = []
                    self.peak2_pos = 0
                    self.peak2_val = 0
                                                                             
                digits.append(ckov_digit.copy())
                            
        spill['daq_data']['ckov'] = {}
        if 'digits' not in spill:
            spill['digits'] = {}
            
            spill['digits'] = digits
            
        return json.dumps(spill)

    def death(self):#pylint: disable=R0201
        """death"""

        return True
