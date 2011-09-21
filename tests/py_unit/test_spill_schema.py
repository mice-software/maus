import unittest
import validictory
from SchemaSchema import schema
from SpillSchema import spill


class TestSpillSchema(unittest.TestCase):

    def test_schema(self):
        validictory.validate(spill, schema, required_by_default=False)

if __name__ == '__main__':
    unittest.main()
