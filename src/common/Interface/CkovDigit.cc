///////////////////////////////////////////////////////////////////////////////
/// CkovDigit
///
////////////////////////////////////////////////////////////////////////////////

#include "Interface/CkovHit.hh"
#include "Interface/CkovDigit.hh"
#include "Interface/dataCards.hh"
#include "CLHEP/Random/RandGauss.h"
using CLHEP::RandGauss;
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
using CLHEP::nm;

#include "Randomize.hh"
#include <fstream>

using namespace std;
		 
CkovDigit::CkovDigit( VmefAdcHit * fadc, VmeTdcHit * tdc, int pmt, int plane )
//	:pmtNo( pmt ),planeNo( plane ),m_adc( 0 ),m_tdc( 0 ), m_good( false ), m_pe( 0.0 )
{
	pmtNo = pmt;
	planeNo = plane;
	m_adc = 0;
	m_tdc = 0;
	m_good = false; 
	m_pe = 0.0;
	miceMemory.addNew( Memory::CkovDigit );
	m_adc = -99;
	m_adcHit = NULL;
	setVmefAdcHit( fadc );
	setVmeTdcHit( tdc );
	CreateDigit();
}

CkovDigit::CkovDigit( VmefAdcHit * fadc, int pmt, int plane )
{
	pmtNo = pmt;
	planeNo = plane;
	m_adc = 0;
	m_tdc = 0;
	m_good = false; 
	m_pe = 0.0;
	miceMemory.addNew( Memory::CkovDigit );
	m_adc = -99;
	m_tdc = -99;
	m_adcHit = NULL;
	m_tdcHit = NULL;
	setVmefAdcHit( fadc );
	CreateDigit();
}

CkovDigit::CkovDigit( VmeAdcHit * adc, VmeTdcHit * tdc, int pmt, int plane )
{
	pmtNo = pmt;
	planeNo = plane;
	m_adc = 0;
	m_tdc = 0;
	m_good = false; 
	m_pe = 0.0;
	miceMemory.addNew( Memory::CkovDigit );
	m_fadcHit = NULL;
	setVmeAdcHit( adc );
	setVmeTdcHit( tdc );
	CreateDigit();
}


CkovDigit::CkovDigit( CkovHit* Hit,  dataCards* dc)
{
	miceMemory.addNew( Memory::CkovDigit );
	planeNo = Hit->GetPlaneNo();
	pmtNo = Hit->GetPmtNo();
	m_adc = 0;
	m_tdc = 0;
	m_pe = 0;

	AddHitToThisDigit(Hit, dc);
}

CkovDigit::CkovDigit( int pmt, int plane, dataCards* dc)
		:pmtNo( pmt ),planeNo( plane ),m_adc( 0 ),m_tdc( 0 ), m_good( false ), m_pe( 0.0 )
{
	miceMemory.addNew( Memory::CkovDigit );
}


CkovDigit::~CkovDigit()
{
  miceMemory.addDelete( Memory::CkovDigit );
}

void CkovDigit::CreateDigit()
{
	m_good = false;
	if( ( m_tdcHit || m_fadcHit || m_adcHit ) && (planeNo >= 0) && ( pmtNo >= 0 ) ){
		m_good = true;
	}

}

void CkovDigit::setVmeAdcHit( VmeAdcHit* hit )	
{
	 m_adcHit = hit;
	 if( m_adcHit )
	   m_adc = m_adcHit->adc();
}

void CkovDigit::setVmefAdcHit( VmefAdcHit* hit )	
{
	 m_fadcHit = hit;
         if( m_fadcHit )
	   m_pe = m_fadcHit->GetCharge();
}

void	CkovDigit::setVmeTdcHit( VmeTdcHit* hit )
{
	m_tdcHit = hit;
        if( m_tdcHit )
	  m_tdc = m_tdcHit->tdc();
}

void CkovDigit::MergeWithOther( CkovDigit* dig )
{
  m_pe += dig->GetPe();
  m_adc += dig->GetADC();

  const std::vector<CkovHit*>& mhits = dig->GetMotherHits();

  for( unsigned int i = 0; i < mhits.size(); ++i )
    m_hitMothers.push_back( mhits[i] );
}

void CkovDigit::AddHitToThisDigit( CkovHit* hit, dataCards* dc )
{


	double IncidenceAngle = hit->GetAngle();
	double PolarizationAngle = hit->GetPAngle();
	Hep3Vector pol = hit->GetPol();
	double wavelenght = hit->GetWavelenght();

	double probability1, probability2;

	// PMT efficiency
	// have to be specified
	if( wavelenght/nm < 700 && wavelenght/nm > 500 ) probability1 = 0.12;
	else if( wavelenght/nm < 500 && wavelenght/nm > 400 ) probability1 = 0.22;
	else if( wavelenght/nm < 400 && wavelenght/nm > 350 ) probability1 = 0.33;
	else if( wavelenght/nm < 350 && wavelenght/nm > 300 ) probability1 = 0.23;
	else if( wavelenght/nm < 300 && wavelenght/nm > 270 ) probability1 = 0.08;
	else probability1 = 0.0;

	double n1 = 1.0;  // Air
	double n2 = 1.5;  // Glass - have to be specified

	double cos2PA = cos(PolarizationAngle) * cos(PolarizationAngle);
	double sin2PA = sin(PolarizationAngle) * sin(PolarizationAngle);
	double RefractAngle = asin( sin(IncidenceAngle)*n1/n2 );

	/*
	//         Probability for Refraction
	//  T1 - Probability for polarization parallel to the plane of incidence
	//  T2 - Probability for polarization perpendicularto the plane of incidence
	double N = ( n2/n1 )*( cos(RefractAngle)/cos(IncidenceAngle) );
	double T1 = N*pow( 2*sin(RefractAngle)*cos(IncidenceAngle)/( sin(IncidenceAngle + RefractAngle)*cos(IncidenceAngle - RefractAngle) ), 2 );
	double T2 = N*pow(2*sin(RefractAngle)*cos(IncidenceAngle)/sin(IncidenceAngle+RefractAngle),2);
	*/

	//         Probability for Reflection
	//  T1 - Probability for polarization parallel to the plane of incidence
	//  T2 - Probability for polarization perpendicularto the plane of incidence
	double R1 = pow( tan(IncidenceAngle-RefractAngle)/tan(IncidenceAngle+RefractAngle), 2 );
	double R2 = pow( sin(IncidenceAngle-RefractAngle)/sin(IncidenceAngle+RefractAngle), 2 );

	//  Total probability -    cos2PA*R1 + sin2PA*R2 + cos2PA*T1 + sin2PA*T2 = 1
	probability2 = 1 - ( cos2PA*R1 + sin2PA*R2 );
	//probability2 = cos2PA*T1 + sin2PA*T2;

	double rand1 = G4UniformRand();
	double rand2 = G4UniformRand();

	if(rand1 < probability1 && rand2 < probability2 )
	{
		m_pe ++;
		double adc2pe = dc->fetchValueDouble("CKOVadcConversionFactor");
		SetADC( (int)( GetADC() + 1/adc2pe ) );

		double tdc2time = dc->fetchValueDouble("CKOVtdcConversionFactor");
		double timeUncert = dc->fetchValueDouble("CKOVpmtTimeResolution");
		double time = RandGauss::shoot( hit->GetTime(), timeUncert );
		int tdc = (int)( time / tdc2time );
		if( m_tdc==0 || m_tdc>tdc)
		{
			SetTDC( tdc );
		}
	}
	m_hitMothers.push_back( hit );
}

void CkovDigit::Print()
{
  	cout << "____________________________" << endl;
  	cout << "  CkovDigit" << endl;
	cout << "  Plane Number : "<< planeNo << endl;
  	cout << "  PMT Number : "<< pmtNo << endl;
	cout << "  Pe: " << m_pe << endl;
	cout << "  Number of Mothers (CkovHits): " << m_hitMothers.size() << endl;
	cout << "  TDC: " << m_tdc << endl;
	cout << "  ADC: " << m_adc << endl;
  	cout << "____________________________" << endl;
}

