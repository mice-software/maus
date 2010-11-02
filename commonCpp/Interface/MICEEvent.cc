// @(#) $Id: MICEEvent.cc,v 1.12 2010-03-08 16:39:02 rogers Exp $ $Name:  $
// Definition of struct MICEEvent
// Struct to encapsulate hits/digits/recon in a G4Mice event


#include "MICEEvent.hh"

MICEEvent::MICEEvent()
{
    vlpcHits.resize(0);
    vlpcEventTime = NULL;
    ckovHits.resize(0);
    ckovDigits.resize(0);
    emrHits.resize(0);
    emrDigits.resize(0);
    klHits.resize(0);
    klDigits.resize(0);
    klCellHits.resize(0);
    sciFiHits.resize(0);
    sciFiDigits.resize(0);
    sciFiClusters.resize(0);
    sciFiSpacePoints.resize(0);
    sciFiTracks.resize(0);
    sciFiKalTracks.resize(0);
    sciFiExtraps.resize(0);
    tofHits.resize(0);
    tofDigits.resize(0);
    tofSlabHits.resize(0);
    tofSpacePoints.resize(0);
    vmeAdcHits.resize(0);
    vmefAdcHits.resize(0);
    vmeTdcHits.resize(0);
    emCalHits.resize(0);
    emCalDigits.resize(0);
    emCalTracks.resize(0);
    virtualHits.resize(0);
    specialHits.resize(0);
    mcParticles.resize(0);
    mcVertices.resize(0);
    mcHits.resize(0);
    zustandVektors.resize(0);
    rfData.resize(0);
    stagePosition = NULL;
    killHits.resize(0);
    pgHits.resize(0);
}

//Handy method for quickly seeing what's in a MICEEvent M.Rayner
void MICEEvent::Print() {
    std::cout << "This event contains:" << std::endl;

    std::cout << " - VLPC.......... ";
    std::cout << vlpcHits.size() << " hits";

    std::cout << " - Tracker....... ";
    std::cout << sciFiDigits.size() << " digits, ";
    std::cout << sciFiClusters.size() << " clusters, ";
    std::cout << sciFiSpacePoints.size() << " space points, ";
    std::cout << sciFiTracks.size() << " tracks, ";
    std::cout << sciFiKalTracks.size() << " Kalman tracks, ";
    std::cout << sciFiExtraps.size() << " extrapolations" << std::endl;

    std::cout << " - VME........... ";
    std::cout << vmeTdcHits.size() << " TDC hits, ";
    std::cout << vmeAdcHits.size() << " ADC hits, ";
    std::cout << vmefAdcHits.size() << " fADC hits" << std::endl;

    std::cout << " - TOF........... ";
    std::cout << tofDigits.size() << " digits, ";
    std::cout << tofSlabHits.size() << " slab hits, ";
    std::cout << tofSpacePoints.size() << " space points, ";
    std::cout << tofTracks.size() << " tracks" << std::endl;

    std::cout << " - Cherenkov..... ";
    std::cout << ckovDigits.size() << " digits" << std::endl;

    std::cout << " - EmCal......... ";
    std::cout << emCalDigits.size() << " digits, ";
    std::cout << emCalTracks.size() << " tracks" << std::endl;

    std::cout << " - KL......... ";
    std::cout << klHits.size() << " hits, ";
    std::cout << klDigits.size() << " digits, ";
    std::cout << klCellHits.size() << " cell hits, ";

    std::cout << " - EMR......... ";
    std::cout << emrHits.size() << " hits, ";
    std::cout << emrDigits.size() << " digits, ";

    std::cout << " - Monte Carlo... ";
    std::cout << sciFiHits.size() << " Tracker hits, ";
    std::cout << tofHits.size() << " TOF hits, ";
    std::cout << ckovHits.size() << " Cherenkov hits, ";
    std::cout << emCalHits.size() << " EmCal hits" << std::endl;
    std::cout << pgHits.size()   << " PG hits" << std::endl;
}
