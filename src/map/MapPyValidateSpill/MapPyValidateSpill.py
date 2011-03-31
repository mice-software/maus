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

## @class MapPyValidateSpill
#
#  MapPyValidateSpill checks the format of the spill against some the schema
#  defined in SpillSchema. Throws an exception if it finds an error, which can
#  either exit the application or just push an error into the Json scheme
#  depening on user set up.
#
#  Useful for debugging.
#

import ErrorHandler
import SpillSchema
import validictory

import json
import io
import sys


class MapPyValidateSpill:
  ## Initialisate empty map
  def __init__(self):
    pass

  ## Sets up the validator.
  def Birth(self):
    pass

  ## Process the spill and enter the exception handler if an error is found.
  def Process(self, spill_data):
    try:
      spill_doc = {} #default if we fail to loads spill_data
      spill_doc = json.loads(spill_data)
      validictory.validate(spill_doc, SpillSchema.spill, required_by_default=False)
      return json.dumps(spill_doc)
    except:
      return json.dumps(ErrorHandler.HandleException(spill_doc, self))

  ## Does nothing
  def Death(self):
    pass

