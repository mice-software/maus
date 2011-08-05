import json
import unittest
import subprocess
import glob
import os
import sys
import io

from Configuration import Configuration

class ConfigurationTParserCase(unittest.TestCase):
    def test_argparse_write(self):
        sys.argv=['Configuration.py', '-sigmaPz', '1039']
        c = Configuration()
        value = c.getConfigJSON()

        jsonValue = json.loads(value)
        self.assertEqual(jsonValue["sigmaPz"], 1039)

    def test_argparse_card_value(self):
        sys.argv=['Configuration.py', '-card', 'test_parser_configuration.txt']
        c = Configuration()
        value = c.getConfigJSON()

        jsonValue = json.loads(value)
        self.assertEqual(jsonValue['test_integer'], 15) 

    def test_argparse_card_str(self):
        sys.argv=['Configuration.py', '-card', 'test_parser_configuration.txt']
        c = Configuration()
        value = c.getConfigJSON()

        jsonValue = json.loads(value)
        self.assertEqual(jsonValue['test_string'], 'testing') 

    def test_argparse_card_dict(self):
        sys.argv=['Configuration.py', '-card', 'test_parser_configuration.txt']
        c = Configuration()
        value = c.getConfigJSON()

        jsonValue = json.loads(value)
        self.assertEqual(jsonValue['test_Dict'], {"test_position":{"x":0.0, "y":-0.0, "z":-8000.0}, "random_seed":6995}) 

if __name__ == '__main__':
    unittest.main()
