# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.ReadGenericCalibration import *
from src.region import *
import logging, os, sys


class ReadRepairModuleList(ReadGenericCalibration):
    "The repair list reader"

    # Repair list made from Denis Santos' slides
    # http://indico.cern.ch/getFile.py/access?contribId=1&resId=1&materialId=slides&confId=50814
    repairs = ['EBC_m13',\
               'EBC_m22',\
               'EBC_m26',\
               'EBC_m37',\
               'EBC_m38',\
               'EBC_m45',\
               'LBC_m10',\
               'LBC_m11',\
               'LBC_m13',\
               'LBC_m15',\
               'LBC_m17',\
               'LBC_m20',\
               'LBC_m21',\
               'LBC_m24',\
               'LBC_m25',\
               'LBC_m28',\
               'LBC_m29',\
               'LBC_m34',\
               'LBC_m37',\
               'LBC_m47',\
               'LBC_m59',\
               'LBC_m64',\
               'LBC_m02',\
               'LBC_m03',\
               'LBA_m23',\
               'LBA_m18',\
               'LBA_m17',\
               'LBA_m11',\
               'LBA_m08',\
               'LBA_m01',\
               'LBA_m59',\
               'LBA_m54',\
               'LBA_m53',\
               'LBA_m50',\
               'LBA_m48',\
               'LBA_m39',\
               'LBA_m37',\
               'EBA_m04',\
               'EBA_m11',\
               'EBA_m18',\
               'EBA_m23',\
               'EBA_m24',\
               'EBA_m26',\
               'EBA_m33',\
               'EBA_m44',\
               'EBA_m53',\
               'EBA_m55',\
               ]
    n = 0
            
    
    def ProcessRegion(self, region):
        newevents = region.events

        if 'gain' not in region.GetHash(True):
            return
        else:
            found = False
            for bad_region in self.repairs:
                if bad_region in region.GetHash(True):
                    found = True

            data = {}
            data['isBad'] = found
            if data['isBad']:
                self.n += 1 
            newevents.add(Event('Repair', 0, data, 0))


        region.events = newevents

    def ProcessStop(self):
        print 'There are ', self.n, 'bad gains in the repair list'
                

