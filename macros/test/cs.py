execfile('src/load.py', globals()) # don't remove this!


#x1 = ReadCsFile(processingDir="/afs/cern.ch/user/t/tilecali/w0/ntuples/cs")

u1 = UseCs(run="-3week",runType='cesium',region='LBA_m22')

x1 = ReadCsFile(processingDir="/data/cs",normalize=True)

i1 = ReadIntegratorDB(useSqlite=True,intgain=3)

w1 = WriteCsDB(offline_tag='HLT-UPD1-01',offline_iov = 'use latest run',register=False,\
               def_iov=75815,threshold=0.05)

p1 = Print()



processors = [u1, x1, i1, w1, p1]
Go(processors)


