# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# Define 'region' objects, which is what makes up the detector tree.
# Each 'region' has various attributes: it's parent(s), it child(ren),
# and any 'event'-objects associated with it.  One can also call the
# GetHash() method to get a unique location for this region in the
# detector geometry tree.
#
# NOTE: Do *not* access variables with '__' before them.  Use the
# functions that start with 'Get'.  For instance, GetHash().
#
# March 04, 2009
#

class Region:
    def __init__(self, name, type='readout'):
        # The parents of a node form a list, which is just an ordered
        # set.  The reason for this is that since nodes can have many parents,
        # we need some way to determine what the primary parent is.  This is
        # necessary when trying to determine the hash of a region (ie. where
        # it is in the tree). This is a 'hack' or 'feature' to get around the
        # detector 'tree' actually being a graph... yet we still want it to be
        # tree-like.
        self.__parents = []

        # The children of a node form a set.  No preference is given to one child
        # or anything (like any good parent :P). Unlike the case of many parents,
        # trees are allowed to have many children.  
        self.__children = set()

        # These are the events associated with a particular region. See src/event.py
        self.__events = set()

        # This dictionary (or map) stores a unique identifier for each region.
        # The reason this is a map is because there are many ways this hash
        # can be formed, so we store different hashes for each naming convention
        # and each pseudo-geometry (physical with cells or read-out
        self.__hashs = {}

        # There are many possible names for any given region. For instance,
        # one could call a region PMT 39 or channel 18. The name variable
        # should either be a string or a list of strings, whilst the __name
        # variable is a list of strings.
        assert(isinstance(name, (str, list)))
        if isinstance(name, str):
            self.__names = [name]
        elif isinstance(name, list):
            assert(len(name) > 0)
            for item in name:
                assert(isinstance(item, str))
            self.__names = name

        # The 'type' for any given region says if region is part of
        # the read-out electronics (partitions, modules, channels) or
        # the physical geometry (cells, towers).  In the case of ambiguity,
        # then assume 'readout'.  
        assert(type == 'readout' or type == 'physical' or type == 'b175' or type== 'testbeam')
        self.__type = type


    def GetEvents(self):
        return self.__events

    def AddEvent(self,event):
        self.__events.add(event)

    def SetEvents(self,events):
        self.__events = events

    def GetHash(self, name_index=0, parent_index=0):
        assert(isinstance(name_index, int) and name_index >= 0)
        assert(isinstance(parent_index, int) and parent_index >= 0)
        
        key = '%d_%s_%d' % (name_index, self.__type, parent_index)
        if self.__hashs.has_key(key):
            return self.__hashs[key]
        
        parent = self.GetParent(self.__type, parent_index)

        if parent:
            self.__hashs[key] = parent.GetHash(name_index, parent_index) + '_' + self.GetName(name_index)
        else:
            self.__hashs[key] = self.GetName(name_index)

        return self.__hashs[key]
                                 

    def GetName(self, name_index=0):
        assert(isinstance(name_index, int) and name_index >= 0)
        if 0 <= name_index < len(self.__names):
            return self.__names[name_index]
        else:
            return self.__names[0]

    def GetChildren(self, type):
        # Return regions that are of the type requested. If none
        # exist, then return all regions.
        assert(type == 'readout' or type == 'physical' or type == 'b175' or type== 'testbeam')
        if not self.__children or self.__children == set():
            return set()
        
        found = False
        newregions= set()
        for region in self.__children:
            if region.GetType() == type:
                newregions.add(region)
                found = True
                
        if found:
            return newregions
        else:
            return self.__children

    def SetChildren(self, children):
        # The children should either be a region or set of regions
        assert(isinstance(children,(Region, set)))
        if isinstance(children, Region):
            self.__children = self.__children | set([children])
        elif isinstance(children, set):
            for item in children:
                assert(isinstance(item, Region))
            self.__children = self.__children | children

    def GetType(self):
        return self.__type

    def GetParent(self, type='readout', parent_index = 0):
        assert(type == 'readout' or type == 'physical' or type == 'b175' or type== 'testbeam')
        assert(isinstance(parent_index, int) and parent_index >= 0)

        if not self.__parents or len(self.__parents) == 0:
            return None
        
        for p in self.__parents:
            if p.GetType()==type:
                return p
        
        if 0 <= parent_index < len(self.__parents):
            return self.__parents[parent_index]
        else:
            return self.__parents[0]
    
    def SetParent(self, parent):
        # The name should either be a region or list of regions
        assert(isinstance(parent,(Region, list)))
        if isinstance(parent, Region):
            self.__parents.append(parent)
        elif isinstance(parent, list):
            for item in parent:
                assert(isinstance(item, Region))
                self.__parents.append(item)

    def SanityCheck(self):
        if self.__parents:
            for region in self.__parents:
                if self not in region.__children:
                    print "Error: My parents have disowned me"

        if self.__children:
            for region in self.__children:
                if self not in region.__parents:
                        print "Error: My children have disowned me"
    
    # The depth corresponds to how far down the tree the Print() method
    # should go before stopping.  A depth of -1 means that this parameter
    # is ignored and the whole tree is printed.
    def Print(self, depth = -1, name_index=0, type='readout', parent_index=0):
        print self.GetName(name_index), self.GetHash(name_index, type, parent_index)
        depth = depth - 1

        if depth != 0 and self.GetChildren(type) != set():
            for child in self.GetChildren(type):
                child.Print(depth)

    
    def RegionGenerator(self,type='readout'):
        if self.GetChildren(type):
            for child in self.GetChildren(type):
                for i in child.RegionGenerator(type):
                    yield i
        yield self

    def RecursiveGetEvents(self,type='readout'):
        newevents = set()
        if self.GetEvents():
            newevents = newevents | self.GetEvents()
        if self.GetChildren(type) != set():
            for region in self.GetChildren(type):
                newevents = newevents | region.RecursiveGetEvents()
        return newevents
                
    def GetNumber(self, name_index=0, parent_index=0):
        hash = self.GetHash(name_index)
        split = hash.split('_')[1:]
        number = []
        if len(split) >= 1: # get partition or side
            part = {'LBA':1,'LBC':2,'EBA':3,'EBC':4}
            number.append(part[split[0]])
        if len(split) >= 2: # get module
            number.append(int(split[1][1:]))
        if len(split) >= 3: # get channel or sample
            if self.__type == 'physical':
                samp= {'A':0,'BC':1,'D':2,'E':3}
                number.append(samp[split[2][1:]])
            else:
                number.append(int(split[2][1:]))
        if len(split) >= 4: # get ADC
            if self.__type == 'physical':
                number.append(int(split[3][1:]))
            else:
                gain = {'lowgain':0,'highgain':1}
                number.append(gain[split[3]])
        return number
    
    def GetChannels(self):
        '''Returns list of channel names belonging to tower'''
        chans = []
        if not 't' in self.GetName():
            print 'GetChannels only meaningful for tower regions'
            return chans
        # returns list of channels in cell
        # -1 denotes special PMT in D0 that is actually on LBC
        # FIXME: array does not reflect cross module connections of some cryostat scintillators
        cell2chan= [\
                    #LB
                    [\
                       #A
                       [ [1,4],[5,8],[9,10],[15,18],[19,20],[23,26],[29,32],[35,38],[37,36],[45,46] ],\
                       #BC
                       [ [3,2],[7,6],[11,12],[17,16],[21,22],[27,28],[33,34],[39,40],[47,42] ],\
                       #D
                       [ [-1,0],[],[13,14],[],[25,24],[],[41,44] ],\
                       #E
                       []
                    ],\
                    #EB
                    [\
                       #A
                       [ [],[],[],[],[],[],[],[],[],[],[],[7,6],[11,10],[21,20],[32,31],[40,41] ],\
                       #BC
                       [ [],[],[],[],[],[],[],[],[],[5,4],[9,8],[15,14],[23,22],[35,30],[36,39] ],\
                       #D
                       [ [],[],[],[],[],[],[],[],[3,2],[],[17,16],[],[37,38] ],\
                       #E
                       [ [],[],[],[],[],[],[],[],[],[],[13],[12],[],[1],[],[0] ] \
                    ]\
                   ]
        
        part, module, sample, tower = self.GetNumber()
        if part<2: barrel=0
        else: barrel=1
        
        chans = cell2chan[barrel][sample][tower]
        return chans
        
        
def constructTileCal():
    """
    constructTileCal() will construct all of the regions of TileCal:
    partitions, module, channels/pmts, gains, cells, and towers.  It
    will also appropriately set the parents and children of each module.
    So, for example, LBA module 32 has a child called PMT 3. This function
    will first construct the 'readout' tree (ie. no cells, towers).  Then
    it will appropriately add cells and towers. In the future, it may contain
    digitizers, DMUs, etc.
    """

    #
    # Level 1: TileCal and it's partitions
    #

    # TileCalorimeter
    tilecal = Region('TILECAL')

    # there are 4 partitions
    partitions = set([Region('EBA'),\
                  Region('LBA'),\
                  Region('LBC'),\
                  Region('EBC')])
                  
    # set parents and children
    tilecal.SetChildren(partitions)
    for partition in partitions:
        partition.SetParent(tilecal)

    #
    # Level 2: Tell the partitions which modules they have modules
    #

    # holder for LBC's D0 channel
    chD0 = {}
    
    for partition in tilecal.GetChildren(type='readout'):
        # construct each of the 64 modules
        modules = set([Region('m%02d' % x) for x in range(1, 65)])

        #modules.add(Region('m00', type='testbeam'))
        #modules.add(Region('m65', type='b175'))
        
        # and make them children of the partition
        partition.SetChildren(modules)
        for module in modules:
            # tell the modules who their parent is
            module.SetParent(partition)
        
        # The chan2pmt variable provides the mapping between channel number
        # and PMT number.  Negative values means the PMT doesn't exist. Use
        # the variable as follows: pmt_number = chan2pmt[channel_number]
        chan2pmt = []
        if partition.GetName() == 'EBA' or partition.GetName() == 'EBC':
            chan2pmt=[1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,  \
                          13, 14, 15, 16, 17, 18,-19,-20, 21, 22, 23, 24,  \
                          -27,-26,-25,-31,-32,-28, 33, 29, 30,-36,-35, 34, \
                          44, 38, 37, 43, 42, 41,-45,-39,-40,-48,-47,-46]
        elif partition.GetName() == 'LBA' or partition.GetName() == 'LBC':
            chan2pmt=[1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, \
                          13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, \
                          27, 26, 25, 30, 29, 28,-33,-32, 31, 36, 35, 34, \
                          39, 38, 37, 42, 41, 40, 45,-44, 43, 48, 47, 46]

        # EBA15 and EBC18 are special
        # https://twiki.cern.ch/twiki/bin/view/Atlas/SpecialModules#Module_Type_11
        # The PMT mapping is a little different since it's a physically
        # smaller drawer
        chan2pmtSpecial = [-1,   -2,  -3,  -4,  5,  6,  7,  8,  9, 10, 11, 12,  \
                           13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,  \
                           -27,-26,-25,-31,-32,-28, 33, 29, 30,-36,-35, 34, \
                           44, 38, 37, 43, 42, 41,-45,-39,-40,-48,-47,-46]
        
        for module in partition.GetChildren(type='readout'):
            # For each module, create the PMTs/channels within it
            if (partition.GetName() == 'EBA' and module.GetName() == 'm15') or\
               (partition.GetName() == 'EBC' and module.GetName() == 'm18'):
                channels = set([Region(['c%02d' % x, 'p%02d' % chan2pmtSpecial[x]]) for x in range(48) if chan2pmtSpecial[x] > 0])
            else:
                channels = set([Region(['c%02d' % x, 'p%02d' % chan2pmt[x]]) for x in range(48) if chan2pmt[x] > 0])
            module.SetChildren(channels)
            for channel in module.GetChildren(type='readout'):
                channel.SetParent(module)
                # save D0 channel for later matching with cell
                if partition.GetName()=='LBC' and channel.GetName() == 'c00':
                    chD0[module.GetName()] = channel

            # Create both gains
            for channel in module.GetChildren(type='readout'):
                gains = set([Region('lowgain'), Region('highgain')])
                channel.SetChildren(gains)
                for gain in channel.GetChildren(type='readout'):
                    gain.SetParent(channel)

    # For each module, create the cells ('physical' part of detector tree)
    for partition in tilecal.GetChildren(type='readout'):
        for module in partition.GetChildren(type='readout'):
            samples = set([Region(name='s%s' % x , type='physical') for x in ['A','BC','D','E']])
            module.SetChildren(samples)
            for sample in module.GetChildren('physical'):
                sample.SetParent(module)
                towers = set()
                if 'LB' in partition.GetName(): 
                    # Towers in long barrel 
                    if 'A'  in sample.GetName():
                        towers = set([Region('t%02d' % x , type='physical') for x in xrange(10)])
                    elif 'BC' in sample.GetName():
                        towers = set([Region('t%02d' % x , type='physical') for x in xrange(9)])
                    elif  'D' in sample.GetName():
                        if 'LBA' == partition.GetName(): #Draw D0 cell on A-side
                            towers = set([Region('t%02d' % x , type='physical') for x in [0,2,4,6]])
                        else:
                            towers = set([Region('t%02d' % x , type='physical') for x in [2,4,6]])
                else:                
                    # Towers in extended barrel 
                    if 'A'  in sample.GetName():
                        towers = set([Region('t%02d' % x , type='physical') for x in [11,12,13,14,15]])
                    elif 'BC' in sample.GetName():
                        towers = set([Region('t%02d' % x , type='physical') for x in [9,10,11,12,13,14]])
                    elif  'D' in sample.GetName():
                        towers = set([Region('t%02d' % x , type='physical') for x in [8,10,12]])
                    elif  'E' in sample.GetName():
                        towers = set([Region('t%02d' % x , type='physical') for x in [10,11,13,15]])
                
                sample.SetChildren(towers)
                for tower in sample.GetChildren(type='physical'):
                    tower.SetParent(sample)
                    # Connect readout channels to physical cells
                    chans = set()
                    chansName = tower.GetChannels()
                    for ch in module.GetChildren(type='readout'):
                        if int(ch.GetName()[1:]) in chansName:
                            chans.add(ch)
                        # take care of D0 cell
                        if -1 in chansName:
                            chans.add(chD0[module.GetName()])
                            
                    tower.SetChildren(chans)
                    for ch in tower.GetChildren('readout'):
                        ch.SetParent(tower)
                        
    return tilecal

