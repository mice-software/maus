execfile('src/load.py', globals()) # don't remove this!

u1 = Use(run=90555, region='LBA_m33_c26')
x1 = ReadCISFile(getScans=False, cut=3)

u2 = Use(run=90555, region='LBA_m33_c26_')
x2 = ReadCISFile(getScans=True, cut=3)

processors = [u1, x1, RemoveGoodEvents(), Print(), Clear(), u2, x2, Print()]

Go(processors)


