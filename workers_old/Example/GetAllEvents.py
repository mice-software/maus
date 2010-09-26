from src.GenericWorker import *
import random

class GetAllEvents(GenericWorker):
    "A demo class that gets all detector events"

    def ProcessRegion(self, region):
        if region.GetHash() == 'TILECAL':
            print region.GetHash()
            print region.RecursiveGetEvents()
