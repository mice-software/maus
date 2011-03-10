import SpillSchema
import validator
import json
import io
import sys

class MapPyValidateSpill:
  def __init__(self):
    self.__val = None

  def Birth(self):
    self.__val = validator.JSONSchemaValidator(interactive_mode=False)

  def Process(self, spill_data):
    try:
      spill_doc = {} #default if we fail to loads spill_data
      spill_doc = json.loads(spill_data)
      self.__val.validate(spill_doc, SpillSchema.spill)
      return json.dumps(spill_doc)
    except:
      return json.dumps(self.ExceptionHandler(spill_doc))

  def ExceptionHandler(self, doc):
    if self.__error_to_console:
      self.ErrorsToUser()
    if self.__error_to_json:
      self.ErrorsToJson(doc)
    if self.__halt_on_error:
      sys.exit()
    return doc
  
  def ErrorsToUser(self):
    sys.excepthook(*sys.exc_info())

  def ErrorsToJson(self, doc):
    class_name = self.__class__.__name__
    if not 'errors' in doc:
      doc['errors'] = {}
    if not class_name in doc['errors']:
      doc['errors'][class_name] = []
    doc['errors'][class_name].append(str(sys.exc_info()[0])+": "+str(sys.exc_info()[1]))
    return doc

  def Death(self):
    pass

  def Validate(self, spill_doc):
    val.validate(spill_doc, schema_doc)
    return True

  __error_to_console = True
  __error_to_json    = True
  __halt_on_error    = False


# [{"position": { "x": 0.0, "y": -0.0, "z": -5000 },"particle_id" : 13,"energy" : 210, "random_seed" : 10, "unit_momentum" : { "x":0, "y":0, "z":1 }}]}\n"""

