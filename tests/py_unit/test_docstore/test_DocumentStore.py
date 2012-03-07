"""
Tests for InMemoryDocumentStore module.
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

# pylint: disable=C0103

import unittest

from docstore.DocumentStore import DocumentStoreException

class DocumentStoreExceptionTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for docstore.DocumentStoreException class.
    """

    def test_init(self):
        """ 
        Invoke constructor and check values.
        @param self Object reference.
        """
        wrapped_exception = ValueError("TEST")
        exception = DocumentStoreException(wrapped_exception)
        print str(exception)
        self.assertEquals(wrapped_exception, exception.exception,
            "Unexpected exception")
        self.assertEquals("Exception when using document store: TEST",
            str(exception), "Unexpected string")
      
if __name__ == '__main__':
    unittest.main()
