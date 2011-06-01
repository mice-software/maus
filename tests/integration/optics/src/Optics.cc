//Optics includes


#include "TTree.h"
#include "TFile.h"
#include "G4VSolid.hh"

#include "json/json.h"

#include "Optics.hh"

#include "src/cpp_core/Utils/JsonWrapper.hh"
#include "src/common/Interface/MiceMaterials.hh"
#include "src/cpp_core/Simulation/MAUSPrimaryGeneratorAction.hh"
#include "src/cpp_core/Simulation/MAUSPrimaryGeneratorAction.hh"
#include "src/common/BeamTools/BTFieldConstructor.hh"
#include "src/common/Interface/SpecialHit.hh"
#include "src/common/Interface/MiceEventManager.hh"
#include "TransferMap.hh"
#include "src/common/Interface/MMatrixToCLHEP.hh"

void  writeEvent() {throw Squeal(Squeal::recoverable, "Not implemented", "writeEvent()");}
std::vector<MICEEvent*> ReadLoop( std::string filename, std::vector<std::string> classesToProcess ) 
{return Output::ReadLoop(filename, classesToProcess) ;}

namespace Simulation
{
bool                        settingRF  = false;
MICERun&                    simRun     = *MICERun::getInstance();
MAUSGeant4Manager*          g4Manager  = NULL;

dataCards* LoadDataCards(int argc, char** argv)
{
  if (argc > 1)
  {
    if (MyDataCards.readKeys(argv[1])==0)
    {
      std::string cardName(argv[1]);
      Squeak::mout(Squeak::error) << "Data Card file " << cardName << " not found" << std::endl;
      return NULL;
    }
    MICERun::getInstance()->DataCards = &MyDataCards;
  }
  else
  {
      Squeak::mout(Squeak::error) << "You should call this application using a command like:\napplication_name datacards_file\nI couldn't find any datacards." << std::endl;
      return NULL;
  }
  return &MyDataCards;
}

void SetupSimulation(MiceModule* root, std::vector<CovarianceMatrix> envelope)
{
  simRun.DataCards     = &MyDataCards;
  simRun.miceModule    = root;
  simRun.miceMaterials = new MiceMaterials();
  simRun.jsonConfiguration = new Json::Value
                                (JsonWrapper::StringToJson("{\"maximum_number_of_steps\":10000}")); // BUG
  Squeak::setStandardOutputs();
  fillMaterials(simRun);

  g4Manager = MAUSGeant4Manager::GetInstance();

  BTPhaser::Print(Squeak::mout(Squeak::info));
}

std::vector<G4VSolid*> MakeCylinderEnvelope(std::vector<CovarianceMatrix> matrix, double InterpolationSize)
{
  return std::vector<G4VSolid*>();
}

MICEEvent* PhaseCavities(PhaseSpaceVector ref)
{
  Squeak::mout(Squeak::debug) << "Rephasing cavities with reference particle " << ref << std::endl;
  BTPhaser::IsPhaseSet(false); //force to rephase
  simEvent = MICEEvent();
  if(BTPhaser::NumberOfCavities() == 0) return &simEvent;
  int nTries = 0;
  MAUSPrimaryGeneratorAction::PGParticle p = ConvertToPGParticle(ref);
  while(!BTPhaser::IsPhaseSet() && nTries < 5*BTPhaser::NumberOfCavities())  
  {
    g4Manager->GetPrimaryGenerator()->Push(p);
    g4Manager->GetRunManager()->BeamOn(1);
    nTries++;
    Squeak::mout(Squeak::info) << "." << std::flush;
  }
  Squeak::mout(Squeak::info) << std::endl;
  if(!BTPhaser::IsPhaseSet())
    throw(Squeal(Squeal::recoverable, "Error setting RF cavity phase", "Envelope::RunSimulation"));
  if(nTries > 1)
    Squeak::mout(Squeak::debug) << "Cavities were rephased. New fields:\n" << *(BTPhaser::GetGlobalField()) << std::endl;
  return &simEvent;
}

MICEEvent* RunSimulation(PhaseSpaceVector psv)
{
  simEvent = MICEEvent();
  BTPhaser::IsRefPart(true);
  if(psv.E()<0) return &simEvent;
  MAUSPrimaryGeneratorAction::PGParticle p = ConvertToPGParticle(psv);
  g4Manager->GetPrimaryGenerator()->Push(p);
  g4Manager->GetRunManager()->BeamOn(1);
  return &simEvent;
}

MAUSPrimaryGeneratorAction::PGParticle ConvertToPGParticle(PhaseSpaceVector psv) {
  MAUSPrimaryGeneratorAction::PGParticle p;
  p.x = psv.x();
  p.y = psv.y();
  p.z = psv.z();
  p.time = psv.t();
  p.px = psv.Px();
  p.py = psv.Py();
  p.pz = psv.Pz();
  p.energy = psv.E();
  p.pid = g_pid;
  p.seed = 0; //BUG
  return p;
}

} /////////// Simulation Namespace End ///////////


namespace Optics
{

CovarianceMatrix EllipseIn(const MiceModule* root)
{
  std::vector<const MiceModule*> envelope_mods = root->findModulesByPropertyExists("string", "EnvelopeType");
  if(envelope_mods.size() == 0) throw(Squeal(Squeal::recoverable, "No envelope modules detected in Optics", "Optics::EllipseIn"));
  if(envelope_mods.size() >  1) throw(Squeal(Squeal::recoverable, "More than 1 envelope modules detected in Optics", "Optics::EllipseIn"));
  Simulation::g_pid = envelope_mods[0]->propertyInt("Pid");
  return CovarianceMatrix(envelope_mods[0]);
}

void Envelope(const MiceModule* root, std::vector<CovarianceMatrix>& env_out, std::vector<TransferMap*>& tm_out)
{
  CovarianceMatrix mat = EllipseIn(root);
  g_mean = mat.GetMean();
  CLHEP::HepVector delta = GetDelta(root);

  std::string env_type = root->findModulesByPropertyExists("string", "EnvelopeType")[0]->propertyString("EnvelopeType");

  if(env_type == "Simple" || env_type == "TrackingDerivative")
  {
    Squeak::mout(Squeak::info) << "Finding TrackingDerivative envelope functions" << std::endl;
    std::vector<PhaseSpaceVector> hits = BuildHitsIn(g_mean, delta);
    tm_out = TrackingDerivativeTransferMaps(hits, false);
    Squeak::mout(Squeak::debug) << "TM_OUT SIZE " << tm_out.size() << std::endl;
  }
  else if(env_type == "PolyFit")
  {
    Squeak::mout(Squeak::info) << "Finding PolyFit envelope functions (this may take some time)" << std::endl;
    tm_out = PolynomialFitTransferMaps(mat.GetMean(), root);
  }
  else if(env_type == "FieldDerivative")
  {
    Squeak::mout(Squeak::info) << "Finding FieldDerivative envelope functions" << std::endl;
    tm_out = FieldDerivativeTransferMaps();
  }
  env_out = Transport(tm_out, mat);
  for(unsigned int i=0; i<tm_out.size(); i++) Squeak::mout(Squeak::debug) << env_out[i] << std::endl;
  for(unsigned int i=0; i<tm_out.size(); i++) Squeak::mout(Squeak::debug) << *tm_out[i] << std::endl;

}

CLHEP::HepVector GetDeltaMax(const MiceModule* root) {
  std::vector<const MiceModule*> envelope_mods = root->findModulesByPropertyExists("string", "EnvelopeType");
  if(envelope_mods.size() == 0) throw(Squeal(Squeal::recoverable, "No envelope modules detected in Optics", "Optics::GetDeltaMax"));
  if(envelope_mods.size() >  1) throw(Squeal(Squeal::recoverable, "More than 1 envelope modules detected in Optics", "Optics::GetDeltaMax"));
  const MiceModule* mod = envelope_mods[0];
  CLHEP::HepVector deltaMax(6);
  for(int i=1; i<=deltaMax.num_row(); i++) deltaMax(i) = numeric_limits<double>::max();
  try{deltaMax[0] = mod->propertyDouble("Delta_t_Max" );} catch(...) {}
  try{deltaMax[1] = mod->propertyDouble("Delta_E_Max" );} catch(...) {}
  try{deltaMax[2] = mod->propertyDouble("Delta_x_Max" );} catch(...) {}
  try{deltaMax[3] = mod->propertyDouble("Delta_Px_Max");} catch(...) {}
  try{deltaMax[4] = mod->propertyDouble("Delta_y_Max" );} catch(...) {}
  try{deltaMax[5] = mod->propertyDouble("Delta_Py_Max");} catch(...) {}
  return deltaMax;
}

CLHEP::HepVector GetDelta(const MiceModule* root) {
  std::vector<const MiceModule*> envelope_mods = root->findModulesByPropertyExists("string", "EnvelopeType");
  if(envelope_mods.size() == 0) throw(Squeal(Squeal::recoverable, "No envelope modules detected in Optics", "Optics::GetDelta"));
  if(envelope_mods.size() >  1) throw(Squeal(Squeal::recoverable, "More than 1 envelope modules detected in Optics", "Optics::GetDelta"));
  const MiceModule* mod = envelope_mods[0];
  CLHEP::HepVector delta(6);
  delta[0] = 0.1;
  for(int i=1; i<6; i++) delta[i] = 1.;
  try{delta[0] = mod->propertyDouble("Delta_t" );} catch(...) {}
  try{delta[1] = mod->propertyDouble("Delta_E" );} catch(...) {}
  try{delta[2] = mod->propertyDouble("Delta_x" );} catch(...) {}
  try{delta[3] = mod->propertyDouble("Delta_Px");} catch(...) {}
  try{delta[4] = mod->propertyDouble("Delta_y" );} catch(...) {}
  try{delta[5] = mod->propertyDouble("Delta_Py");} catch(...) {}
  return delta;
}

std::vector<PhaseSpaceVector> BuildHitsIn(PhaseSpaceVector ref, CLHEP::HepVector delta)
{
  std::vector<PhaseSpaceVector> hitsIn(13, ref);
  CLHEP::HepVector refVec = ref.getSixVector();
  for(int i=0; i<6; i++)
  {
    CLHEP::HepVector vec = refVec;
    vec[i] += delta[i];
    hitsIn[i+1].setSixVector(vec, ref.mass());
    vec[i] -= 2.*delta[i];
    hitsIn[i+7].setSixVector(vec, ref.mass());
    if(ref.Pz() < 0.) 
    {
      hitsIn[i+1].setPz( hitsIn[i+1].Pz()*(-1.) );
      hitsIn[i+7].setPz( hitsIn[i+7].Pz()*(-1.) );
    }
  }
  return hitsIn;
}

void AddHitsToMap(Json::Value Tracks, std::map< StationId,   std::vector<PhaseSpaceVector> >& map)
{
  Json::Value VHits = Tracks["virtual_hits"];
  for(size_t i=0; i<VHits.size(); i++) {
    Json::Value hit_v = VHits[i];
    PhaseSpaceVector psv;
    psv.setT(hit_v["time"].asDouble());
    psv.setQ(hit_v["charge"].asDouble());
    psv.setX(hit_v["position"]["x"].asDouble());    
    psv.setY(hit_v["position"]["y"].asDouble());    
    psv.setZ(hit_v["position"]["z"].asDouble());    
    psv.setPx(hit_v["momentum"]["x"].asDouble());    
    psv.setPy(hit_v["momentum"]["y"].asDouble());    
    psv.setPz(hit_v["momentum"]["z"].asDouble());    
    psv.setBx(hit_v["b_field"]["x"].asDouble());    
    psv.setBy(hit_v["b_field"]["y"].asDouble());    
    psv.setBz(hit_v["b_field"]["z"].asDouble());    
    psv.setEx(hit_v["e_field"]["x"].asDouble());    
    psv.setEy(hit_v["e_field"]["y"].asDouble());    
    psv.setEz(hit_v["e_field"]["z"].asDouble());
    double mass = hit_v["mass"].asDouble();
    psv.setE(sqrt(psv.P()*psv.P()+mass*mass));

    int stationNumber = hit_v["station_id"].asInt();
    StationId id(hit_v["station_id"].asInt(), StationId::virt);
    map[id].push_back(psv);
  }
}

std::vector<TransferMap*>      TrackingDerivativeTransferMaps(std::vector<PhaseSpaceVector> hitsIn, bool referenceOnly)
{
  Simulation::PhaseCavities(hitsIn[0]);
  g_hitsIn = hitsIn;
  g_hits   = std::map<Optics::StationId, std::vector<PhaseSpaceVector> >();

  std::vector<PhaseSpaceVector> psvByEvent;

  std::map< StationId,   std::vector<PhaseSpaceVector> > hits;
  for(unsigned int j=0; j<hitsIn.size() && (!referenceOnly || j==0); j++)
  {
    MICEEvent* event = Simulation::RunSimulation(hitsIn[j]);
    AddHitsToMap(MAUSGeant4Manager::GetInstance()->GetStepping()->GetTracks(), g_hits);
    event->specialHits    = std::vector<SpecialHit*>();
    event->virtualHits    = std::vector<VirtualHit*>();
    event->zustandVektors = std::vector<ZustandVektor*>();
    MAUSGeant4Manager::GetInstance()->GetStepping()->SetTracks(Json::Value(Json::objectValue));
    VirtualPlaneManager::StartOfEvent();
  }
  int order = 2;
  if(referenceOnly) order = 1;
  return MakePolyfitMaps(hitsIn, g_hits, order);
}

std::vector<TransferMap*>      PolynomialFitTransferMaps(PhaseSpaceVector reference, const MiceModule* root)
{
  //Algorithm
  //First do any set up like phasing cavities etc
  //Then look for the fit station
  //Make StationId for that station, set it to TrackingFunctionStationId
  //Make TrackingFunction as a Function type
  //Run SweepingLeastSquaresFit to define map at z (where PolynomialVector calls TrackingFunction.F())
  //Run LeastSquaresFit to define maps at all other z points
  //Return maps
  Simulation::PhaseCavities(reference);
  SetPolyFitModule(root);
  Function trackingFunction(PolyFitFunction, 6, 6);
  DoPolyfit(trackingFunction, *(root->findModulesByPropertyExists("string", "EnvelopeType")[0]));
  return MakePolyfitMaps(g_hitsIn, g_hits,  root->findModulesByPropertyExists("string", "EnvelopeType")[0]->propertyInt("PolynomialOrder")+1 );
}

std::vector<TransferMap*> MakePolyfitMaps(std::vector<PhaseSpaceVector> hitsIn, std::map<StationId, std::vector<PhaseSpaceVector> > hits, int order)
{
  typedef std::map< StationId, std::vector<PhaseSpaceVector> >::iterator vec_it;
  std::vector<TransferMap*> maps;
  for(vec_it it = hits.begin(); it!=hits.end(); it++) {
    if( it->second.size() != hitsIn.size() ) 
      Squeak::mout(Squeak::warning) << "Warning - derivatives may be screwy. I only got " << it->second.size() << " hits in station " << it->first << std::endl;
    else {
      try { 
        if(order == 0) maps.push_back( new TransferMap( CLHEP::HepMatrix(6,6,1), hitsIn[0], it->second[0], NULL) );
        else           maps.push_back( new TransferMap( TransferMapCalculator::GetPolynomialTransferMap(hitsIn, it->second, order ) ) );
        AddStationToMapping(it->first, maps.back());
      }
      catch(Squeal squee) {
        Squeak::mout(Squeak::warning) << "Failed to find " << order << " order transfer map for plane " << it->first << " from " << it->second.size() << " hits" << std::endl;
      }
    }
  }
  for(unsigned int i=0; i<maps.size(); i++) {maps[i]->IsCanonical(false);}

  return maps;
}

void DoPolyfit(const Function& func, const MiceModule& mod)
{
  CLHEP::HepVector delta    = GetDelta(&mod);
  CLHEP::HepVector deltaMax = GetDeltaMax(&mod);
  int    order            = mod.propertyIntThis   ("PolynomialOrder")+1;
  int    maxNumberOfSteps = 100;
  double chi2Max          = mod.propertyDoubleThis("Tolerance");
  double deltaFactor      = 2.;
  if(mod.propertyExistsThis("DeltaFactor",      "double" )) deltaFactor      = mod.propertyDoubleThis("DeltaFactor");
  if(mod.propertyExistsThis("MaxNumberOfSteps", "int"    )) maxNumberOfSteps = mod.propertyIntThis   ("MaxNumberOfSteps");
  if(deltaFactor      <= 1.) throw(Squeal(Squeal::recoverable, "DeltaFactor must be > 1",      "Optics::DoPolyfit"));
  if(maxNumberOfSteps <= 0 ) throw(Squeal(Squeal::recoverable, "MaxNumberOfSteps must be > 0", "Optics::DoPolyfit"));
	TransferMap tm = TransferMapCalculator::GetSweepingPolynomialTransferMap(&func, order, chi2Max, delta, deltaMax, deltaFactor, maxNumberOfSteps, g_mean, g_mean);
  Squeak::mout(Squeak::info) << "Found polynomial fit at order " << tm.GetOrder() << " with valid region ";
  for(int i=1; i<=delta.num_row(); i++) Squeak::mout(Squeak::info) << delta(i) << " ";
  MVector<m_complex> eig = CLHEP_to_MMatrix(tm.GetFirstOrderMap()).eigenvalues();
  Squeak::mout(Squeak::info)  << std::endl; 
  Squeak::mout(Squeak::debug) << "Eigenvalue modulus: ";
  for(size_t i=1; i<=eig.num_row(); i++) Squeak::mout(Squeak::debug) << re(eig(i)*conj(eig(i))) << " ";
  Squeak::mout(Squeak::debug) << " determinant: " << tm.GetFirstOrderMap().determinant() << "\nMap:";
  Squeak::mout(Squeak::debug) << tm << std::endl; 
}

void PolyFitFunction(const double* psv_in, double* psv_out)
{
  CLHEP::HepVector vec = g_mean.getSixVector();
  for(size_t i=0; i<6; i++) vec[i] += psv_in[i];
  double     mass  = ModuleConverter::PdgPidToMass(Simulation::g_pid);
  PhaseSpaceVector psvIn(vec, mass);
  psvIn.setZ(g_mean.z());
  MICEEvent* event = Simulation::RunSimulation(psvIn);
  g_hitsIn.push_back(psvIn);
  AddHitsToMap(MAUSGeant4Manager::GetInstance()->GetStepping()->GetTracks(), g_hits);
  PhaseSpaceVector psvOut = g_hits[g_polyfit_module].back();
  CLHEP::HepVector vecOut = psvOut.getSixVector();
  MAUSGeant4Manager::GetInstance()->GetStepping()->SetTracks(Json::Value());
  for(int i=0; i<vecOut.num_row(); i++) psv_out[i] = vecOut[i] - g_mean.getSixVector()[i];
}

void SetPolyFitModule(const MiceModule* root)
{
  bool                           foundModule = false;
  std::vector<const MiceModule*> polyFitModule = root->findModulesByPropertyExists("bool", "PolyFitDetector");
  if(polyFitModule.size() == 0) throw(Squeal(Squeal::recoverable, "Could not find PolyFit Detector", "Optics::Optics::SetPolyFitModule()") );
  for(size_t i=0; i<polyFitModule.size(); i++) 
    if(polyFitModule[i]->propertyBool("PolyFitDetector"))
    {
      if(!foundModule) g_polyfit_module = StationId(*polyFitModule[i]);
      else             throw(Squeal(Squeal::recoverable, "Found more than one PolyFit Module", "Optics::SetPolyFitModule()") );
    }
}

void  AddStationToMapping(StationId id, TransferMap* map)
{
  if(id.StationType() != StationId::virt) return;
  const MiceModule* mod = VirtualPlaneManager::GetModuleFromStationNumber(id.StationNumber());
  g_module_to_map[mod] = map;
}


const std::string StationId::station_names[2] = {"virtual", "special"};    

StationId::StationId(VirtualHit hit) :   _type(virt), _station_number(hit.GetStationNumber())
{}

StationId::StationId(SpecialHit hit) :   _type(special), _station_number(hit.GetStationNo())
{}

StationId::StationId(const MiceModule& mod) throw(Squeal)       :   _type(virt), _station_number(VirtualPlaneManager::GetStationNumberFromModule(&mod ))
{
  std::string sd = mod.propertyStringThis("SensitiveDetector");
  if(sd!="Envelope") 
    throw(Squeal(Squeal::recoverable, "Attempt to make a StationId when SensitiveDetector was "+sd+" - should be Envelope", "Optics::StationId()"));
}

bool          operator < (const StationId& id1, const StationId& id2)
{
  if(id1._type == id2._type) return id1._station_number < id2._station_number;
  return id1._type < id2._type;
}

std::string StationId::StationTypeByString() const
{
  return station_names[int(_type)];
}

int          StationId::StationNumber() const
{
  return _station_number;
}

std::ostream& operator <<(std::ostream& out, const StationId& id)
{
  out << id.StationTypeByString() << " " << id.StationNumber() << std::endl;
  return out;
}

std::vector<TransferMap*>     FieldDerivativeTransferMaps   ()
{
  
//SOMETHING HERE PLEASE
//TransferMap GetDerivativeTransferMap(double target_indie, PhaseSpaceVector x_in, const BTField* field, var indep, double step_size,
//                                                    std::vector<double> delta);
  return std::vector<TransferMap*>();
}



bool cov_comp(CovarianceMatrix a, CovarianceMatrix b) {return (a.GetMean().t() < b.GetMean().t());}

std::vector<CovarianceMatrix> Transport(std::vector<TransferMap*> maps, CovarianceMatrix ellipse)
{
  std::vector<CovarianceMatrix> covs(1, ellipse);
  for(unsigned int i=0; i<maps.size(); i++)
  {
    covs.push_back((*maps[i])*ellipse);
  }
  sort(covs.begin(), covs.end(), cov_comp);
  return covs;
}

template < class T_VALUE, class T_MAP, class T_ITER >
T_ITER map_find_value(T_VALUE value, T_MAP a_map)
{
  for(T_ITER it=a_map.begin(); it!=a_map.end(); it++ )
    if( value==it->second ) return it;
  return a_map.end();
}

void AppendPSV(PhaseSpaceVector psv, int trackID, TransferMap& tm, MICEEvent& event)
{
  typedef std::map<const MiceModule*, TransferMap*> mod_map;
  const MiceModule* mod = map_find_value<TransferMap*, mod_map, mod_map::iterator>(&tm, g_module_to_map)->first;
  int   stationID       = VirtualPlaneManager::GetStationNumberFromModule(mod);
  event.virtualHits.push_back(new VirtualHit( psv.virtualHit(trackID, stationID) ));
}



} ///////////////// Optics Namespace End ///////////////

namespace Output
{

void MakeOutput(std::vector<CovarianceMatrix> matrix, std::vector<TransferMap*> tms, const MiceModule* env_mod)
{
  try{TextOutput(matrix, env_mod->propertyString("ShortTextOutput"));} catch(...) {Squeak::mout(Squeak::debug) << "Failed to find short text file" << std::endl;}
  try{LongTextOutput(matrix, tms, env_mod->propertyString("LongTextOutput"));} catch(...) {Squeak::mout(Squeak::debug) << "Failed to find long text file" << std::endl;}
  try{RootOutput(matrix, tms, env_mod->propertyString("RootOutput"), "Output from G4MICE Optics");} catch(...) {Squeak::mout(Squeak::debug) << "Failed to find root file" << std::endl;}
}

void TextOutput(std::vector<CovarianceMatrix> matrix, std::string outfile)
{
  if(outfile == "") return;
  ofstream out(outfile.c_str());
  if(!out) throw(Squeal(Squeal::recoverable, "Failed to open "+outfile, "Optics::TextOutput"));
  for(unsigned int i=0; i<matrix.size(); i++)
    out << matrix[i].GetAnalysisPlaneBank() << std::endl;
  out.close();
}

void LongTextOutput(std::vector<CovarianceMatrix> matrix, std::vector<TransferMap*> tms, std::string outfile)
{
  std::string names[6] = {"t","E","x","Px","y","Py"};
  ofstream out(outfile.c_str());
  if(!out) throw(Squeal(Squeal::recoverable, "Failed to open "+outfile, "Optics::TextOutput"));

  out << std::scientific << std::setprecision(8);
  out << "mean_t mean_E mean_x mean_Px mean_y mean_Py mean_z  mean_Pz planeType planeNumber statisticalWeight betaTrans alphaTrans gammaTrans phiTrans angularMommentum amplitudePzCovariance beta_x alpha_x gamma_x phi_x  beta_y alpha_y gamma_y phi_y beta_t alpha_t gamma_t dispersion_x dispersion_y emittance_x emittance_y emittance_t emittance4D emittance6D" << std::endl;
  for(int j=0; j<6; j++)
  {
    for(int k=0; k<6; k++) out << "Covariance("<< names[j] << "," << names[k] << ") ";
    out << std::endl;
  }

  for(unsigned int i=0; i<matrix.size(); i++)
  {
    AnalysisPlaneBank f_theBank = matrix[i].GetAnalysisPlaneBank();

    CovarianceMatrix cov = matrix[i];
    double alpha_x=cov.GetTwoDAlpha(1), gamma_x=cov.GetTwoDGamma(1), alpha_y=cov.GetTwoDAlpha(2),     gamma_y=cov.GetTwoDGamma(2), 
           alpha_t=cov.GetTwoDAlpha(0), gamma_t=cov.GetTwoDGamma(0), alpha_trans=cov.GetAlphaTrans(), gamma_trans=cov.GetGammaTrans(),
           disp_x=cov.GetDispersion(1), disp_y=cov.GetDispersion(1);
    double phi_trans(0), phi_x(0), phi_y(0), phi_t(0);
    if(i>0)
    {
      try{phi_t     = tms[i-1]->PhaseAdvance(0);} catch(Squeal squee) {}
      try{phi_x     = tms[i-1]->PhaseAdvance(1);} catch(Squeal squee) {}
      try{phi_y     = tms[i-1]->PhaseAdvance(2);} catch(Squeal squee) {}
      phi_trans = (phi_x+phi_y)/2.;
    }
    for(int j=0; j<6; j++)
    out << f_theBank.sums[j] << " ";
    out << f_theBank.z << " " << f_theBank.pz << " " << f_theBank.planeType << " " << f_theBank.planeNumber << " " << f_theBank.weight << " " 
        << f_theBank.beta_p << " " << alpha_trans << " " << gamma_trans << " " << phi_trans << " " << f_theBank.l_can << " " 
        << f_theBank.amplitudePzCovariance << " " << f_theBank.beta[1] << " " << alpha_x << " " << gamma_x << " " << phi_x << " " 
        << f_theBank.beta[2] << " " << alpha_y << " " << gamma_y << " " << phi_y << " " << f_theBank.beta[0] << " " 
        << alpha_t << " " << gamma_t << " " << disp_x << " " << disp_y << " " << f_theBank.em2d[1] << " " << f_theBank.em2d[2] << " "
        << f_theBank.em2d[0]  << " " << f_theBank.em4dXY << " " << f_theBank.em6dTXY << std::endl;
    for(int j=0; j<6; j++)
    {
      for(int k=0; k<6; k++)
        out << matrix[i].GetKineticCovariances()[j][k] << " ";
      out << std::endl;
    }
  }

}


void RootOutput(std::vector<CovarianceMatrix> matrix, std::vector<TransferMap*> tms, std::string outfile, std::string comment)
{
  TFile rootfile((outfile).c_str(),"RECREATE",comment.c_str());
  TTree * f_theTree = new TTree("envelope_data", comment.c_str());
  std::string names[6] = {"t","E","x","Px","y","Py"};

  f_theTree->Branch("mean_t", 0,"mean_t/D");
  f_theTree->Branch("mean_E", 0,"mean_E/D");
  f_theTree->Branch("mean_x", 0,"mean_x/D");
  f_theTree->Branch("mean_Px",0,"mean_Px/D");
  f_theTree->Branch("mean_y", 0,"mean_y/D");
  f_theTree->Branch("mean_Py",0,"mean_Py/D");
  f_theTree->Branch("mean_z", 0,"mean_z/D");
  f_theTree->Branch("mean_Pz",0,"mean_Pz/D");
  for(int i=0; i<6; i++)
          for(int j=0; j<6; j++)
                  f_theTree->Branch(("covariance_"+names[i]+"_"+names[j]).c_str(),0,("covariance_"+names[i]+"_"+names[j]+"/D").c_str());
  f_theTree->Branch("planeType",0,"planeType/I");
  f_theTree->Branch("planeNumber",0,"planeNumber/I");
  f_theTree->Branch("statisticalWeight",0,"statisticalWeight/D");
  f_theTree->Branch("betaTrans", 0,"betaTrans/D");
  f_theTree->Branch("alphaTrans",0,"alphaTrans/D");
  f_theTree->Branch("gammaTrans",0,"gammaTrans/D");
  f_theTree->Branch("phiTrans",0,"phiTrans/D");
  f_theTree->Branch("angularMomentum",0,"angularMomentum/D");
  f_theTree->Branch("amplitudePzCovariance",0,"amplitudePzCovariance/D");
  f_theTree->Branch("beta_x", 0,"beta_x/D");
  f_theTree->Branch("alpha_x",0,"alpha_x/D");
  f_theTree->Branch("gamma_x",0,"gamma_x/D");
  f_theTree->Branch("beta_y", 0,"beta_y/D");
  f_theTree->Branch("alpha_y",0,"alpha_y/D");
  f_theTree->Branch("gamma_y",0,"gamma_y/D");
  f_theTree->Branch("beta_t", 0,"beta_t/D");
  f_theTree->Branch("alpha_t",0,"alpha_t/D");
  f_theTree->Branch("gamma_t",0,"gamma_t/D");
  f_theTree->Branch("phi_t",0,"phi_t/D");
  f_theTree->Branch("phi_x",0,"phi_t/D");
  f_theTree->Branch("phi_y",0,"phi_t/D");
  f_theTree->Branch("emittance_x",0,"emittance_x/D");
  f_theTree->Branch("emittance_y",0,"emittance_y/D");
  f_theTree->Branch("emittance_t",0,"emittance_t/D");
  f_theTree->Branch("emittance4D",0,"emittance4D/D");
  f_theTree->Branch("emittance6D",0,"emittance6D/D");
  f_theTree->Branch("dispersion_x",0,"dispersion_x/D");
  f_theTree->Branch("dispersion_y",0,"dispersion_y/D");

  for(unsigned int i=0; i<matrix.size(); i++)
  {
    AnalysisPlaneBank f_theBank = matrix[i].GetAnalysisPlaneBank();
    std::string names[6] = {"t","E","x","Px","y","Py"};
    double covarianceMatrix[6][6];
    for(int j=0; j<6; j++)
        for(int k=0; k<6; k++)
            covarianceMatrix[j][k] = matrix[i].GetKineticCovariances()[j][k];

    CovarianceMatrix cov = matrix[i];
    double alpha_x=cov.GetTwoDAlpha(1), gamma_x=cov.GetTwoDGamma(1), alpha_y=cov.GetTwoDAlpha(2),     gamma_y=cov.GetTwoDGamma(2), 
           alpha_t=cov.GetTwoDAlpha(0), gamma_t=cov.GetTwoDGamma(0), alpha_trans=cov.GetAlphaTrans(), gamma_trans=cov.GetGammaTrans(),
           disp_x=cov.GetDispersion(1), disp_y=cov.GetDispersion(1);
    double phi_trans(0), phi_x(0), phi_y(0), phi_t(0);
    if(i>0)
    {
      try{phi_t     = tms[i-1]->PhaseAdvance(0);} catch(Squeal squee) {}
      try{phi_x     = tms[i-1]->PhaseAdvance(1);} catch(Squeal squee) {}
      try{phi_y     = tms[i-1]->PhaseAdvance(2);} catch(Squeal squee) {}
      phi_trans = (phi_x+phi_y)/2.;
    }
    //Set the address to the variables in the plane bank
    //Could use f_theTree->SetBranchAddress("branchName",&foo);
    for(int j=0; j<6; j++)
    {
            std::string name_str = "mean_"+names[j];
            f_theTree->GetBranch(name_str.c_str())->SetAddress(&f_theBank.sums[j]);
    }
    f_theTree->GetBranch("mean_z")->SetAddress (&f_theBank.z);
    f_theTree->GetBranch("mean_Pz")->SetAddress(&f_theBank.pz);
    for(int j=0; j<6; j++)
            for(int k=0; k<6; k++)
                    f_theTree->GetBranch(("covariance_"+names[j]+"_"+names[k]).c_str())->SetAddress(&covarianceMatrix[j][k]);
    f_theTree->GetBranch("planeType")->SetAddress  (&f_theBank.planeType);
    f_theTree->GetBranch("planeNumber")->SetAddress(&f_theBank.planeNumber);
    f_theTree->GetBranch("statisticalWeight")->SetAddress(&f_theBank.weight);
    f_theTree->GetBranch("betaTrans" )->SetAddress(&f_theBank.beta_p);
    f_theTree->GetBranch("alphaTrans")->SetAddress(&alpha_trans);
    f_theTree->GetBranch("gammaTrans")->SetAddress(&gamma_trans);
    f_theTree->GetBranch("phiTrans")->SetAddress(&phi_trans);
    f_theTree->GetBranch("angularMomentum")->SetAddress(&f_theBank.l_can);
    f_theTree->GetBranch("amplitudePzCovariance")->SetAddress(&f_theBank.amplitudePzCovariance);
    f_theTree->GetBranch("beta_x" )->SetAddress(&f_theBank.beta[1]);
    f_theTree->GetBranch("alpha_x")->SetAddress(&alpha_x);
    f_theTree->GetBranch("gamma_x")->SetAddress(&gamma_x);
    f_theTree->GetBranch("phi_x")->SetAddress(&phi_x);
    f_theTree->GetBranch("beta_y" )->SetAddress(&f_theBank.beta[2]);
    f_theTree->GetBranch("alpha_y")->SetAddress(&alpha_y);
    f_theTree->GetBranch("gamma_y")->SetAddress(&gamma_y);
    f_theTree->GetBranch("phi_y")->SetAddress(&phi_y);
    f_theTree->GetBranch("beta_t" )->SetAddress(&f_theBank.beta[0]);
    f_theTree->GetBranch("alpha_t")->SetAddress(&alpha_t);
    f_theTree->GetBranch("gamma_t")->SetAddress(&gamma_t);
    f_theTree->GetBranch("dispersion_x")->SetAddress(&disp_x);
    f_theTree->GetBranch("dispersion_y")->SetAddress(&disp_y);
    f_theTree->GetBranch("emittance_x")->SetAddress(&f_theBank.em2d[1]);
    f_theTree->GetBranch("emittance_y")->SetAddress(&f_theBank.em2d[2]);
    f_theTree->GetBranch("emittance_t")->SetAddress(&f_theBank.em2d[0]);
    f_theTree->GetBranch("emittance4D")->SetAddress(&f_theBank.em4dXY);
    f_theTree->GetBranch("emittance6D")->SetAddress(&f_theBank.em6dTXY);
    f_theTree->Fill();

  }

//  f_theTree->Write();
  rootfile.Write();
  rootfile.Close();
}

int evtno=0;
void  writeEvent()
{
  if(settingRF) return;
  ++evtno;
  throw Squeal(Squeal::recoverable, "Not implemented", "Output::writeEvent()");

  clearEvent( simEvent );
}

//BUG!!!
//Hack for now - ReadLoop will read in persistency objects from filename
//classesToProcess should contain objects from PersistClasses::Blah
//if classes to Process is empty, will just get everything
//This is for use in MICEPrimaryGeneratorAction and will likely be removed for mice-2-4-0
std::vector<MICEEvent*> ReadLoop( std::string filename, std::vector<std::string> classesToProcess )
{
  throw Squeal(Squeal::recoverable, "Not implemented", "Output::ReadLoop");
}


void CloseGzFile()
{
  throw Squeal(Squeal::recoverable, "Not implemented", "Output::CloseGzFile");
  return;
}


} ///////////////// Output Namespace End ////////////////


namespace Optimiser
{
  //Variables
  MiceModule*             g_root_mod = NULL;
  std::vector<Parameter*> g_parameters;
  std::vector<const MiceModule*> g_score_mod;
  bool                       g_redo_tracking = true;
  bool                       g_rebuild_simulation = true;
  std::vector<TransferMap*>  g_transfer_maps;

  //Find optimiser module, build and run minuit
  void RunOptimiser(MiceModule* root)
  {
    g_root_mod  = root;
    g_score_mod = root->findModulesByPropertyString("SensitiveDetector", "Envelope");
    std::vector<const MiceModule*> optimiser_mods = root->findModulesByPropertyExists("string", "Optimiser");
    if(optimiser_mods.size() < 1) return;
    if(optimiser_mods.size() > 1) throw(Squeal(Squeal::recoverable, "Multiple optimiser modules detected when only 1 is allowed", "Optimiser::Optimiser"));
    if(optimiser_mods[0]->propertyString("Optimiser") == "Minuit")
      RunMinuit(optimiser_mods[0]);
    else throw(Squeal(Squeal::recoverable, "Did not recognise optimiser option "+optimiser_mods[0]->propertyString("Optimiser"), "Optics::RunOptimiser") );
  }

  //Return nth parameter in mod; return NULL if not found
  Parameter::Parameter() : value(0), min(0), max(0), delta(1), fixed(false), name("") {}
  std::vector<Parameter*> BuildParameters(const MiceModule* optimiser)
  {
    //Extract parameters from mod
    std::vector<Parameter*> parameters;
    do parameters.push_back( BuildParameter(optimiser, parameters.size()+1) );
    while(parameters.back() != NULL );
    parameters.pop_back(); //last one is NULL
    return parameters;
  }

  Parameter* BuildParameter(const MiceModule* mod, int n)
  {
      Parameter* par = new Parameter();
      std::stringstream ss; 
      ss << n;
      std::string p    = "Parameter"+ss.str()+"_";
      std::string name = "";
      try{par->name  = mod->propertyString(p+"Name"); par->value = mod->propertyDouble(p+"Start");} catch(...) 
      {
        if(par->name != "") 
        {delete par; throw(Squeal(Squeal::recoverable, "In optimisation "+p+"Name defined but not "+p+"Start", "Optimiser::BuildParameter"));}
        if(fabs(par->value) > 1e-9) 
        {delete par; throw(Squeal(Squeal::recoverable, "In optimisation "+p+"Start defined but not "+p+"Name", "Optimiser::BuildParameter"));}
        else delete par;
        return NULL;
      }

      try{par->min   = mod->propertyDouble(p+"Min");} catch(...) {}
      try{par->max   = mod->propertyDouble(p+"Max");} catch(...) {}
      try{par->delta = mod->propertyDouble(p+"Delta");} catch(...) {}
      try{par->fixed = mod->propertyBool  (p+"Fixed");} catch(...) {}
      return par;
  }

  //Build and run Minuit
  void RunMinuit(const MiceModule* mod)
  {
    int                     errFlag(0);
    g_parameters = BuildParameters(mod);
    //Send parameters to minuit
    TMinuit* minimiser = new TMinuit(g_parameters.size());
    for(unsigned int i=0; i<g_parameters.size(); i++)
    {
      minimiser->mnparm(i, g_parameters[i]->name.c_str(), g_parameters[i]->value, g_parameters[i]->delta, g_parameters[i]->min, g_parameters[i]->max, errFlag);
      if(g_parameters[i]->fixed) minimiser->FixParameter(i); 
    }
    try{g_rebuild_simulation = mod->propertyBool("RebuildSimulation");} catch(Squeal squee) {}
    try{g_redo_tracking = mod->propertyBool("RedoTracking");} catch(Squeal squee) {}
    //Final setup
    minimiser->SetFCN(RunOptics);
    double argList[2];
    argList[0] = mod->propertyDouble("StartError");
    minimiser->mnexcm("SET ERR", argList, 1, errFlag);
    //run minimiser
    argList[0] = mod->propertyInt   ("NumberOfTries");
    argList[1] = mod->propertyDouble("EndError");
    minimiser->mnexcm(mod->propertyString("Algorithm").c_str(),  argList, 2, errFlag);
    delete minimiser;
  }

  //Run the optics calculation
  void RunOptics(int& npar, double * gin, double& f, double* x, int flag)
  {
    f = 0;
    for(int i=0; i<npar; i++) g_parameters[i]->value = x[i];
    PushParameters(g_root_mod, g_parameters);
    if(g_rebuild_simulation)
    {
      Squeak::mout(Squeak::debug) << "Rebuilding fields" << std::endl;
      BTFieldConstructor*   field   = (BTFieldConstructor*)BTPhaser::GetGlobalField();
      BTFieldGroup*         mfield  = (BTFieldGroup*)field->GetMagneticField();
      BTFieldGroup*         emfield = (BTFieldGroup*)field->GetElectroMagneticField();
      std::vector<BTField*> field_v = mfield->GetFields();
      for(std::vector<BTField*>::iterator it = field_v.begin(); it!=field_v.end(); it++)
        mfield->Erase((*it), false);
      field_v = emfield->GetFields();
      for(std::vector<BTField*>::iterator it = field_v.begin(); it!=field_v.end(); it++)
        if(*it != mfield) emfield->Erase((*it), false);
      Squeak::mout(Squeak::debug) << "Erased fields" << std::endl;
      mfield->Close();
      emfield->Close();
      Squeak::mout(Squeak::debug) << "Deleted fields" << std::endl;
      field->BuildFields(g_root_mod);
      BTPhaser::Print(Squeak::mout(Squeak::debug)); 
    }
    std::vector<double> score = GetScore();
    for(int i=0; i<int(g_parameters.size()); i++)
      Squeak::mout(Squeak::info) << g_parameters[i]->name << ": " << g_parameters[i]->value << " ";
    Squeak::mout(Squeak::info) << " ** ";
    for(int i=0; i<int(score.size()); i++)
      Squeak::mout(Squeak::info) << "Score" << i+1 << ": " << score[i] << " ";
    for(int i=0; i<int(score.size()); i++) f += score[i];
    Squeak::mout(Squeak::info) << " ** Total: " << f << std::endl;
  }

  //Recursively push parameters onto the MiceModules
  void PushParameters(MiceModule* mod, std::vector<Parameter*> parameters)
  {
    std::vector<MiceModule*> daughters = mod->allDaughters();
    for(int i=0; i<int(daughters.size()); i++)
    {
      for(int j=0; j<int(parameters.size()); j++) 
        daughters[i]->addParameter(parameters[j]->name, parameters[j]->value);
      PushParameters(daughters[i], parameters);
    }
  }

  //Calculate scores based on optics output
  std::vector<double> GetScore()
  {
    //Find the virtual plane number corresponding to a score module
    //Get the covariance matrix corresponding to the virtual plane
    //extract score data
    //push onto parameter list for optimiser module
    //calculate scores
    std::vector<double>           total_score;
    MiceModule*                   opt_mod     = g_root_mod->findModulesByPropertyExistsNC("string", "Optimiser")[0];
    CovarianceMatrix              ellipse_in  = Optics::EllipseIn(g_root_mod);
    bool referenceOnly      = false;
    if(opt_mod->propertyExistsThis("ReferenceOnly","bool")) referenceOnly = opt_mod->propertyBool("ReferenceOnly");
    if(g_transfer_maps.size() < 1 || g_redo_tracking) //if the field map could have changed or the tms are not yet defined
    {
      Optics::g_module_to_map = std::map<const MiceModule*, TransferMap*>();
      g_transfer_maps = Optics::TrackingDerivativeTransferMaps( Optics::BuildHitsIn(ellipse_in.GetMean(), Optics::GetDelta(g_root_mod)), referenceOnly );
    }
    std::map<std::string, double> scores;
    for(int i=0; i<int(g_score_mod.size()); i++)
    {
      if(Optics::g_module_to_map.find(g_score_mod[i]) == Optics::g_module_to_map.end())
        throw( Squeal(Squeal::recoverable, "No transfer matrix for module "+g_score_mod[i]->fullName()+" needed to calculate score. Tracking out of bounds?", "Optics::GetScore") );
      TransferMap* tm_i  = Optics::g_module_to_map[g_score_mod[i]];
      GetScore((*tm_i)*ellipse_in, g_score_mod[i], scores);
    }
    for(std::map<std::string, double>::iterator it=scores.begin(); it!=scores.end(); it++)
    {
      Squeak::mout(Squeak::debug) << "Setting score " << it->first << " " << it->second << std::endl;
      opt_mod->addParameter(it->first, it->second);
    }
    int index = 2;
    total_score.push_back(opt_mod->propertyDouble("Score1"));//check that Score1 exists
    while(index > 0)
    {
      std::stringstream ss;
      ss << "Score" << index;
      if(opt_mod->propertyExistsThis(ss.str(), "double")) total_score.push_back(opt_mod->propertyDouble(ss.str()));
      else break; 
      index++;
    } //while
    for(int i=0; i<int(g_transfer_maps.size()) && g_redo_tracking; i++) delete g_transfer_maps[i];
    return total_score;
  }

  //keep going until we run out of score variables, when we will get a "property not found" exception... return
  void GetScore(CovarianceMatrix mat, const MiceModule* mod, std::map<std::string, double>& scores)
  {
    int index=0;
    while(true)
    {
      index++;
      std::stringstream ss;
      ss << "EnvelopeOut" << index << "_";
      std::string name;
      try{ name = mod->propertyString(ss.str()+"Name"); }
      catch(Squeal squee) {return;}
      std::string var  = mod->propertyString(ss.str()+"Type");
      try{ var += " "+mod->propertyString(ss.str()+"Variable");} catch(...) {;}
      for(int i=0; i<int(var.size()); i++)
        if(var[i] == ',' || var[i] == '(' || var[i] == ')') var[i] = ' ';
      scores[name]     = mat.FullVariable(var);
    }//while
  }

} ///////////////// Optimise Namespace End /////////////



int main(int argc, char **argv)
{
  try{
  //Some global setup
  if( Simulation::LoadDataCards(argc, argv) == NULL ) exit(1);
  Squeak::setStandardOutputs();
  Squeak::mout() << "//============\\\\" << std::endl;
  Squeak::mout() << "||   Optics   ||" << std::endl;
  Squeak::mout() << "\\\\============//" << std::endl;
  Squeak::mout(Squeak::info) << "Parsing control files" << std::endl;
  MiceModule* root   = new MiceModule( MyDataCards.fetchValueString( "MiceModel" ) );
  std::vector<MiceModule*> optim = root->findModulesByPropertyExistsNC("string", "Optimiser");
  if(optim.size()>0)
  {
    Optimiser::g_parameters = Optimiser::BuildParameters(optim[0]);
    Optimiser::PushParameters(root, Optimiser::g_parameters);
  }
  Squeak::mout(Squeak::info) << "Building geometry" << std::endl;
  Simulation::SetupSimulation(root, std::vector<CovarianceMatrix>());

  //Try to run the optimiser
  Squeak::mout(Squeak::info) << "Looking for optimisation requests" << std::endl;
  Optimiser::RunOptimiser(root);
  //Run an envelope through
  Squeak::mout(Squeak::info) << "Running beam envelope through" << std::endl;
  std::vector<CovarianceMatrix>  envelope;
  std::vector<TransferMap*>      tms;
  Optics::Envelope(root, envelope, tms);
  //Write output
  Output::MakeOutput(envelope, tms, root->findModulesByPropertyExists("string", "EnvelopeType")[0]);
  

  for(int i=0; i<int(tms.size()); i++) delete tms[i];

  Squeak::mout(Squeak::info) << "done" << std::endl;
  }
  catch(Squeal exc)         {Squeak::mout(Squeak::error) << "Caught Squeal        " << std::endl; exc.Print();}
  catch(std::exception exc) {Squeak::mout(Squeak::error) << "Caught std::exception" << std::endl; Squeak::mout(Squeak::error) << exc.what() << std::endl;}
  exit(0);
}

