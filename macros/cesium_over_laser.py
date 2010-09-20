execfile('src/load.py', globals()) # don't remove this!

run = 91381

u = Use(run="tmp/good_laser_list",useDateProg=False, region='EBA_m07_c41_lowgain')
l = ReadLaser()
x1 = ReadCsFile(processingDir="/data/cs",period=7)

p = Print()
p.SetVerbose()

r = Ratio()

processors = [u, l, u, x1, p, r]

Go(processors)


