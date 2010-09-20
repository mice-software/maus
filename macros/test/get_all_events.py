execfile('src/load.py', globals()) # don't remove this!

u = Use(90555)

processors = [u, GetAllEvents()]

Go(processors)


