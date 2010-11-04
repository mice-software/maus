// SciFiDoubletClusterTextfileIo.hh
//
// M.Ellis 22/8/2005


#ifndef PERSIST_SCIFIDOUBLETCLUSTERTEXTFILEIO_H
#define PERSIST_SCIFIDOUBLETCLUSTERTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/SciFiHit.hh"
#include "Recon/SciFi/SciFiDoubletCluster.hh"
#include "Interface/Memory.hh"

class SciFiDigitTextFileIo;

class SciFiDoubletClusterTextFileIo : public TextFileIoBase
{
  public :

    SciFiDoubletClusterTextFileIo( SciFiDoubletCluster*, int );
    SciFiDoubletClusterTextFileIo( std::string );

    virtual ~SciFiDoubletClusterTextFileIo() { miceMemory.addDelete( Memory::SciFiDoubletClusterTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    SciFiDoubletCluster*	theCluster() const { return m_cluster; };

    SciFiDoubletCluster*	makeSciFiDoubletCluster();

    void		completeRestoration();

    void		setSciFiDigits( std::vector<SciFiDigitTextFileIo*>& );

  private :

    SciFiDoubletCluster*		m_cluster;

    SciFiDigit*				seed;
    SciFiDigit*				neighbour;
    std::vector<SciFiDigit*>		digits;
    double				aveChanNoWeighted;
    double				aveChanNoUnweighted;
    int					lowerChanNo;
    int					upperChanNo;
    int					doubletNo;
    int					stationNo;
    int					trackerNo;
    double				nMu;
    double				time;
    double				nPE;
    Hep3Vector				truePos;
    Hep3Vector				trueMom;
    bool				m_used;
    Hep3Vector				pos;
    Hep3Vector				dir;
   
    int					seed_index;
    int					neighbour_index;
    std::vector<int>			digits_indices;

    std::vector<SciFiDigitTextFileIo*>*	m_digits;
};

#endif

