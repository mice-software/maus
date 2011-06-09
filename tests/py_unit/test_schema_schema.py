from unittest import TestCase

import validictory
from SchemaSchema import schema


class TestSchemaSchema(TestCase):

    def test_schema(self):
        validictory.validate(schema, schema, required_by_default=False)
