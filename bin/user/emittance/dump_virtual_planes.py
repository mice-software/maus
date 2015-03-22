

import emittance_analysis as em_ana

import ROOT
import math
import sys
import array

ROOT.gROOT.SetBatch( True )

hist_momentum_numBins = 1000
hist_momentum_min = 0.0
hist_momentum_max = 10.0

hist_pids = [ 11, -11, 22 ]


cut_radius = 200.0

if len( sys.argv ) <= 1 :
  print "No File Supplied."
  sys.exit( 0 )


virtual_hits_dict = {}
virtual_hists_dict = {}
virtual_plane_locations = array.array('d')
number_planes = 0

test_reader = em_ana.extraction.maus_reader( sys.argv[1] )
while test_reader.next_event() :
  mc_event = test_reader.get_event( 'mc' )
  num_virtual_hits = mc_event.GetVirtualHitsSize()
  for i in range( num_virtual_hits ) :
    virt = mc_event.GetAVirtualHit( i )
    plane = virt.GetStationId()
    if plane >= len( virtual_plane_locations ) :
      while plane >= len( virtual_plane_locations ) :
        virtual_plane_locations.append( 0.0 )

    virtual_plane_locations[virt.GetStationId()] = virt.GetPosition().z()

number_planes = len( virtual_plane_locations )

for pid in hist_pids :
  virtual_hits_dict[pid] = array.array('d')
  virtual_hists_dict[pid] = []
  for j in range( number_planes ) :
    virtual_hists_dict[pid].append( ROOT.TH1F( "momentum_pid="+str(pid)+"_plane="+str(j), "pid="+str(pid)+"_plane="+str(j), hist_momentum_numBins, hist_momentum_min, hist_momentum_max ) )
    virtual_hits_dict[pid].append( 0.0 )
  
file_reader = em_ana.extraction.maus_reader( sys.argv[1:] )

print "Loading", file_reader.get_number_files(), "Files"
print "Found", number_planes, "Virtual Planes"
print


counter = 0
while file_reader.next_event() :
  mc_event = file_reader.get_event( 'mc' )
  num_virtual_hits = mc_event.GetVirtualHitsSize()

  for i in range( num_virtual_hits ) :
    virt = mc_event.GetAVirtualHit( i )

    position = virt.GetPosition()
    pid = virt.GetParticleId()

    if pid not in hist_pids :
      print "PID,", pid, "Not Found. Skipping..."

    if position.x()**2 + position.y()**2 > cut_radius**2 : continue

    plane = virt.GetStationId()

## Check Counters
#    if pid not in virtual_hits_dict :
#      virtual_hits_dict[pid] = array.array('d')
#      for j in range( number_planes ) :
#        virtual_hits_dict[pid].append( 0.0 )
#
## Check Histograms
#    if pid not in virtual_hists_dict :
#      print "HERE"
#      virtual_hists_dict[pid] = []
#      for j in range( number_planes ) :
#        virtual_hists_dict[pid].append( ROOT.TH1F( "pid="+str(pid)+"_plane="+str(j), "pid="+str(pid)+"_plane="+str(j), hist_momentum_numBins, hist_momentum_min, hist_momentum_max ) )

    try :
      virtual_hits_dict[pid][plane] += 1
      virtual_hists_dict[pid][plane].Fill( virt.GetMomentum().mag() )
    except :
      print virtual_hists_dict
      print
      print pid, plane
      raise

  counter += 1

#print virtual_hits_dict
#print virtual_plane_locations

print "Found Virtuals:"
for plane_id in range( number_planes ) :
  print virtual_plane_locations[plane_id],

  for pid, hit_hist in virtual_hits_dict.iteritems() :
    print pid, ":", hit_hist[plane_id],

  print


outfile = ROOT.TFile( "dump_virtual_planes.root", "RECREATE" )
for pid, hit_hist in virtual_hits_dict.iteritems() :
  graph = ROOT.TGraph( number_planes, virtual_plane_locations, hit_hist )
  graph.Write( "virtuals_"+str(pid) )

for pid, hit_hist in virtual_hists_dict.iteritems() :
  for j in range( number_planes ) :
    hit_hist[j].Write()

outfile.Close()


