#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

"""
Tests for the download data utility
"""

import unittest
import subprocess
import os
import time
import glob

class TestDataDownload(unittest.TestCase): #pylint: disable=R0904
    """
    Tests for the data download utility
    """
    @classmethod
    def setUpClass(cls): # pylint: disable=C0103
        cls.here = os.getcwd()
        cls.mrd = os.environ['MAUS_ROOT_DIR']
        cls.log_file = open(cls.mrd+'/tmp/test_download_data.log', 'w')
        cls.test_dir = cls.mrd+'/tmp/test_download_data'
        cls.test_data_file = 'MICE/Step1/04000/04000.tar'
        try:
            os.mkdir(cls.test_dir)
        except OSError:
            pass # dir already exists?
        os.chdir(cls.test_dir)
        print 'Moved to ', os.getcwd()

    @classmethod
    def tearDownClass(cls): # pylint: disable=C0103
        cls.log_file.close()
        os.chdir(cls.here)
        print 'Moved back to ', os.getcwd()

    def setUp(self): # pylint: disable=C0103
        try:
            os.remove('./robots.txt')
        except OSError:
            pass
        self.wget_file('robots.txt', url_in='http://www.google.com/',
                       directory_out='./')
        if not os.path.exists('./robots.txt'):
            self.skipTest('Could not connect to internet (google.com)')

    def tearDown(self): # pylint: disable=C0103
        for a_file in glob.glob('./04000.*')+glob.glob('./robots.txt'):
            os.remove(a_file)

    def wget_file(self, file_name_in, url_in=None,
                  directory_out=None, file_name_out=None,
                  will_overwrite=None, no_untar=None, dummy=None):
        """wget file; leave input as None to ignore"""
        args = [os.path.join(self.mrd, 'bin', 'utilities', 'download_data.py'),
               '--file_name_in', file_name_in]
        if url_in != None:
            args += ['--url_in', url_in]
        if directory_out != None:
            args += ['--directory_out', directory_out]
        if file_name_out != None:
            args += ['--file_name_out', directory_out]
        if will_overwrite != None:
            args += ['--will_overwrite', will_overwrite]
        if no_untar != None:
            args += ['--no_untar', no_untar]
        if dummy != None:
            args += ['--dummy', dummy]
        proc = subprocess.Popen(args, stdout=self.log_file,
                                                       stderr=subprocess.STDOUT)
        proc.wait()
        return proc

    def test_file_in(self):
        """
        Check test_download_data.py --file_in switch
        """
        self.assertTrue(False)

    def test_will_overwrite(self):
        """
        Check test_download_data.py --will_overwrite switch
        """
        test_string = 'hello world'
        fout = open('robots.txt', 'w')
        print >> fout, test_string,
        fout.close()
        time.sleep(1) # let the filesystem catch up
        self.wget_file('robots.txt', url_in='http://www.google.com/',
                              directory_out='./')
        fin = open('robots.txt')
        self.assertEqual(fin.readline(), test_string)
        self.wget_file('robots.txt', url_in='http://www.google.com/',
                              directory_out='./', will_overwrite=True)
        self.assertNotEqual(fin.readline(), test_string)

    def test_no_untar(self):
        """Check test_download_data.py --no_untar switch"""
        proc = self.wget_file(self.test_data_file, directory_out='./')
        self.assertTrue(os.path.exists('04000.000'))
        os.remove('./04000.000')
        os.remove('./04000.tar')
        proc = self.wget_file(self.test_data_file, directory_out='./',
                              no_untar=True)
        self.assertFalse(os.path.exists('04000.000'))
        self.assertTrue(os.path.exists('04000.tar'))
        proc = self.wget_file('robots.txt', url_in='http://www.google.com/',
                              directory_out='./')
        self.assertNotEqual(proc.returncode, 0)

    def test_dummy(self):
        """Should return non-zero on bad argument"""
        proc = self.wget_file(self.test_data_file, dummy=True)
        self.assertNotEqual(proc.returncode, 0)

    def test_defaults(self):
        """Check test_download_data.py defaults"""
        # should default to download from http://www.hep.ph.ic.ac.uk/micedata/,
        # putting things in tmp/data_files
        target = os.path.join(self.mrd, 'tmp', 'data_files', '04000.000')
        self.wget_file(self.test_data_file)
        self.assertTrue(os.path.exists(target))


if __name__ == "__main__":
    unittest.main()



