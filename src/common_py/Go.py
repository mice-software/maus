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


## @class Go
#  The Go class will handle the map-reduce and
#  gets passed an:
#   - input
#   - map
#   - reduce
#   - output
#   .
#
#  and has many different ways to do the map-
#  reduce.  For example:
#   - native single-threaded python
#   .
#
import gzip
import os
import types
import tempfile
import json
import uuid
import sys
from functools import reduce

# For profiling
import cProfile as profile
import pstats 

# MAUS
from Configuration import Configuration

class Go:
    def __init__(self, argInput, argMapper, argReducer, argOutput, argConfigFile = None):
        maus_root_dir = os.environ.get('MAUS_ROOT_DIR')
        current_dir = os.getcwd()

        if maus_root_dir not in current_dir:
            print("\nWARNING: YOU ARE RUNNING MAUS OUTSIDE ITS MAUS_ROOT_DIR")
            print(("\tMAUS_ROOT_DIR = %s" % (maus_root_dir)))
            print(("\tCURRENT DIRECTORY = %s\n" % (current_dir)))
            

        self.input = argInput
        self.mapper = argMapper
        self.reducer = argReducer
        self.output = argOutput

        print("Welcome to MAUS:")
        print(("\tProcess ID (PID): %d" % os.getpid()))
        print(("\tUniversally Unique ID (UUID): %s" % uuid.uuid4()))
        print(("\tProgram Arguments: %s" % str(sys.argv)))

        self.jsonConfigDocument = Configuration().getConfigJSON(argConfigFile)
        jsonConfigDictionary = json.loads(self.jsonConfigDocument)
        mapReduceType = jsonConfigDictionary['map_reduce_type']

        # Be sure to add other assertions here when new
        # map reduce implementations get put in.
        assert mapReduceType in ["native_python" , "native_python_profile"]
        
        if mapReduceType == "native_python":
            self.NativePythonMapReduce()
        elif mapReduceType == "native_python_profile":
            profile.runctx('self.NativePythonMapReduce()', globals(), locals(), 'list.prof')
            p = pstats.Stats('list.prof') 
            p.strip_dirs().sort_stats('time').print_stats() 
        else:
            # for future methods.  Be sure to add to assertion above
            pass


    def NativePythonMapReduce(self):
        # write intermediary step to disk
        fileObj = tempfile.SpooledTemporaryFile(max_size=524288) # 512 MB
        tempFile = gzip.GzipFile(filename='temp', mode='wb', fileobj=fileObj)

        ####                   ####
        ######  Input Phase  ######
        ####                  #####
        print("INPUT: Reading some input")
        assert(self.input.birth(self.jsonConfigDocument) == True)
        emitter = self.input.emitter()
        mapBuffer = self.BufferInput(emitter)

        ####                 ####
        ######  Map Phase  ######
        ####                #####
        print("MAP: Setting up mappers")
        assert(self.mapper.birth(self.jsonConfigDocument) == True)

        while len(mapBuffer) != 0:
            print(("MAP: Processing %d events" % len(mapBuffer)))
            #self.mapper.Process(mapBuffer[0])
            mapResults = list(list(map(self.mapper.process, mapBuffer)))
            for result in mapResults:
                tempFile.write('%s\n' % result)
            mapBuffer = self.BufferInput(emitter)

        print("MAP: Closing input and mappers")
        self.input.death()
        self.mapper.death()
        

        
        tempFile.close()
        
        
        ####                    ####
        ######  Reduce Phase  ######
        ####                   ##### 
        print("REDUCE: Setting up reducers")
        assert(self.reducer.birth(self.jsonConfigDocument) == True)
        
        # read back
        fileObj.seek(0) # go to beginning of file
        tempFile = gzip.GzipFile(filename='temp', mode='rb', fileobj=fileObj)
        i = 0
        reduceBuffer = []
        reduced = []
        for line in tempFile:
            if line.rstrip() == "":
                continue
            reduceBuffer.append(line)
            i = i + 1
            if i % 1000 == 0:
                print(('REDUCE: Reducing %d events in the %d pass' % (len(reduceBuffer), len(reduced))))
                reduced.append(reduce(self.reducer.process, reduceBuffer))
                reduceBuffer = []

        print(('REDUCE: Merging %d passes and reducing the %d events left in the buffer' % (len(reduced), len(reduceBuffer))))
        reduceResult = reduce(self.reducer.process, reduceBuffer + reduced)

        tempFile.close()
        fileObj.close()

        self.reducer.death()
        
        ####                    ####
        ######  Output Phase  ######
        ####                   #####

        assert(self.output.birth(self.jsonConfigDocument) == True)
        
        self.output.save(reduceResult)
                
        self.output.death()
            

    def BufferInput(self, theEmitter):
        my_buffer = []
        
        for i in range(1024):
            try:
                value = next(theEmitter)  
                my_buffer.append(value.encode('ascii'))
            except StopIteration:
                return my_buffer

        return my_buffer
                


