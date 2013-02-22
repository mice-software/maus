"""test_numpy.py"""
import unittest

class NumpyTestCase(unittest.TestCase):#pylint: disable =R0904
    """NumpyTestCase"""
    def test_numpy_exists(self):#pylint: disable =R0201
        """test_numpy_exists"""
        import numpy#pylint: disable =W0612



if __name__ == '__main__':
    unittest.main()
