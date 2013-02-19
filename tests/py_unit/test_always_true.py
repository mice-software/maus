"""test_always_true.py"""
import unittest

class AlwaysTrueTestCase(unittest.TestCase):#pylint: disable =R0904
    """AlwaysTrueTestCase"""
    def test_always_true(self):
        """test_always_true"""
        self.assertEqual(1, 1, '1 != 1?!')

if __name__ == '__main__':
    unittest.main()

