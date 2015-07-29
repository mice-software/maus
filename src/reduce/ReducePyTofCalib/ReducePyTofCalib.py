"""
ReducePyTofCalib gets slab hits and makes a Tree for calibration.
"""
#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
# 
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
#
# Turn off false positives related to ROOT
#pylint: disable = E1101
# Disable messages about too many branches and too many lines.
#pylint: disable = R0912, R0914, R0915
#pylint: disable = C0103
#pylint: disable = W0201, W0612, W0613
import os
import json
from array import array
import ErrorHandler
import maus_cpp.converter
import ROOT

class ReducePyTofCalib: # pylint: disable=R0902
    """
    ReducePyTofCalib plots processes tof slab hits
    and fills a ROOT Tree.
    For each slab hit, the slab number, time, charge
    for either PMT is stored.
    The output ROOT file is named tofcalibdata_RN.root
    where RN = run_number from the data.
    This reducer should be run with the following
    extra argument: "-TOFTdcConversion 25"
    The argument is necessary for storing the times
    in picoseconds as required by the downstream
    calibration utility.
    """

    def __init__(self):
        """
        Set initial attribute values.
        @param self Object reference.
        """
        # Do initialisation specific to this class.
        # Refresh_rate determines how often (in spill counts) the
        # canvases are updated.
        self.run_ended = False
        self.tdcPico = 1000.0
        self.calib_file = "tofcalibdata_"
        self.output_dir = ""
        if not self.__make_tree():
            print 'ReducePyTofCalib: Failed to make tree'
            return

    def birth(self, config_json):
        """
        Configure worker from data cards. Overrides super-class method. 
        @param self Object reference.
        @param config_doc JSON document.
        @returns True if configuration succeeded.
        """
        # setup the root tree
        self.rnum = '000'
        self.run_ended = False
        config_doc = json.loads(config_json)
        self.output_dir = config_doc["end_of_run_output_root_directory"]
        return True


    def process(self, spill_in):
        """
        Process a spill, get slab hits via get_slab_hits
        @param self Object reference.
        @param spill Current spill.
        @returns list of JSON documents. If the the spill count is
        divisible by the current refresh rate then a list of 3
        histogram JSON documents are returned. Otherwise a single list
        with the input spill is returned. 
        @throws ValueError if "slab_hits" and "space_points" information
        is missing from the spill.
        """
        # the mergers now process MAUS::Data
        # the output are images in json strings
        # check if input is Data, 
        # if it's not data, load json and convert to data
        # note that online celery/mongo returns json string
        def_doc = {"maus_event_type":"Spill"}
        if spill_in.__class__.__name__ == 'MAUS::Data':
            spill_data = spill_in
            del spill_in
        else:
            try:
                json_doc = json.loads(spill_in.rstrip())
            except Exception, e: # pylint:disable=W0703
                def_doc = ErrorHandler.HandleException(def_doc, self)
                return unicode(json.dumps(def_doc))
            try:
                spill_data = maus_cpp.converter.data_repr(json_doc)
                json_doc = None
            except Exception: # pylint:disable=W0703
                def_doc = ErrorHandler.HandleException(def_doc, self)
                return unicode(json.dumps(def_doc))

        daq_evtype = spill_data.GetSpill().GetDaqEventType()
        spill = spill_data.GetSpill()
        if daq_evtype == "end_of_run":
            if (not self.run_ended):
                self.run_ended = True
            else:
                return spill_data

        data_spill = True
        if daq_evtype == "start_of_run" \
              or daq_evtype == "start_of_burst" \
              or daq_evtype == "end_of_burst":
            data_spill = False

        self.rnum = spill.GetRunNumber()
        # Get TOF slab hits & fill the relevant histograms.
        if not self.get_slab_hits(spill):
            def_doc["errors"] = "ReducePyTofCalib: no slab hits in spill"
            return unicode(json.dumps(def_doc))

        # delete references to data if any
        try:
            maus_cpp.converter.del_data_repr(spill)
        except: # pylint: disable=W0702
            pass
        return spill_data

    def get_slab_hits(self, spill):
        """ 
        Get the TOF slab hits and update the histograms.

        @param self Object reference.
        @param spill Current spill.
        @return True if no errors or False if no "slab_hits" in
        the spill.
        """
        if spill.GetReconEventSize() == 0:
            return False
        # print 'nevt = ', len(spill['recon_events']), spill['recon_events']
        reconevents = spill.GetReconEvents()
        for evn in range(spill.GetReconEventSize()):
            tof_event = reconevents[evn].GetTOFEvent()
            if tof_event is None:
                return False
            # Return if we cannot find slab_hits in the event.
            tof_slab_hits = tof_event.GetTOFEventSlabHit()
            if tof_slab_hits is None:
                return False

            tof0_sh = tof_slab_hits.GetTOF0SlabHitArray()
            tof1_sh = tof_slab_hits.GetTOF1SlabHitArray()
            tof2_sh = tof_slab_hits.GetTOF2SlabHitArray()
            sh_list = [tof0_sh, tof1_sh, tof2_sh ]
            # setup the detectors for which we want to look at hits
            dets = ['tof0', 'tof1', 'tof2']
            self.slabA[0] = -99
            self.slabB[0] = -99
            self.slabC[0] = -99
            self.slabD[0] = -99
            self.slabE[0] = -99
            self.slabF[0] = -99
            self.adc0[0] = self.adc1[0] = self.adc2[0] = self.adc3[0] = 0
            self.adc4[0] = self.adc5[0] = self.adc6[0] = self.adc7[0] = 0
            self.adc8[0] = self.adc9[0] = self.adc10[0] = self.adc11[0] = 0
            self.t0[0] = self.t1[0] = self.t2[0] = self.t3[0] = 0.
            self.t4[0] = self.t5[0] = self.t6[0] = self.t7[0] = 0.
            self.t8[0] = self.t9[0] = self.t10[0] = self.t11[0] = 0.
      
            ntof0_sh = ntof1_sh = ntof2_sh = 0
            ntof0_sh = tof0_sh.size()
            ntof1_sh = tof1_sh.size()
            ntof2_sh = tof2_sh.size()

            # require 2 hits in each TOF
            # perhaps this requirement is too stringent (?)
            if ntof0_sh != 2 or ntof1_sh != 2 or ntof2_sh != 2:
                continue
            for index, dethits in enumerate(sh_list):
                # print 'index, size >> ',index, dethits.size()
                for i in range(dethits.size()):
                    pos = dethits[i].GetSlab()
                    # print '>> hit#, slab ',i,pos
                    plane_num = dethits[i].GetPlane()
                    # print '>> hit#, plane ',i,plane_num
                    if plane_num < 0 or plane_num > 1:
                        return False
                    rt0 = 0.0
                    rt1 = 0.0
                    q0 = 0
                    q1 = 0
                    # plane 0, pmt0 hit for this slab
                    if (dethits[i].GetPmt0() is not None):
                        rt0 = dethits[i].GetPmt0().GetRawTime() * self.tdcPico
                        q0 = dethits[i].GetPmt0().GetCharge()
                        # plane 0, pmt1 hit for this slab
                    if (dethits[i].GetPmt1() is not None):
                        rt1 = dethits[i].GetPmt1().GetRawTime() * self.tdcPico
                        q1 = dethits[i].GetPmt1().GetCharge()

                    # TOF0
                    if index == 0:
                        if plane_num == 0:
                            self.slabA[0] = pos
                            self.t0[0] = rt0
                            self.t1[0] = rt1
                            self.adc0[0] = q0
                            self.adc1[0] = q1
                        if plane_num == 1:
                            self.slabB[0] = pos
                            self.t2[0] = rt0
                            self.t3[0] = rt1
                            self.adc2[0] = q0
                            self.adc3[0] = q1

                    # TOF1
                    if index == 1:
                        if plane_num == 0:
                            self.slabC[0] = pos
                            self.t4[0] = rt0
                            self.t5[0] = rt1
                            self.adc4[0] = q0
                            self.adc5[0] = q1
                        if plane_num == 1:
                            self.slabD[0] = pos
                            self.t6[0] = rt0
                            self.t7[0] = rt1
                            self.adc6[0] = q0
                            self.adc7[0] = q1

                    # TOF2
                    if index == 2:
                        if plane_num == 0:
                            self.slabE[0] = pos
                            self.t8[0] = rt0
                            self.t9[0] = rt1
                            self.adc8[0] = q0
                            self.adc9[0] = q1
                        if plane_num == 1:
                            self.slabF[0] = pos
                            self.t10[0] = rt0
                            self.t11[0] = rt1
                            self.adc10[0] = q0
                            self.adc11[0] = q1
            self.dataTree.Fill()
        return True


    def __make_tree(self): #pylint: disable=R0201, R0914
        """
        Initialise ROOT to display histograms.

        @param self Object reference.
        """

        self.slabA = array('i', [-99])
        self.slabB = array('i', [-99])
        self.slabC = array('i', [-99])
        self.slabD = array('i', [-99])
        self.slabE = array('i', [-99])
        self.slabF = array('i', [-99])
        self.t0 = array('d', [0])
        self.t1 = array('d', [0])
        self.t2 = array('d', [0])
        self.t3 = array('d', [0])
        self.t4 = array('d', [0])
        self.t5 = array('d', [0])
        self.t6 = array('d', [0])
        self.t7 = array('d', [0])
        self.t8 = array('d', [0])
        self.t9 = array('d', [0])
        self.t10 = array('d', [0])
        self.t11 = array('d', [0])
        self.adc0 = array('i', [0])
        self.adc1 = array('i', [0])
        self.adc2 = array('i', [0])
        self.adc3 = array('i', [0])
        self.adc4 = array('i', [0])
        self.adc5 = array('i', [0])
        self.adc6 = array('i', [0])
        self.adc7 = array('i', [0])
        self.adc8 = array('i', [0])
        self.adc9 = array('i', [0])
        self.adc10 = array('i', [0])
        self.adc11 = array('i', [0])
        self.dataTree = ROOT.TTree("dataTree", "tofdata")
        if self.dataTree is None:
            print 'Could not create Tree'
            return False
        self.dataTree.SetDirectory(0)
        self.dataTree.Branch("slabA", self.slabA, "slabA/I")
        self.dataTree.Branch("slabB", self.slabB, "slabB/I")
        self.dataTree.Branch("t0", self.t0, "t0/D")
        self.dataTree.Branch("t1", self.t1, "t1/D")
        self.dataTree.Branch("t2", self.t2, "t2/D")
        self.dataTree.Branch("t3", self.t3, "t3/D")
        self.dataTree.Branch("adc0", self.adc0, "adc0/I")
        self.dataTree.Branch("adc1", self.adc1, "adc1/I")
        self.dataTree.Branch("adc2", self.adc2, "adc2/I")
        self.dataTree.Branch("adc3", self.adc3, "adc3/I")

        self.dataTree.Branch("slabC", self.slabC, "slabC/I")
        self.dataTree.Branch("slabD", self.slabD, "slabD/I")
        self.dataTree.Branch("t4", self.t4, "t4/D")
        self.dataTree.Branch("t5", self.t5, "t5/D")
        self.dataTree.Branch("t6", self.t6, "t6/D")
        self.dataTree.Branch("t7", self.t7, "t7/D")
        self.dataTree.Branch("adc4", self.adc4, "adc4/I")
        self.dataTree.Branch("adc5", self.adc5, "adc5/I")
        self.dataTree.Branch("adc6", self.adc6, "adc6/I")
        self.dataTree.Branch("adc7", self.adc7, "adc7/I")

        self.dataTree.Branch("slabE", self.slabE, "slabE/I")
        self.dataTree.Branch("slabF", self.slabF, "slabF/I")
        self.dataTree.Branch("t8", self.t8, "t8/D")
        self.dataTree.Branch("t9", self.t9, "t9/D")
        self.dataTree.Branch("t10", self.t10, "t10/D")
        self.dataTree.Branch("t11", self.t11, "t11/D")
        self.dataTree.Branch("adc8", self.adc8, "adc8/I")
        self.dataTree.Branch("adc9", self.adc9, "adc9/I")
        self.dataTree.Branch("adc10", self.adc10, "adc10/I")
        self.dataTree.Branch("adc11", self.adc11, "adc11/I")
        return True


    def save_file(self):       
        """
        Save the ROOT file. This is called
        at death.
        @param self Object reference.
        """
        
        outDir = str(self.output_dir) + str(self.rnum)
        outfilename = outDir + "/" + self.calib_file + str(self.rnum) + ".root"
        try:
            if not os.path.exists(outDir):
                os.makedirs(outDir)
            if not os.path.isdir(outDir):
                raise ValueError("image_directory is a file: %s" % outDir)
        except OSError:
            pass
        self.calibFile = ROOT.TFile(outfilename,"recreate")
        self.calibFile.cd()
        self.dataTree.Write()
        self.calibFile.ls()
        self.calibFile.Close()
        return True

    def death(self):
        """
        Save the output ROOT file
        """
        self.save_file()
        return True
