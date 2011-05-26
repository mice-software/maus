// SciFiFieldMap.cc - field map description for the Sci FI tracker used by the Kalman package

#include "Recon/SciFi/SciFiFieldMap.hh"

SciFiFieldMap::SciFiFieldMap( BTFieldConstructor* bt )
{
  _bt = bt;
}

EVector 	SciFiFieldMap::compute_vector( const EVector& pos ) const
{
  double EMField[6];
  double bpos[4]; 

  bpos[0] = pos[0];
  bpos[1] = pos[1];
  bpos[2] = pos[2];
  bpos[3] = 0.;

  _bt->GetMagneticField()->GetFieldValue( bpos, EMField );

  EVector B( 3, 0 );
  B[0] = EMField[0];
  B[1] = EMField[1];
  B[2] = EMField[2];

  return B;
}
