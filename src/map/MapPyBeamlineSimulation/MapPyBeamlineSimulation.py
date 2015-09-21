"""
BeamlineSimulation simulates MICE beam from Target to GVA1
"""
#!/usr/bin/env python

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

import os
import json
import Queue
import random
from CallG4bl import CallG4bl

class MapPyBeamlineSimulation: #pylint: disable = R0902 
    """

    Class takes beam as defined by user in Configuration file and
    simulates MICE experiment from Target to Geneva 1 Counter using
    G4beamline. The output is written to a json document. This is
    read by BeamMaker allowing the entire MICE experiment to be
    simulated end-to-end.
    
    -q_1,q_2,q_3,d_1 & d_s correspond to the currents in each of the magnets.
    Can be positive or negative to change magnet polarity
    -rotation_angle is the angle with which the MAUS coordinates are rotated
    clockwise with respect to the G4BL coordinates around the y-axis.
    -translation_z is the the distance between the MAUS center and the G4BL center.
    It assumes the G4BL center is in front of the MAUS center.
    -run_number is the spill number, starting from 0 
    -get_magnet_currents_pa_cdb if set to 'True' magnet currents and
    proton_absorber_thickness will be retrieved from CDB
    -particles_per_spill takes particles out of buffer for each spill, if
    set to zero then all particles are taken from buffer each spill
    -proton_number is the number of protons generated in the g4bl input file
    By default it generates 79k protons
    -Particle Charge refers to the charge of the simulated particle.
     Users have the following three options:
      positive-simulate only positive charges.
      negative-simulate only negative charges.
      all-simulate both charges of particles.
    -short particles is used to keep track of the number of particles needed
    if the buffer runs out
    -queue is used to define the buffer
    Initialize and read in default configuration files and overide with data
    card parameters
    -protonabsorberin set to 1 for proton absorber to be included in deck and
    0 for no proton absorber
    """
    def __init__(self):
        
        self.q_1 = 0
        self.q_2 = 0
        self.q_3 = 0
        self.d_1 = 0
	self.d_2 = 0
        self.d_s = 0
        self.proton_weight = 0
        self.particles_per_spill = 0 
        self.proton_absorber_thickness = 0
        self.proton_number = 0
        self.particle_charge = ''
        self.rotation_angle = 0
        self.translation_z = 0
        self.run_number = 0
        self.get_magnet_currents_pa_cdb = 0
        self.random_seed = 0
        self.queue = 0
        self.file_path = ''
        self.path_g4bl = ''
        self.output_path = ''
        self.charge = 3
        self.newline = ''
        self.grid_job = 0 
	self.protonabsorberin = 1
   
    def birth(self, json_configuration): #pylint: disable=R0912, R0915
        "birth doc string"      
        good_birth = True 

        try:      
            config_doc = json.loads(json_configuration) 
        except Exception: #pylint: disable=W0703
            print("Error: No configuration file!")
            good_birth = False

        try:
            self.protonabsorberin = config_doc["g4bl"]["protonabsorberin"]
        except Exception: #pylint: disable=W0703
            print("Error: protonabsorberin is not found in the config file!")
            good_birth = False 

        try:
            self.q_1 = config_doc["g4bl"]["q_1"]
        except Exception: #pylint: disable=W0703
            print("Error: q_1 is not found in the config file!")
            good_birth = False

        try:
            self.run_number = config_doc["g4bl"]["run_number"]
        except Exception: #pylint: disable=W0703
            print("Error: run_number is not found in the config file!")
            good_birth = False

        try:
            self.get_magnet_currents_pa_cdb = config_doc["g4bl"]\
                                              ["get_magnet_currents_pa_cdb"]
        except Exception: #pylint: disable=W0703, C0301
            print("Error: get_magnet_currents_pa_cdb is not \
            found in the config file!")
            good_birth = False
   
        try:
            self.q_2 = config_doc["g4bl"]["q_2"]
        except Exception: #pylint: disable=W0703
            print("Error: q_2 is not found in the config file!")
            good_birth = False

        try:
            self.q_3 = config_doc["g4bl"]["q_3"]
        except Exception: #pylint: disable=W0703
            print("Error: q_3 is not found in the config file!")
            good_birth = False        

        try:
            self.d_1 = config_doc["g4bl"]["d_1"]
        except Exception: #pylint: disable=W0703
            print("Error: d_1 is not found in the config file!")
            good_birth = False             

	try:
            self.d_2 = config_doc["g4bl"]["d_2"]
        except Exception: #pylint: disable=W0703
           print("Error: d_2 is not found in the config file!")
           good_birth = False
	
	try:
            self.d_s = config_doc["g4bl"]["d_s"]
        except Exception: #pylint: disable=W0703
            print("Error: d_s current is not found in" +\
                  " the config file!")
            good_birth = False                    
                  
        try:
            self.particles_per_spill = config_doc["g4bl"]["particles_per_spill"]
        except Exception: #pylint: disable=W0703
            print("Error: particles_per_spill is not found in the config file!")
            good_birth = False
       
        try:
            self.file_path = os.path.join(os.environ['MAUS_ROOT_DIR'], \
            'src', 'map', 'MapPyBeamlineSimulation', 'G4bl')
        except Exception: #pylint: disable=W0703
            print('Error: Cannot find file path'+\
            ' /src/map/MapPyBeamlineSimulation/G4bl')  
            good_birth = False

#	try:
#            self.grid_job = config_doc["g4bl"]["grid_job"]
#	except Exception: #pylint: disable=W0703
#	    print("Error: grid_job is not found in the config file!")
#	    good_birth = False

#	if str(grid_job) in ['True']:
#        try: 
#            self.file_path_param = ''
#        except Exception: #pylint: disable=W0703 
#            print('Error: Cannot find file path'+\
#            ' /src/map/MapPyBeamlineSimulation/G4bl')
#            good_birth = False
            
        try:
            self.path_g4bl = os.path.join(os.environ['MAUS_THIRD_PARTY'], \
            'third_party', 'build', 'G4beamline-2.12-source', 'bin')
        except Exception: #pylint: disable=W0703
            print('Error: Cannot find file path'+\
            ' /third_party/install/bin')
            good_birth = False

        try:
            self.output_path = os.path.join(os.getcwd())
        except Exception: #pylint: disable=W0703
            print('Error: Cannot find output path'+\
            ' /bin')
            good_birth = False

        try:
            self.rotation_angle = config_doc["g4bl"]["rotation_angle"]
        except Exception: #pylint: disable=W0703
            print("Error: rotation_angle is not found in the config file!")
            good_birth = False

        try:
            self.translation_z = config_doc["g4bl"]["translation_z"]
        except Exception: #pylint: disable=W0703
            print("Error: translation_z is not found in the config file!")
            good_birth = False      
 
        try:
            self.proton_absorber_thickness = config_doc["g4bl"]\
            ["proton_absorber_thickness"]
            if self.proton_absorber_thickness < 0:
                print('Error: proton_absorber_thickness is negative!')
                good_birth = False
        except Exception: #pylint: disable=W0703
            print("Error: proton_absorber_thickness is not found in"+\
                  " the config file!")
            good_birth = False

        try:
            self.proton_weight = config_doc["g4bl"]["proton_weight"]
            if self.proton_weight <= 0:
                print('Error: Number of protons must be greater than zero!')
                good_birth = False
        except Exception: #pylint: disable=W0703
            print("Error: proton_weight is not found in the config file!")
            good_birth = False

        try:
            self.proton_number = config_doc["g4bl"]["proton_number"]
            if self.proton_number <= 0:
                print('Error: Number of protons must be greater than zero!')
                good_birth = False
        except Exception: #pylint: disable=W0703
            print("Error: proton_numer is not found in the config file!")
            good_birth = False

	try:
            self.random_seed = config_doc["g4bl"]["random_seed"]
	    if self.random_seed == -1:
		    self.random_seed = random.randint(1,1000000)
        except Exception: #pylint: disable = W0703
            print("Error: random_seed is not found in the config file!")
	    good_birth = False
           
        try:
            self.particle_charge = config_doc["g4bl"]["particle_charge"]
            if self.particle_charge != 'positive' and \
            self.particle_charge != 'negative' and \
            self.particle_charge != 'all':
                print('Error: particle_charge is not properly defined: '+\
                'positive, negative, or all.')
                good_birth = False
            else:
                self.charge = 3
                if self.particle_charge  == 'positive':
                    self.charge = 1
                elif self.particle_charge == 'negative':
                    self.charge = 2
        except Exception: #pylint: disable=W0703
            print("Error: particle_charge is not found in the config file!")
            good_birth = False

        try:
            self.newline = "param -unset q_1="+str(self.q_1)+ \
            " q_2="+str(self.q_2)+ \
	    " protonabsorberin="+str(self.protonabsorberin)+ \
            " q_3="+str(self.q_3)+" d_1="+ str(self.d_1) +\
	    " d_2="+ str(self.d_2) +\
            " proton_absorber_thickness="+str(self.proton_absorber_thickness)+\
            " proton_number="+str(self.proton_number)+\
            " proton_weight="+str(self.proton_weight)+\
            " WRKDIR="+str(self.file_path)+\
            " g4bl_output_file_name="+str(self.file_path)[:-5]+ \
            '/maus_beam_output.txt'+\
            " d_s="+str(self.d_s)+\
            " particle_charge="+str(self.charge)+\
            " DecayMapFileDir="+self.file_path+'/Magnets/DecaySolenoid'+"\n" 
        except Exception: #pylint: disable=W0703
            print('Error: Could not write G4bl parameters')
            good_birth = False

        return good_birth               

    def process(self, json_spill_doc): #pylint: disable=R0912
        """
        Execute G4beamline and create a simulation based on the read in parameters.
        Fill buffer first time spill is called for
        """

        spill = {}
        spill = json.loads(json_spill_doc)

        if self.queue == 0:
            self.queue = Queue.Queue()
            particles = CallG4bl(self.newline, self.file_path, \
                                 self.rotation_angle, \
                                 self.translation_z, \
                                 self.path_g4bl, \
                                 self.output_path, \
                                 self.run_number, \
                                 self.get_magnet_currents_pa_cdb, \
                                 self.random_seed)
            print("Filling buffer. This may take a while...")
            self.particles = particles.particles
            for i in range(len(self.particles)):
                self.queue.put_nowait(self.particles['entry'+str(i)])

        #print len(self.particles)
        if len(self.particles) != 0:
            if self.particles_per_spill == 0:
                spill = {}
                spill = json.loads(json_spill_doc)
                print spill
                spill['mc_events'] = []
                for i in range(self.queue.qsize()):
                    primary = (self.queue.get_nowait())
                    spill['mc_events'].append({"primary":primary})
                with open('G4BLoutput.json', 'w') as outfile:
                    json.dump(spill, outfile)
                    #self.queue = 0  
                return json.dumps(spill)
            else:
                spill = {}
                spill = json.loads(json_spill_doc)
                spill['mc_events'] = []
                if self.queue.qsize() < self.particles_per_spill:
                    print("Warning: particles per spill( "\
                    +str(self.particles_per_spill)+\
                    " ) is beyond the total particles \
                    available in buffer("+\
                    " %d"%self.queue.qsize()+" ).")
                    print("Generating more particles in buffer. \
                          May take a while...") 
                    while self.queue.qsize()<self.particles_per_spill:
                        try:
                            particles = CallG4bl(self.newline, \
                                             self.file_path, \
                                             self.rotation_angle, \
                                             self.translation_z, \
                                             self.path_g4bl, \
                                             self.output_path, \
                                             self.run_number, \
                                             self.get_magnet_currents_pa_cdb, \
                                             self.random_seed)
                            self.particles = particles.particles #pylint: disable = W0201, C0301
                            for i in range(len(self.particles)):
                                self.queue.put_nowait \
                                (self.particles['entry'+str(i)])
                        except Exception: #pylint: disable = W0703
                            print("could not fill buffer")
       
                    for i in range(self.particles_per_spill):
                        primary = (self.queue.get_nowait())
                        spill['mc_events'].append({"primary":primary})
                    with open('G4BLoutputsimulation'+str(i)+'.json', 'w') \
                                                              as outfile:
                        json.dump(spill, outfile)
                    return json.dumps(spill)
                else:
                    for i in range(self.particles_per_spill):
                        primary = (self.queue.get_nowait())
                        spill['mc_events'].append({"primary":primary})
                    with open('G4BLoutputsimulation'+str(i)+'.json', 'w') \
                                                              as outfile:
                        json.dump(spill, outfile)
                    return json.dumps(spill)
        else:
            print("Warning: G4BL simulated zero output particles!")
            raise SystemExit
       
    def death(self): #pylint: disable = R0201
        """
        death of class
        """
        return True    
