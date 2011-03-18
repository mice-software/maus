import json
import unittest
import io

from InputPyJSON import InputPyJSON

class InputPyJSONTestCase(unittest.TestCase):
    """Tests InputPyJSON
    """
    
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
        test_doc = json.loads(test_string1)
        i = 0
        for doc in my_input.emitter():
            json_doc = json.loads(doc)
            self.assertTrue(json_doc == test_doc)
            i += 1
        self.assertEqual(i, 100)

    def test_bad(self):
        """test bad file content
        """
        my_file = io.StringIO(u"AFJKLAFJKALKF")
        my_input = InputPyJSON(my_file)
        with self.assertRaises(ValueError):
            next(my_input.emitter())

    def test_counter_lt(self):
        """test internal counter lt
        """
        my_file = io.StringIO(10*u"""{"test": 4}\n""")
        
        my_input = InputPyJSON(my_file, arg_number_of_events=5)

        i = 0
        for doc in my_input.emitter():
            i += 1

        self.assertEqual(i, 5)

    def test_counter_gt(self):
        """test internal counter gt
        """
        my_file = io.StringIO(5*u"""{"test": 4}\n""")

        my_input = InputPyJSON(my_file, arg_number_of_events=10)

        i = 0
        for doc in my_input.emitter():
            i += 1

        self.assertEqual(i, 5)

    def test_twice(self):
        """test trying to read twice gives nothing
        """
        my_file = io.StringIO(10*u"""{"test": 4}\n""")
        my_input = InputPyJSON(my_file, arg_number_of_events=5)

        for doc in my_input.emitter():
            pass

        with self.assertRaises(StopIteration):
            next(my_input.emitter())
            



if __name__ == '__main__':
    unittest.main()
