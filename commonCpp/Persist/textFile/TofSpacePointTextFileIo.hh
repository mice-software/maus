// TofSpacePoint.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_TofSpacePointTEXTFILEIO_H
#define PERSIST_TofSpacePointTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "Recon/TOF/TofSpacePoint.hh"
#include "TofSlabHitTextFileIo.hh"
#include "Interface/Memory.hh"

class TofSpacePointTextFileIo : public TextFileIoBase
{
  public :

    TofSpacePointTextFileIo( TofSpacePoint*, int );
    TofSpacePointTextFileIo( std::string );

    virtual ~TofSpacePointTextFileIo() { miceMemory.addDelete( Memory::TofSpacePointTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    TofSpacePoint*		theHit() const { return m_hit; };

    TofSpacePoint*		makeTofSpacePoint();

    void		completeRestoration();

	 void    setTofSlabHits(std::vector<TofSlabHitTextFileIo*>& hits)     {m_tofSlabHits = & hits;};

  private :

    TofSpacePoint*		m_hit;

	int  		 	_station;
	Hep3Vector 		_pos;
	double			_time;
	bool			m_good;



		//Indices for the digit member data
		//(these also are written in the file)

    int m_slabA_index;
    int m_slabB_index;

		//Pointer to vectors of all Io objects of the relevant type
		//Set in the set_____ methods above by TextFileWriter/Reader

    std::vector<TofSlabHitTextFileIo*>* m_tofSlabHits;


		//SpacePoint member data pointers
		//These are converted to BlahTextFileIo.blah_index and stored above
		//The index is written to file

    TofSlabHit* m_slabHit_A;
    TofSlabHit* m_slabHit_B;

};

#endif

