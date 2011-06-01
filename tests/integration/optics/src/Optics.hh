#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"

#include "src/common/Config/MiceModule.hh"

#include "src/common/Interface/dataCards.hh"
#include "src/common/Interface/Squeak.hh"

#include "src/common/Optics/CovarianceMatrix.hh"
#include "src/common/Optics/TransferMap.hh"
#include "src/common/Optics/TransportManager.hh"

#include "G4RunManager.hh"

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

#include "TMinuit.h"

dataCards                MyDataCards("Simulation");
MICEEvent                simEvent   = MICEEvent();
bool                     settingRF;

void  writeEvent();
std::vector<MICEEvent*> ReadLoop( std::string filename, std::vector<std::string> classesToProcess ); //HACK for primarygeneratoraction

namespace Simulation
{
  int g_pid;
  //Load datacards
  dataCards*             LoadDataCards(int argc, char** argv);
  //Setup geometry etc; will add ellipses using matrix, make it a std::vector<CovarianceMatrix>() to ignore
  void                   SetupSimulation(MiceModule* root, std::vector<CovarianceMatrix> matrix);
  //Phase cavities
  MICEEvent*             PhaseCavities  (PhaseSpaceVector ref);
  //Run an event through the simulation
  MICEEvent*             RunSimulation  (PhaseSpaceVector psv);
  //Make a set of elliptical cylinders using the matrix RMS x,y and correlations - for visualisation
  //Not implemented (returns empty vector)
  std::vector<G4VSolid*> MakeCylinderEnvelope(std::vector<CovarianceMatrix> matrix, double InterpolationSize);
  //Generate an event from PrimaryGenerator
  PhaseSpaceVector       GetPrimaryGeneratorEvent();
  // Convert an event from a PSV to a MAUSPrimaryGeneratorAction particle
  MAUSPrimaryGeneratorAction::PGParticle ConvertToPGParticle(PhaseSpaceVector psv);
} /////////// Simulation Namespace End ///////////

namespace Optics
{
  std::map<const MiceModule*, TransferMap*>    g_module_to_map; //mapping of module name to transfer map
  //Build input ellipse
  CovarianceMatrix              EllipseIn(const MiceModule* root);
  //Build transfer maps and then push ellipse through the transfer maps
  //Append transfer maps and envelopes to tm_out and env_out
  void                          Envelope(const MiceModule* mod, std::vector<CovarianceMatrix>& env_out, std::vector<TransferMap*>& tm_out); 
  //Build set of vectors \vec{v_i} = \vec{ref}+(0,0,...,delta_i,0.,0,...)
  std::vector<PhaseSpaceVector> BuildHitsIn(PhaseSpaceVector ref, CLHEP::HepVector delta); 
  //Transport hitsIn through lattice; calculate Jacobian d(u_out)/d(u_in); build maps
  std::vector<TransferMap*>     TrackingDerivativeTransferMaps(std::vector<PhaseSpaceVector> hitsIn, bool referenceOnly); 
  //Polynomial fit transfer maps
  std::vector<TransferMap*>     PolynomialFitTransferMaps(PhaseSpaceVector reference, const MiceModule* root);
  //Transport hitsIn through lattice; calculate Jacobian d(u_out)/d(u_in); build maps
  std::vector<TransferMap*>     FieldDerivativeTransferMaps   (); 
  //Transport ellipse through maps
  std::vector<CovarianceMatrix> Transport(std::vector<TransferMap*> maps, CovarianceMatrix ellipse); 
  //Build vector of delta_x, delta_y etc from mod for calculation of Jacobian
  CLHEP::HepVector              GetDelta(const MiceModule* mod); 
  //Append PhaseSpaceVectors as MCHits in MICEEvent
  void                          AppendPSV(PhaseSpaceVector psv, int trackID, TransferMap& tm, MICEEvent& event);
  //A tag that uniquely IDs a station
  class StationId
  {
  public:
    //List of station types
    enum station_type {virt, special};
    //Default is virtual -999    
    StationId() : _type(virt), _station_number(-999) {}
    //Make a station id from a virtual hit
    StationId(int number, station_type tp) : _type(tp), _station_number(number) {};
    //Make a station id from a virtual hit
    StationId(VirtualHit hit);
    //Make a station id from a special hit
    StationId(SpecialHit hit); 
    //Make a station id from a MiceModule
    StationId(const MiceModule& mod) throw(Squeal);
    //Return string that names the station type (virtual, special, ...)
    std::string  StationTypeByString() const;
    station_type StationType()         const {return _type;}
    //Return an int that identifies the station uniquely for each type
    int          StationNumber()       const;
    friend bool operator < (const StationId& id1, const StationId& id2);

  private:
    static const std::string station_names[];    

    station_type _type;
    int          _station_number;

  };
  //sort by type and then by station number
  bool          operator < (const StationId& id1, const StationId& id2);
  //output format is '<string_station_type> <int_station_number>'  
  std::ostream& operator <<(std::ostream& out,    const StationId& id );
  //Add Hits in vector T to map of PhaseSpaceVectors -> StationId
  void AddHitsToMap(Json::Value Tracks, std::map< StationId,   std::vector<PhaseSpaceVector> >& map);
  //Add mapping of module name to transfer map to g_module_to_map
  void                    AddStationToMapping(StationId id, TransferMap* map);
  //Close the gzip file
  void                    CloseGzFile        ();
  //Function for PolyFit
  void                   SetPolyFitModule(const MiceModule* root);
  void                   PolyFitFunction (const double* psv_in, double* psv_out);
  StationId                                           g_polyfit_module;
  std::vector<PhaseSpaceVector>                       g_hitsIn;
  std::map<StationId, std::vector<PhaseSpaceVector> > g_hits;
  PhaseSpaceVector                                    g_mean;
  std::vector<TransferMap*> MakePolyfitMaps(std::vector<PhaseSpaceVector> hitsIn, std::map<StationId, std::vector<PhaseSpaceVector> > hits, int order);
  //Do the polyfit
  void DoPolyfit(const Function& func, const MiceModule& mod);

  //find the first element a_map that has element->second==value
  //note that multiple elements are allowed to hold this property so use with caution
  //throw an exception if the value can't be found
  template < class T_VALUE, class T_MAP, class T_ITER >
  T_ITER map_find_value(T_VALUE value, T_MAP a_map);
} ///////////////// Optics Namespace End ///////////////


namespace Output
{
  void MakeOutput(std::vector<CovarianceMatrix> matrix, std::vector<TransferMap*> tms, MiceModule* env_mod);
  void LongTextOutput(std::vector<CovarianceMatrix> matrix, std::vector<TransferMap*> tms, std::string outfile);
  void TextOutput(std::vector<CovarianceMatrix> matrix, std::string outfile);
  void RootOutput(std::vector<CovarianceMatrix> matrix, std::vector<TransferMap*> tms, std::string outfile, std::string comment);
  void  writeEvent();
  std::vector<MICEEvent*> ReadLoop( std::string filename, std::vector<std::string> classesToProcess ); //HACK for primarygeneratoraction
  //Close the gz output file
  void CloseGzFile();

} //////////////// Output Namespace End ////////////////

namespace Optimiser
{
  class Parameter{public: double value; double min; double max; double delta; bool fixed; std::string name;
                  Parameter();};
  //Build all parameters in optimiser
  std::vector<Parameter*> BuildParameters(const MiceModule* optimiser);
  //Build nth parameter; return NULL if not found
  Parameter*              BuildParameter (const MiceModule* mod, int n);
  //Find optimiser module, build and run minuit
  void   RunOptimiser(MiceModule* root);
  //Find parameters; pass to Minuit
  void   RunMinuit(const MiceModule* mod);
  //Wrapper for Minuit
  void   MinuitWrapper();
  //Run the optics package
  void   RunOptics(int& npar, double * gin, double& f, double* x, int flag);
  //Push parameters onto the MiceModules
  void   PushParameters(MiceModule* mod, std::vector<Parameter*> parameters);
  //Calculate scores based on optics output
  std::vector<double> GetScore();
  void GetScore(CovarianceMatrix mat, const MiceModule* mod, std::map<std::string, double>& scores);

} ///////////////// Optimise Namespace End /////////////
