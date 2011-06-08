"""
Simple script to excercise the optics application. Don't test the output
automatically - but we should return errors from RF, 6D, polyfit; mapping should
return a beta trans that is nearly periodic; optimiser should return a beta
trans that is almost exactly periodic.
"""

import os

os.chdir(os.getenv('MAUS_ROOT_DIR')+'/tests/integration/optics')
for test in ['mapping', 'optimiser', 'rf', '6d', 'polyfit']:
  run_command = './optics files/cards.'+test+' >& log.'+test
  print run_command
  os.system(run_command)



