## @maus pyexample
#  Documentation for this module.
#
#  More details.

import ROOT

## Documentation for a class. 
#  
#  More details2.  
class Map:
    "A map"

    # use decorators to check access

    ## Documentation for a function.
    #
    #  more details 3
    def Get(self, key):
        return self._title

    def Set(self, key):
        self._title = title

    title = property(get_title, set_title)

    # private
    __data = ROOT.TMap()
    __access = ROOT.TMap()
    
