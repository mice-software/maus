#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus

import json
import ROOT
import ErrorHandler

class MapPyCkovSecondPeaks:

    def __init__(self):
        """Constructor"""
        self.peak1_val = None
        self.peak1_pos = None
        self.peak2_pos = None
        self.peak2_val = None

    def birth(self, json_configuration):
        try:
            config_doc = json.loads(json_configuration)
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

        peaks = []
        ckov_digit = {}
        samples = []
        samples_reduce = []
        second_pulses = []
        second_pulse_threshold = 1
        
        for i in range(len(ckov)):
            #print json.dumps(ckov[i], sort_keys=True, indent=4)
            if ckov[i] != None :
                ckov_digit['B'] = {}
                ckov_digit['A'] = {}
                ckov_digit['B']['peak2_value']    = [0,0,0,0]
                ckov_digit['B']['peak2_position'] = [0,0,0,0]
                ckov_digit['A']['peak2_value']    = [0,0,0,0]
                ckov_digit['A']['peak2_position'] = [0,0,0,0]

                #To find secondary peaks, a secons sample array, sample-reduce, is created with the
                #original samples, but with fADC values within the largest peak set to 200
                #(so as not to be counted as the minimum).
                #The minimum within the new array is found and bins from -10 to +20 are summed.
                
                for pmt in range(0,8):
                    for s in range(len(ckov[i]['V1731'][pmt]['samples'])):
                        samples.append(ckov[i]['V1731'][pmt]['samples'][s])
                    pedestal = ckov[i]['V1731'][pmt]['pedestal']
                    self.peak1_val = ckov[i]['V1731'][pmt]['charge_mm']
                    self.peak1_pos = samples.index(min(samples))
                    
                    if (pmt < 4 and self.peak1_val > 0 and  self.peak1_pos > 10 and  self.peak1_pos < (len(samples)-20)):
                        lower_bound = self.peak1_pos - 10
                        upper_bound = self.peak1_pos + 20
                        end = len(samples)

                   #Create new array excluding the inital peak
                   
                        for a in range(0, lower_bound):
                            samples_reduce.append(samples[a])
                            
                        for b in range(lower_bound, upper_bound):
                            samples_reduce.append(200)
                                
                        for c in range(upper_bound, end):
                            samples_reduce.append(samples[c])

                    #Find the new minimum using python's built-in "min" function.
                        if (pedestal - min(samples_reduce))> second_pulse_threshold:
                            self.peak2_val = pedestal - min(samples_reduce)
                            self.peak2_pos = samples_reduce.index(min(samples_reduce))
                            ckov_digit['B']['peak2_position'][pmt] = self.peak2_pos
                            ckov_digit['B']['peak2_value'][pmt] = self.peak2_val

                    #If there is no initial peak, there will not be a second.
                    
                        else:
                            self.peak2_val = 0
                            self.peak2_pos = 0
                            ckov_digit['B']['peak2_position'][pmt] = self.peak2_pos
                            ckov_digit['B']['peak2_value'][pmt] = self.peak2_val

                    #second Cherenkov; same procedure.
                    
                    elif (pmt > 3 and self.peak1_val > 0 and  self.peak1_pos > 10 and  self.peak1_pos < (len(samples)-20)):
                        lower_bound = self.peak1_pos - 10
                        upper_bound = self.peak1_pos + 20
                        end = len(samples)
                        
                        for a in range(0, lower_bound):
                            samples_reduce.append(samples[a])
                            
                        for b in range(lower_bound, upper_bound):
                            samples_reduce.append(200)
                                
                        for c in range(upper_bound, end):
                            samples_reduce.append(samples[c])
                                    
                        if (pedestal - min(samples_reduce))> second_pulse_threshold:
                            self.peak2_val = pedestal - min(samples_reduce)
                            self.peak2_pos = samples_reduce.index(min(samples_reduce))
                            ckov_digit['A']['peak2_position'][pmt - 4] = self.peak2_pos
                            ckov_digit['A']['peak2_value'][pmt - 4] = self.peak2_val
                        
                        else:
                            self.peak2_val = 0
                            self.peak2_pos = 0
                            ckov_digit['A']['peak2_position'][pmt - 4] = self.peak2_pos
                            ckov_digit['A']['peak2_value'][pmt - 4] = self.peak2_val
                                                
                    else:
                        self.peak2_val = 0
                        self.peak2_pos = 0
                        ckov_digit['A']['peak2_position'][pmt - 4] = self.peak2_pos
                        ckov_digit['A']['peak2_value'][pmt - 4] = self.peak2_val
                            
                    samples_reduce = []
                    samples = []
                    self.peak2_pos = 0
                    self.peak2_val = 0
                                                                             
                peaks.append(ckov_digit.copy())
                            
        spill['daq_data']['ckov'] = {}
        if 'peaks' not in spill:
            spill['peaks'] = {}
            
            spill['peaks'] = peaks
            
        return json.dumps(spill)

    def death(self):
        return True
