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
#
#!/usr/bin/env python

"""
Load test the execution runner
"""

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
        
    print("Testing with %d events..." % big_number)


    inputer = MAUS.InputPyEmptyDocument(big_number)
    # mapMapGroup
    reducer = MAUS.ReducePyDoNothing()
    outputer = MAUS.OutputPyDoNothing()


    Go(inputer, map_group, reducer, outputer)


if __name__ == '__main__':
    run()
