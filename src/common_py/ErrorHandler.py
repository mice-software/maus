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


## @class ErrorHandler
#  Error handler handles errors in a globally managed way


import sys
import json
import libMausCpp

class ErrorHandler:
    """
    @class ErrorHandler
    Error handler controls how errors are managed by the map

    Handle error according to error handling tags.
    * if __error_to_stderr is true, send errors to std::err
    * if __error_to_json is true, send errors to the json tree
    * if __halt_on_error is true, call sys.exit() on error
    Either instantiate a special error handler or (more usually) use the default
    error handler.
    """
    def __init__(self):
        """
        Iniitialise with default settings
        * no errors are passed to stderr
        * errors are passed to json
        * does not halt on error
        """
        self.error_to_stderr = False
        self.error_to_json = True
        self.on_error = 'none'

    def SetUp(self, cards_doc):
        """
        Set up the error handler with errors from datacards (Not implemented)
        """
        if (cards_doc): pass
        raise NotImplementedError()


    def HandleException(self, doc=None, caller=None):
        """
        Handle a raised exception - put the error message in the right IO
        streams etc.
        @param doc the json data stream
        @param caller the object that called the ExceptionHandler (determines
                      which branch to use in the data stream). If a string is
                      used, then errors go into the branch with name like that
                      string.
        @returns the datastream
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
        else:
            raise KeyError('Did not recognise on_error directive '
                                                            +str(self.on_error))
        return doc

    def ErrorsToUser(self):
        """Prints the excetption to stderr (using sys.excepthook)"""
        sys.excepthook(*sys.exc_info())

    def ErrorsToJson(self, doc=None, caller=None):
        """
        Puts the exception into the json stream
        @param doc the json document
        @param caller the object that called the error handler
        ErrorsToJson sends error messages into the json document. ErrorsToJson
        puts errors into the doc["error"]["<classname>"] property of the root
        object, where <classname> is found dynamically at runtime. The error
        branch is appended with <exception type>: <exception message>.
        """
        if doc == None: doc = {}
        class_name = "<unknown caller>"
        if caller == None:
            pass
        elif type(caller) == type(''):
            class_name = caller
        else:
            class_name = caller.__class__.__name__
        if not 'errors' in doc:
            doc['errors'] = {}
        if not class_name in doc['errors']:
            doc['errors'][class_name] = []
        doc['errors'][class_name].append(str(sys.exc_info()[0])+": "
                                                        +str(sys.exc_info()[1]))
        return doc

class CppError(Exception):
  """
  Error that is raised by MAUS C++ code. Takes simply an error message as
  argument, which is printed when the error is raised.
  """
  def __init__(self, message):
    self.args = (str(message), )

  def __repr__(self):
    return self.args[0]

__default_handler = ErrorHandler()

def DefaultHandler():
    """
    Returns the default handler object
    """
    return __default_handler

def HandleException(doc, caller):
    """
    Handle an exception with the default exception handler
    @param doc the json data stream
    @param caller the object that called the ExceptionHandler (determines which
               branch to use in the data stream)
    @returns the datastream
    """
    out = __default_handler.HandleException(doc, caller)
    return out

def HandleCppException(doc, caller, error_message):
    """
    Handle an exception with the default exception handler
    @param doc string representation of the json data stream
    @param string representation of the object that called the ExceptionHandler
               (determines which branch to use in the data stream)
    @returns the datastream
    """
    json_doc = json.loads(doc)
    try:
      raise(CppError(error_message))
    except:
      out = json.dumps(__default_handler.HandleException(json_doc, caller))
    return out

# Sets the function call for CppErrorHandler. If not set, assume we
# don't use python error handler; libMausCpp is defined in 
#   src/common_cpp/Utils/PyMausCpp.hh
libMausCpp.SetHandleException(HandleCppException)

