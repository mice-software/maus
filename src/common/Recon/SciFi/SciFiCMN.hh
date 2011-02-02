/* 
SciFiCMN

M.Ellis - February 2006
*/

#ifndef SCIFICMN_HH
#define SCIFICMN_HH

#include "Interface/MICEEvent.hh"
#include "Config/VlpcCable.hh"
#include "Calib/VlpcCalib.hh"

void SciFiCMN( struct MICEEvent&, VlpcCable*, VlpcCalib* );

static const int CHANSCMN = 32;
static const int NUMBINS = 512 / CHANSCMN;

class CmnCount
{
  public :

    CmnCount( int );

    ~CmnCount()					{};

    int		board() const			{ return _board; };

    double	ped( int chan ) const;

    void	calculatePedestals();

    void	addChannel( int, double );

  private :

    int _board;

    double		_ped[NUMBINS];

    std::vector<double>  _peds[NUMBINS];
};

#endif

