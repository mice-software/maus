# Author: Sebastien Viret <viret@in2p3.fr>
# Python Porter: Christopher Tunnell <tunnell@hep.uchicago.edu>  
#
# April 20, 2009
#
# This macro compares two LASER runs and plots a variation map
#


from src.GenericWorker import *
from src.region import *
from array import *
import ROOT
import math

class Stability(GenericWorker):
    "This worker computes the stability of laser constants between two runs"

    run_type     = None
    referenceRun = None
    mainRun      = None

    # make this runs eventually
    def __init__(self, run_type, referenceRun, mainRun):
        self.run_type     = run_type
        self.referenceRun = referenceRun
        self.mainRun      = mainRun


    # Get the channel position (SV: how to get that from region.py???)
    def GetNumber(self, hash):
        split = hash.split('_')[1:]
        number = []
        if len(split) >= 1:
            if   split[0] == 'LBA': number.append(1)
            elif split[0] == 'LBC': number.append(2)
            elif split[0] == 'EBA': number.append(3)
            elif split[0] == 'EBC': number.append(4)
            else:
                number.append(-1)
        if len(split) >= 2:
            number.append(int(split[1][1:]))
        if len(split) >= 3:
            number.append(int(split[2][1:]))
        if len(split) >= 4:
            if   split[3] == 'lowgain':  number.append(0)
            elif split[3] == 'highgain': number.append(1)
            else:
                number.append(-1)

        return number


    # Reorder the PMTs (SV: how to get that from region.py???)
    
    def get_PMT_index(self,part,j):
    
        chan2PMT_LB=[1,2,3,4,5,6,7,8,9,10,
                     11,12,13,14,15,16,17,18,19,20,
                     21,22,23,24,27,26,25,30,29,28,
                     -33,-32,31,36,35,34,39,38,37,42,41,
                     40,45,-44,43,48,47,46]
        
        chan2PMT_EB=[1,2,3,4,5,6,7,8,9,10,
                     11,12,13,14,15,16,17,18,-19,-20,
                     21,22,23,24,-25,-26,-27,-28,-31,-32,
                     33,29,30,-35,-36,34,44,38,37,43,42,
                     41,-39,-40,-45,-46,-47,-48]
    
        if part <= 1: 
            chan = chan2PMT_LB[j]-1
        else:
            chan = chan2PMT_EB[j]-1
    
        return chan
    
    # Macro necessary for the fiber re-ordering
    def get_index(self, part, module, channel):
        
        if part <= 1: 
            return  2*module+part+2*(0.5-part)*(channel%2)	  
        else: 
            return  2*module+(channel%2)	

    # Also for fiber stuff
    def get_part(self, part):
        if part <= 1: 
            return  0
        else: 
            return  part-1

        
    def ProcessRegion(self, region):

        events = set()

        # This macro just works for TileCal
         
        if 'TILECAL' == region.GetHash():

            events = set()

            # First got all the LASER events

            for event in region.RecursiveGetEvents(): 
                if event.runType == self.run_type: # Just collect the LASER stuff 
                    events.add(event)
                 
            # Only process events we should, but we return main_events + other_events + reference
            main_events = set()
            reference_events = set()
            other_events = set()

            channel_s_brut    = ROOT.TH2F("channels variation map","",128,0.,128.,96,0.,96.)
            channel_s_brut1   = ROOT.TH1F("variation summary","",200,-2.,2.)
             
            # Initialization of variables
             
            n_channels_tot = 9856 # Total number of LASER channels (per gain)
            n_evts = 1000         # Minimum number of events recorded for a channel
            n_sig  = 2            # How many sigmas do we tolerate for filter correction (for outlier rejection ONLY) ?
            n_sig2 = 2            # How many sigmas do we tolerate for fiber correction (for outlier rejection ONLY) ?
            
            isBadLimit = 20       # Variation which put the channel automatically bad, after filter correction tough
            isProblem  = 2        # Maximum accpeted variation (after all corrections)
            
            # Reference values 
            
            rat_ref   = [0 for x in range(12288)]
            
            # Partition shifts 
            
            n_FIL     = [0 for x in range(4)]
            e_FIL     = [0 for x in range(4)]
            e_FIL_n   = [0 for x in range(4)]        
            sig_FIL   = [0 for x in range(4)]
            sig_FIL_n = [100000 for x in range(4)]
            
            # Patch panel fibers shifts
            
            n_FIB     = [0 for x in range(384)]
            e_FIB     = [0 for x in range(384)]
            e_FIB_n   = [0 for x in range(384)]        
            sig_FIB   = [0 for x in range(384)]
            sig_FIB_n = [100000 for x in range(384)]
            
            m_gain = 0   #  Low gain by default
            
            
            # Start by getting the variation 
            
            n_channels =  0
            
            to_remove = set()

            filter_main = 0
            filter_ref  = 0
            amp_ref     = 0
            amp_main    = 0            
            
            for event in events:                
                if event.runNumber == self.mainRun:
                    [p, i, j, w] = self.GetNumber(event.data['region'])
                    if self.get_PMT_index(p-1,j) >= 0: # a good channel    
                        main_events.add(event)
                        filter_main = event.data['wheelpos']
                        amp_main    = event.data['requamp']
                elif event.runNumber == self.referenceRun:
                    [p, i, j, w] = self.GetNumber(event.data['region'])
                    if self.get_PMT_index(p-1,j) >= 0: # a good channel
                        index = 3072*(p-1) + 48*(i-1) + j
                        rat_ref[index] = event.data['calib_const']
                        filter_ref = event.data['wheelpos']
                        amp_ref    = event.data['requamp']
                else:
                    other_events.add(event) # las   

            #print filter_ref
            #print amp_ref

            # Sanity check

            if filter_main != filter_ref or amp_main != amp_ref:
                print 'Run don\'t have the same characteristics: abort!'
                return

            # Then compute the variation w.r.t. the reference run
            for event in main_events:
                [p, i, j, w] = self.GetNumber(event.data['region'])
                index = 3072*(p-1) + 48*(i-1) + j       
                if rat_ref[index] == 0: # No reference data, skip the event
                    to_remove.add(event)
                    continue

                event.data['variation'] = 200*(event.data['calib_const']-rat_ref[index])/(event.data['calib_const']+rat_ref[index])
                n_channels = n_channels + 1

            main_events = main_events - to_remove
             
        #################################################
        ##
        ## LOOP 1 : run over all the channels just to
        ##          get the overall variation due to the 
        ##          filter wheel (one per partition)
        ##     
        ##          We do a certain number of iterations here
        ##
        #################################################


            for iter in range(4): # Iteration
            
                n_FIL   = [0 for x in range(4)]
                e_FIL   = [0 for x in range(4)]
                sig_FIL = [0 for x in range(4)]
            
                for event in main_events:
                
                    [part_num, i, j, w] = self.GetNumber(event.data['region'])
                    part_num -= 1

                    var      = event.data['variation'] - e_FIL_n[part_num]  
               
                    # Outlier removal
                    if iter>0 and math.fabs(var)>isBadLimit:
                        continue

                    if math.fabs(var) < n_sig*sig_FIL_n[part_num]:
                        n_FIL[part_num] += 1
                        e_FIL[part_num] += var
                

                for i in range(4):
                    if n_FIL[i] != 0:
                        e_FIL[i] /= n_FIL[i]
                        
                for event in main_events:
                
                    [part_num, i, j, w] = self.GetNumber(event.data['region'])
                    part_num -= 1
                
                    var      = event.data['variation'] - e_FIL[part_num] - e_FIL_n[part_num]

                    if iter>0 and math.fabs(var)>isBadLimit:
                        continue 

                    if math.fabs(var) < n_sig*sig_FIL_n[part_num]:
                        sig_FIL[part_num] += var*var

                #print
                #print
                #print n_channels, '/', n_channels_tot, ' channels are initially considered'
                #print
                    
                for i in range(4):
                    if n_FIL[i] > 1:
                        sig_FIL_n[i] = math.sqrt(sig_FIL[i]/(n_FIL[i]-1))
                        e_FIL_n[i]  += e_FIL[i]
#                        print 'Filter wheel is introducing a global ', e_FIL_n[i], '+/-', sig_FIL_n[i], '% shift in partition ', i


        #################################################
        ##
        ##
        ## LOOP 2 : now we will start to tackle the PP fibers
        ##          discrepancies. We collect the mean calibration
        ##          value for all the fibers, corrected from the global shift
        ##          Once again we will need to do some outlier removal. So we compute the 
        ##          sigma for all the fibers, and perform some iterations
        ##
        #################################################


            for iter in range(4): # Iteration
            
                n_FIB   = [0 for x in range(384)]
                e_FIB   = [0 for x in range(384)]
                sig_FIB = [0 for x in range(384)]
            
                for event in main_events:
                
                    [part_num, i, j, w] = self.GetNumber(event.data['region'])
                    part_num -= 1
                    j = self.get_PMT_index(part_num,j)      
                    
                    index    = self.get_index(part_num,i-1,j)
                    part     = self.get_part(part_num)
                    indice   = int(128*part+index)

                    var      = event.data['variation'] - e_FIL_n[part_num]
                     
                    if (math.fabs(var) > isBadLimit):
                        continue

                    var      = var - e_FIB_n[indice]

                    # Outlier removal
                    if math.fabs(var) < n_sig2*sig_FIB_n[indice]:
                        n_FIB[indice] += 1
                        e_FIB[indice] += var
                

                for i in range(384):
                    if n_FIB[i] != 0:
                        e_FIB[i] /= n_FIB[i]
                    
                for event in main_events:

                    [part_num, i, j, w] = self.GetNumber(event.data['region'])
                    #[part_num, i, j, w] = event.region.GetNumber()
                    part_num -= 1
                    j = self.get_PMT_index(part_num,j)                 
                
                    index    = self.get_index(part_num,i-1,j)
                    part     = self.get_part(part_num)
                    indice   = int(128*part+index)
                     
                    var      = event.data['variation'] - e_FIL_n[part_num]
                     
                    if (math.fabs(var) > isBadLimit):
                        continue
                     
                    var      = var - e_FIB_n[indice]
                
                    if math.fabs(var) < n_sig2*sig_FIB_n[indice]:
                        sig_FIB[indice] += var*var
                    
                for i in range(384):
                    if n_FIB[i] > 1:
                        sig_FIB_n[i] = math.sqrt(sig_FIB[i]/(n_FIB[i]-1))
                        e_FIB_n[i]  += e_FIB[i]

#            print 'Filter wheel is introducing a global ', e_FIL_n[0], '+/-', sig_FIL_n[0], '% shift '
 #           print 'Fiber is introducing a global ', e_FIB_n[0], '+/-', sig_FIB_n[0], '% shift '

        #################################################
        ##
        ##
        ## LOOP 3 : get the final comparison, removing the 
        ##          fiber correction
        ##
        #################################################

            for event in main_events:

                [part_num, i, j, w] = self.GetNumber(event.data['region'])                
                part_num -= 1
                j = self.get_PMT_index(part_num,j)         
                 
                index    = self.get_index(part_num,i-1,j)
                part     = self.get_part(part_num)
                indice   = int(128*part+index)
                
                var      = event.data['variation'] - e_FIL_n[part_num] - e_FIB_n[indice]
                
                channel_s_brut.Fill(i-0.5+64*(part_num%2),j+0.5+48*((part_num/2)%2),var)  
                channel_s_brut1.Fill(var)    
                

       ## Then do the cosmetics
                
        ##### STAT ######  #TODO! DO NOT ACCESS gStyle directly!  Hurts everybody else.  fixme
            ROOT.gStyle.SetStatW(0.2)
            ROOT.gStyle.SetStatH(0.2)
            ROOT.gStyle.SetStatX(0.8)
            ROOT.gStyle.SetStatY(0.8)
            ROOT.gStyle.SetStatFont(42)
            ROOT.gStyle.SetOptStat(0)
            ROOT.gStyle.SetOptFit(0)
        ##################
             
            # Colored Gradient                                                                                                                                                                                              
            Number = 5;
            
            Red = [ 1.0, 1.0, 1.0, 1.0, 0.0 ] 
            Green = [ 1.0, 1.0, 0.6, 0.0, 0.0 ] 
            Blue  = [ 1.0, 0.0, 0.1, 0.0, 0.0 ] 
            
            # Define the length of the (color)-interval between this points                                                                                                                                                 
            Length = [ 0.00, 0.25, 0.5, 0.75, 1.00 ]
            
            ROOT.TColor.CreateGradientColorTable(Number, array('d', Length), array('d', Red),
                                                 array('d', Green), array('d', Blue), 300);
            
            c1 = ROOT.TCanvas("c1","Filter+fiber correction",200,200,1000,700)
            c1.SetFillColor(0);
            c1.SetBorderMode(0)
            c1.Divide(1,2)
            
            c1.cd(1)
            channel_s_brut.GetXaxis().SetTitle("Module number")
            channel_s_brut.GetYaxis().SetTitle("Channel number")
            channel_s_brut.Draw("colz")
            pt = ROOT.TPaveText(20.,95.,110.,105.,"br")
            pt.SetTextSize(0.05)
            
            filename = "Diff. bet. runs %d and %d : all partitions" % (self.mainRun, self.referenceRun)
            
            text = pt.AddText(filename);
            pt.Draw();
            
            c1.cd(2);
            
            filename = "Diff. bet. runs %d and %d : all partitions (in %%)" % (self.mainRun, self.referenceRun)
            
            channel_s_brut1.GetXaxis().SetTitle(filename)
            channel_s_brut1.Draw()
            
            c1.Update()
            c1.Print('plots/laser_comparison1.eps')
            
        return events
        
