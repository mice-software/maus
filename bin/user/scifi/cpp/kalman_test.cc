/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


// C++ Headers
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <iomanip>

// ROOT Headers
#include "TMatrix.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TFile.h"

// MAUS Headers
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Recon/Kalman/KalmanState.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSeed.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"
#include "src/common_cpp/Recon/Kalman/KalmanHelicalPropagator.hh"

// Other headers


const static double speed_of_light = 299.792458;
const static double magnetic_field = 0.004;
//const static char* configuration_file = "src/common_py/ConfigurationDefaults.json";
const static char* configuration_file = "ConfigurationDefaults.json";
const static unsigned int number_repetitions = 100;
const static unsigned int number_delta = 100;
const static double max_delta_pos = 1.0;
const static double min_delta_pos = 0.0001;
const static double max_delta_mom = 1.0;
const static double min_delta_mom = 0.0001;
const static double max_delta_kappa = 0.0001;
const static double min_delta_kappa = 0.0000001;
const static unsigned int number_parameters = 5;

//**************************************************************************************************
// Function Declarations
//**************************************************************************************************

void print_state( TMatrixD );

//**************************************************************************************************
// Class Defintions
//**************************************************************************************************

class helixMaker
{
  public :
    helixMaker( double Bz, double x0, double y0, double rad, double advance, double phi0 = 0.0 );

    void setZ0( double z0 ) { _z0 = z0; }
    void setBz( double Bz ) { _Bz = Bz; }
    void setPhi0( double phi0 ) { _phi0 = phi0; }

    TMatrixD getState( double z ) const;

    double getGradient( double z, unsigned int index0, unsigned int index1 );

  private :
    double _Bz;
    double _x0;
    double _y0;
    double _rad;
    double _advance; // ds/dz
    double _phi0;

    double _z0;
};


//**************************************************************************************************
// MAIN
//**************************************************************************************************


int main( int, char** ) 
{
  srand( 100 );

  double delta_a_array[number_parameters][number_delta];

  for ( unsigned int j = 0; j < number_delta; ++j )
    delta_a_array[0][j] = min_delta_pos + ( j * ( ( max_delta_pos - min_delta_pos ) / number_delta ) );
  for ( unsigned int j = 0; j < number_delta; ++j )
    delta_a_array[1][j] = min_delta_mom + ( j * ( ( max_delta_mom - min_delta_mom ) / number_delta ) );
  for ( unsigned int j = 0; j < number_delta; ++j )
    delta_a_array[2][j] = min_delta_pos + ( j * ( ( max_delta_pos - min_delta_pos ) / number_delta ) );
  for ( unsigned int j = 0; j < number_delta; ++j )
    delta_a_array[3][j] = min_delta_mom + ( j * ( ( max_delta_mom - min_delta_mom ) / number_delta ) );
  for ( unsigned int j = 0; j < number_delta; ++j )
    delta_a_array[4][j] = ( min_delta_kappa + ( j * ( ( max_delta_kappa - min_delta_kappa ) / number_delta ) ) );


  TH1F* residuals[number_parameters];
  TH2F* differentialResiduals[number_parameters][number_parameters];
  std::stringstream converter;
  std::string current_name( "" );
  for ( unsigned int i = 0; i < number_parameters; ++i )
  {
    for ( unsigned int j = 0; j < number_parameters; ++j )
    {
      converter.str( "" );
      converter << "differentialResiduals_" << i << "_" << j;
      differentialResiduals[i][j] = new TH2F( converter.str().c_str(), converter.str().c_str(), 100, -10.0, 10.0, number_delta, 0.0, 1.0 );
    }
    converter.str( "" );
    converter << "residuals_" << i;
    residuals[i] = new TH1F( converter.str().c_str(), converter.str().c_str(), 100, -10.0, 10.0 );
  }

  // Load Example Configuration
  std::cout << "Loading MAUS Globals. A necessary step...\n" << std::endl;

  std::ifstream configFileIn( configuration_file );
  if ( ! configFileIn.is_open() )
  {
    std::cout << "ERROR : Could not open file: " << configuration_file << "\n";
    return 0;
  }
  std::stringstream strStream;
  strStream << configFileIn.rdbuf();
  std::string config = strStream.str();
  configFileIn.close();

//  std::cout << "CONFIG = \n" << config << "\n\n" << std::endl;

  MAUS::GlobalsManager::InitialiseGlobals( config );
  Squeak::setStandardOutputs( Squeak::debug ); //IRStream uses Squeak! This gives back std::cout.

  std::cout << "Testing Kalman. Please be patient.\n" << std::endl;

  std::cout << "Making some helices." << std::endl;
  helixMaker maker( magnetic_field, 0.0, 0.0, 35.0, 0.20, 0.0 );
  MAUS::KalmanHelicalPropagator prop( magnetic_field );

  for ( unsigned int i = 0; i < number_delta; ++i )
  {
    for ( unsigned int j = 0; j < number_repetitions; ++j )
    {
      std::cout << "\n\n";

      double new_z = 0.0001 * double( rand() % 1000000 );
      double new_phi0 = 2.0 * 3.14159265 * 0.001 * double( rand() % 1000 );
//      double da = 0.00001 * double( rand() % 100000 );

      maker.setPhi0( new_phi0 );

      std::cout << "Z Set to " << new_z << ". Phi-0 set to " << new_phi0 << ".\n\n";

      TMatrixD state_zero = maker.getState( 0.0 );
      TMatrixD state_theory = maker.getState( new_z );

      MAUS::KalmanState s0; s0.Initialise( 5 ); s0.set_z( 0.0 );
      MAUS::KalmanState s_theory; s_theory.Initialise( 5 ); s_theory.set_z( new_z );

      s0.set_a( state_zero, MAUS::KalmanState::Filtered );
      s_theory.set_a( state_theory, MAUS::KalmanState::Filtered );


      for ( unsigned int k = 0; k < number_parameters; ++k )
      {
        TMatrixD state_minus = state_zero; state_minus( k, 0 ) -= delta_a_array[k][i];
        TMatrixD state_plus = state_zero; state_plus( k, 0 ) += delta_a_array[k][i];
        MAUS::KalmanState s_minus; s_minus.Initialise( 5 ); s_minus.set_z( 0.0 );
        MAUS::KalmanState s_plus; s_plus.Initialise( 5 ); s_plus.set_z( 0.0 );
        s_minus.set_a( state_minus, MAUS::KalmanState::Filtered );
        s_plus.set_a( state_plus, MAUS::KalmanState::Filtered );


        TMatrixD F_central( 5, 5 );
        TMatrixD F_plus( 5, 5 );
        TMatrixD F_minus( 5, 5 );

        TMatrixD state_prop = prop.GetIntermediateState( &s0, new_z, F_central );
        TMatrixD state_prop_minus = prop.GetIntermediateState( &s_minus, new_z, F_minus );
        TMatrixD state_prop_plus = prop.GetIntermediateState( &s_plus, new_z, F_plus );

        residuals[k]->Fill( state_prop( k, 0 ) - state_theory( k, 0 ) );


        std::cout << "\n------------------------------------------------------------\n\nStarting States:\n";
        std::cout << " Minus         : "; print_state( s_minus.a( MAUS::KalmanState::Filtered ) ); std::cout << '\n';
        std::cout << " Zero          : "; print_state( s0.a( MAUS::KalmanState::Filtered ) ); std::cout << '\n';
        std::cout << " Plus          : "; print_state( s_plus.a( MAUS::KalmanState::Filtered ) ); std::cout << "\n\n";
        std::cout << "Propagated States:\n";
        std::cout << " Theory        : "; print_state( s_theory.a( MAUS::KalmanState::Filtered ) ); std::cout << "\n\n";
        std::cout << " Prop Minus    : "; print_state( state_prop_minus ); std::cout << "\n";
        std::cout << " Prop Central  : "; print_state( state_prop ); std::cout << "\n";
        std::cout << " Prop Plus     : "; print_state( state_prop_plus ); std::cout << "\n\n";

        std::cout << "\nDifferentials:\n";
        for ( unsigned int m = 0; m < 5; ++m )
        {
          double delta = ( state_plus( k, 0 ) - state_minus( k, 0 ) );
          double diff = ( state_prop_plus( m, 0 ) - state_prop_minus( m, 0 ) );
          double approx_grad = diff/delta;
          double theory_grad = maker.getGradient( new_z, m, k );

          std::cout << " Index " << m << " wrt " << k << " : Delta = ";
          std::cout << std::setw( 8 ) << std::setfill( ' ' ) << delta << " Diff = ";
          std::cout << std::setw( 8 ) << std::setfill( ' ' ) << diff << " => ";
          std::cout << std::setw( 8 ) << std::setfill( ' ' ) << diff / delta  << " | Theory = ";
          std::cout << std::setw( 8 ) << std::setfill( ' ' ) << theory_grad << "\n";

          if ( delta > 1.0e-09 )
          {
            differentialResiduals[k][m]->Fill( approx_grad - theory_grad, delta_a_array[k][i] );
          }
        }
      }

      std::cout << std::endl;

      std::cout << std::flush;
    }
  }


  TFile* outfile = new TFile( "kalman_testing.root", "RECREATE" );
  for ( unsigned int i = 0; i < number_parameters; ++i )
  {
    for ( unsigned int j = 0; j < number_parameters; ++j )
    {
      differentialResiduals[i][j]->Write();
    }
  }

  for ( unsigned int i = 0; i < number_parameters; ++i )
  {
    residuals[i]->Write();
  }
  outfile->Close();

  MAUS::GlobalsManager::DeleteGlobals();

  std::cout << "All tests completed.\n" << std::endl;
  return 0;
}

//**************************************************************************************************
// Function Defintions
//**************************************************************************************************

void print_state( TMatrixD state )
{
  int num = state.GetNrows();
  for ( int i = 0; i < num; )
  {
    std::cout << std::setprecision( 6 ) << std::setw( 10 ) << std::setfill( ' ' ) << state( i++, 0 );
    std::cout << ( ( i == num ) ? ( "" ) : ( ", " ) );
  }
}


//**************************************************************************************************
// Class Function Defintions
//**************************************************************************************************

helixMaker::helixMaker( double Bz, double x0, double y0, double rad, double advance, double phi0 ) :
  _Bz( Bz ),
  _x0( x0 ),
  _y0( y0 ),
  _rad( rad ),
  _advance( advance ),
  _phi0( phi0 ),
  _z0( 0.0 )
{
}

TMatrixD helixMaker::getState( double z ) const 
{
  TMatrixD state( 5, 1 );

  double deltaZ = z - _z0;
  double s = _advance * deltaZ;
  double phi = _phi0 + s / _rad;
  double u = speed_of_light * 1.0 * _Bz;
  double pt = _rad * u;

  double x = _rad * std::cos( phi );
  double y = _rad * std::sin( phi );
  double px = - pt * std::sin( phi );
  double py = pt * std::cos( phi );
  double kappa = _advance / pt;

  state( 0, 0 ) = x;
  state( 1, 0 ) = px;
  state( 2, 0 ) = y;
  state( 3, 0 ) = py;
  state( 4, 0 ) = kappa;

  return state;
}

double helixMaker::getGradient( double z, unsigned int index0, unsigned int index1 )
{
  double deltaZ = z - _z0;
  double s = _advance * deltaZ;
  double deltaPhi = s / _rad;
//  double phi = _phi0;
  double u = speed_of_light * 1.0 * _Bz;
  double pt = _rad * u;

//  double x = _rad * std::cos( phi0 );
//  double y = _rad * std::sin( phi0 );
  double px = - pt * std::sin( _phi0 );
  double py = pt * std::cos( _phi0 );
//  double kappa = _advance / pt;

  switch( index0 )
  {
    case 0:
      switch( index1 ) 
      {
        case 0 :
          return 1.0;
          break;
        case 1 :
          return std::sin( deltaPhi ) / u;
          break;
        case 2 :
          return 0.0;
          break;
        case 3 :
          return - ( 1 - std::cos( deltaPhi ) ) / u;
          break;
        case 4 :
          return deltaZ * ( px * std::cos( deltaPhi ) - py * std::sin( deltaPhi ) );
          break;
        default :
          return 0.0;
      }
      break;
    case 1:
      switch( index1 ) 
      {
        case 0 :
          return 0.0;
          break;
        case 1 :
          return std::cos( deltaPhi );
          break;
        case 2 :
          return 0.0;
          break;
        case 3 :
          return - std::sin( deltaPhi );
          break;
        case 4 :
          return -u * deltaZ * ( px * std::sin( deltaPhi ) + py * std::cos( deltaPhi ) );
          break;
        default :
          return 0.0;
      }
      break;
    case 2:
      switch( index1 ) 
      {
        case 0 :
          return 0.0;
          break;
        case 1 :
          return ( 1 - std::cos( deltaPhi ) ) / u;
          break;
        case 2 :
          return 1.0;
          break;
        case 3 :
          return std::sin( deltaPhi ) / u;
          break;
        case 4 :
          return deltaZ * ( px * std::sin( deltaPhi ) + py * std::cos( deltaPhi ) );
          break;
        default :
          return 0.0;
      }
      break;
    case 3:
      switch( index1 ) 
      {
        case 0 :
          return 0.0;
          break;
        case 1 :
          return std::sin( deltaPhi );
          break;
        case 2 :
          return 0.0;
          break;
        case 3 :
          return std::cos( deltaPhi );
          break;
        case 4 :
          return u * deltaZ * ( px * std::cos( deltaPhi ) - py * std::sin( deltaPhi ) );
          break;
        default :
          return 0.0;
      }
      break;
    case 4:
      switch( index1 ) 
      {
        case 0 :
          return 0.0;
          break;
        case 1 :
          return 0.0;
          break;
        case 2 :
          return 0.0;
          break;
        case 3 :
          return 0.0;
          break;
        case 4 :
          return 1.0;
          break;
        default :
          return 0.0;
      }
      break;
    default :
      return 0.0;
  }
}

