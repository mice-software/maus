"""ErrorHandler.py"""
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

class ErrorHandler:
    """
    @class ErrorHandler
    Error handler controls how errors are managed by the map

    Handle error according to error handling tags.
    * if __error_to_stderr is true, send errors to std::err
    * if __error_to_json is true, send errors to the json tree
    * if __on_error is 'halt', call sys.exit() on error; if on_error is 'raise',
      raise the error; if on_error is 'none', ignore errors.
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
        self.error_to_stderr = True
        self.error_to_json = True
        self.on_error = 'none'

    def HandleException(self, doc=None, caller=None):# pylint:disable = C0103
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
            raise # pylint:disable = E0704
        else: # pylint:disable = E0704
            raise KeyError('Did not recognise on_error directive '
                                                            +str(self.on_error))
        return doc

    def ErrorsToUser(self):# pylint:disable = C0103,R0201
        """Prints the excetption to stderr (using sys.excepthook)"""
        sys.excepthook(*sys.exc_info())

    def ErrorsToJson(self, doc=None, caller=None):# pylint:disable = C0103,R0201
        """
        Puts the exception into the json stream
        @param doc the json document
        @param caller the object that called the error handler
        ErrorsToJson sends error messages into the json document. ErrorsToJson
        puts errors into the doc["error"]["<classname>"] property of the root
        object, where <classname> is found dynamically at runtime. The error
        branch is appended with <exception type>: <exception message>.
        """
        if doc == None:
            raise
        class_name = "<unknown caller>"
        if caller == None:
            pass
        elif type(caller) == type(''):
            class_name = caller
        else:
            class_name = caller.__class__.__name__
        if not 'errors' in doc:
            doc['errors'] = {}
        err_string = str(sys.exc_info()[0])+": "+str(sys.exc_info()[1])
        doc['errors'][class_name] = err_string
        return doc

    def ConfigurationToErrorHandler(self, config):# pylint:disable = C0103
        """
        Hand configuration information to the default handler
          @param config json configuration information default handler.
                 ErrorHandler will take:
                    on_error,
                    errors_to_stderr,
                    errors_to_json
        """
        if config['errors_to_stderr'] == None:
            self.error_to_stderr = config['verbose_level'] < 4
        else:
            self.error_to_stderr = bool(config['errors_to_stderr'])
        self.error_to_json = bool(config['errors_to_json'])
        self.on_error = str(config['on_error']).lower()

class CppError(Exception):
    """
    Error that is raised by MAUS C++ code. Takes simply an error message as
    argument, which is printed when the error is raised.
    """
    def __init__(self, message):
        super(CppError, self).__init__()
        self.args = (str(message), )

    def __repr__(self):
        return self.args[0]

__default_handler = ErrorHandler()# pylint:disable = C0103

def DefaultHandler():# pylint:disable = C0103
    """
    Returns the default handler object
    """
    return __default_handler

def HandleException(doc, caller):# pylint:disable = C0103
    """
    Handle an exception with the default exception handler
    @param doc the json data stream
    @param caller the object that called the ExceptionHandler (determines which
               branch to use in the data stream)
    @returns the datastream
    """
    out = __default_handler.HandleException(doc, caller)
    return out

def HandleCppException(doc, caller, error_message):# pylint:disable = C0103
    """
    Handle an exception with the default exception handler
    @param doc string representation of the json data stream
    @param string representation of the object that called the ExceptionHandler
               (determines which branch to use in the data stream)
    @returns the datastream
    """
    json_doc = json.loads(doc)
    try:
        raise CppError(error_message)
    except CppError:
        try:
            out = json.dumps(__default_handler.HandleException(json_doc,
                                                               caller))
        except: # pylint: disable = W0702
            out = """{"errors":"Failed to handle exception"}"""
    
    return out

