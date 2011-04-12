#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################

import sys

from InputPyEmptyDocument import InputPyEmptyDocument
from MapPyGroup import MapPyGroup 
from MapPyFakeTestSimulation import MapPyFakeTestSimulation
from MapPyDoNothing import MapPyDoNothing
from ReducePyDoNothing import ReducePyDoNothing
from OutputPyDoNothing import OutputPyDoNothing

from Go import Go

def run():
    """Run a load test
    """
    map_group = MapPyGroup()

    big_number = 100

    if len(sys.argv) == 3:
        print 'Using only empty events'
        map_group.append(MapPyDoNothing())
    else:
        big_number = 1000
        print 'Using fake simulation data'
        map_group.append(MapPyFakeTestSimulation())
        
    if len(sys.argv) >= 2:
        big_number = int(sys.argv[1])


    print("Testing with %d events..." % big_number)


    inputer = InputPyEmptyDocument(big_number)
    # mapMapGroup
    reducer = ReducePyDoNothing()
    outputer = OutputPyDoNothing()


    Go(inputer, map_group, reducer, outputer)


if __name__ == '__main__':
    run()
