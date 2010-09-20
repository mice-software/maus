# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.ReadGenericCalibration import *
from src.region import *
import logging, os, sys


class ReadDQList(ReadGenericCalibration):
    "The DQ List reader"

    matteo =['EBA_m19_p04',
             'EBA_m23_p17',
             'EBA_m22_p44',
             'EBA_m23_p17',
             'EBC_m25_p29_high',
             'EBC_m37_p07',
             #EBC28 DMU0,1,10,11 with some 1/2 single str in CI
             'EBC_m64_p29',
             'LBA_m03_p08',
             'LBA_m50_p48',
             'LBA_m51_p12_high',
             'LBA_m53_p09',
             'LBA_m59_p34_low',
             'LBA_m63_p17',
             'LBC_m14_p02',
             'LBC_m27_p30',
             'LBC_m31_p24',
             'LBC_m32_p12_low',
             'LBC_m41_p46_low',
             'LBC_m41_p48_low',
             'EBC_m41_',
             'LBC_m36_',
             'EBC_m38_c00_',
             'EBC_m38_c01_',
             'EBC_m38_c02_',
             'LBC_m55_p18',
             'LBC_m55_p16',
             'EBA_m05_p11',
             'EBA_m16_p29',
             'EBA_m18_p21',
             'EBA_m25_p16',
             'EBA_m39_p29',
             'EBC_m06_p09',
             'EBC_m09_p01',
             'EBC_m34_p01',
             'EBC_m37_p42',
             'EBC_m40_p44_high',
             'EBC_m48_p34',
             'LBA_m07_p28',
             'LBA_m45_p28_low',
             'LBA_m57_p22',
             'LBC_m15_p20',
             'LBC_m24_p45',
             'LBC_m28_p47',
             ]
    n = 0
            
    
    def ProcessRegion(self, region):
        newevents = set()

        if 'gain' not in region.GetHash(True):
            for event in region.events:
                if event.runType != 'staging':
                    newevents.add(event)
        else:
            for event in region.events:
                if event.runType != 'staging':
                    newevents.add(event)
                    continue

                found = False
                for bad_region in self.matteo:
                    if bad_region in region.GetHash(True):
                        found = True

                data = {}
                data['isBad'] = found
                if data['isBad']:
                    self.n += 1 
                newevents.add(Event('DQ', event.runNumber, data, event.time))


        region.events = newevents

    def ProcessStop(self):
        print 'There are ', self.n, 'bad PMTs in the DQ list'
                

