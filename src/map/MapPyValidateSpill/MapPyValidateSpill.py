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
#  @author Chris Rogers <chris.rogers@stfc.ac.uk> 


import SpillSchema
import Validator
import json
import io
import sys


class MapPyValidateSpill:
  ## Initialisate empty map
  def __init__(self):
    self.__val = None

  ## Sets up the validator.
  def Birth(self):
    self.__val = Validator.JSONSchemaValidator(interactive_mode=False)

  ## Process the spill and enter the exception handler if an error is found.
  def Process(self, spill_data):
    try:
      spill_doc = {} #default if we fail to loads spill_data
      spill_doc = json.loads(spill_data)
      self.__val.validate(spill_doc, SpillSchema.spill)
      return json.dumps(spill_doc)
    except:
      return json.dumps(self.ExceptionHandler(spill_doc))

  ## Does nothing
  def Death(self):
    pass

  ## Exception handler controls how errors are managed by the map

  #  \param doc json document for dumping errors

  #  Handle error according to error handling tags.
  #  * if __error_to_console is true, send errors to std::out
  #  * if __error_to_json is true, send errors to the json tree
  #  * if __halt_on_error is true, call sys.exit() on error
  #
  #  TODO: option to throw on error (i.e. raise the exception again)
  def ExceptionHandler(self, doc):
    if self.__error_to_console:
      self.ErrorsToUser()
    if self.__error_to_json:
      self.ErrorsToJson(doc)
    if self.__halt_on_error:
      sys.exit()
    return doc
  
  ## Prints the exception (using sys.excepthook)
  def ErrorsToUser(self):
    sys.excepthook(*sys.exc_info())

  ## Puts the exception into the json stream
  #
  #  \param doc the json document
  #
  #  ErrorsToJson sends error messages into the json document. ErrorsToJson puts
  #  errors into the doc["error"]["<classname>"] property of the root object,
  #  where <classname> is found dynamically at runtime. The error branch is
  #  filled with <exception type>: <exception message>.
  def ErrorsToJson(self, doc):
    class_name = self.__class__.__name__
    if not 'errors' in doc:
      doc['errors'] = {}
    if not class_name in doc['errors']:
      doc['errors'][class_name] = []
    doc['errors'][class_name].append(str(sys.exc_info()[0])+": "+str(sys.exc_info()[1]))
    return doc

  __error_to_console = False
  __error_to_json    = True
  __halt_on_error    = False


