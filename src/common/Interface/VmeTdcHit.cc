// VmeTdcHit.cc
//
// A class describing a hit on an TDC channel in a VME module
//
// A. Fish - 12th September 2005

#include "Interface/VmeTdcHit.hh"
#include <iostream>
#include <fstream>

using namespace std;

VmeTdcHit::VmeTdcHit( int crate,  int board, std::string type, int chan, int Ltdc, int Ttdc )
:VmeBaseHit( crate, board, type, chan )
{
  miceMemory.addNew( Memory::VmeTdcHit );
  m_Ltdc = Ltdc;
  m_Ttdc = Ttdc;
	m_TTT = 0;
	m_BunchId = 0;
}

VmeTdcHit::VmeTdcHit( int crate, int board, std::string type, int chan, int Ltdc ) 
:VmeBaseHit( crate, board, type, chan )
{
  miceMemory.addNew( Memory::VmeTdcHit );
  m_Ltdc = Ltdc;
  m_Ttdc = 0;
  m_TTT = 0;
	m_BunchId = 0;
}

void VmeTdcHit::Print()
{
  	cout << "____________________________" << endl;
  	cout << "  VmeTdcHit" << endl;
		cout << "  Crate Id : " << crate() << endl;
		cout << "  Board Id : "<< board() << endl;
  	cout << "  Board Type : "<< moduleType() << endl;
  	cout << "  Channel : " << channel() << endl;
  	cout << "  Leading time : " << m_Ltdc <<" Tdc units"<< endl;
  	cout << "  Trailing time : " << m_Ttdc <<" Tdc units"<< endl;
  	cout << "  Trigger Time Tag : " << m_TTT << endl;
  	cout << "  Bunch Id : " << m_BunchId << endl; 
  	cout << "____________________________" << endl;
}

