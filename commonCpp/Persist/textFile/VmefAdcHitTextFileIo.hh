// VmefAdcHit.hh
//
// Y.Karadzhov  April 2008


#ifndef PERSIST_VmefAdcHitTEXTFIILEIO_H
#define PERSIST_VmefAdcHitTEXTFILEIO_H

#include "TextFileIoBase.hh"


#include "Interface/VmefAdcHit.hh"

#include "Interface/Memory.hh"

class VmefAdcHitTextFileIo : public TextFileIoBase
{
  public :

    VmefAdcHitTextFileIo( VmefAdcHit*, int );
    VmefAdcHitTextFileIo( std::string );

    virtual ~VmefAdcHitTextFileIo() { miceMemory.addDelete( Memory::VmefAdcHitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    VmefAdcHit*		theHit() const { return hit; };

    VmefAdcHit*		makeVmefAdcHit();

    void		completeRestoration();

  private :

    VmefAdcHit*       hit;
    int               crate;
    int               board;
    std::string       module_type;
    int               channel;
    std::vector<int>  data;
    unsigned int      dataSize;
    // ***** variables go here ******
};

#endif

