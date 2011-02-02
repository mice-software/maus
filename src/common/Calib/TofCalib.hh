// TofCalib.hh
//
// Class to contain the calibration of the ADC/TDC readout for the TOF Counter
//
// M.Ellis 5th October 2005

#ifndef TOFCALIB_H
#define TOFCALIB_H

#include <vector>
#include <string>
#include <cstdlib>
#include <sstream>

class TofCalib
{
  public :

    TofCalib( std::string );

    double			ped( int station, int plane, int slabNum, int pmtNum ) const;

    double			gain( int station, int plane, int slabNum, int pmtNum ) const;
   
    std::vector<double>		timeWalk( int station, int plane, int slabNum, int pmtNum ) const;

    double			tdc2time( int station, int plane, int slabNum, int pmtNum ) const;

  private :

    int			index( int station, int plane, int slabNum, int pmtNum ) const;

    int					_numChannels;
    std::vector<double>			_station;
    std::vector<double>			_plane;
    std::vector<double>			_slabNum;
    std::vector<double>			_pmtNum;
    std::vector<double>			_ped;
    std::vector<double>			_gain;
    std::vector<double>			_tdc2time;
    std::vector< std::vector<double> >	_timeWalk;
};
#endif
