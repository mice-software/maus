// VmeBaseHit.cc
//
// A base class to describe the information common to all hits recorded
// on a VME module
//
// A. Fish - 12th September 2005

#include "VmeBaseHit.hh"

VmeBaseHit::VmeBaseHit( int crate, int board, std::string type, int chan )
{
  m_crate = crate;
  m_board = board;
  m_module_type = type;
  m_channel = chan;
}
