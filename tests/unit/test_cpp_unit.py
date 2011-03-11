
import unittest,os

class CppUnitTests(unittest.TestCase):
    def test_run_test_cpp_unit(self):
        cpp_exe = os.path.join(os.environ['MAUS_ROOT_DIR'],'build','test_cpp_unit')
        cpp_test = os.system(cpp_exe)
        self.assertEqual(cpp_test, 0, 'Failed cpp tests with '+str(cpp_test))

if __name__ == '__main__':
    unittest.main()



