// TofDigit.hh
//
// M.Ellis 27/8/2005


#ifndef PERSIST_TofDigitTEXTFIILEIO_H
#define PERSIST_TofDigitTEXTFILEIO_H

#include "TextFileIoBase.hh"
#include "TofHitTextFileIo.hh"
#include "Interface/TofDigit.hh"

#include "Interface/Memory.hh"

class TofSlabHitTextFileIo;
class VmeAdcHitTextFileIo;
class VmefAdcHitTextFileIo;
class VmeTdcHitTextFileIo;

class TofDigitTextFileIo : public TextFileIoBase
{
  public :

    TofDigitTextFileIo( TofDigit*, int );
    TofDigitTextFileIo( std::string );

    virtual ~TofDigitTextFileIo() { miceMemory.addDelete( Memory::TofDigitTextFileIo ); };

    std::string		storeObject();
    void		restoreObject( std::string );

    TofDigit*		theDigit() const { return m_digit; };

    TofDigit*		makeTofDigit();

    void		completeRestoration();

	void		setVmeAdcHits(std::vector<VmeAdcHitTextFileIo*>& hits)		{m_vmeAdcHits =  & hits;};
	void		setVmefAdcHits(std::vector<VmefAdcHitTextFileIo*>& hits)	{m_vmefAdcHits =  & hits;};	
	void		setVmeTdcHits(std::vector<VmeTdcHitTextFileIo*>& hits)		{m_vmeTdcHits = & hits;};
	void		setTofSlabHits(std::vector<TofSlabHitTextFileIo*>& hits)	{m_tofSlabHits = & hits;};
	void		setTofHits(std::vector<TofHitTextFileIo*>& hits)		{m_tofHits = & hits;}

  private :

		//The digit
    TofDigit*		m_digit;

			//The data in the digit that are written to the file
    int   		m_stationNumber; //  TOF station
    int   		m_planeNumber;   //  x or y plane
    int			m_pmt;
    int   		m_slab;
    bool		m_good;
    double 		_pe;
    double		_time;
    int			_adc;
    int			_tdc;

		//Indices for the digit member data
		//(these also are written in the file)

    int m_adc_index;
    int m_fadc_index;    
    int m_tdc_index;
    int m_slab_index;
    int m_hit_index;

		//Pointer to vectors of all Io objects of the relevant type
		//Set in the set_____ methods above by TextFileWriter/Reader

    std::vector<VmeAdcHitTextFileIo*>*  m_vmeAdcHits;
    std::vector<VmefAdcHitTextFileIo*>* m_vmefAdcHits;    
    std::vector<VmeTdcHitTextFileIo*>*  m_vmeTdcHits;
    std::vector<TofSlabHitTextFileIo*>* m_tofSlabHits;
    std::vector<TofHitTextFileIo*>*     m_tofHits;

		//Digit member data pointers
		//These are converted to BlahTextFileIo.blah_index and stored above
		//The index is written to file

    VmeAdcHit*	m_adc;
    VmefAdcHit*	m_fadc;    
    VmeTdcHit*	m_tdc;
    TofSlabHit* m_slabHit;
    TofHit*	m_tofHit;
};

#endif

