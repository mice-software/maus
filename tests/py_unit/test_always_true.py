import unittest

class AlwaysTrueTestCase(unittest.TestCase):
    def test_always_true(self):
        self.assertEqual(1, 1, '1 != 1?!')

if __name__ == '__main__':
    unittest.main()

