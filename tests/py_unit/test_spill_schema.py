from unittest import TestCase

import validictory
from SchemaSchema import schema
from SpillSchema import spill


class TestSpillSchema(TestCase):

    def test_schema(self):
        validictory.validate(spill, schema, required_by_default=False)
