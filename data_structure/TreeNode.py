import ROOT

class TreeNode:
    "A tree node"

    def getType(self):
        assert self.__type == "head" 
        return self.__type

    # private
    __type = "none"
    __daughters = []
    __data = ROOT.TMap()

    
