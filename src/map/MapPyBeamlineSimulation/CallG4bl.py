"""
Class to call G4BL and generate primaries for MAUS
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

import cdb 
from cdb import Beamline
import random
import sys
from subprocess import call
import json
import os
import fileinput   
import math
import numpy

class CallG4bl: #pylint: disable=R0903
    """
    Calls G4BL and writes output to python dictionary

    Creates primaries for MAUS depending on the parameters
    set in the dictionary 'g4bl' in the json configuration
    document
    - magnet currents and proton absorber thickness can be set
    along with the number of particles to generate
    - to retrieve magnet currents and proton absorber
    thickness from CDB set get_magnet_currents_pa_cdb to
    True and run_number to the run of interest in json
    configuration document
    """
    
    def __init__(#pylint: disable=C0103, R0903, R0912, R0913, R0914, R0915 
                 self,newline='', file_path='', theta=0, deltaz=0, 
                 path_g4bl='', output_path='', run_number=0, \
                 get_magnet_currents_pa_cdb='', random_seed=0):
        """
        - particles will be a dictionary filled by G4BL
        - pdgid masses are input to calculate total energy,
        masses are in MeV/C^2
        """

        self.particles = 0
        self.pdgid_mass = {}

        self.pdgid_mass = {'211':139.5700, '-211':139.5700, '2212':938.27231, \
        '-13':105.6584, '13':105.6584, '11':0.5109906, '-11':0.5109906, \
        '22':0, '2112':939.5653, '14':0, '-14':0, '12':0, '-12':0, \
	'1000010020':1875.6, '1000010030':2808.9, '1000020030':2808.30665, \
        '1000080160':14895.10000, '1000020040':3727.417000, \
	'1000060140':13043.93400, '1000030070': 6535.36500, \
	'1000060120':11174.90000, '1000050100':9326.98900, \
	'1000060130':12112.54500, '1000050110':10255.10100} #pylint: disable=C0103

        try:
            off = open(file_path+'/MAY09-B1B2-positives.in',"r")
        except IOError as err:
            raise IOError(err)
        text = off.read()

        if str(get_magnet_currents_pa_cdb) in ['True']:
            try:
                beamline = Beamline()
                sf3 = open(output_path+'/key.txt',"w")
                sf3.close()
                for k, v in beamline.get_beamline_for_run(run_number).iteritems(): #pylint: disable = C0301
                    for k, v in v.iteritems():
                        if k == 'magnets':
                            for k, v in v.iteritems():
                                newlines = str(k)+'\n'
                                sff = open(output_path+'/key.txt',"r")
                                text5 = sff.read()
                                sff.close()
                                sf2 = open(output_path+'/key.txt',"w")
                                sf2.write(text5)
                                sf2.write(newlines)
                                sf2.close()
                        if k == 'proton_absorber_thickness':
                            newlines = str(k)+'\n'
                            sff = open(output_path+'/key.txt',"r")
                            text5 = sff.read()
                            sff.close()
                            sf2 = open(output_path+'/key.txt',"w")
                            sf2.write(text5)
                            sf2.write(newlines)
                            sf2.close()
                line = []
                for row in fileinput.input([output_path+'/key.txt']):
                    line.append(row[:-1])
                sf2.close()
                sf4 = open(output_path+'/value.txt',"w")
                sf4.close()
                for k, v in beamline.get_beamline_for_run(run_number).iteritems(): #pylint: disable = C0301
                    for k, v in v.iteritems():
                        if k == 'magnets':
                            for k, v in v.iteritems():
                                if k in ['q1', 'q3']:
                                    for k, v in v.iteritems():
                                        if k == 'set_current':
                                            v = v / 96
                                            newlines = str(v)+'\n'
                                            sf5 = open(output_path+'/value.txt',"r") #pylint: disable = C0301
                                            text6 = sf5.read()
                                            sf5.close()
                                            sf6 = open(output_path+'/value.txt',"w") #pylint: disable = C0301
                                            sf6.write(text6)
                                            sf6.write(newlines)
                                            sf6.close()
                                if k in ['q2']:
                                    for k, v in v.iteritems():
                                        if k == 'set_current':
                                            v = - v / 96
                                            newlines = str(v)+'\n'
                                            sf5 = open(output_path+'/value.txt',"r") #pylint: disable = C0301
                                            text6 = sf5.read()
                                            sf5.close() 
                                            sf6 = open(output_path+'/value.txt',"w") #pylint: disable = C0301
                                            sf6.write(text6)
                                            sf6.write(newlines)
                                            sf6.close()
                                if k in ['d2']: #pylint: disable = W0631
                                    for k, v in v.iteritems(): #pylint: disable = W0631, C0301
                                        if k == 'set_current':
                                            coeff = [39.59, -55.998, 256.914, -v] #pylint: disable = C0301
                                            roots = numpy.roots(coeff)
                                            root =  roots[2]
                                            newlines = str(root)[1:-4]+'\n'
                                            sf5 = open(output_path+'/value.txt',"r") #pylint: disable = C0301
                                            text6 = sf5.read()
                                            sf5.close()
                                            sf6 = open(output_path+'/value.txt',"w") #pylint: disable = C0301
                                            sf6.write(text6)
                                            sf6.write(newlines)
                                            sf6.close()
                                if k in ['ds']:
                                    for k, v in v.iteritems():
                                        if k == 'set_current':
                                            v = v / 174
                                            newlines = str(v)+'\n'
                                            sf5 = open(output_path+'/value.txt',"r") #pylint: disable = C0301
                                            text6 = sf5.read()
                                            sf5.close()
                                            sf6 = open(output_path+'/value.txt',"w") #pylint: disable = C0301
                                            sf6.write(text6)
                                            sf6.write(newlines)
                                            sf6.close()
                                if k in ['d1']:
                                    for k, v in v.iteritems():
                                        if k == 'set_current':
                                            coeff = [39.59, -55.998, 256.914, -v] #pylint: disable = C0301
                                            roots = numpy.roots(coeff)
                                            root =  roots[2]
                                            newlines = str(root)[1:-4]+'\n'
                                            sf5 = open(output_path+'/value.txt',"r") #pylint: disable = C0301
                                            text6 = sf5.read()
                                            sf5.close()
                                            sf6 = open(output_path+'/value.txt',"w") #pylint: disable = C0301
                                            sf6.write(text6)
                                            sf6.write(newlines)
                                            sf6.close()
                        if k == 'proton_absorber_thickness':
                            newlines = str(v)+'\n'
                            sf5 = open(output_path+'/value.txt',"r")
                            text6 = sf5.read()
                            sf5.close()
                            sf6 = open(output_path+'/value.txt',"w")
                            sf6.write(text6)
                            sf6.write(newlines)
                            sf6.close()
                     
                line2 = []
                for row in fileinput.input([output_path+'/value.txt']):
                    line2.append(row[:-1])
                sf6.close()

                sf7 = open(output_path+'/magnet_currents.txt',"w")
                sf7.close()

                for c, d in zip(line, line2):
                    text2 = 'param {0}={1}'.format(c, d)+'\n'              
                    sf8 = open(output_path+'/magnet_currents.txt',"r")
                    text10 = sf8.read()
                    sf8.close()
                    sf9 = open(output_path+'/magnet_currents.txt',"w")
                    sf9.write(text10)
                    sf9.write(text2)
                    sf9.close()

                sf8 = open(output_path+'/magnet_currents.txt',"r")
                text3 = sf8.read()
         
                os.remove(output_path+'/key.txt')
                os.remove(output_path+'/value.txt')
                                
            except cdb.CdbTemporaryError:
                sys.excepthook(*sys.exc_info())
                print """
                Failed to run - check network connection!
                """

            try:  
                sf = open(output_path+'/MAY09-B1B2-positives-param.in', "w")   
            except IOError as err:
                raise IOError(err)
            sf.write(newline)
            sf.write('param random_seed='+str(random_seed)+'\n')
            sf.write(text3)
            sf.write(text)
            sf.close()
            off.close()

        else:

            try:
                sf = open(output_path+'/MAY09-B1B2-positives-param.in', "w")
            except IOError as err:
                raise IOError(err)
            sf.write(newline)
            sf.write('param random_seed='+str(random_seed))
            sf.write(text)
            sf.close()
            off.close()
        
        g4bl_command_line_input = path_g4bl + '/g4bl ' + output_path + '/MAY09-B1B2-positives-param.in' #pylint: disable = C0301
        try:
            call(g4bl_command_line_input, shell=True)
        except Exception: #pylint: disable = W0703
            print('Error: Cannot execute G4Beamline.')

        try:
            of = open(output_path+'/G4BLoutput.txt',"r")
        except IOError as err:
            raise IOError(err)   
        line = []
        for row in fileinput.input([output_path+'/G4BLoutput.txt']):
            line.append(row)
        of.close()

        for i in range(3):
            line.pop(0)
        random.shuffle(line)

        self.particles = {} 
        theta = math.radians(theta)
        for i in range(0, len(line)):
            element = []
            for word in line[i].split(' '):
                element.append(word) 
            px = float(element[3])
            py = float(element[4])
            pz = float(element[5])
         
            rest_mass = self.pdgid_mass[str(element[7])]
            part_energy = math.sqrt(px**2+py**2+pz**2+rest_mass**2)
         
            try:     
                key = 'entry'+str(i)

                self.particles[key] = dict(spin = dict(\
                x = float(element[21]),
                y = float(element[22]), 
                z = float(element[23])))

                self.particles[key].update(position = dict(\
            #    x = math.cos(theta) * float(element[0]) + 392.39,
	    #    x = math.cos(theta) * float(element[0]) + 99.92,
	        x = math.cos(theta) * float(element[0]),
                y = float(element[1]), 
                z = math.sin(theta) * float(element[0]) + deltaz + 26.21))

                self.particles[key].update(momentum = dict(\
                x = math.cos(theta) * float(element[3]) - math.sin(theta) * float(element[5]), #pylint: disable = C0301
                y = float(element[4]),
                z = math.sin(theta) * float(element[3]) + math.cos(theta) * float(element[5]))) #pylint: disable = C0301
                self.particles[key].update(time = float(element[6]), \
                particle_id = int(element[7]))
                self.particles[key].update(random_seed = random_seed+i, \
                energy = part_energy)    
            except IOError as err:
                print('Bad G4beamline file, not enough elements or empty file')
                raise IOError(err)
        with open('maus_beam_output.txt', 'w') as outfile:
            json.dump(self.particles, outfile)

#        for i in range(0, len(line)):
#            for j in range(i + 1, len(line)):
###                if self.particles['entry' + str(i)]['random_seed']\
#                   == self.particles['entry' + str(j)]['random_seed']:
#                    self.particles['entry' + str(j)]['random_seed']\
#                   =self.particles['entry' + str(j)]['random_seed']\
#                   +1            

        if(os.path.exists(output_path + '/magnet_currents.txt') == True):
            os.remove(output_path + '/magnet_currents.txt')
