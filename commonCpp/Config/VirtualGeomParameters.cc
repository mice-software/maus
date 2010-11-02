//----------------------------------------------------------------------------
//
//     File :      VirtualGeomParameters.cc
//     Purpose :   Supply geometry parameters to the VirtualStation.
//     Created :   10-NOV-2002  by Steve Kahn
//
//----------------------------------------------------------------------------

#include "VirtualGeomParameters.hh"
#include "Interface/dataCards.hh"
#include <cstdlib>
#include <fstream>

using std::map;
using std::vector;
using std::endl;
using std::cerr;
using std::string;

VirtualGeomParameters* VirtualGeomParameters::_instance = 
    (VirtualGeomParameters*) NULL;

VirtualGeomParameters* VirtualGeomParameters::getInstance()
{  if(_instance != NULL) return _instance;
   _instance = new VirtualGeomParameters;
   return _instance;
}

VirtualGeomParameters::VirtualGeomParameters()
{  initialize(); 

  miceMemory.addNew( Memory::VirtualGeomParameters ); 
}

void VirtualGeomParameters::initialize()
{

  //  _stationIDs = vdc->fetchValueVector("VIRTstationIDs");
  // vector<double> zpos = vdc->fetchValueVector("VIRTzpositions");
  // vector<double> rads = vdc->fetchValueVector("VIRTradii");
  // vector<double> lengs  = vdc->fetchValueVector("VIRTlengths");
  
  string filename = MyDataCards.fetchValueString("VIRTUALParamsFile");
  readFile(filename);
}


void VirtualGeomParameters::readFile(string& filename)
{
  std::ifstream fin(filename.c_str());
  int nkeys;
  if (!fin) {
    cerr << "VirtualGeomParameters: failed to open input file" << endl;
    nkeys = -1;
  } else {
    fin >> nkeys;
    for(int i=0; i<nkeys; i++)
    {  int ikey;
       double zpos, rad, leng;
       fin >> ikey >> zpos >> rad >> leng;
       _stationIDs.push_back(ikey);
       _zpositions[ikey] = zpos;
       _radii[ikey] = rad;
       _lengths[ikey] = leng;
    }
  }
}

// accessors

int VirtualGeomParameters::numberStations() const
{  return _stationIDs.size(); }

double VirtualGeomParameters::getRadius(int ista) const
{  map<int, float>::const_iterator it =  _radii.find(ista);
   if( it == _radii.end())
   {   cerr << "VirtualGeomParameters: radius for station " << ista 
	    << " not found" << endl;
       return -999.;
   }
   return (double) (*it).second;
}

double VirtualGeomParameters::getLength(int ista) const
{  map<int, float>::const_iterator it = _lengths.find(ista);
   if( it == _radii.end())
     {  cerr << "VirtualGeomParameters: length for station " << ista
	     << " not found" << endl;
        return -999;
     }
   return (double) (*it).second;
}

Hep3Vector VirtualGeomParameters::getPosition(int ista) const
{
    map<int, float>::const_iterator it =  _zpositions.find(ista);
    if( it == _zpositions.end())
      {  cerr << "VirtualGeomParameters: z position for station " << ista
	      << " not found" << endl;
         return Hep3Vector( 0., 0., -999.);
      }
    double zpos = (double) (*it).second;
    return Hep3Vector( 0., 0., zpos);
}

/*
Material* VirtualGeomParameters::getMaterial(int ista) const
{
  map<int, Material*>::const_iterator it = _materials.find(ista);
  if( it == _materials.end())
    {  cerr << "VirtualGeomParameters: material for station " << ista
	    << " not found"  << endl;
       return (Material*) NULL;
    }
  return (*it).second;
}
*/

int VirtualGeomParameters::getStationID(int i) const
{   return _stationIDs[i]; }

std::vector<int> VirtualGeomParameters::getStationIDs() const
{   return _stationIDs; }

std::ostream& operator <<(std::ostream& os, const VirtualGeomParameters& me)
{ char sp[] = "  ";
  os << "Number of virtual stations : " << me.numberStations() << endl;
  for( int i=0; i<me.numberStations(); i++)
    {  int ikey = me.getStationID(i);
      double zpos = me.getPosition(ikey).z();
      double rad = me.getRadius(ikey);
      double leng = me.getLength(ikey);
      os << ikey << sp << zpos << sp << rad << sp << leng << endl;
    }
  return os;
}

bool VirtualGeomParameters::exists(int ikey) const
{
  vector<int>::const_iterator it = _stationIDs.begin();
  while( it != _stationIDs.end())
    {
      if( ikey == (int) (*it)) return true;
      it++;
    }
  return false;
}
