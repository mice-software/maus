# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

import os, datetime

def getPlotDirectory():
    date = datetime.date.today().strftime('%Y_%m_%d')
    dir = 'plots/%s' % date
    createDir(dir)
    if os.path.exists('plots/latest'):
        os.unlink('plots/latest')
    os.symlink(date, 'plots/latest')
    return dir
    
    
def createDir(name):
    if not os.path.isdir(name):
        os.makedirs(name)
        
        
    
