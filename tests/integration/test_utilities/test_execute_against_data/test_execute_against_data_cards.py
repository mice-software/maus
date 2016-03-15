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

"""
test_execute_against_data_cards check that Batch iteration number works
"""

import os
import subprocess
import unittest
import cdb

MRD = os.getenv('MAUS_ROOT_DIR')
MTP = os.getenv('MAUS_THIRD_PARTY')
TMP = os.getenv('MAUS_TMP_DIR')

# set a different test file for stepIV
DATA_TAR = "04234.tar"
if os.environ['MAUS_UNPACKER_VERSION'] == "StepIV":
    DATA_TAR = "06008.tar"

class CardsUploadTestCase(unittest.TestCase): # pylint: disable=R0904
    """
    test_execute_against_data_cards check that Batch iteration number works
    """
    @classmethod
    def setUpClass(cls): # pylint: disable=C0103
        """test_execute_against_data_cards.setUpClass"""
        cls.test_dir = TMP+'/test_execute_against_batch_cards'
        try:
            os.mkdir(cls.test_dir)
        except OSError:
            pass
        os.chdir(cls.test_dir)

    @classmethod
    def run_cards(cls, bi_hint, comment, reco_cards, mc_cards):
        """test_execute_against_data_cards.run_cards"""
        bi_super = cdb.BatchIterationSuperMouse(
                                              "http://preprodcdb.mice.rl.ac.uk")
        i = 0
        for i in range(bi_hint, bi_hint+10000):
            if bi_super.get_mc_datacards(i)['mc'] == 'null':
                break # the bi number does not exist - we can set it
        print 'Found free row with batch iteration number', i
        bi_super.set_datacards(i, comment, reco_cards, mc_cards)
        ead = MRD+'/bin/utilities/execute_against_data.py'
        data = MTP+'/third_party/install/share/test_data/'+DATA_TAR
        proc = subprocess.Popen(['python', ead, '--test',
                                 '--input-file', data,
                                 '--batch-iteration', str(i)])
        proc.wait()
        return proc

    def test_cards_good(self):
        """test_execute_against_data_cards.test_cards_good"""
        proc = self.run_cards(1, 'good data', 'physics_processes = "reco"',
                                              'physics_processes = "mc"')
        self.assertEqual(proc.returncode, 0)
        fin = open('reco.log')
        for line in fin.readlines():
            if line.find('"physics_processes"') > -1:
                self.assertGreater(line.find('"reco"'), -1)
        fin = open('sim.log')
        for line in fin.readlines():
            if line.find('"physics_processes"') > -1:
                self.assertGreater(line.find('"mc"'), -1)

    def test_cards_not_string(self):
        """test_execute_against_data_cards.test_cards_not_string"""
        proc = self.run_cards(1, 'not string data but parsable', 1, 2)
        self.assertEqual(proc.returncode, 0)

    def test_empty_cards(self):
        """test_execute_against_data_cards.test_empty_cards"""
        proc = self.run_cards(1, 'empty cards', "", "")
        self.assertEqual(proc.returncode, 0)

    def test_cards_reco_not_parsable(self):
        """test_execute_against_data_cards.test_cards_reco_not_parsable"""
        proc = self.run_cards(1, 'bad data (not parsable)', "1abc", "")
        self.assertEqual(proc.returncode, 2)

    def test_cards_mc_not_parsable(self):
        """test_execute_against_data_cards.test_cards_mc_not_parsable"""
        proc = self.run_cards(1, 'bad data (not parsable)', "", "1abc")
        self.assertEqual(proc.returncode, 2)

    def test_cards_no_number(self):
        """test_execute_against_data_cards.test_cards_no_number"""
        ead = MRD+'/bin/utilities/execute_against_data.py'
        data = MTP+'/third_party/install/share/test_data/'+DATA_TAR
        proc = subprocess.Popen(['python', ead, '--test',
                                 '--input-file', data,
                                 '--batch-iteration', str(1000000)])
        proc.wait()
        self.assertEqual(proc.returncode, 1)

if __name__ == "__main__":
    unittest.main()
