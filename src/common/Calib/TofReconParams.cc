//---------------------------------------------------------------------------
//
//    File :          TofReconParams.cpp
//     Purpose :   Supply parameters for the TOF  reconstruction
//     Created :   10-DEC-2002   by Steve Kahn
//
//----------------------------------------------------------------------------

#include "Calib/TofReconParams.hh"
#include "Interface/dataCards.hh"
#include "CLHEP/Units/SystemOfUnits.h"

ThreeVector TofReconParams::getTofPosition(int ista) const
{   if( ista == 1) return _tof1Position;
	else if( ista == 2) return _tof2Position;
	else if( ista == 3) return _tof3Position;
	return _tof2Position;
}

int TofReconParams::getNumberSegments(int ista) const
{	if( ista == 1) return _numberSegments1;
	else if( ista == 2) return  _numberSegments2;
	else if( ista == 3) return  _numberSegments3;
	return _numberSegments2; 
}

TofReconParams* TofReconParams::_instance = (TofReconParams*) NULL;

TofReconParams* TofReconParams::getInstance()
{	if(!_instance) _instance = new TofReconParams;
	return _instance;
}

double TofReconParams::getTofSize(int ista) const
{   if( ista == 1) return _tof1Size;
	else if( ista == 2) return _tof2Size;
	else if( ista == 3) return _tof3Size;
	return _tof2Size;
}

TofReconParams::TofReconParams()
{
	_tof1Size = MyDataCards.fetchValueDouble("TOF1Size")*CLHEP::mm;
	_tof2Size = MyDataCards.fetchValueDouble("TOF2Size")*CLHEP::mm;
	_tof3Size = MyDataCards.fetchValueDouble("TOF3Size")*CLHEP::mm;

  _numberSegments1 =  MyDataCards.fetchValueInt("TOF1numbSegments");
  _numberSegments2 =  MyDataCards.fetchValueInt("TOF2numbSegments");
  _numberSegments3 =  MyDataCards.fetchValueInt("TOF3numbSegments");

  _timeCoincidence = MyDataCards.fetchValueDouble("TOFtimeCoincidence");

  double zpos = MyDataCards.fetchValueDouble("TOF1Position")*CLHEP::mm;
  _tof1Position = ThreeVector(0., 0., zpos);
  zpos = MyDataCards.fetchValueDouble("TOF2Position")*CLHEP::mm;
  _tof2Position = ThreeVector(0., 0., zpos);
  zpos = MyDataCards.fetchValueDouble("TOF3Position")*CLHEP::mm;
  _tof3Position = ThreeVector(0., 0., zpos);
}

double TofReconParams::tdcTimeCoincidence() const
{  return _timeCoincidence;  }
