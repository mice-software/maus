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

"""
Test error handler
"""

#pylint: disable=R0904

import StringIO
import sys

import json
import unittest

import ErrorHandler
import libMausCpp

class ErrorHandlerTestCase(unittest.TestCase):
    """
    Test error handler
    """
    def test_error_handler(self): #pylint: disable=R0201
        """
        Test initialisation of the error handler
        """
        error_handler = ErrorHandler.ErrorHandler()
        assert(error_handler.error_to_stderr == True)
        assert(error_handler.error_to_json == True)
        assert(error_handler.on_error == 'none')

    def test_errors_to_json(self):
        """Check we send error to json correctly"""
        error_handler = ErrorHandler.ErrorHandler()
        doc = {}
        try:
            raise RuntimeError("Test error 1")
        except RuntimeError:
            doc = error_handler.ErrorsToJson(doc, self)
        try:
            raise RuntimeError("Test error 2")
        except RuntimeError:
            doc = error_handler.ErrorsToJson(doc, self)
        assert(doc["errors"]["ErrorHandlerTestCase"] == [
            "<type 'exceptions.RuntimeError'>: Test error 1",
            "<type 'exceptions.RuntimeError'>: Test error 2"
        ])

    def test_errors_to_user(self): #pylint: disable=R0201
        """Check that we send errors to sys.stderr if required"""
        stderr = sys.stderr
        test_err = StringIO.StringIO(u'')
        sys.stderr = test_err
        error_handler = ErrorHandler.ErrorHandler()
        try:
            raise RuntimeError("Test error 1")
        except RuntimeError:
            error_handler.ErrorsToUser()
        lines = test_err.getvalue().split('\n')
        assert(lines[3] == 'RuntimeError: Test error 1')
        sys.stderr = stderr

    def test_handle_exception_json(self):
        """Check the flag that controls sending errors to json works"""
        error_handler = ErrorHandler.ErrorHandler()
        error_handler.error_to_json = True
        doc = {}
        try:
            raise RuntimeError("Test error 1")
        except RuntimeError:
            doc = error_handler.HandleException(doc, self)
        assert(doc["errors"]["ErrorHandlerTestCase"] == [
            "<type 'exceptions.RuntimeError'>: Test error 1",
        ])
        error_handler.error_to_json = False
        try:
            raise RuntimeError("Test error 2")
        except RuntimeError:
            doc = error_handler.HandleException(doc, self)
        assert(doc["errors"]["ErrorHandlerTestCase"] == [
            "<type 'exceptions.RuntimeError'>: Test error 1",
        ])
        error_handler.error_to_json = True
        try:
            raise RuntimeError("Test error 3")
        except RuntimeError:
            doc = error_handler.HandleException(doc, self)
        assert(doc["errors"]["ErrorHandlerTestCase"] == [
            "<type 'exceptions.RuntimeError'>: Test error 1",
            "<type 'exceptions.RuntimeError'>: Test error 3",
        ])

    def test_handle_exception_user(self):
        """Check the flag that controls sending errors to stderr"""
        error_handler = ErrorHandler.ErrorHandler()
        doc = {}
        error_handler.error_to_stderr = True
        stderr = sys.stderr
        test_err = StringIO.StringIO(u'')
        print test_err.getvalue()
        sys.stderr = test_err
        try:
            raise RuntimeError("Test error 4")
        except RuntimeError:
            doc = error_handler.HandleException(doc, self)
        error_handler.error_to_stderr = False
        try:
            raise RuntimeError("Test error 4")
        except RuntimeError:
            doc = error_handler.HandleException(doc, self)
        lines = test_err.getvalue().split('\n')
        assert(lines[3] == 'RuntimeError: Test error 4')
        sys.stderr = stderr

    def test_handle_exception_on_error(self):
        """Check that we handle the exception correctly (raise, halt, etc)"""
        error_handler = ErrorHandler.ErrorHandler()
        doc = {}
        error_handler.on_error = 'halt'
        try:
            raise RuntimeError("Test error 1")
        except RuntimeError:
            try:
                doc = error_handler.HandleException(doc, self)
            except SystemExit:
                assert(sys.exc_info()[0] == type(SystemExit()))
        error_handler.on_error = 'raise'
        try:
            raise RuntimeError("Test error 1")
        except RuntimeError:
            try:
                doc = error_handler.HandleException(doc, self)
            except RuntimeError:
                assert(sys.exc_info()[0] == type(RuntimeError()))
        error_handler.on_error = 'none'
        try:
            raise RuntimeError("Test error 1")
        except RuntimeError:
            doc = error_handler.HandleException(doc, self)  
        error_handler.on_error = 'BAD_STRING'
        try:
            raise RuntimeError("Test error 1")
        except RuntimeError:
            with self.assertRaises(KeyError):
                error_handler.HandleException(doc, self)

    def test_default_handler(self):
        """
        Check default handler calls correctly
        """
        ErrorHandler.DefaultHandler().error_to_stderr = False
        ErrorHandler.DefaultHandler().error_to_json = True
        ErrorHandler.DefaultHandler().on_error = 'none'
        doc = {}
        try:
            raise RuntimeError("Test error 1")
        except RuntimeError:
            doc = ErrorHandler.HandleException(doc, self)
        assert(doc["errors"]["ErrorHandlerTestCase"] == [
            "<type 'exceptions.RuntimeError'>: Test error 1",
        ])
        try:
            raise RuntimeError("Test error 2")
        except RuntimeError:
            doc = json.loads(ErrorHandler.HandleCppException(json.dumps(doc),
                             "some caller", "Test error 3"))
        assert(doc["errors"]["some caller"] == [
            "<class 'ErrorHandler.CppError'>: Test error 3",
        ])

    def test_cpp_error(self): #pylint: disable = R0201
        """
        Test CppError type works okay
        """
        my_error = ErrorHandler.CppError("Some message")
        assert(my_error.args == ("Some message",))
        assert(repr(my_error) == "Some message")

    def test_set_handle_exception(self): #pylint:disable = R0201
        """
        Test we can do interface with Cpp correctly
        """
        self.assertRaises(TypeError,  libMausCpp.SetHandleException, 1)
        refcount = sys.getrefcount(ErrorHandler.HandleCppException)
        for i in range(100): #pylint: disable = W0612
            # should decref correctly
            libMausCpp.SetHandleException(ErrorHandler.HandleCppException)
        assert(refcount == sys.getrefcount(ErrorHandler.HandleCppException))

    def test_config_to_error_handler(self):
        """
        Test error handler setup from configuration
        """
        config_test = {'verbose_level':0, 'errors_to_stderr':None, 
                       'errors_to_json':True, 'on_error':'none'}

        error_handler = ErrorHandler.ErrorHandler()
        error_handler.ConfigurationToErrorHandler(config_test)
        self.assertTrue(error_handler.error_to_stderr)

        config_test['verbose_level'] = 4
        error_handler.ConfigurationToErrorHandler(config_test)
        self.assertFalse(error_handler.error_to_stderr)

        self.assertTrue(error_handler.error_to_json)
        self.assertEqual(error_handler.on_error, 'none')

        config_test = {'verbose_level':4, 'errors_to_stderr':True,
                      'errors_to_json':False, 'on_error':'raise'}

        error_handler.ConfigurationToErrorHandler(config_test)
        self.assertTrue(error_handler.error_to_stderr)
        self.assertFalse(error_handler.error_to_json)
        self.assertEqual(error_handler.on_error, 'raise')


if __name__ == '__main__':
    unittest.main()

