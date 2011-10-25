import os
import xboa.Common
import xboa.Bunch
from xboa.Bunch import Bunch

print 'xboa version', xboa.Common.xboa_version
maus_root_dir = os.environ.get('MAUS_ROOT_DIR')
bunch = Bunch.new_from_read_builtin('maus_primary', maus_root_dir + '/tutorial/simulation.out')
(x_canvas, x_hist) = bunch.root_histogram('x', 'mm')
(x_canvas, x_hist) = bunch.root_histogram('y', 'mm')
(x_canvas, x_hist) = bunch.root_histogram('energy')
(x_canvas, x_hist) = bunch.root_histogram('mass')

#bunch = Bunch.new_from_read_builtin('maus_primary', '/home/chris/maushole/work_maus/bin/simulation.out')
#(x_canvas, x_hist) = bunch.root_histogram('x', 'mm')
#(xy_canvas, xy_hist) = bunch.root_histogram('x', 'mm', 'y', 'mm')

print "Press <Enter> to end the tutorial"
raw_input()
