"""test_cpp_unit.py"""
import os
import unittest

class CppUnitTests(unittest.TestCase):#pylint: disable =R0904
    """CppUnitTests"""
    def test_run_test_cpp_unit(self):
        """test_run_test_cpp_unit"""
        cpp_exe = os.path.join(os.environ['MAUS_ROOT_DIR'],
                               'build/tests',
                               'test_cpp_unit')
        cpp_test = os.system(cpp_exe)

        # Remove stray files
        if os.path.isfile('_delete_mice_module_test.dat'):
            os.unlink('_delete_mice_module_test.dat')

        # Especially remove stray files in MAUS_ROOT_DIR
        _dir = os.environ.get('MAUS_ROOT_DIR')
        if os.path.isfile('%s/_delete_mice_module_test.dat' % _dir):
            os.unlink('%s/_delete_mice_module_test.dat' % _dir)

        self.assertEqual(cpp_test, 0, 'Failed cpp tests with '+str(cpp_test))

if __name__ == '__main__':
    unittest.main()
