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

import json
import ROOT
import ErrorHandler

class MapPyCkov:

    def __init__(self):
        """Constructor"""
        self._position_threshold = -1
      
    def birth(self, json_configuration):
        """
        
        """
        try:
            config_doc = json.loads(json_configuration)
            self._position_threshold = config_doc["ckov_position_threshold"]
            if config_doc:
                return True
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)
        return False

    def process(self, doc):

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

        #initializing variables and arrays
        samples_reduce = []
        second_pulses = []
        second_pulse_threshold = 4 #this is the height of the pulse, not the area

        pulse_area_threshold = 1
        pulse = []
        count = 0
        self._position_threshold = 40

        #fixed window bounds on fADC samples
        window_min = 19
        window_max = 80

        area = []
        position = []
        deltas = []
        coincidences = 0
        total_charge = 0

        #array of charge of 1pe values for all 8 PMTs
        charge_to_pe = [20, 20, 20, 20, 20, 20, 20, 20]
                    
        for i in range(len(ckov)):
            reducer = []
            if ckov[i] != None:
                ckov_digit['B'] = {}
                ckov_digit['A'] = {}
                ckov_digit['B']['coincidences']  = 0
                ckov_digit['B']['total_charge']  = 0
                ckov_digit['A']['coincidences']  = 0
                ckov_digit['A']['total_charge']  = 0
                ckov_digit['A']['part_event_number'] = 0
                ckov_digit['B']['part_event_number'] = 0

                ckov_digit['B']['arrival_time_1'] = ckov[i]['V1731'][0]['arrival_time']
                ckov_digit['B']['arrival_time_2'] = ckov[i]['V1731'][1]['arrival_time']
                ckov_digit['B']['arrival_time_3'] = ckov[i]['V1731'][2]['arrival_time']
                ckov_digit['B']['arrival_time_4'] = ckov[i]['V1731'][3]['arrival_time']
                ckov_digit['A']['arrival_time_5'] = ckov[i]['V1731'][4]['arrival_time']
                ckov_digit['A']['arrival_time_6'] = ckov[i]['V1731'][5]['arrival_time']
                ckov_digit['A']['arrival_time_7'] = ckov[i]['V1731'][6]['arrival_time']
                ckov_digit['A']['arrival_time_8'] = ckov[i]['V1731'][7]['arrival_time']
                
                ckov_digit['B']['pulse_1'] = ckov[i]['V1731'][0]['pulse_area']
                ckov_digit['B']['pulse_2'] = ckov[i]['V1731'][1]['pulse_area']
                ckov_digit['B']['pulse_3'] = ckov[i]['V1731'][2]['pulse_area']
                ckov_digit['B']['pulse_4'] = ckov[i]['V1731'][3]['pulse_area']
                ckov_digit['A']['pulse_5'] = ckov[i]['V1731'][4]['pulse_area']
                ckov_digit['A']['pulse_6'] = ckov[i]['V1731'][5]['pulse_area']
                ckov_digit['A']['pulse_7'] = ckov[i]['V1731'][6]['pulse_area']
                ckov_digit['A']['pulse_8'] = ckov[i]['V1731'][7]['pulse_area']

                ckov_digit['B']['max_pos_1'] = ckov[i]['V1731'][0]['max_pos']
                ckov_digit['B']['max_pos_2'] = ckov[i]['V1731'][1]['max_pos']
                ckov_digit['B']['max_pos_3'] = ckov[i]['V1731'][2]['max_pos']
                ckov_digit['B']['max_pos_4'] = ckov[i]['V1731'][3]['max_pos']
                ckov_digit['A']['max_pos_5'] = ckov[i]['V1731'][4]['max_pos']
                ckov_digit['A']['max_pos_6'] = ckov[i]['V1731'][5]['max_pos']
                ckov_digit['A']['max_pos_7'] = ckov[i]['V1731'][6]['max_pos']
                ckov_digit['A']['max_pos_8'] = ckov[i]['V1731'][7]['max_pos']
                
                for pmt in range(0,8):
                    #A pulse pulse threshold is set, as well as a window within which we will look for pulses.
                    #If the 3 criteria are met, the value of the integrated peaks is put into an array "area,"
                    #and their corresponding positons are put into an array "position. If criteria are not met
                    #area = 0 and postion = 300 (well outside the 256 sample values)

                    if ckov[i]['V1731'][pmt]['pulse_area'] >= pulse_area_threshold\
                           and ckov[i]['V1731'][pmt]['position_min'] > window_min\
                           and ckov[i]['V1731'][pmt]['position_min'] < window_max:
                        area.append(ckov[i]['V1731'][pmt]['pulse_area'])
                        #if you want pe instead of total charge divide pulse_area by charge_to_pe[pmt] 
                        position.append(ckov[i]['V1731'][pmt]['position_min'])
                        max_pulse_ind = area.index(max(area))
                    else:
                        area.append(0)
                        position.append(300)
                        max_pulse_ind = 0
                        
                    if pmt == 3 or pmt == 7:
                        #Once the pulse areas are filled for the first 4 PMTs, coincidences are created. 
                        #No pulses are associated with no charge and no coincidences

                        if area.count(0) == 4:
                            ckov_digit['B']['total_charge'] = 0
                            ckov_digit['B']['coincidences'] = 0
                            ckov_digit['B']['part_event_number'] = ckov[i]['V1731'][pmt]['part_event_number']

                            #One hit corresponds to one coincidecne and
                            #the total charge is the value of that single peak

                        if area.count(0) == 3:
                            ckov_digit['B']['total_charge'] = sum(area)
                            ckov_digit['B']['coincidences'] = 1
                            ckov_digit['B']['part_event_number'] = ckov[i]['V1731'][pmt]['part_event_number']

                            #For multiple peaks, the pulse areas are correlated to their position.
                            #The peaks are constrained to fall within a position threshold from the largest peak
                            
                        elif area.count(0) < 3:
                            for k in range(len(area)):
                                deltas.append(abs(position[max_pulse_ind]-position[k]))
                            reducer.append(deltas)
                            reducer.append(area)

                            for pos in range(len(reducer[0])):
                                if reducer[0][pos] > self._position_threshold:
                                    reducer[1][pos] = 0

                            ckov_digit['B']['total_charge'] = sum(reducer[1])
                            ckov_digit['B']['coincidences'] = len(reducer[1]) - reducer[1].count(0)
                            ckov_digit['B']['part_event_number'] = ckov[i]['V1731'][pmt]['part_event_number']

                        deltas = []
                        reducer = []
                        area = []
                        position = []
         
        #spill['daq_data']['ckov'] = {}                                      
        if 'digits' not in spill:
            spill['digits'] = {}

        spill['digits'] = digits
            
        return json.dumps(spill)
    
    def process_pmts(pmts):
        pass

    def death(self):
        
        return True
   
