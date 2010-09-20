from src.GenericWorker import *

class CheckNoiseBounds(GenericWorker):
    '''Check noise values against established bounds.  Fails are reccomended for DB update.'''

    c1 = src.MakeCanvas.MakeCanvas()

    def __init__(self):
        self.outFileName = 'errors.txt'

        self.noiseThresholds = {}
        self.noiseThresholds['ped'] = [20,100]
        self.noiseThresholds['hfnHG'] = [0.1,  2.50,  26.]
        self.noiseThresholds['hfnLG'] = [0.1,  1.25,  13.]
        
    def ProcessStart(self):
        self.outFile = open(self.outFileName,'w')

    def ProcessStop(self):
        self.outFile.close()
    
    def ProcessRegion(self, region):
        hash = region.GetHash()
        if 'gain' not in hash:
            return
        gain = 'HG'
        if 'low' in hash:
            gain = 'LG'
            
        for event in region.events:
            if event.runType != 'Ped':
                continue
            data = event.data

            # Pedestal check
            if data['ped'] < self.noiseThresholds['ped'][0]:
                self.outFile.write(hash+' pedestal under threshold. ped = '+str(data['ped'])+'\n')
            elif data['ped'] > self.noiseThresholds['ped'][1]:
                self.outFile.write(hash+' pedestal over threshold. ped = '+str(data['ped'])+'\n')

            # HFN check
            if data['hfn'] > self.noiseThresholds['hfn'+gain][2]:
                self.outFile.write(hash+' hf noise over VeryLargeHfNoise threshold. hfn = '+str(data['hfn'])+'\n')
            elif data['hfn'] > self.noiseThresholds['hfn'+gain][1]:
                self.outFile.write(hash+' hf noise over LargeHfNoise threshold. hfn = '+str(data['hfn'])+'\n')
            elif data['hfn'] < self.noiseThresholds['hfn'+gain][0]:
                self.outFile.write(hash+' hf noise too low. hfn = '+str(data['hfn'])+'\n')
    

