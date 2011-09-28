import json
import unittest
import io

from InputPySpillGenerator import InputPySpillGenerator

class InputPyJSONTestCase(unittest.TestCase):
    """Tests InputPyJSON
    """

    def test_birth_bad(self):
        """test bad file content
        """
        my_input = InputPySpillGenerator()
        test_conf_1 = unicode('{"spill_generator_number_of_spills":"bob"}')
        test_conf_2 = unicode("{}")
        test_conf_3 = unicode('{"spill_generator_number_of_spills":-1}')
        self.assertFalse(InputPySpillGenerator().birth(test_conf_1))
        self.assertFalse(InputPySpillGenerator().birth(test_conf_2))
        self.assertFalse(InputPySpillGenerator().birth(test_conf_3))
    
    def test_generate100(self):
        """test generate 100 events
        """
        my_input = InputPySpillGenerator()
        test_conf = unicode('{"spill_generator_number_of_spills":100}')
        self.assertTrue(my_input.birth(test_conf))
        n_spills = 0
        for doc in my_input.emitter():
            json_doc = json.loads(doc)
            self.assertTrue(json_doc == {})
            n_spills += 1
        self.assertEqual(n_spills, 100)
        self.assertTrue(my_input.death())

    def test_generate0(self):
        """should be able to generate 0 events (and return nothing)
        """
        my_input = InputPySpillGenerator()
        test_conf = unicode('{"spill_generator_number_of_spills":0}')
        self.assertTrue(my_input.birth(test_conf))
        for doc in my_input.emitter():
            raise RuntimeError("Shouldn't generate any documents here")

if __name__ == '__main__':
    unittest.main()
