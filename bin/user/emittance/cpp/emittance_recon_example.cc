/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <iostream>
#include <iomanip>

#include "Utils/EventIterator.hh"

#include "Recon/EmittanceRecon/EmittanceHit.hh"
#include "Recon/EmittanceRecon/EmittanceHitCut.hh"
#include "Recon/EmittanceRecon/EmittanceHitFinder.hh"
#include "Recon/EmittanceRecon/EmittanceCovarianceCalculator.hh"


int main( int argn, char** argv )
{
  Squeak::setStandardOutputs( Squeak::debug ); //IRStream uses Squeak! This gives back std::cout.

//////////////////////////////////////////////////
  //
  // Say Hello!
  //
//////////////////////////////////////////////////


  std::cout << "Emittance Reconstruction Example.\n";
  std::cout << "A brief exmaple to test the back end code\n";

  std::cout << std::endl;


//////////////////////////////////////////////////
  //
  // Check the arguments
  //
//////////////////////////////////////////////////

  if ( argn != 2 )
  {
    std::cout << "Incorrect Number of Arguments.\n";
    std::cout << "Please provide the name of a single ROOT File\n";
    return 0;
  }

  std::string inputFileName( argv[1] );

  std::cout << "Loading Data from file: " << inputFileName << "\n";

//////////////////////////////////////////////////
  // 
  // Load and Inspect Data
  //
//////////////////////////////////////////////////

  MAUS::EventIterator events( inputFileName );


  MAUS::EmittanceRecon::covarianceCalculator calc_upstream;
  MAUS::EmittanceRecon::covarianceCalculator calc_downstream;


  MAUS::EmittanceRecon::hitFinder finder;
  finder.setHitMatchSensitivity( 10.0 );
  finder.setCutTarget( MAUS::EmittanceRecon::hitFinder::cut_virtual_recon );
  finder.addReconCut( new MAUS::EmittanceRecon::cut_int( MAUS::EmittanceRecon::cut_base::plane_number, MAUS::EmittanceRecon::cut_base::not_equal_to, 0 ) );
  finder.addReconCut( new MAUS::EmittanceRecon::cut_int( MAUS::EmittanceRecon::cut_base::station_number, MAUS::EmittanceRecon::cut_base::not_equal_to, 1 ) );
  finder.addReconCut( new MAUS::EmittanceRecon::cut_double( MAUS::EmittanceRecon::cut_base::trans_momentum, MAUS::EmittanceRecon::cut_base::greater_than, 150.0 ) );
  finder.addReconCut( new MAUS::EmittanceRecon::cut_double( MAUS::EmittanceRecon::cut_base::z_momentum, MAUS::EmittanceRecon::cut_base::greater_than, 300.0 ) );
  finder.addReconCut( new MAUS::EmittanceRecon::cut_double( MAUS::EmittanceRecon::cut_base::p_value, MAUS::EmittanceRecon::cut_base::less_equal, 0.05 ) );
  //finder.addVirtualCut( new MAUS::EmittanceRecon::cut_double( MAUS::EmittanceRecon::cut_base::trans_position, MAUS::EmittanceRecon::cut_base::greater_than, 189.0 ) );
  finder.addVirtualCut( new MAUS::EmittanceRecon::cut_int( MAUS::EmittanceRecon::cut_base::pid, MAUS::EmittanceRecon::cut_base::not_equal_to, -13 ) );


  MAUS::MCEvent* virtualEvent = NULL;
  MAUS::SciFiEvent* scifiEvent = NULL;

  std::cout << std::endl;

  unsigned int counter_missingTrack = 0;

  // while ( ( virtualEvent = events.nextEvent<MAUS::MCEvent>() ), ( virtualEvent != NULL ) ) 
  while ( events.next() ) 
  {
    virtualEvent = events.currentEvent< MAUS::MCEvent >();
    scifiEvent = events.currentEvent< MAUS::SciFiEvent >();

    if ( ( ! virtualEvent ) || ( ! scifiEvent ) )
    {
      std::cout << "HELP!\n";
      continue;
    }
    MAUS::SciFiTrackPArray* temp = new MAUS::SciFiTrackPArray( scifiEvent->scifitracks() );

    const MAUS::EmittanceRecon::hitPairListT& hitList = finder.findHits( temp, virtualEvent->GetVirtualHits() );

    if ( ( hitList.size() == 2 ) && ( hitList.begin()->first.getTrackerNumber() != hitList.rend()->first.getTrackerNumber() ) )
    {
      for ( MAUS::EmittanceRecon::hitPairListT::const_iterator it = hitList.begin(); it != hitList.end(); ++it )
      {
        if ( it->first.getTrackerNumber() == 0 )
        {
          calc_upstream.addHit( it->first, it->second );
        }
        else
        {
          calc_downstream.addHit( it->first, it->second );
        }
      }
      std::cout << '\n';
    }
    else ++counter_missingTrack;

  }
  std::cout << "\n\n" << std::endl;

  std::cout << "Read a total of " << events.getTotalNumberEvents() << std::endl;
  std::cout << "Some Numbers:\n" 
    << "Number Events Analysed       : " << events.getTotalNumberEvents() << '\n'
    << "Number Spills Analysed       : " << events.getNumberSpills() << '\n'
    << "Number of skipped events     : " << events.getNumberSkippedEvents() << '\n'
    << "Number of skipped spills     : " << events.getNumberSkippedSpills() << '\n'
    << "Virtual Hits Analysed        : " << finder.getNumberVirtualHitsAnalysed() << '\n'
    << "Recon Hits Analysed          : " << finder.getNumberReconHitsAnalysed() << '\n'
    << "Recon Tracks Analysed        : " << finder.getNumberReconTracksAnalysed() << '\n'
    << "Number of Hit Pairs Found    : " << finder.getNumberPairsFound() << '\n'
    << "Number Cut from Recon        : " << finder.getNumberCutReconHits() << '\n'
    << "Number Cut from Virtual Hits : " << finder.getNumberCutVirtualHits() << '\n'
    << "Number Cut Mutually          : " << finder.getNumberCutMutualHits() << '\n'
    << "Number Paired Hits           : " << finder.getNumberMatchedHits() << '\n'
    << "Number Un-Paired Hits        : " << finder.getNumberUnmatchedHits() << '\n'
    << "Number Straight Tracks       : " << finder.getNumberStraightTracks() << '\n';

  std::cout << "Found " << counter_missingTrack << " Incomplete Tracks\n";

  std::cout << "There are " << calc_upstream.getNumberHits() << " and " << calc_downstream.getNumberHits() << " Hits\n";
  std::cout << "\nUpstream Emittance:\nTrue          : " << std::setprecision(15) << calc_upstream.getTrueEmittance( MAUS::EmittanceRecon::covarianceCalculator::dim_4 )
                                 << "\nReconstructed : " << std::setprecision(15) << calc_upstream.getReconEmittance( MAUS::EmittanceRecon::covarianceCalculator::dim_4 )
                                 << "\nCorrected     : " << std::setprecision(15) << calc_upstream.getCorrectEmittance( MAUS::EmittanceRecon::covarianceCalculator::dim_4 )
        << "\n\nDownstream Emittance:\nTrue          : " << std::setprecision(15) << calc_downstream.getTrueEmittance( MAUS::EmittanceRecon::covarianceCalculator::dim_4 )
                                 << "\nReconstructed : " << std::setprecision(15) << calc_downstream.getReconEmittance( MAUS::EmittanceRecon::covarianceCalculator::dim_4 )
                                 << "\nCorrected     : " << std::setprecision(15) << calc_downstream.getCorrectEmittance( MAUS::EmittanceRecon::covarianceCalculator::dim_4 )
                                 << "\n" << std::endl;

  const MAUS::EmittanceRecon::cutListT& virtCuts = finder.getVirtualCuts();
  const MAUS::EmittanceRecon::cutListT& reconCuts = finder.getReconCuts();

  std::cout << "-----------------------------------------------------\n";
  std::cout << "Virtual Cut Results:\n\n";

  unsigned int counter = 1;
  for ( MAUS::EmittanceRecon::cutListT::const_iterator it = virtCuts.begin(); it != virtCuts.end(); ++it )
  {
    std::cout << "Cut : " << counter << " cut : " << it->second << '\n';
    ++counter;
  }

  std::cout << "\n\nRecon Cut Results:\n\n";
  counter = 1;
  for ( MAUS::EmittanceRecon::cutListT::const_iterator it = reconCuts.begin(); it != reconCuts.end(); ++it )
  {
    std::cout << "Cut : " << counter << " cut : " << it->second << '\n';
    ++counter;
  }

  std::cout << std::endl;

  return 0;
}

