# Author: Mikhail Makouski
# June 2009
# Look through some cesium runs and write constants to COOL

execfile('src/load.py', globals()) # don't remove this!


#x1 = ReadCsFile(processingDir="/afs/cern.ch/user/t/tilecali/w0/ntuples/cs")

u1 = UseCs(run="-3week",runType='cesium',region='LBA')

x1 = ReadCsFile(processingDir="/data/cs",normalize=True)

i1 = ReadIntegratorDB(useSqlite=True,intgain=3)

w1 = WriteCsDB(offline_tag='HLT-UPD1-01',offline_iov = 'use latest run',register=False,\
               def_iov=95815,threshold=0.05)

p1 = Print()



processors = [u1, x1, i1, w1]
Go(processors)


