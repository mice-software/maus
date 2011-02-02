/* 
*/

#ifndef EMCALREC_HH
#define EMCALREC_HH

#include "CLHEP/Vector/ThreeVector.h"
#include <fstream>
#include <vector>

#include "Interface/MICEEvent.hh"
#include "Interface/MICERun.hh"

class EmCalTrack;
class TArrayD;

class EmCalRec
{
public:

  //! constructor
        EmCalRec( MICEEvent&, MICERun* );

  //! Default destructor
  ~EmCalRec();

  //! Perform the processing for a single event
  void Process();

private:
  MICEEvent&  theEvent;
  MICERun*  m_run;
//   std::vector< EmCalTrack* > m_tracks;

  std::vector< double > m_cellLength;
  int m_layers;
  double m_highLvlTriggerSW;
  double m_highLvlTriggerKL;

};

#endif
