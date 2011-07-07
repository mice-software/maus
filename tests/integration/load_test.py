#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################

import sys
import math
import MAUS

from Go import Go

def run():
    """Run a load test
    """
    map_group = MAUS.MapPyGroup()

    big_number = 100
    if len(sys.argv) == 2:
        big_number = int(math.fabs(float(sys.argv[1])))


    if len(sys.argv) == 2 and float(sys.argv[1]) < 0:
        print 'Using only empty events'
        map_group.append(MAUS.MapPyDoNothing())
    else:
        print 'Using fake simulation data'
        map_group.append(MAUS.MapPyFakeTestSimulation())
        map_group.append(MAUS.MapPyRemoveTracks())
        
    print("Testing with %d events..." % big_number)


    inputer = MAUS.InputPyEmptyDocument(big_number)
    # mapMapGroup
    reducer = MAUS.ReducePyDoNothing()
    outputer = MAUS.OutputPyDoNothing()


    Go(inputer, map_group, reducer, outputer)


if __name__ == '__main__':
    run()
