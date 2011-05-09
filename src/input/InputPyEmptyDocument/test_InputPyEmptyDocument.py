import json
import unittest

from InputPyEmptyDocument import InputPyEmptyDocument

class InputPyEmptyDocumentTestCase(unittest.TestCase):
    """
    Test class for InputPyEmptyDocument
    """
    
    def test_read100(self):
        """
        Read 100 events only
        """
        my_input = InputPyEmptyDocument(100)
        self.assertTrue(my_input.birth())

        test_doc = json.loads("{}")
        i = 0
        for doc in my_input.emitter():
            test_doc = json.loads("""{"spill_number" : %d }""" % i)
            json_doc = json.loads(doc)
            self.assertEqual(json_doc, test_doc)
            i += 1
        self.assertEqual(i, 100)
        self.assertTrue(my_input.death())

    def test_forever_nearly(self):
        """
        Test runnig forever by trying to get 10000 empty documents
        """
        big_number = 10000
        my_input = InputPyEmptyDocument(arg_number_of_events = -1)
        self.assertTrue(my_input.birth())

        i = 0
        for doc in my_input.emitter():
            test_doc = json.loads("""{"spill_number" : %d }""" % i)
            json_doc = json.loads(doc)
            self.assertEqual(json_doc, test_doc)

            if i == big_number:
                break

            i += 1

        self.assertEqual(i, big_number)

        self.assertTrue(my_input.death())

    def test_twice(self):
        """
        Test to make sure that if the input is done spitting out documents, then you can't get more documents from it
        """
        my_input = InputPyEmptyDocument(arg_number_of_events=5)
        self.assertTrue(my_input.birth())

        some_value = my_input.emitter()
        next(some_value)
        next(some_value)
        next(some_value)
        next(some_value)
        next(some_value)

        with self.assertRaises(StopIteration):
            next(some_value)

        self.assertTrue(my_input.death())


if __name__ == '__main__':
    unittest.main()
