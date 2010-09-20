execfile('src/load.py', globals()) # don't remove this!

u = Use(run=90555)
r = ReadDQList()
p1 = Print()
p1.SetVerbose(True)

processors = [u, r, p1]

Go(processors)


