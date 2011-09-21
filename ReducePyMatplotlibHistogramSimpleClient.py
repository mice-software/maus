#!/usr/bin/env python

from Configuration import Configuration

import MAUS
from ReducePyMatplotlibHistogram import ReducePyMatplotlibHistogram

reducer = ReducePyMatplotlibHistogram()
configJson = Configuration()
success = reducer.birth(configJson.getConfigJSON())
# self.assertTrue(success)

file = open('histogram.json','r')
line = file.readline()
print reducer.process(line, line)
print reducer.process(line, line)

success = reducer.death()
# self.assertTrue(success)
