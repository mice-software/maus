import json
import unittest

from InputPyEmptyDocument import InputPyEmptyDocument

class InputPyEmptyDocumentTestCase(unittest.TestCase):
    def test_read100(self):
        input = InputPyEmptyDocument(100)
        input.Birth()

        testDoc = json.loads("{}")
        i = 0
        for doc in input.Emitter():
            testDoc = json.loads("""{"spill_number" : %d }""" % i)
            jsonDoc = json.loads(doc)
            self.assertEqual(jsonDoc, testDoc)
            i += 1
        self.assertEqual(i, 100)

    def test_forever_nearly(self):
        big_number = 10000
        input = InputPyEmptyDocument()
        input.Birth()

        i = 0
        for doc in input.Emitter():
            if i == big_number:
                break

            i += 1

        self.assertEqual(i, big_number)
            

    def test_twice(self):
        input = InputPyEmptyDocument(argNumberOfEvents=5)
        input.Birth()

        y = input.Emitter()
        next(y)
        next(y)
        next(y)
        next(y)
        next(y)

        with self.assertRaises(StopIteration):
            next(y)


if __name__ == '__main__':
    unittest.main()
