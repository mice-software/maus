// SciFiSpacePoint.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_SciFiSpacePointTEXTFIILEIO_H
#define PERSIST_SciFiSpacePointTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Recon/SciFi/SciFiSpacePoint.hh"
#include "Interface/Memory.hh"

class SciFiDoubletClusterTextFileIo;

class SciFiSpacePointTextFileIo : public TextFileIoBase
{
  public :

    SciFiSpacePointTextFileIo( SciFiSpacePoint*, int );
    SciFiSpacePointTextFileIo( std::string );

    virtual ~SciFiSpacePointTextFileIo() { miceMemory.addDelete( Memory::SciFiSpacePointTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    SciFiSpacePoint*		thePoint() const { return m_point; };

    SciFiSpacePoint*		makeSciFiSpacePoint();

    void		completeRestoration();

    void setSciFiDoubletClusters(std::vector<SciFiDoubletClusterTextFileIo*>& clusters) {m_doubletClusters = & clusters;};

  private :

  SciFiSpacePoint*     m_point;

  CLHEP::Hep3Vector pos;
  CLHEP::Hep3Vector posE;
  double time, timeE;

  int nChans, nMuChans;

  double nPE, chi2;

  int stationNo, trackerNo;

  bool m_used;

  SciFiDoubletCluster* m_firstCluster;
  SciFiDoubletCluster* m_secondCluster;
  SciFiDoubletCluster* m_thirdCluster;
  int m_numClusters;

  int m_first_index, m_second_index, m_third_index;

  std::vector<SciFiDoubletClusterTextFileIo*>* m_doubletClusters;

};

#endif

