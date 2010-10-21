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
                
    def GetNumber(self, type='readout'):
        pass
    
        
        
def constructMICE():
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
    # Level 1: MICE detectors
    #

    # MICE
    mice = Region('MICE')

    # there are 4 partitions
    detector = set([Region('GVA1'),\
                        Region('TOF0'),\
                        Region('TOF1'),\
                        Region('TOF2')])
                  
    # set parents and children
    tilecal.SetChildren(detector)
    for partition in partitions:
        partition.SetParent(mice)

    # 
    # TOF channels
    # 

                        
    return mice

