/* 
SciFiCMN

M.Ellis - February 2006
*/

#include <cmath>

#include "Interface/VlpcHit.hh"
#include "Recon/SciFi/SciFiCMN.hh"

CmnCount::CmnCount( int board ) : _board( board )
{
  for( int i = 0; i < NUMBINS; ++i )
    _ped[i] = 0.;
}

void	CmnCount::calculatePedestals()
{
  for( int bin = 0; bin < NUMBINS; ++bin )
  {
    double low1 = 1e8;
    double low2 = 1e9;
    double max1 = -1e9;
    double max2 = -1e8;
    double sum = 0.;
    double count = 0.;

    for( unsigned int i = 0; i < _peds[bin].size(); ++i )
    {
      double adc = _peds[bin][i];
      sum += adc;
      ++count;

      if( adc < low2 )
      {
        low1 = low2;
        low2 = adc;
      }
      else if( adc < low1 )
       low1= adc;
      else if( adc > max2 )
      {
        max1 = max2;
        max2 = adc;
      }
      else if( adc > max1 )
        max1 = adc;
    }

    if( count > 8 )
    {
      // remove lowest two and highest two counts and calculate average
      sum -= ( low1 + low2 + max1 + max2 );
      sum /= ( count - 4 );
    }
    else
      sum = 0;

    _ped[bin] = sum;
  }
}

double	CmnCount::ped( int chan ) const
{
  int bin = chan / CHANSCMN;

  if( bin >= 0 && bin < NUMBINS )
    return _ped[bin];
  else
    return 0.0;
}

void	CmnCount::addChannel( int chan, double adc )
{
  int bin = chan / CHANSCMN;

  if( bin >= 0 && bin < NUMBINS )
  {
    _peds[ bin ].push_back( (double) adc );
  }
}

void SciFiCMN( MICEEvent& event, VlpcCable* cable, VlpcCalib* calib )
{
  // make a list of all boards

  std::vector<CmnCount> cmn;

  for( int cas = 0; cas < cable->cassettes(); ++cas )
  {
    cmn.push_back( CmnCount( cable->LeftHandBoard( cas ) ) );
    cmn.push_back( CmnCount( cable->RightHandBoard( cas ) ) );
  }

  // accumulate CMN shifts

  for( unsigned int i = 0; i < event.vlpcHits.size(); ++i )
  {
    int board = event.vlpcHits[i]->afe();
    int mcm = event.vlpcHits[i]->mcm();
    int chan = event.vlpcHits[i]->chan();
    double diff = event.vlpcHits[i]->adc() - calib->pedestal( board, mcm, chan );

    CmnCount* mycmn = NULL;

    for( unsigned int i = 0; i < cmn.size(); ++i )
      if( cmn[i].board() == board )
        mycmn = & cmn[i];

    if( mycmn )
      mycmn->addChannel( ( mcm - 1 ) * 64 + chan - 1, diff );
  }

  // calculate the common noise

  for( unsigned int i = 0; i < cmn.size(); ++i )
    cmn[i].calculatePedestals();

  // apply the change to each channel

  for( unsigned int i = 0; i < event.vlpcHits.size(); ++i )
  {
    int board = event.vlpcHits[i]->afe();
 
    CmnCount* mycmn = NULL;

    for( unsigned int j = 0; j < cmn.size(); ++j )
      if( cmn[j].board() == board )
        mycmn = & cmn[j];
 
    if( mycmn )
      event.vlpcHits[i]->removeNoise( mycmn->ped( ( event.vlpcHits[i]->mcm() - 1 ) * 64 + event.vlpcHits[i]->chan() - 1 ) );
  }
}

