# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
#

# pylint: disable=C0103
"""
tests for InputPyJSON
"""

import json
import unittest
import io
import os
import gzip

from InputPyJSON import InputPyJSON

class InputPyJSONTestCase(unittest.TestCase): #pylint: disable=R0904
    """Tests InputPyJSON
    """
    
    def test_datacards_text(self):
        """
        Test that birth reads in datacards okay for case of gzip and raw json
        files
        """
        mrd = os.environ['MAUS_ROOT_DIR']

        file_dict = {'gzip':os.path.join(mrd, 'tmp', 'inputpyjsontest.json'),
                  'text':os.path.join(mrd, 'tmp', 'inputpyjsontest.gz')}
        for file_type, fname in file_dict.iteritems():
            if file_type == 'gzip':
                out = gzip.GzipFile(fname, 'w')
            elif file_type == 'text':
                out = open(fname, 'w')
            print >> out, '{}\n'*10
            out.close()
            my_input = InputPyJSON()
            my_input.birth(json.dumps({'input_json_file_name':fname,
                           'input_json_file_type':file_type}))
            index = 0
            for item in my_input.emitter():
                self.assertEqual(item, '{}')
                index += 1
            self.assertEqual(index, 10)

    def test_read100(self):
        """test read 100 events
        """

        test_json = {}
        test_json['mc'] = []
        test_json['mc'].append({'tracks': {}})
        test_json['mc'][0]['tracks']['track1'] = {'particle_id' : 13}
        test_json['mc'][0]['tracks']['track1'] = {'particle_id' : -13}
        test_json['mc'][0]['tracks']['track1'] = {'particle_id' : 5}
        test_json['mc'][0]['hits'] = {}
        test_json['mc'].append(test_json['mc'][0])
        test_json['mc'].append({})
        test_string1 = json.dumps(test_json) + '\n'

        my_file = io.StringIO(100*unicode(test_string1))
        my_input = InputPyJSON(my_file)
        self.assertTrue(my_input.birth())
        test_doc = json.loads(test_string1)
        i = 0
        for doc in my_input.emitter():
            json_doc = json.loads(doc)
            self.assertTrue(json_doc == test_doc)
            i += 1
        self.assertEqual(i, 100)
        self.assertTrue(my_input.death())

    def test_bad(self):
        """test bad file content
        """
        my_file = io.StringIO(u"AFJKLAFJKALKF")
        my_input = InputPyJSON(my_file)
        self.assertTrue(my_input.birth())
        with self.assertRaises(ValueError):
            next(my_input.emitter())

        self.assertTrue(my_input.death())

    def test_counter_lt(self):
        """test internal counter lt
        """
        my_file = io.StringIO(10*u"""{"test": 4}\n""")
        
        my_input = InputPyJSON(my_file, arg_number_of_events=5)

        i = 0
        self.assertTrue(my_input.birth())
        for doc in my_input.emitter(): #pylint: disable=W0612
            i += 1

        self.assertEqual(i, 5)

        self.assertTrue(my_input.death())

    def test_counter_gt(self):
        """test internal counter gt
        """
        my_file = io.StringIO(5*u"""{"test": 4}\n""")

        my_input = InputPyJSON(my_file, arg_number_of_events=10)

        i = 0
        self.assertTrue(my_input.birth())
        for doc in my_input.emitter(): #pylint: disable=W0612
            i += 1

        self.assertEqual(i, 5)

        self.assertTrue(my_input.death())

    def test_twice(self):
        """test trying to read twice gives nothing
        """
        my_file = io.StringIO(10*u"""{"test": 4}\n""")
        my_input = InputPyJSON(my_file, arg_number_of_events=5)
        self.assertTrue(my_input.birth())
        for doc in my_input.emitter(): #pylint: disable=W0612
            pass

        with self.assertRaises(StopIteration):
            next(my_input.emitter())

        self.assertTrue(my_input.death())

    def test_eof_whitespace(self):
        """test what happens if there is one line with some spaces
        """
        my_file = io.StringIO(u"""{"test": 4}\n    """)
        my_input = InputPyJSON(my_file)

        self.assertTrue(my_input.birth())
        for doc in my_input.emitter(): #pylint: disable=W0612
            pass

        with self.assertRaises(StopIteration):
            next(my_input.emitter())

        self.assertTrue(my_input.death())


            



if __name__ == '__main__':
    unittest.main()
