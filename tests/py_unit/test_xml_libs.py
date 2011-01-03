"""Tests for import libxml and libxslt"""

#pylint: disable=W0612, R0201

import unittest

class XMLLibsTestCase(unittest.TestCase): #pylint: disable = R0904
    """Tests for import libxml and libxslt"""

    def libxml2_test(self):
        """
        Check we can import libxml2
        """
        import libxml2

    def libxslt_test(self):
        """
        Check we can import libxslt
        """
        import libxslt

