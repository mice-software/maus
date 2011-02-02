/*
** Modified example 1 from the GEANT4 distribution to simulate the
** MICE scintillating fiber tracker for the MICE proposal
** Ed McKigney - August 21, 2002
*/

#include <iostream>
#include <sstream>

#include "SciFiHit.hh"

//G4Allocator<SciFiHit> SciFiHitAllocator;

SciFiHit::SciFiHit(const SciFiHit& right)
{
  fiberNo   = right.fiberNo;
  planeNo   = right.planeNo;
  stationNo = right.stationNo;
  trackerNo = right.trackerNo;
}

const SciFiHit& SciFiHit::operator=(const SciFiHit& right)
{
  fiberNo   = right.fiberNo;
  planeNo   = right.planeNo;
  stationNo = right.stationNo;
  trackerNo = right.trackerNo;

  return *this;
}

int SciFiHit::operator==(const SciFiHit& right) const
{
  return 0;
}

void SciFiHit::Print()
{
}

void SciFiHit::mergeHit( const SciFiHit* hit )
{
  if( hit->GetTime() < GetTime() )
  {
    SetTime( hit->GetTime() );
    SetCharge( hit->GetCharge() );
    SetMass( hit->GetMass() );
    SetPos( hit->GetPos() );
    SetMom( hit->GetMom() );
    SetEnergy( hit->GetEnergy() );
    SetPID( hit->GetPID() );
  }

  SetEdep( GetEdep() + hit->GetEdep() );
}

