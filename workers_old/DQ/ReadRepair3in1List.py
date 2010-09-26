# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.ReadGenericCalibration import *
from src.region import *
import logging, os, sys


class ReadRepair3in1List(ReadGenericCalibration):
    "The repair list reader"

    # Repair list made from Denis Santos' e-mail. Replaced 3in1 cards
    repairs = ['LBA_m23_p29',\
               'LBA_m23_p30',\
               'LBA_m23_p27',\
               'EBA_m23_p01',\
               'EBA_m23_p16',\
               'EBA_m55_p15',\
               'EBA_m53_p03',\
               'EBA_m53_p04',\
               'EBA_m53_p03',\
               'EBA_m04_p17',\
               'EBA_m04_p17',\
               'LBC_m20_p29',\
               'LBA_m18_p29',\
               'LBA_m18_p37',\
               'EBA_m11_p22',\
               'LBC_m37_p48',\
               'LBC_m64_p17',\
               'LBC_m24_p45',\
               'LBC_m15_p20',\
               'LBC_m28_p47',\
               'EBC_m37_p42',\
               'LBA_m08_p28',\
               'LBA_m39_p27',\
               'LBA_m39_p26',\
               'LBC_m03_p36',\
               'LBC_m11_p03',\
               'EBA_m24_p17',\
               'LBA_m08_p28',\
               'EBA_m25_p16',\
               'EBA_m25_p30',\
               'EBA_m02_p01',\
               'EBA_m02_p04',\
               'EBA_m08_p22',\
               'LBA_m57_p22',\
               'LBA_m07_p28',\
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
                

