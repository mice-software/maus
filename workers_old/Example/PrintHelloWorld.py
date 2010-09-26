from src.GenericWorker import *
import random

class PrintHelloWorld(GenericWorker):
    "A demo class that just prints hello world"

    def ProcessRegion(self, region):
        if region.GetHash() == 'TILECAL':
            print "Hello world from the worker PrintHelloWorld!"
