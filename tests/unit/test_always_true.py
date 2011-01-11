import unittest

class AlwaysTrueTestCase(unittest.TestCase):
    def runTest(self):
        self.assertEqual(1, 1, '1 != 1?!')

if __name__ == '__main__':
    unittest.main()

