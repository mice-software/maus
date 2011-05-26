// TofCalib.cc
//
// Class to contain the calibration of the ADC/TDC readout for the TOF counter
//
// A.Fish 5th October 2005

#include "Calib/TofCalib.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <sstream>
#include <fstream>

#include <iostream>

TofCalib::TofCalib( std::string file )
{
  // first get the enviroment variable MICEFILES as this is where the model files are kept

  char* MICEFILES = getenv( "MICEFILES" );

  // the full name of the input file:

  std::string fnam = std::string( MICEFILES ) + "/Calibration/" + file;

  std::ifstream inf( fnam.c_str() );

  std::string line, tmp;

  // read in the number of channels

  getline( inf, line );
  std::istringstream ist( line.c_str() );
  ist >> _numChannels;

  _station.resize( _numChannels );
  _plane.resize( _numChannels );
  _slabNum.resize( _numChannels );
  _pmtNum.resize( _numChannels );
  _ped.resize( _numChannels );
  _gain.resize( _numChannels );
  _tdc2time.resize( _numChannels );
  _timeWalk.resize( _numChannels );

  // read in the calibration

  for( int i = 0; i < _numChannels; ++i )
  {
    int station, plane, slabNum, pmtNum;
    double ped, gain, tdc2time;
    int numTW;
    
    std::string tmp;
    getline( inf, line );
    std::istringstream ist1( line.c_str() );

    ist1 >> station >> plane >> slabNum >> pmtNum >> ped >> gain >> tdc2time >> numTW;

    std::vector<double> tw( numTW );

    for( int j = 0; j < numTW; ++j )
    {
      double tmp;
      ist1 >> tmp;
      tw[j] = tmp * CLHEP::picosecond;
    }

    _station[i] = station;
    _plane[i] = plane;
    _slabNum[i] = slabNum;
    _pmtNum[i] = pmtNum;
    _ped[i] = ped;
    _gain[i] = gain;
    _tdc2time[i] = tdc2time * CLHEP::picosecond;
    _timeWalk[i] = tw;
  }

  inf.close();
}

int	TofCalib::index( int station, int plane, int slabNum, int pmtNum ) const
{
  int ret = -1;

  for( unsigned int i = 0; i < _station.size(); ++i )
    if( _station[i] == station && _plane[i] == plane && _slabNum[i] == slabNum && _pmtNum[i] == pmtNum ) 
      ret = i;

  return ret;
}

double	TofCalib::ped( int station, int plane, int slabNum, int pmtNum ) const
{
  int ret = index( station, plane, slabNum, pmtNum );

  if( ret >= 0 )
    return _ped[ret];
  else
    return -9.999e4;
}

double	TofCalib::gain( int station, int plane, int slabNum, int pmtNum ) const
{
  int ret = index( station, plane, slabNum, pmtNum );

  if( ret >= 0 )
    return _gain[ret];
  else
    return -9.999e4;
}

double	TofCalib::tdc2time( int station, int plane, int slabNum, int pmtNum ) const
{
  int ret = index( station, plane, slabNum, pmtNum );

  if( ret >= 0 )
    return _tdc2time[ret];
  else
    return -9.999e4;
}

std::vector<double>	TofCalib::timeWalk( int station, int plane, int slabNum, int pmtNum ) const
{
  int ret = index( station, plane, slabNum, pmtNum );

  if( ret >= 0 )
    return _timeWalk[ret];
  else
    return std::vector<double>(0);
}
