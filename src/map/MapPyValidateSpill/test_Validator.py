import Validator
import unittest

class ValidatorTestCase(unittest.TestCase):

  def test_validate_optionalItems(self):
    schema  = {"type":"array","optionalItems":[{"type":"integer"}]}
    val = Validator.JSONSchemaValidator(interactive_mode=False)
    json_good = [0, 1, 2]
    val.validate(json_good, schema)
    json_bad  = [0, 1, 2.4]
    with self.assertRaises(ValueError): val.validate(json_bad, schema)


if __name__ == '__main__':
    unittest.main()


