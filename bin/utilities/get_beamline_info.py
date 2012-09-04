#!/usr/bin/env python
"""
get_beamline_info.py
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

from geometry.ConfigReader import Configreader
import cdb

def main():
    """
    This is the executbale which gets information relating to beamline stored
    on the configuration Database. It will print either to screen or to file
    the details of the beamline or whether it is on the database or not. It takes 
    arguments in the form of which method you wish to call the cdb. It currently only 
    has a query of run number and a time frame. These
    arguments can be specified in the ConfigurationDefaults file.
    """
    configuration = Configreader()
    server = cdb.Beamline()
    print "Server status is " + server.get_status()
    if configuration.get_beamline_by == 'run_number':
        run = configuration.get_beamline_run_number
        id_dict = server.get_beamline_for_run(run)
        if str(id_dict.keys()) == '[]':
            print '!WARNING! No beamline information on the CDB for ' \
                                 + str(configuration.get_beamline_run_number)
        else:
            print 'There is information for run number ' \
                + str(configuration.get_beamline_run_number) + ' on the CDB'
    elif configuration.get_beamline_by == 'dates':
        if configuration.get_beamline_start_time == "":
            raise IOError('Start time not entered!' + \
                                    ' Have you used a configuration file?')
        if configuration.get_beamline_stop_time == "":
            stop_time = None
        else:
            stop_time = configuration.get_beamline_stop_time
        start_time = configuration.get_beamline_start_time
        id_dict = server.get_beamlines_for_dates(start_time, stop_time)
        keys = id_dict.viewkeys()
        ids_list = []
        ids_list = list(keys)
        length = len(ids_list)
        print 'The CDB has information on the following runs;'
        for i in range(0, length):
            print ids_list[i]
        print 'If the run you are looking for is not here ' + \
                          'the it is most likely not on the CDB'

if __name__ == "__main__":
    main()
