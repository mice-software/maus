# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.ReadGenericCalibration import *
from src.region import *

class CISSysRefit(ReadGenericCalibration):
    "Refit the CIS scan with systematic uncertainties"

    def __init__(self, systematic = 0.5):
        pass

    def ProcessStart(self):
        pass

    def ProcessStop(self):
        pass
    
    def ProcessRegion(self, region):
        newevents = set()

        for event in region.events:
            if event.data.has_key('scan') and event.data['scan']:
                gscan = event.data['scan']
                    
                for i in range(gscan.GetN()):
                    gscan.SetPointError(i, 0, math.sqrt(gscan.GetErrorY(i)*gscan.GetErrorY(i) + 0.5*0.5))
                        
                if 'high' in region.GetHash():
                    fit = ROOT.TF1('fit', '[0]*x', 3.0, 10.0)
                    fit.SetParameter(0, 81.8)
                else:
                    fit = ROOT.TF1('fit', '[0]*x', 300.0, 700.0)
                    fit.SetParameter(0, 1.29)

                gscan.Fit(fit, 'qr')
#                print event.data['calibration'], fit.GetParameter(0)
                event.data['scan'] = gscan

            newevents.add(event)
                                                                                                                                                                                                    
                        
        region.events = newevents
