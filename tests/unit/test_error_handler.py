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

import StringIO
import sys
import string

import unittest

import ErrorHandler

class ErrorHandlerTestCase(unittest.TestCase):
  def test_ErrorHandler(self):
      error_handler = ErrorHandler.ErrorHandler()
      assert(error_handler.error_to_stderr == False)
      assert(error_handler.error_to_json == True)
      assert(error_handler.on_error == 'none')

  def test_ErrorsToJson(self):
    error_handler = ErrorHandler.ErrorHandler()
    doc = {}
    try:
      raise RuntimeError("Test error 1")
    except:
      doc = error_handler.ErrorsToJson(doc, self)
    try:
      raise RuntimeError("Test error 2")
    except:
      doc = error_handler.ErrorsToJson(doc, self)
    assert(doc["errors"]["ErrorHandlerTestCase"] == [
      "<type 'exceptions.RuntimeError'>: Test error 1",
      "<type 'exceptions.RuntimeError'>: Test error 2"
    ])

  def test_ErrorsToUser(self):
    stderr = sys.stderr
    test_err = StringIO.StringIO(u'')
    sys.stderr = test_err
    error_handler = ErrorHandler.ErrorHandler()
    doc = {}
    try:
      raise RuntimeError("Test error 1")
    except:
      doc = error_handler.ErrorsToUser()
    lines = test_err.getvalue().split('\n')
    assert(lines[3] == 'RuntimeError: Test error 1')
    sys.stderr = stderr

  def test_HandleException_Json(self):
    error_handler = ErrorHandler.ErrorHandler()
    error_handler.error_to_json = True
    doc = {}
    try:
      raise RuntimeError("Test error 1")
    except:
      doc = error_handler.HandleException(doc, self)
    assert(doc["errors"]["ErrorHandlerTestCase"] == [
        "<type 'exceptions.RuntimeError'>: Test error 1",
    ])
    error_handler.error_to_json = False
    try:
      raise RuntimeError("Test error 2")
    except:
      doc = error_handler.HandleException(doc, self)
    assert(doc["errors"]["ErrorHandlerTestCase"] == [
        "<type 'exceptions.RuntimeError'>: Test error 1",
    ])
    error_handler.error_to_json = True
    try:
      raise RuntimeError("Test error 3")
    except:
      doc = error_handler.HandleException(doc, self)
    assert(doc["errors"]["ErrorHandlerTestCase"] == [
        "<type 'exceptions.RuntimeError'>: Test error 1",
        "<type 'exceptions.RuntimeError'>: Test error 3",
    ])

  def test_HandleException_User(self):
    error_handler = ErrorHandler.ErrorHandler()
    doc = {}
    error_handler.error_to_stderr = True
    stderr = sys.stderr
    test_err = StringIO.StringIO(u'')
    print test_err.getvalue()
    sys.stderr = test_err
    try:
      raise RuntimeError("Test error 4")
    except:
      doc = error_handler.HandleException(doc, self)
    error_handler.error_to_stderr = False
    try:
      raise RuntimeError("Test error 4")
    except:
      doc = error_handler.HandleException(doc, self)
    lines = test_err.getvalue().split('\n')
    assert(lines[3] == 'RuntimeError: Test error 4')
    sys.stderr = stderr

  def test_HandleException_On_Error(self):
    error_handler = ErrorHandler.ErrorHandler()
    doc = {}
    error_handler.on_error = 'halt'
    try:
      raise RuntimeError("Test error 1")
    except:
      try:
        doc = error_handler.HandleException(doc, self)
      except:
        assert(sys.exc_info()[0] == type(SystemExit()))
    error_handler.on_error = 'raise'
    try:
      raise RuntimeError("Test error 1")
    except:
      try:
        doc = error_handler.HandleException(doc, self)
      except:
        assert(sys.exc_info()[0] == type(RuntimeError()))
    error_handler.on_error = 'none'
    try:
      raise RuntimeError("Test error 1")
    except:
      doc = error_handler.HandleException(doc, self)  

  def test_DefaultHandler(self):
    ErrorHandler.DefaultHandler().error_to_stderr = False
    ErrorHandler.DefaultHandler().error_to_json = True
    ErrorHandler.DefaultHandler().on_error = 'none'
    doc = {}
    try:
      raise RuntimeError("Test error 1")
    except:
      doc = ErrorHandler.HandleException(doc, self)
    assert(doc["errors"]["ErrorHandlerTestCase"] == [
        "<type 'exceptions.RuntimeError'>: Test error 1",
    ])
    

if __name__ == '__main__':
    unittest.main()

