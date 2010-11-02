// VlpcHit.cc
//
// A description of a recorded hit on a VLPC
//
// M.Ellis 22/10/2003
// Imported to G4MICE April 2005

#include "VlpcHit.hh"

void	clearVlpcHitVector( VlpcHitVector& vect )
{
  for( unsigned int i = 0; i < vect.size(); ++i )
    delete vect[i];
  vect.clear();
}

