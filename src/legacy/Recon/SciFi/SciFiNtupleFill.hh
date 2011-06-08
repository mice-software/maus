/* 
SciFiNtupleFill - take reconstructed information for the SciFi from the MICE Event and fill the NTuple

M. Ellis - April, 2005
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include "CLHEP/Vector/ThreeVector.h"
#include "SciFiRec.hh"
#include "Interface/SciFiDigit.hh"
#include "SciFiReconstructionParams.hh"
#include "SciFiTrack.hh"
#include "SciFiKalTrack.hh"
#include "SciFiSpacePoint.hh"
#include "SciFiDoubletCluster.hh"
#include "Interface/dataCards.hh"
using std::vector;

#include "MICEEvent.hh"

void SciFiNtupleFill( struct MICEEvent&, RecSixDBank&, int&, int& );
