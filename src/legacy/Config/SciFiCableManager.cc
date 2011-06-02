// MAUS WARNING: THIS IS LEGACY CODE.
// VlpcCableManager.cc
//
// Code to allow Datacards to read which SciFiCableType to use, 
// either Imperial or either of the Osaka versions.
//
// A.Fish 29/9/2006
// Modified: M. Ellis 10/2008

#include "Config/SciFiCableManager.hh"

#include "Config/VlpcCableImperial.hh"
#include "Config/VlpcCableOsaka.hh"
#include "Config/VlpcCableOsaka2.hh"
#include "Config/VlpcCableOsaka3.hh"

void makeVlpcCable( MICERun* run )
{
  std::string type = run->DataCards->fetchValueString( "SciFiCableType" );
  std::string fname = run->DataCards->fetchValueString( "SciFiCable" );

  if( type == "Imperial" )
  {
    run->vlpcCable = new VlpcCableImperial( fname );
  }
  else if( type == "Osaka" )
  {
    run->vlpcCable = new VlpcCableOsaka( fname, true );
  }
  else if( type == "Osaka2" )
  {
    run->vlpcCable = new VlpcCableOsaka2( fname );
  }
  else if( type == "Osaka3" )
  {
    run->vlpcCable = new VlpcCableOsaka3( fname );
  }
}
