#ifndef _TOFRECONPARAMS_HPP_
#define _TOFRECONPARAMS_HPP_ 1
//---------------------------------------------------------------------------
//
//    File :          TofReconParams.hpp
//     Purpose :   Supply parameters for the TOF  reconstruction
//     Created :   10-DEC-2002   by Steve Kahn
//
//----------------------------------------------------------------------------

#include "Calib/TofDigitizationParams.hh"
#include "CLHEP/Vector/ThreeVector.h"

typedef CLHEP::Hep3Vector ThreeVector;

class TofReconParams : public TofDigitizationParams
{
   private:
	   double  _tof1Size, _tof2Size, _tof3Size;
	   int _numberSegments1, _numberSegments2, _numberSegments3;
	   ThreeVector  _tof1Position, _tof2Position, _tof3Position;
	   static TofReconParams* _instance;
	   double _timeCoincidence;

   public:
	   TofReconParams();
	   static TofReconParams* getInstance();
	   double getTofSize(int) const;
	   ThreeVector  getTofPosition(int) const;
	   int  getNumberSegments(int) const;
	   double  tdcTimeCoincidence() const;
};
#endif

