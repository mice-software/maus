#!/usr/bin/env python

#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

import Configuration
import libMausCpp

def main():
    print "Welcome to MAUS field map maker"
    configuration = Configuration.Configuration().\
                                          getConfigJSON(command_line_args=True)
    print "Initialising (this can take a while)"
    libMausCpp.initialise(configuration)
    print "Getting field value"
    for z in range(0, 1501, 50):
      print z, '**', libMausCpp.Field.get_field_value(0., 0., z, 0.)

if __name__ == "__main__":
    main()


