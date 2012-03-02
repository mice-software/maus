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
#

"""Convert from json file to root file"""

import io   #  generic python library for I/O
import gzip #  For compressed output # pylint: disable=W0611

import MAUS

def run():
    """Run the macro"""

    # This input generates spills from json document
    my_input = MAUS.InputCppRoot()
    # This input outputs spills to root file
    my_output = MAUS.OutputPyJSON()

    # Execute inputter and outputter
    # Mapper and Reducer does nothing 
    MAUS.Go(my_input, MAUS.MapPyDoNothing(), MAUS.ReducePyDoNothing(), \
            my_output, io.StringIO(u""))

if __name__ == '__main__':
    run()
