// VmeBaseHit.hh
//
// A base class to describe the information common to all hits recorded
// on a VME module
//
// A. Fish - 12th September 2005

#ifndef INTERFACE_VMEBASEHIT_H
#define INTERFACE_VMEBASEHIT_H

#include <string>

class VmeBaseHit
{
  public :

   VmeBaseHit()		{};

   VmeBaseHit( int, int, std::string, int );

   int			crate() const		{ return m_crate; };

   int			board() const		{ return m_board; };

   std::string		moduleType() const	{ return m_module_type; };

   int			channel() const		{ return m_channel; };

  private :

    int 		m_crate;
    int			m_board;
    std::string		m_module_type;
    int			m_channel;
};

#endif

