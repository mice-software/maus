/* 
SciFiNtupleFill - take reconstructed information for the SciFi from the MICE Event and fill the NTuple

M. Ellis - April, 2005
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include "CLHEP/Vector/ThreeVector.h"
#include "Recon/SciFi/SciFiRec.hh"
#include "Interface/SciFiDigit.hh"
#include "Calib/SciFiReconstructionParams.hh"
#include "Recon/SciFi/SciFiTrack.hh"
#include "Recon/SciFi/SciFiKalTrack.hh"
#include "Recon/SciFi/SciFiSpacePoint.hh"
#include "Recon/SciFi/SciFiDoubletCluster.hh"
#include "Interface/dataCards.hh"
using std::vector;

#include "Interface/MICEEvent.hh"

void SciFiNtupleFill( struct MICEEvent&, RecSixDBank&, int&, int& );
