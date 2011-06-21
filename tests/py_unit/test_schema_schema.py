import unittest
import validictory
from SchemaSchema import schema


class TestSchemaSchema(unittest.TestCase):

    def test_schema(self):
        validictory.validate(schema, schema, required_by_default=False)

if __name__ == '__main__':
    unittest.main()

