import os
import unittest

class CppUnitTests(unittest.TestCase):
    def test_run_test_cpp_unit(self):
        cpp_exe = os.path.join(os.environ['MAUS_ROOT_DIR'],'build','test_cpp_unit')
        cpp_test = os.system(cpp_exe)

        # Remove stray files
        if os.path.isfile('_delete_mice_module_test.dat'):
            os.unlink('_delete_mice_module_test.dat')

        # Especially remove stray files in MAUS_ROOT_DIR
        dir = os.environ.get('MAUS_ROOT_DIR')
        if os.path.isfile('%s/_delete_mice_module_test.dat' % dir):
            os.unlink('%s/_delete_mice_module_test.dat' % dir)

        self.assertEqual(cpp_test, 0, 'Failed cpp tests with '+str(cpp_test))

if __name__ == '__main__':
    unittest.main()



