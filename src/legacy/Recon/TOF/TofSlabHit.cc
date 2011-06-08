//TofSlabHit.cc
//
// A class representing a slab hit on Tof
//
// A. Fish - 2nd October 2005

#include "Interface/TofDigit.hh"
#include "TofSlabHit.hh"
#include "Interface/MICERun.hh"
#include "Config/MiceModule.hh"

#include <iostream>
using namespace std;

TofSlabHit::TofSlabHit( TofDigit* dig1, TofDigit* dig2 )
:theRun(NULL), theModule(NULL)
{
  miceMemory.addNew( Memory::TofSlabHit );
  
  _digit1 = dig1;
  _digit2 = dig2;

  m_good = false;

  // check if this is a good slab hit

  int pmt1 = _digit1->pmt();
  int pmt2 = _digit2->pmt();

  int slab1 = _digit1->slab();
  int slab2 = _digit2->slab();

  int stat1 = _digit1->stationNumber();
  int stat2 = _digit2->stationNumber();
 
  int plane1 = _digit1->planeNumber();
  int plane2 = _digit2->planeNumber();

 if( stat1 == stat2 && plane1 == plane2 && slab1 == slab2 && pmt1 != pmt2 )
  { 
    m_station = stat1;
    m_plane = plane1;
    m_slabNumber = slab1;
    m_good = true;

    rawTime = ( _digit1->time() + _digit2->time() ) / 2. ;
  }
}  

TofSlabHit::TofSlabHit( MICERun* r, const MiceModule* m, TofDigit* dig1 )
:theRun(r), theModule(m)
{
  miceMemory.addNew( Memory::TofSlabHit );
  
  _digit1 = dig1;
  _digit2 = NULL;

  m_good = true;

  // check if this is a good slab hit

  int slab1 = _digit1->slab();

  int stat1 = _digit1->stationNumber();
 
  int plane1 = _digit1->planeNumber();
 
  m_station = stat1;
  m_plane = plane1;
  m_slabNumber = slab1;

  rawTime = _digit1->time();

  double half_L = theModule->propertyHep3Vector( "Pmt1Pos" ).mag();
  double c = theRun->DataCards->fetchValueDouble("TOFscintLightSpeed");

  correctedTime = rawTime - half_L/c;
}

TofSlabHit::TofSlabHit( MICERun* r, const MiceModule* m, TofDigit* dig1, TofDigit* dig2 )
:theRun(r), theModule(m)
{
  miceMemory.addNew( Memory::TofSlabHit );
  
  _digit1 = dig1;
  _digit2 = dig2;

  m_good = false;

  // check if this is a good slab hit

  int pmt1 = _digit1->pmt();
  int pmt2 = _digit2->pmt();

  int slab1 = _digit1->slab();
  int slab2 = _digit2->slab();

  int stat1 = _digit1->stationNumber();
  int stat2 = _digit2->stationNumber();
 
  int plane1 = _digit1->planeNumber();
  int plane2 = _digit2->planeNumber();

 if( stat1 == stat2 && plane1 == plane2 && slab1 == slab2 && pmt1 != pmt2 )
  { 
    m_station = stat1;
    m_plane = plane1;
    m_slabNumber = slab1;

    rawTime = ( _digit1->rawTime() + _digit2->rawTime() ) / 2. ;
    m_good = true;
  }
}

double TofSlabHit::pe() const
{
  double pe = 0.;

  if( _digit1 )
    pe += _digit1->pe();

  if( _digit2 )
    pe += _digit2->pe();

  return pe;
}

bool TofSlabHit::CalibrateTime(TofCalibrationMap* calib, TriggerKey* TrKey )
{
  PmtKey pmtKey1(_digit1->stationNumber(), _digit1->planeNumber(), _digit1->slab(), _digit1->pmt());
  PmtKey pmtKey2(_digit2->stationNumber(), _digit2->planeNumber(), _digit2->slab(), _digit2->pmt());
  double dt1 = calib->dT(pmtKey1, *TrKey, _digit1->adc());
  double dt2 = calib->dT(pmtKey2, *TrKey, _digit2->adc());
  //cout<<"TofSlabHit::CalibrateTime  dt1 = "<<dt1<<"  dt2 = "<<dt2<<endl;
  if(dt1==-99999 || dt2==-99999) return false;
  
  _digit1->setTime( _digit1->rawTime() - dt1 );
  _digit2->setTime( _digit2->rawTime() - dt2 );

  correctedTime = ( _digit1->time() + _digit2->time() ) / 2. ;
  return true;
}

bool TofSlabHit::CalibrateTime()
{
  double half_L = theModule->propertyHep3Vector( "Pmt1Pos" ).mag();
  double c = theRun->DataCards->fetchValueDouble("TOFscintLightSpeed");
  
  rawTime = ( _digit1->time() + _digit2->time() )/ 2.;
  correctedTime = rawTime - half_L/c;
  //cout<<"TofSlabHit::CalibrateTime()  rawTime = "<<rawTime<<"  dt = "<<-half_L/(c*picosecond)<<endl;
  return true;
}

void TofSlabHit::Print( bool verbose )
{
	if ( verbose ) {
	cout << "____________________________" << endl;
	cout << "  TofSlabHit" <<dec<< endl;
	cout << "  Station Number : " << m_station << endl;
	cout << "  Plane Number : "<< m_plane << endl;
	cout << "  Slab Number : "<< m_slabNumber << endl;
	cout << "  Raw Time : " << RawTime()<<" ps" << endl;
	cout << "  Corrected Time : " << CorrectedTime() <<" ps" << endl;
	cout << "____________________________" << endl;
	}
	if ( !verbose ) {
		cout << "TofSlabHit: (" << m_station << ", " << m_plane << ", " << m_slabNumber << "), time (ns): " << time()/CLHEP::ns << ", Charge: " << pe() << endl; 
	}
}

TofDigit* TofSlabHit::digit( int const pmt ) const {
	TofDigit* Digit = 0;
	if ( digit1()->pmt() == pmt ) Digit = digit1();
	if ( digit2()->pmt() == pmt ) Digit = digit2();
	return Digit;
}
