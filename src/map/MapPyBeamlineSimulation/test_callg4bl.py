#!/usr/bin/env python

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

"""Script to test the CallG4bl Application"""

import unittest
import os
import fileinput
from ast import literal_eval

from CallG4bl import CallG4bl

class TestCallG4bl(unittest.TestCase): #pylint: disable = R0904, R0902
    """
    Set of tests for CallG4bl.
    """
    @classmethod
    def set_up_class(cls): #pylint: disable = R0904
        """
        A beam of known parameters will be simulated
        and compared to a pre-generated sample
        """

        cls.rotation_angle = 0
        cls.translation_z = 0
        cls.test_particles = {}
        cls.random_seed = 0

        cls.test_file_path = os.path.join(os.environ['MAUS_ROOT_DIR'], \
        'src', 'map', 'MapPyBeamlineSimulation')

        cls.g4bl_file_path = os.path.join(os.environ['MAUS_ROOT_DIR'], \
        'src', 'map', 'MapPyBeamlineSimulation', 'G4bl')

        cls.path_g4bl = os.path.join(os.environ['MAUS_ROOT_DIR'], \
        'third_party', 'build', 'G4beamline-2.12-source', 'bin')

        cls.output_path = os.path.join(os.getcwd())
        
        cls.test_newline = "param -unset q_1=1.155989 q_2=-1.445 \
        q_3=1.005937 " + "d_1=-1.413 proton_absorber_thickness=29 \
        proton_weight=100 " + "proton_number=1.97E9 " + \
        "g4bl_output_file_name=" + cls.test_file_path + \
        "/maus_beam_output " + "d_s=86.02 particle_charge=3 " + \
        "WRKDIR=" + cls.g4bl_file_path + " Random_Seed=0" + \
        " DecayMapFileDir=" + cls.g4bl_file_path + \
        "/Magnets/DecaySolenoid"

        cls.test_db = {}
        try:
            off = open(cls.test_file_path + \
                       '/test_maus_beam_output_db.txt',"r")
        except IOError as err:
            raise IOError(err) 
        cls.test_db = literal_eval(off.read())
        off.close()
        
        for i in range(0, len(cls.test_db)):
            for key in cls.test_db['entry' + str(i)]['position']:
                cls.test_db['entry' + str(i)]['position'][key] = \
                int(cls.test_db['entry' + str(i)]['position'][key])
            for key in cls.test_db['entry' + str(i)]['momentum']:
                cls.test_db['entry' + str(i)]['momentum'][key] = \
                int(cls.test_db['entry' + str(i)]['momentum'][key])
            cls.test_db['entry' + str(i)]['time'] = \
            int(cls.test_db['entry' + str(i)]['time'])
            cls.test_db['entry' + str(i)]['energy'] = \
            int(cls.test_db['entry' + str(i)]['energy'])

    def test_callg4bl(cls): #pylint: disable = R0912, R0915, E0213, R0914
        """
        Test CallG4bl class
        """

        cls.rotation_angle = 0
        cls.translation_z = 0
        cls.test_particles = {}
        cls.random_seed = 0

        cls.test_file_path = os.path.join(os.environ['MAUS_ROOT_DIR'], \
        'src', 'map', 'MapPyBeamlineSimulation')

        cls.g4bl_file_path = os.path.join(os.environ['MAUS_ROOT_DIR'], \
        'src', 'map', 'MapPyBeamlineSimulation', 'G4bl')

        cls.path_g4bl = os.path.join(os.environ['MAUS_ROOT_DIR'], \
        'third_party', 'build', 'G4beamline-2.12-source', 'bin')

        cls.output_path = os.path.join(os.getcwd())

        cls.test_newline = "param -unset q_1=1.155989 q_2=-1.445 \
        q_3=1.005937 " + "d_1=-1.413 proton_absorber_thickness=29 \
        proton_weight=100 " + "proton_number=1.97E9 " + \
        "g4bl_output_file_name=" + cls.test_file_path + \
        "/maus_beam_output " + "d_s=86.02 particle_charge=3 " + \
        "WRKDIR=" + cls.g4bl_file_path + " random_seed=0" + \
        " DecayMapFileDir=" + cls.g4bl_file_path + \
        "/Magnets/DecaySolenoid"

        cls.test_db = {}
        try:
            off = open(cls.test_file_path + \
                       '/test_maus_beam_output_db.txt',"r")
        except IOError as err:
            raise IOError(err)
        cls.test_db = literal_eval(off.read())
        off.close()

        for i in range(0, len(cls.test_db)):
            for key in cls.test_db['entry' + str(i)]['position']:
                cls.test_db['entry' + str(i)]['position'][key] = \
                int(cls.test_db['entry' + str(i)]['position'][key])
            for key in cls.test_db['entry' + str(i)]['momentum']:
                cls.test_db['entry' + str(i)]['momentum'][key] = \
                int(cls.test_db['entry' + str(i)]['momentum'][key])
            cls.test_db['entry' + str(i)]['time'] = \
            int(cls.test_db['entry' + str(i)]['time'])
            cls.test_db['entry' + str(i)]['energy'] = \
            int(cls.test_db['entry' + str(i)]['energy'])

        test_particles = CallG4bl(cls.test_newline, cls.g4bl_file_path, \
                                  cls.rotation_angle, cls.translation_z, \
                                  cls.path_g4bl, cls.output_path, \
                                  cls.random_seed)

        cls.assertNotEqual(cls.test_file_path + \
        '/test_maus_beam_output_db.txt', \
        cls.test_file_path + '/maus_beam_output.txt' ,\
        'Error: The randomization process in CallG4bl failed.')

        of1 = open(cls.test_file_path + '/maus_beam_output.txt')
        test_line = []
        for column in fileinput.input\
                ([cls.test_file_path + '/maus_beam_output.txt']):
            test_line.append(column)
        of1.close()

        for i in range(3):
            test_line.pop(0)
   
        of2 = open(cls.test_file_path + '/maus_beam_random.txt')
        line_for_test = []
        for column in fileinput.input([cls.test_file_path + \
        '/maus_beam_random.txt']):
            line_for_test.append(column)
        of2.close()

        for i in range(3):
            line_for_test.pop(0)

        cls.assertTrue(len(line_for_test)  == 105, \
        'The amount of entries is not correct.')

        no_error = True
        i = 0
        while i <= 104 and no_error:
            j = 0
            keep_on = True
            no_error = False
            while j + 1 <= len(line_for_test) and keep_on:
                if line_for_test[j] == test_line[i]:
#                    print('The entry'+str(i)+' : '+str(line_for_test[j]))
                    line_for_test.pop(j)
                    keep_on = False
                    no_error = True
                j = j + 1
            i = i + 1
        if no_error:
            test_result = True
        else:
            test_result = False
        cls.assertTrue(test_result, 'Randomization Test failed.')

        cls.test_particles = test_particles.particles

        cls.assertTrue(len(cls.test_particles) == 105, \
        'The amount of elements in dictionary is not correct.')

        for i in range(0, len(cls.test_particles)):
            for key in cls.test_particles['entry' + str(i)]['position']:
                cls.test_particles['entry' + str(i)]['position'][key] = \
                int(cls.test_particles['entry' + str(i)]['position'][key])
            for key in cls.test_particles['entry' + str(i)]['momentum']:
                cls.test_particles['entry' + str(i)]['momentum'][key] = \
                int(cls.test_particles['entry' + str(i)]['momentum'][key])
            cls.test_particles['entry' + str(i)]['time'] = \
            int(cls.test_particles['entry' + str(i)]['time'])
            cls.test_particles['entry' + str(i)]['energy'] = \
            int(cls.test_particles['entry' + str(i)]['energy'])
        
        no_error = True
        matched = 0
        for i in range(0, 104):
            #print('--------'+str(i)+'---------')
            #print('checking:'+str(cls.test_db['entry'+str(i)]))
            j = 0
            keep_on = True
            while j + 1 <= len(cls.test_particles) and keep_on:
#                print cls.test_db['entry' + str(i)]
#                print cls.test_particles['entry' + str(j)]
                if cls.test_db['entry' + str(i)] == \
                    cls.test_particles['entry' + str(j)]:
#print('Good:'+str(cls.test_particles['entry'+str(j)]))                   
                    keep_on = False
                    matched = matched + 1                   
                j = j + 1

        print(str(matched) + ' out of 105 entries match the default \
        dictionary.')

        if matched >= 90:   
            test_result = True
        else:
            test_result = False
       
        cls.assertTrue(test_result, \
        'CallG4bl cannot generate an appropriate dictionary.')  

        os.remove(cls.test_file_path + '/maus_beam_output.txt')

    @classmethod
    def teardownclass(cls):
        """
        doc string
        """
        cls.test_file_path = ''
        cls.test_newline = ''

if __name__ == '__main__':
    unittest.main()
        
        
