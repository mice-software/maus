/* 
SciFiDigitRec 

M.Ellis - August 2005
*/

#include "Interface/MICEEvent.hh"
#include "Interface/VlpcHit.hh"
#include "Interface/SciFiDigit.hh"

#include "SciFiDigitRec.hh"

void SciFiDigitRec( struct MICEEvent& event, VlpcCalib* calib, VlpcCable* cable )
{
  for( unsigned int i = 0; i < event.vlpcHits.size(); ++i )
  {
    int stat, plane, fib, adc;
    double pe, ped, gain;

    VlpcHit* hit = event.vlpcHits[i];

    int board = hit->afe();
    int mcm = hit->mcm();
    int chan = hit->chan();

    cable->statPlanFib( board, mcm, chan, stat, plane, fib );

    adc = (int) hit->adc();
    ped = calib->pedestal( board, mcm, chan );
    gain = calib->gain( board, mcm, chan );

    if( ped > 0. && gain > 0. )
      pe = ( hit->adc() - ped ) / gain;
    else
      pe = -10.0;

    if( stat >= 0 && plane >= 0 && fib >= 0 && ped > 0. && gain > 0. && pe > 1.0 )
    {
      SciFiDigit* digi = new SciFiDigit( hit, stat, plane, fib-1, adc, pe );
      event.sciFiDigits.push_back( digi );
    }
  }
}

