// VmeScalerDataTextFileIo.hh
//
// V.Verguilov 2 February 2009


#ifndef PERSIST_VmeScalerDataTEXTFIILEIO_H
#define PERSIST_VmeScalerDataTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Interface/VmeScalerData.hh"

#include "Interface/Memory.hh"

class VmeScalerDataTextFileIo : public TextFileIoBase
{
  public :

    VmeScalerDataTextFileIo( VmeScalerData*, int );
    VmeScalerDataTextFileIo( std::string );

    virtual ~VmeScalerDataTextFileIo() { miceMemory.addDelete( Memory::VmeScalerDataTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    VmeScalerData*		theHit() const { return m_scaler; };

    VmeScalerData*		makeVmeScalerData();

    void		completeRestoration();

  private :

    VmeScalerData*		m_scaler;
    int 		m_crate;
    int			m_board;
    std::string		m_module_type;
    int			m_channel;
    int	    m_data;
};

#endif

