#!/usr/bin/env python
"""
get_geometry_ids.py
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
import os

def main():
    """
    This is the executbale which gets information relating to geometries stored
    on the configuration Database. It will print either to screen or to file
    the ID number, geometry description and valid from and to date. It takes 
    arguments in the form of a time window of which to search the Database and 
    also whether the user wants the information printed to screen or file. These
    arguments can be specified in the ConfigurationDefaults file.
    """
    configuration = Configreader()
    if configuration.get_ids_start_time == "":
        raise IOError('Start time not entered!' + \
                                         ' Have you used a configuration file?')
    if configuration.get_ids_stop_time == "":
        stop_time = None
    else:
        stop_time = configuration.get_ids_stop_time
    server = cdb.Geometry()
    server_name = configuration.cdb_download_url + \
                                            configuration.geometry_download_wsdl
    server.set_url(server_name)
    id_dict = server.get_ids(configuration.get_ids_start_time, stop_time)
    id_numbers = id_dict.keys()
    length = len(id_numbers)
    print "Server status is " + server.get_status()
    if configuration.get_ids_create_file == True:
        file_path = os.environ['MAUS_ROOT_DIR'] + '/tmp/geometry_ids.txt'
        fin = open(file_path, 'w')
        for num in range(0, length):
            fin.write("Geometry Number = " + str(id_numbers[num]) + '\n')
            fin.write("Geometry Note   = " + \
                                  str(id_dict[id_numbers[num]]['notes']) + '\n')
            fin.write("ValidFrom       = " + \
                              str(id_dict[id_numbers[num]]['validFrom']) + '\n')
            fin.write("Date Created    = " + \
                              str(id_dict[id_numbers[num]]['created']) + '\n\n')
        fin.close()
        print "File saved to " + file_path
    else:
        for num in range(0, length):
            print "Geometry Number = " + str(id_numbers[num])
            print "Geometry Note   = " + str(id_dict[id_numbers[num]]['notes'])
            print "ValidFrom       = " + \
                                      str(id_dict[id_numbers[num]]['validFrom'])
            print "Date Created    = " + \
                                 str(id_dict[id_numbers[num]]['created']) + '\n'
        
if __name__ == "__main__":
    main()
