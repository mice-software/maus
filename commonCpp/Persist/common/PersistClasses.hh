// PersistClasses.hh
//
// M.Ellis 22/8/2005

#ifndef PERSIST_PERSISTCLASSES_H
#define PERSIST_PERSISTCLASSES_H

#include <string>
#include <vector>
#include <map>

#include "Interface/Memory.hh" 

class PersistClasses
{
  public :

    PersistClasses();

    PersistClasses( PersistClasses& );

    PersistClasses( bool );

    ~PersistClasses()   { miceMemory.addDelete( Memory::PersistClasses ); };
    
    void  setClassProcessing( std::string );

    void  setClassProcessing( std::string, bool );

    bool  processClass( std::string );

    void  processNone();
 
    void  processAll();

    void  dumpProcessing();

    static const std::string VlpcHit;
    static const std::string VlpcEventTime;
    static const std::string CkovHit;
    static const std::string CkovDigit;
    static const std::string SciFiHit;
    static const std::string SciFiDigit;
    static const std::string SciFiDoubletCluster;
    static const std::string SciFiSpacePoint;
    static const std::string SciFiTrack;
    static const std::string SciFiKalTrack;
    static const std::string SciFiExtrap;
    static const std::string TofHit;
    static const std::string TofDigit;
    static const std::string TofSlabHit;
    static const std::string TofSpacePoint;
    static const std::string VmeAdcHit;
    static const std::string VmefAdcHit;
    static const std::string VmeTdcHit;
    static const std::string VmeScalerData;
    static const std::string EmCalHit;
    static const std::string EmCalDigit;
    static const std::string EmCalTrack;
    static const std::string KLHit;
    static const std::string KLDigit;
    static const std::string KLCellHit;
    static const std::string VirtualHit;
    static const std::string SpecialHit;
    static const std::string MCParticle;
    static const std::string MCVertex;
    static const std::string ZustandVektor;
    static const std::string RFData;
    static const std::string KillHit;
    static const std::string PrimaryGenHit;

  private :

    std::map<std::string, bool, std::less<std::string> >  processing;

    std::vector<std::string>  classes;
    
    void    fillClasses();

    bool    classKnown( std::string );
};

#endif

