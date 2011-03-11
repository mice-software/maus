import sys

#TODO: option to throw on error (i.e. raise the exception again)
class ExceptionHandler:
  """
  Error handler controls how errors are managed by the map
    \param doc json document for dumping errors
  Handle error according to error handling tags.
    * if __error_to_stderr is true, send errors to std::err
    * if __error_to_json is true, send errors to the json tree
    * if __halt_on_error is true, call sys.exit() on error
  """
  def __init__(self):
    """
    Iniitialise with default settings
      no errors are passed to stderr
      errors are passed to json
      does not halt on error
    """
    self.error_to_stderr = False
    self.error_to_json = True
    self.on_error = 'none'

  def SetUp(self, cards_doc):
    """
    Set up the error handler with errors from datacards
    """
    raise NotImplementedError()
  

  def HandleException(self, doc=None, caller=None):
    """
    Handle a raised exception - put the error message in the right IO streams etc.
      \param doc the json data stream
      \param caller the object that called the ExceptionHandler (determines which
                    branch to use in the data stream)
    Returns the datastream
    """
    if self.error_to_stderr:
      self.ErrorsToUser()
    if self.error_to_json:
      self.ErrorsToJson(doc, caller)
    if self.on_error == 'none':
      pass
    elif self.on_error == 'halt':
        sys.exit(1)
    elif self.on_error == 'raise':
        raise
    return doc

  def ErrorsToUser(self):
    """Prints the excetption to stderr (using sys.excepthook)"""
    sys.excepthook(*sys.exc_info())

  def ErrorsToJson(self, doc=None, caller=None):
    """
    Puts the exception into the json stream
      \param doc the json document
      \param caller the object that called the error handler
    ErrorsToJson sends error messages into the json document. ErrorsToJson puts
    errors into the doc["error"]["<classname>"] property of the root object,
    where <classname> is found dynamically at runtime. The error branch is
    appended with <exception type>: <exception message>.
    """
    if doc == None: doc = {}
    class_name = "<unknown caller>"
    if caller != None:
      class_name = caller.__class__.__name__
    if not 'errors' in doc:
      doc['errors'] = {}
    if not class_name in doc['errors']:
      doc['errors'][class_name] = []
    doc['errors'][class_name].append(str(sys.exc_info()[0])+": "+str(sys.exc_info()[1]))
    return doc

__default_handler = ExceptionHandler()

def HandleException(doc, caller):
  """
  Handle an exception with the default exception handler
    \param doc the json data stream
    \param caller the object that called the ExceptionHandler (determines which
                  branch to use in the data stream)
  Returns the datastream
  """
  return __default_handler.HandleException(doc, caller)


