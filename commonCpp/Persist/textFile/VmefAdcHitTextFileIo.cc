// VmefAdcHitTextFileIo.cc
//
// Y.Karadzhov  April 2008

#include <sstream>

#include "VmefAdcHitTextFileIo.hh"

VmefAdcHitTextFileIo::VmefAdcHitTextFileIo( VmefAdcHit* h, int index ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::VmefAdcHitTextFileIo ); 
	
  setTxtIoIndex( index );
  hit = h;
}

VmefAdcHitTextFileIo::VmefAdcHitTextFileIo( std::string def ) : TextFileIoBase()
{
  miceMemory.addNew( Memory::VmefAdcHitTextFileIo ); 
  
  restoreObject( def );
}

std::string	VmefAdcHitTextFileIo::storeObject()
{
  // fill the line with the information about the class here, don't forget the index!
    crate = hit->crate();
    board = hit->board();
    module_type = hit->moduleType();
    channel = hit->channel();

    data = hit->GetData();
    dataSize = data.size();

    std::stringstream ss;
    ss << txtIoIndex() <<" "<< crate <<" "<< board <<" "<< channel <<" "<< module_type.c_str()<<" "<<dataSize<<" ";
    for(unsigned int i=0;i<data.size();i++) ss << data[i] <<" ";
    return( ss.str() );
}

void		VmefAdcHitTextFileIo::restoreObject( std::string def )
{
    int myindex;
    std::istringstream ist( def.c_str() );
    int point;
    ist >> myindex >> crate >> board >> channel >> module_type >> dataSize;
//std::cout<<"tuka sum restoreObject"<<dataSize<<std::endl;
    for(unsigned int i=0;i<dataSize;i++)
    {
        ist >> point; 
        data.push_back(point);
    }
    setTxtIoIndex( myindex );
}

VmefAdcHit*	VmefAdcHitTextFileIo::makeVmefAdcHit()
{

    hit = new VmefAdcHit(crate, board, module_type, channel, data);

    return hit;
}

void		VmefAdcHitTextFileIo::completeRestoration()
{}


