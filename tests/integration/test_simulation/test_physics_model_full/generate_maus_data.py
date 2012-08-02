import os
import subprocess

print 'Reference dataset from',os.environ['MAUS_ROOT_DIR']
log = open('maus_data/maus.some_version.log', 'w')
proc = subprocess.Popen(['python', 'test_physics_model_tools/generate_reference_data.py', 'maus'], stdout=log, stderr=subprocess.STDOUT)
proc.wait()
os.system('mv maus_ref_data.dat maus_data/maus.some_version.ref_data.dat')
log.close()

#for g4micesim in [ 'head', 'release-2-3-0' ]:
#  os.environ['MICESRC'] = '/home/cr67/G4MICE/MICE-'+g4micesim
#  print 'Running test with MICESRC',os.environ['MICESRC']
#  os.system('python CodeComparison.py --in=g4mice_data/g4mice.2-3-0.ref_data.dat --out=g4mice_data/g4mice_'+g4micesim+'.test_data.dat >& g4mice_data/g4mice.'+g4micesim+'.test.log')

os.system('rm MaterialBlock.dat')
os.system('rm maus_output.root')
os.system('rm configuration.py')


