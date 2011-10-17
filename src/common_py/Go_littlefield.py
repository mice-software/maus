"""
Go controls the running of executables - map reduce functionality etc.
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

import gzip
import os
import tempfile
import json
import sys
import functools

# For profiling
import cProfile
import pstats 

# MAUS
from Configuration import Configuration

class Go:
    """
    @class Go
    The Go class will handle the map-reduce and
    gets passed an:
      - input
      - map
      - reduce
      - output
    and has many different ways to do the map-
    reduce.  For example:
      - native single-threaded python
    """

    def __init__(self, arg_input, arg_mapper, arg_reducer, \
                 arg_output, arg_config_file = None): # pylint: disable=R0201
        """
        Initialise the configuration dictionary, input, mapper, reducer and
        output
        @param arg_input Inputter that defines inputs to the map reduce
        @param arg_mapper Mapper that defines the map that is acted on the input
        @param arg_reducer Reducer that defines reduce that is acted on map
                           output
        @param arg_config_file Configuration file
        """
        maus_root_dir = os.environ.get('MAUS_ROOT_DIR')
        current_dir = os.getcwd()

        if maus_root_dir not in current_dir:
            exception_string = "ERROR: "
            exception_string = "YOU ARE RUNNING MAUS OUTSIDE ITS MAUS_ROOT_DIR."
            exception_string += " MAUS_ROOT_DIR is %s but " % maus_root_dir
            exception_string += "your current directory is %s" % current_dir

            print("\n%s" % exception_string)
            raise EnvironmentError('BAD MAUS_ROOT_DIR', exception_string)
            
            

        self.input = arg_input
        self.mapper = arg_mapper
        self.reducer = arg_reducer
        self.output = arg_output

        print("Welcome to MAUS:")
        print(("\tProcess ID (PID): %d" % os.getpid()))
#        print(("\tUniversally Unique ID (UUID): %s" % uuid.uuid4()))
        print(("\tProgram Arguments: %s" % str(sys.argv)))

        self.json_config_document = \
                                  Configuration().getConfigJSON(arg_config_file)
        json_config_dictionary = json.loads(self.json_config_document)
        map_reduce_type = json_config_dictionary['map_reduce_type']
        version = json_config_dictionary["maus_version"]
        print ("\tVersion: %s" % version)

        # Be sure to add other assertions here when new
        # map reduce implementations get put in.
        assert map_reduce_type in ["native_python" , "native_python_profile"]
        
        if map_reduce_type == "native_python":
            self.native_python_map_reduce()
        elif map_reduce_type == "native_python_profile":
            cProfile.runctx('self.native_python_map_reduce()', globals(), \
                           locals(), 'list.prof')
            profile = pstats.Stats('list.prof') 
            profile.strip_dirs().sort_stats('time').print_stats() 
        else:
            # for future methods.  Be sure to add to assertion above
            pass


    def native_python_map_reduce(self):
        """
        Map-reduce algorithm using own, single threaded, input-map-reduce
        routine
        """
        # write intermediary step to disk
        file_obj = tempfile.SpooledTemporaryFile(max_size=524288) # 512 MB
        temp_file = gzip.GzipFile(filename='temp', mode='wb', fileobj=file_obj)

        ####                   ####
        ######  Input Phase  ######
        ####                  #####
        print("INPUT: Reading some input")
        assert(self.input.birth(self.json_config_document) == True)
        emitter = self.input.emitter()
        map_buffer = self.buffer_input(emitter)

        ####                 ####
        ######  Map Phase  ######
        ####                #####
        print("MAP: Setting up mappers")
        assert(self.mapper.birth(self.json_config_document) == True)

        while len(map_buffer) != 0:
            print(("MAP: Processing %d events" % len(map_buffer)))

            map_results = map(self.mapper.process, map_buffer)
            for result in map_results:
                temp_file.write('%s\n' % result)
            map_buffer = self.buffer_input(emitter)

        print("MAP: Closing input and mappers")
        self.input.death()
        self.mapper.death()
        temp_file.close()
        
        
        ####                    ####
        ######  Reduce Phase  ######
        ####                   ##### 
        print("REDUCE: Setting up reducers")
        assert(self.reducer.birth(self.json_config_document) == True)
        
        # read back
        file_obj.seek(0) # go to beginning of file
        temp_file = gzip.GzipFile(filename='temp', mode='rb', fileobj=file_obj)
        i = 0
        reduce_buffer = []
        reduced = []
        for line in temp_file:
            if line.rstrip() == "":
                continue
            reduce_buffer.append(line)
            i = i + 1
            if i % 1000 == 0:
                print(('REDUCE: Reducing %d events in the %d pass' % \
                       (len(reduce_buffer), len(reduced))))
                reduced.append(reduce(self.reducer.process, reduce_buffer))
                reduce_buffer = []

        print(('REDUCE: Merging %d passes and reducing the %d events left in the buffer' % (len(reduced), len(reduce_buffer)))) # pylint: disable=C0301
        reduce_result = functools.reduce \
                                (self.reducer.process, reduce_buffer + reduced)

        temp_file.close()
        file_obj.close()

        self.reducer.death()
        
        ####                    ####
        ######  Output Phase  ######
        ####                   #####

        assert(self.output.birth(self.json_config_document) == True)
        
        self.output.save(reduce_result)
                
        self.output.death()
            

    def buffer_input(self, the_emitter):
        my_buffer = []
        for i in range(1024):
            try:
                value = next(the_emitter)  
                my_buffer.append(value.encode('ascii'))
            except StopIteration:
                return my_buffer

        return my_buffer
                



