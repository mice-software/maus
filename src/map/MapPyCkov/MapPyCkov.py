"""
Mapper for Cherenkov Detector
"""
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

#pylint:disable=C0301
import json
import ErrorHandler

class MapPyCkov:
    """Class for Cherenkov Detector"""
    
    def __init__(self):
        """Constructor"""
        self._position_threshold = 40
        self._pulse_area_threshold = 0
        self._window_min = 19
        self._window_max = 80
        
    def birth(self, json_configuration):
        """
        Sets up birth. Places variables and parameters
        in ConfigurationDefaults
        """
        try:
            json_configuration = unicode(json_configuration)
            config_doc = json.loads(json_configuration)
            if config_doc:
                keys = []
                keys.append("ckov_position_threshold")
                keys.append("ckov_pulse_area_threshold")
                keys.append("ckov_window_min")
                keys.append("ckov_window_max")
                
                for i in range(len(keys)):
                    if keys[i] in config_doc:
                        self._position_threshold = config_doc[str(keys[0])]
                        self._pulse_area_threshold = config_doc[str(keys[1])]
                        self._window_min = config_doc[str(keys[2])]
                        self._window_max = config_doc[str(keys[3])]
                    
                return True

        except Exception: #pylint:disable=W0703
            ErrorHandler.HandleException({}, self)
            return False
           
    def process(self, doc): #pylint:disable=R0912,R0914,R0915,R0911
        """
        Finds coincidences in each of the PMTs and outputs the total charge
        and number of photoelectrons for a single particle event.
        """

        try:
            spill = json.loads(doc)
        except ValueError:
            spill = {"errors": {"bad_json_document":
                                "unable to do json.loads on input"} }
            return json.dumps(spill)

        if 'daq_data' not in spill:
            return doc
        if spill['daq_data'] is None:
            return doc

        # look for daq only in physics or calib event types
        if spill["daq_event_type"] != "physics_event" and \
           spill["daq_event_type"] != "calibration_event":
            return doc

        daq_event = spill['daq_data']
        if 'ckov' not in daq_event:
            return doc
        ckov = daq_event['ckov']

        digits = {}
        ckov_digit = {}

        #initializing variables and arrays
       
        area = []
        photoelectrons = []
        reducer = []
        position = []
        deltas = []

        #array of charge for a single photo-electron in all 8 PMTs
        one_pe = [23.0, 23.0, 23.0, 23.0, 23.0, 23.0, 23.0, 23.0]
                    
        for i in range(len(ckov)):
            if ckov[i] != None:
                # move on if there is no flash adc data
                # this can happen if the ckov did not readout
                # or was disabled. it is not an error per se
                if not any(ckov[i]['V1731']):
                    continue
                ckov_digit['A'] = {}
                ckov_digit['B'] = {}

                for pmt in range(0, 8):
                    if pmt <= 3:
                        ckov_sta = 'A'
                                            
                    if pmt >= 4:
                        ckov_sta = 'B'
                        
                    arrival_time = "arrival_time_%d" % (pmt)
                    pulse = "pulse_%d" % (pmt)
                    position_min = "position_min_%d" % (pmt)
                    
                    ckov_digit[ckov_sta]['coincidences']      = 0
                    ckov_digit[ckov_sta]['total_charge']      = 0
                    ckov_digit[ckov_sta]['number_of_pes']     = 0.
                    ckov_digit[ckov_sta]['part_event_number'] = 0
                   
                    ckov_digit[ckov_sta][arrival_time] = \
                               ckov[i]['V1731'][pmt]['arrival_time']
                    ckov_digit[ckov_sta][pulse]        = \
                               ckov[i]['V1731'][pmt]['pulse_area']
                    ckov_digit[ckov_sta][position_min] = \
                               ckov[i]['V1731'][pmt]['position_min']
                    
                    
        #A pulse pulse threshold is set, as well as a window within which we
        #will look for pulses. If the 3 criteria are met, the value ofthe
        #integrated peaks is put into an array, "area," and their corresponding
        #positons are put into an array, "position". If criteria are not met
        #area = 0 and postion = 300 (well outside the 256 sample values)
                    
                    
                    if ckov[i]['V1731'][pmt]['pulse_area'] >= self._pulse_area_threshold\
                           and ckov[i]['V1731'][pmt]['position_min'] > self._window_min\
                           and ckov[i]['V1731'][pmt]['position_min'] < self._window_max:
                        area.append(ckov[i]['V1731'][pmt]['pulse_area'])
                        photoelectrons.append(ckov[i]['V1731'][pmt]['pulse_area']/one_pe[pmt])
                                                
                        position.append(ckov[i]['V1731'][pmt]['position_min'])
                        max_pulse_ind = area.index(max(area))
                    else:
                        area.append(0)
                        photoelectrons.append(0)
                        position.append(300)
                        max_pulse_ind = 0
                        
                    if pmt == 3 or pmt == 7:

        #Once the pulse areas are filled for the first 4 PMTs, coincidences
        #are created. No pulses are associated with no charge and no coincidences
                        
                        #CKOVa
                        if pmt == 3:
                            ckov_sta = 'A'

                        #CKOVb
                        if pmt == 7:
                            ckov_sta = 'B'
                            
                        if area.count(0) == 4:
                            ckov_digit[ckov_sta]['total_charge'] = 0
                            ckov_digit[ckov_sta]['number_of_pes'] = 0.
                            ckov_digit[ckov_sta]['coincidences'] = 0
                            ckov_digit[ckov_sta]['part_event_number'] = \
                                       ckov[i]['V1731'][pmt]['part_event_number']
                           
                            #One hit corresponds to one coincidecne and the
                            #total charge is the value of that single peak
                            
                        if area.count(0) == 3:
                            ckov_digit[ckov_sta]['total_charge'] = sum(area)
                            ckov_digit[ckov_sta]['number_of_pes'] = float(sum(photoelectrons))
                            ckov_digit[ckov_sta]['coincidences'] = 1
                            ckov_digit[ckov_sta]['part_event_number'] = \
                                       ckov[i]['V1731'][pmt]['part_event_number']

        #For multiple peaks, the pulse areas are
        #correlated to their position.The peaks are
        #constrained to fall within a position threshold
        #from the largest peak
        #reducer = [[pos1, pos2, pos3, pos4],[area1, area2, area3, area4],\
        #[area1/pe1, area2/pe2, area3/pe3, area4/pe4]]
                            
                        elif area.count(0) < 3:
                            for k in range(len(area)):
                                deltas.append(abs(position[max_pulse_ind]-position[k]))
                            reducer.append(deltas)
                            reducer.append(area)
                            reducer.append(photoelectrons)
                            
                            for pos in range(len(reducer[0])):
                                if reducer[0][pos] > self._position_threshold:
                                    reducer[1][pos] = 0
                                    reducer[2][pos] = 0
                                    
                            ckov_digit[ckov_sta]['total_charge'] = sum(reducer[1])
                            ckov_digit[ckov_sta]['number_of_pes'] = float(sum(reducer[2]))
                            ckov_digit[ckov_sta]['coincidences'] = \
                                      len(reducer[1]) - reducer[1].count(0)
                            ckov_digit[ckov_sta]['part_event_number'] = \
                                      ckov[i]['V1731'][pmt]['part_event_number']

                        #Append a shallow copy of the list
                        if pmt == 7:
                            event_num = ckov_digit[ckov_sta]['part_event_number']
                            if event_num not in digits.keys():
                                digits[event_num] = []
                            digits[event_num].append(ckov_digit.copy())

                        deltas = []
                        reducer = []
                        area = []
                        photoelectrons = []
                        position = []

        #spill['daq_data']['ckov'] = {}
        if 'recon_events' not in spill:
            return json.dumps(spill)

        for event in spill['recon_events']:
            event['ckov_event']['ckov_digits'] = []
            if event['part_event_number'] in digits.keys():
                event['ckov_event']['ckov_digits'] = digits[event['part_event_number']]

        return json.dumps(spill)

    def death(self):#pylint:disable=R0201
        """Death"""
        return True
