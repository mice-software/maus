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

from cdb import Geometry

def main():
    
    display_ids = Geometry()
    twsdl = "http://rgma19.pp.rl.ac.uk:8080/cdb/geometrySuperMouse?wsdl"
    display_ids.set_url(twsdl)
    print "Test server status is " + display_ids.get_status()
    id_dict = display_ids.get_ids(start_time = "2011-09-01 09:00:00")
    print id_dict
    
if __name__ == "__main__":
    main()