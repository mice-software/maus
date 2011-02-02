//TofCable.cc
//
//Code to read in the connection information to allow the decoding of hit information 
//from channel space (Module Type, Crate Number, Module Number, Channel Number) into physical 
//space (station, plane, slab, pmt)
//
//Based on M.Ellis VlpcCable.cc
//
//A.Fish 3rd October 2005

 #include "TofCable.hh"

 #include <iostream>

TofCable::TofCable( std::string file )
{
  miceMemory.addNew( Memory::TofCable ); 	
	
  // first get the environment variable MICEFILES as this is where the model files are kept
  char* MICEFILES = getenv( "MICEFILES" );

  // the full name of the input file:
  std::string fnam = std::string( MICEFILES ) + "/Cabling/" + file;

  std::ifstream inf( fnam.c_str() );

  std::string line, tmp;

  // read in number of variables
  getline( inf, line);
  std::istringstream ist( line.c_str() );
  ist >> numVariables;

  //Make the class' vector<int>'s as long as there are lines in the cabling text file
  moduleType.resize( numVariables );
  crateNum  .resize( numVariables );
  moduleNum .resize( numVariables );
  chan      .resize( numVariables );
  stationNum.resize( numVariables );
  planeNum  .resize( numVariables );
  slabNum   .resize( numVariables );
  pmtNum    .resize( numVariables );

  // read in the module type, crateNum, module number, channel, station number, plane number, slab number, pmt number
  for( int i = 0; i < numVariables; ++i)
  {
    std::string tmp;
    getline( inf, line );
    std::istringstream ist1( line.c_str() );

    ist1 >> moduleType[i] >> crateNum[i] >> moduleNum[i] >> chan[i] >> stationNum[i] >> planeNum[i] >> slabNum[i] >> pmtNum[i];
  }

  inf.close();
}
