/* 
** Written for the MICE proposal simulation
** Ed McKigney - Aug 21 2002
*/

#include <stdlib.h>
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandExponential.h"
#include "CLHEP/Random/RandPoisson.h"
#include <stdio.h>
#include <fstream>
#include "SciFiDigit.hh"
#include "SciFiHit.hh"
#include "VlpcHit.hh"

SciFiDigit::SciFiDigit()
{
  miceMemory.addNew( Memory::SciFiDigit );
  m_used = false;
}

SciFiDigit::SciFiDigit( SciFiHit* hit, int adc, int tdc )
{
  miceMemory.addNew( Memory::SciFiDigit );

  trackerNo = hit->GetTrackerNo();
  stationNo = hit->GetStationNo();
  planeNo = hit->GetPlaneNo();
  fiberNo = hit->GetFiberNo();
  adcCounts = adc;
  tdcCounts = tdc;
  m_mc_hit = hit;
  m_vlpc_hit = NULL;
  m_used = false;
}

SciFiDigit::SciFiDigit( VlpcHit* hit, int stat, int plane, int fib, int adc, double pe )
{
  miceMemory.addNew( Memory::SciFiDigit );

  trackerNo = 0;	//ME - note this hard coded number, we need to fix it before the second tracker arrives!!!!!!
  stationNo = stat;
  planeNo = plane;
  fiberNo = fib;
  adcCounts = adc;
  numPE = pe;
  tdcCounts = (int) hit->tdc();
  m_vlpc_hit = hit;
  m_mc_hit = NULL;
  m_used = false;
}

SciFiDigit::SciFiDigit( const SciFiDigit& rhs )
{
  miceMemory.addNew( Memory::SciFiDigit );

  trackerNo = rhs.trackerNo;
  stationNo = rhs.stationNo;
  planeNo = rhs.planeNo;
  fiberNo = rhs.fiberNo;
  adcCounts = rhs.adcCounts;
  tdcCounts = rhs.tdcCounts;
  m_mc_hit = rhs.m_mc_hit;
  m_vlpc_hit = rhs.m_vlpc_hit;
  m_used = rhs.m_used;
}

bool SciFiDigit::mergeDigit( SciFiDigit* digi )
{
  bool merged = false;

  // first check that these should be merged!

  if( m_mc_hit && digi->mcHit() )
    if( trackerNo == digi->getTrackerNo() &&
        stationNo == digi->getStationNo() &&
        planeNo == digi->getPlaneNo() &&
        fiberNo == digi->getChanNo() )
    {
      // it is OK to merge these digits

      adcCounts += digi->getAdcCounts();
      numPE += digi->getNPE();

      if( digi->getTdcCounts() < tdcCounts )
      {
        tdcCounts = digi->getTdcCounts();
        m_mc_hit = digi->mcHit();
      }

      merged = true;
    }

  return merged;
}
