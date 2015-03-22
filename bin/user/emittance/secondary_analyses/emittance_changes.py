#!/usr/bin/env python

# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
#

import sys
import ROOT


def run() :
  if len( sys.argv ) <= 1 :
    print "No File Supplied."
    sys.exit( 0 )

  print


  mcEmitIn = []
  mcEmitOut = []
  mcEmitDelta = []

  emitIn = []
  emitOut = []
  emitInError = []
  emitOutError = []
  emitFrac = []
  emitFracError = []
  emitDelta = []
  emitaDeltaError = []

  for dirnum in range( 1, len( sys.argv ) ) :
    sys.stdout.write( 'Loading File ' + str( dirnum ) + ' of ' + str( len( sys.argv ) - 1 ) + '        \r' )

    errorsFile = ROOT.TFile.Open( sys.argv[dirnum]+'/measure_emittance_errors.root', "READ" )

    upstreamEmittanceHist = infile.Get( 'emittance_mc_upstream' )
    downstreamEmittanceHist = infile.Get( 'emittance_mc_downstream' )
    fractionalHist = infile.Get( 'emittance_mc_fractional_change' )
    deltaHist = infile.Get( 'delta_mc_emittance_change' )






if __name__ == "__main__" :
  run()

