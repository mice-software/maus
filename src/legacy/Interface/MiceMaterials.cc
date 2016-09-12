// MAUS WARNING: THIS IS LEGACY CODE.
// MiceMaterials.cc
//
// A map of G4Material pointers to strings which can be accessed through the MICE Run for use in the
// GEANT4 representation of a particular MICE configuration
//
// 25th May 2006
//
// Malcolm Ellis

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "Geant4/G4Material.hh"
#include "Interface/MiceMaterials.hh"
#include "Utils/Exception.hh"
#include "CLHEP/Units/SystemOfUnits.h"
using CLHEP::mm;
using CLHEP::cm2;
using CLHEP::mm3;
using CLHEP::g;
using CLHEP::gram;
using CLHEP::eV;
using CLHEP::MeV;
using CLHEP::kelvin;
using CLHEP::atmosphere;

// constructor that will read in the material properties from a file if the application is
// not linking with GEANT4

MiceMaterials::MiceMaterials( bool noG4 )
{
  if( noG4 )
  {
    if(getenv( "MICEFILES" ) == NULL) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error - MICEFILES environment variable was not defined", "MiceMaterials::MiceMaterials"));
    if(getenv( "COMPILER" )  == NULL) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error - COMPILER environment variable was not defined",  "MiceMaterials::MiceMaterials"));
    if(getenv( "G4VERS" )    == NULL) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Error - G4VERS environment variable was not defined",    "MiceMaterials::MiceMaterials"));
    std::string fname = std::string( getenv( "MICEFILES" ) ) + "/Models/Materials/micematerials_" 
                      + std::string( getenv( "COMPILER" ) ) + "_"
                      + std::string( getenv( "G4VERS" ) ) + ".txt";
    std::ifstream inf( fname.c_str() );

    if( inf.good() )
    {
      // read the first couple of lines that state the compiler and GEANT4 versions
      char line[250];
      std::string sline;
      inf.getline( line, 80 );
      inf.getline( line, 80 );

      // now read in each of the materials and store the details in the respective maps

      while( ! inf.eof() && inf.good() )
      {
        std::string name, value, chemical, units, state;
        double density, temp, press, edens, X0, L0, ZNum, ANum, meanExE, densityCor;

        inf.getline( line, 250 );
        sline = std::string( line );
        std::stringstream line1( sline );
        line1 >> value >> name >> value;

        inf.getline( line, 250 );
        sline = std::string( line );
        std::stringstream line2( sline );
        line2 >> value >> value >> chemical;

        inf.getline( line, 250 );
        sline = std::string( line );
        std::stringstream line3( sline );
        line3 >> value >> density >> units;

        inf.getline( line, 250 );
        sline = std::string( line );
        std::stringstream line4( sline );
        line4 >> value >> state;

        inf.getline( line, 250 );
        sline = std::string( line );
        std::stringstream line5( sline );
        line5 >> value >> temp >> units;

        inf.getline( line, 250 );
        sline = std::string( line );
        std::stringstream line6( sline );
        line6 >> value >> press >> units;

        inf.getline( line, 250 );
        sline = std::string( line );
        std::stringstream line7( sline );
        line7 >> value >> value >> edens >> units >> units;

        inf.getline( line, 250 );
        sline = std::string( line );
        std::stringstream line8( sline );
        line8 >> value >> value >> X0 >> units;

        inf.getline( line, 250 );
        sline = std::string( line );
        std::stringstream line9( sline );
        line9 >> value >> value >> L0 >> units;

        inf.getline( line, 250 );
        sline = std::string( line );
        std::stringstream line10( sline );
        line10 >> value >> value >> ZNum;

        inf.getline( line, 250 );
        sline = std::string( line );
        std::stringstream line11( sline );
        line11 >> value >> value >> ANum;

        inf.getline( line, 250 );
        sline = std::string( line );
        std::stringstream line12( sline );
        line12 >> value >> value >> value >> meanExE >> units;

        inf.getline( line, 250 );
        sline = std::string( line );
        std::stringstream line13( sline );
        line13 >> value >> value >> value >> value >> value >> value >> value >> value >> densityCor;

        // skip dEdx line as that is calculated
        inf.getline( line, 250 );

        if( name != "" && name[0] != ' ' )
        {
          _materials[ name ] = NULL;
          updateProperties( name, chemical, density * g / mm3, state, temp * kelvin, press * atmosphere, edens / mm3, X0 * mm, L0 * mm, ZNum, ANum, meanExE * eV, densityCor  );
        }
      }
    }
    else
      std::cerr << "ERROR - unable to find the materials file " << fname << " as a result no mice material information is being loaded" << std::endl;
  }
}

void MiceMaterials::addMaterial( G4Material* mat, std::string name )
{
  _materials[name] = mat;
}

G4Material* MiceMaterials::materialByName( std::string mat ) const
{
  if ( _materials.find( mat ) == _materials.end() ) {
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Unable to find material "+mat, "MiceMaterials::materialByName") );
  }
  G4Material* matter = _materials.find( mat )->second;

  if( ! matter ) // can't find this material!
  {
    throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Unable to find material "+mat, "MiceMaterials::materialByName") );
    matter = _materials.find( "Galactic" )->second;
  }

  return matter;
}

std::string      MiceMaterials::chemicalFomula( std::string name ) const
{
  return _formula.find( name )->second;
}

double           MiceMaterials::density( std::string name ) const
{
  return _density.find( name )->second;
}

std::string      MiceMaterials::state( std::string name ) const
{
  return _state.find( name )->second;
}

double           MiceMaterials::temperature( std::string name ) const
{
  return _temperature.find( name )->second;
}

double           MiceMaterials::pressure( std::string name ) const
{
  return _pressure.find( name )->second;
}

double           MiceMaterials::electronDensity( std::string name ) const
{
  return _electronDensity.find( name )->second;
}

double           MiceMaterials::zNumber( std::string name ) const
{
  return _zNumber.find( name )->second;
}

double           MiceMaterials::aNumber( std::string name ) const
{
  return _aNumber.find( name )->second;
}

double           MiceMaterials::meanExcitationEnergy( std::string name ) const
{
  return _meanExcitationEnergy.find( name )->second;
}

double           MiceMaterials::densityCorrection( std::string name ) const
{
  return _densityCorrection.find( name )->second;
}

double           MiceMaterials::radiationLength( std::string name ) const
{
  return _radiationLength.find( name )->second;
}

double           MiceMaterials::interactionLength( std::string name ) const
{
  return _interactionLength.find( name )->second;
}

double MiceMaterials::dEdx( std::string name ) const
{
  double dEdx = 0;

  if( _radiationLength.find( name ) == _radiationLength.end() ) // this material is not known
    return dEdx;

  // for now, assume a 200 MeV/c muon - we need to work out how to do this a little better later on...

  double K = 0.307075 * MeV  * cm2 / g;
  double ZonA = zNumber( name ) / aNumber( name );
  double beta = 0.88;
  double betaGamma = 1.89;
  double me = 0.511 * MeV;
  double Tmax = 3.63 * MeV;
  double I = meanExcitationEnergy( name );
  dEdx = -1. * K * ZonA / ( beta * beta ) * ( 0.5 * log( 2. * me * betaGamma * betaGamma * Tmax / I / I ) - beta * beta - densityCorrection( name ) / 2.0 );

  return dEdx;
}

double MiceMaterials::dEdxLength( std::string name ) const
{
  
  double dEds = 0;

  if( _radiationLength.find( name ) == _radiationLength.end() ) // this material is not known
    return dEds;

  dEds = dEdx( name ) * density( name );

  return dEds;
}

void MiceMaterials::listMaterials() const
{
  for( std::map<std::string,G4Material*>::const_iterator it = _materials.begin(); it != _materials.end(); ++it )
  {
    std::cout << "Material " << it->first << " has:" << std::endl;
    std::cout << "  Chemical Formula " << _formula.find( it->first )->second << std::endl;
    std::cout << "  Density " << _density.find( it->first )->second / gram * mm3 << " g/mm3" << std::endl;
    std::cout << "  State " << _state.find( it->first )->second << std::endl;
    std::cout << "  Temperature " << _temperature.find( it->first )->second / kelvin << " kelvin" << std::endl;
    std::cout << "  Pressure " << _pressure.find( it->first )->second / atmosphere << " atmosphere" << std::endl;
    std::cout << "  Electron Density " << _electronDensity.find( it->first )->second * mm3 << " per mm3" << std::endl;
    std::cout << "  Radiation Length " << _radiationLength.find( it->first )->second / mm << " mm" << std::endl;
    std::cout << "  Interaction Length " << _interactionLength.find( it->first )->second / mm << " mm" << std::endl;
    std::cout << "  Z Number " << _zNumber.find( it->first )->second << std::endl;
    std::cout << "  A Number " << _aNumber.find( it->first )->second << std::endl;
    std::cout << "  Mean Excitation Energy " << _meanExcitationEnergy.find( it->first )->second / eV << " eV" << std::endl;
    std::cout << "  Density Effect Correction Due to Ionization Energy Loss " << _densityCorrection.find( it->first )->second << std::endl;
    std::cout << "  dE/dx " << dEdx(it->first) / MeV * g / cm2 <<" MeV/(g/cm2)" <<std::endl;

  }
}

void MiceMaterials::updateProperties( std::string name, std::string chemical, double density, std::string state, double temp, double press, double edens, double X0, double L0, double ZNum, double ANum, double meanExE , double densityCor )
{
  _formula[ name ] = chemical;
  _density[ name ] = density;
  _state[ name ] = state;
  _temperature[ name ] = temp;
  _pressure[ name ] = press;
  _electronDensity[ name ] = edens;
  _radiationLength[ name ] = X0;
  _interactionLength[ name ] = L0;
  _zNumber[ name ] = ZNum;
  _aNumber[ name ] = ANum;
  _meanExcitationEnergy[ name ] = meanExE;
  _densityCorrection[ name ] = densityCor;  
}

MiceMaterials::~MiceMaterials() {
    // There is a G4 memory leak where
    //   G4Material::G4MaterialPropertiesTable::G4MaterialPropertyVector
    // contains a list of pointers to G4MPVEntry which are alloc'd on the stack
    // but cannot be reached by user to delete them and are not handled properly
    // in G4MaterialPropertyVector destructor 
    typedef std::map<std::string, G4Material*>::iterator mat_iter;
    for (mat_iter it = _materials.begin(); it != _materials.end(); ++it) {
        G4Material* mat = (*it).second;
        if (mat->GetMaterialPropertiesTable() != NULL)
            delete mat->GetMaterialPropertiesTable();
    }
}

