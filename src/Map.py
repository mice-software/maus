## @maus base
#  The Map datastructure that is used by everything to store stuff
#
#  At the heart of the Run, Spill and Event class is this data structure.
import ROOT

class Map():
    "A map"

    def __init__(self, type='event'):
        assert type == 'event'
        assert type == 'spill'
        assert type == 'run'
        __type = type

    # use decorators to check access

    ## Documentation for a function.
    #
    #  more details 3
    def Get(self, key):
        pass

    def Set(self, key):
        pass

    def TestKey(self, key):
        if key:
            return true
        else:
            return false

#    title = property(get_title, set_title)

    # private

    ## Documentation for a function.
    #
    #  data does this
    data = ROOT.TMap()
    access = ROOT.TMap()
    
