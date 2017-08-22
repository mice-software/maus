// MAUS WARNING: THIS IS LEGACY CODE.
// FillMaterials.cc
//
// Code to create G4Material instances for each material needed in MICE and store them in the MiceMaterial object
// that is found in the MICERun
//
// 25th May 2006
//
// Malcolm Ellis

#include "Simulation/FillMaterials.hh"

#include "Interface/MiceMaterials.hh"
#include "Interface/dataCards.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4IonisParamMat.hh"
#include "Geant4/G4Element.hh"

#include "Geant4/G4NistManager.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include <math.h>

MiceMaterials* fillMaterials(MiceMaterials* materials_list)
{
  if( ! materials_list )
    materials_list = new MiceMaterials();

  // fill materials from the list of NIST materials

  std::string materials[286] = {
  "H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar","K","Ca","Sc","Ti","V",
  "Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr","Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru",
  "Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe","Cs","Ba","La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb",
  "Dy","Ho","Er","Tm","Yb","Lu","Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn",
  "Fr","Ra","Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf",
  "A-150_TISSUE","ACETONE","ACETYLENE","ADENINE","ADIPOSE_TISSUE_ICRP","AIR","ALANINE","ALUMINUM_OXIDE",
  "AMBER","AMMONIA","ANILINE","ANTHRACENE","B-100_BONE","BAKELITE","BARIUM_FLUORIDE","BARIUM_SULFATE",
  "BENZENE","BERYLLIUM_OXIDE","BGO","BLOOD_ICRP","BONE_COMPACT_ICRU","BONE_CORTICAL_ICRP","BORON_CARBIDE",
  "BORON_OXIDE","BRAIN_ICRP","BUTANE","N-BUTYL_ALCOHOL","C-552","CADMIUM_TELLURIDE","CADMIUM_TUNGSTATE",
  "CALCIUM_CARBONATE","CALCIUM_FLUORIDE","CALCIUM_OXIDE","CALCIUM_SULFATE","CALCIUM_TUNGSTATE","CARBON_DIOXIDE",
  "CARBON_TETRACHLORIDE","CELLULOSE_CELLOPHANE","CELLULOSE_BUTYRATE","CELLULOSE_NITRATE","CERIC_SULFATE",
  "CESIUM_FLUORIDE","CESIUM_IODIDE","CHLOROBENZENE","CHLOROFORM","CONCRETE","CYCLOHEXANE","12-DICHLOROBENZENE",
  "DICHLORODIETHYL_ETHER","12-DICHLOROETHANE","DIETHYL_ETHER","NN-DIMETHYL_FORMAMIDE","DIMETHYL_SULFOXIDE",
  "ETHANE","ETHYL_ALCOHOL","ETHYL_CELLULOSE","ETHYLENE","EYE_LENS_ICRP","FERRIC_OXIDE","FERROBORIDE",
  "FERROUS_OXIDE","FERROUS_SULFATE","FREON-12","FREON-12B2","FREON-13","FREON-13B1","FREON-13I1",
  "GADOLINIUM_OXYSULFIDE","GALLIUM_ARSENIDE","GEL_PHOTO_EMULSION","Pyrex_Glass","GLASS_LEAD","GLASS_PLATE",
  "GLUCOSE","GLUTAMINE","GLYCEROL","GUANINE","GYPSUM","N-HEPTANE","N-HEXANE","KAPTON","LANTHANUM_OXYBROMIDE",
  "LANTHANUM_OXYSULFIDE","LEAD_OXIDE","LITHIUM_AMIDE","LITHIUM_CARBONATE","LITHIUM_FLUORIDE","LITHIUM_HYDRIDE",
  "LITHIUM_IODIDE","LITHIUM_OXIDE","LITHIUM_TETRABORATE","LUNG_ICRP","M3_WAX","MAGNESIUM_CARBONATE",
  "MAGNESIUM_FLUORIDE","MAGNESIUM_OXIDE","MAGNESIUM_TETRABORATE","MERCURIC_IODIDE","METHANE","METHANOL",
  "MIX_D_WAX","MS20_TISSUE","MUSCLE_SKELETAL_ICRP","MUSCLE_STRIATED_ICRU","MUSCLE_WITH_SUCROSE",
  "MUSCLE_WITHOUT_SUCROSE","NAPHTHALENE","NITROBENZENE","NITROUS_OXIDE","NYLON-8062","NYLON-6/6",
  "NYLON-6/10","NYLON-11_RILSAN","OCTANE","PARAFFIN","N-PENTANE","PHOTO_EMULSION","PLASTIC_SC_VINYLTOLUENE",
  "PLUTONIUM_DIOXIDE","POLYACRYLONITRILE","POLYCARBONATE","POLYCHLOROSTYRENE","POLYETHYLENE","MYLAR",
  "PLEXIGLASS","POLYOXYMETHYLENE","POLYPROPYLENE","POLYSTYRENE","TEFLON","POLYTRIFLUOROCHLOROETHYLENE",
  "POLYVINYL_ACETATE","POLYVINYL_ALCOHOL","POLYVINYL_BUTYRAL","POLYVINYL_CHLORIDE","POLYVINYLIDENE_CHLORIDE",
  "POLYVINYLIDENE_FLUORIDE","POLYVINYL_PYRROLIDONE","POTASSIUM_IODIDE","POTASSIUM_OXIDE","PROPANE","lPROPANE",
  "N-PROPYL_ALCOHOL","PYRIDINE","RUBBER_BUTYL","RUBBER_NATURAL","RUBBER_NEOPRENE","SILICON_DIOXIDE",
  "SILVER_BROMIDE","SILVER_CHLORIDE","SILVER_HALIDES","SILVER_IODIDE","SKIN_ICRP","SODIUM_CARBONATE",
  "SODIUM_IODIDE","SODIUM_MONOXIDE","SODIUM_NITRATE","STILBENE","SUCROSE","TERPHENYL","TESTES_ICRP",
  "TETRACHLOROETHYLENE","THALLIUM_CHLORIDE","TISSUE_SOFT_ICRP","TISSUE_SOFT_ICRU-4","TISSUE-METHANE",
  "TISSUE-PROPANE","TITANIUM_DIOXIDE","TOLUENE","TRICHLOROETHYLENE","TRIETHYL_PHOSPHATE","TUNGSTEN_HEXAFLUORIDE",
  "URANIUM_DICARBIDE","URANIUM_MONOCARBIDE","URANIUM_OXIDE","UREA","VALINE","VITON","WATER","WATER_VAPOR",
  "XYLENE","lH2","lAr","lKr","lXe","PbWO4","Galactic","gNe" };

  G4NistManager* man = G4NistManager::Instance();

  for( int i = 0; i < 285; ++i )
  {
    std::string g4name = "G4_" + materials[i];
    G4Material* mater = man->FindOrBuildMaterial( g4name );
    if( mater )
      materials_list->addMaterial( mater, materials[i] );
  }
  G4int nComp;

  // pdg listed aerogel
  G4double density = 0.2*g/cm3;
  std::string name = "AeroGel0_2";
  G4Material* aerogel0_2 = new G4Material(name,density, nComp=2, kStateSolid);
  G4Material* quartz = man->FindOrBuildMaterial( "G4_SILICON_DIOXIDE" );
  G4Material* water = man->FindOrBuildMaterial( "G4_WATER" );
  aerogel0_2->AddMaterial(quartz, 0.97);
  aerogel0_2->AddMaterial(water, 0.03);
  materials_list->addMaterial( aerogel0_2, name );

  // MICE Lithium Hydride
  // density of 0.69 g/cm3 is a myth/rumour; NO citation
  // Alan Bross is contact #1551
  density = 0.69*g/cm3;
  name = "MICE_LITHIUM_HYDRIDE";
  G4Material* miceLiH = new G4Material(name, density, nComp=2);
  G4Element* elMiceLi = new G4Element("MICE_LITHIUM", "Li", 2);
  G4Isotope* Li6 = new G4Isotope("Li6", 3, 6);
  G4Isotope* Li7 = new G4Isotope("Li7", 3, 7);
  elMiceLi->AddIsotope(Li6, 0.98);
  elMiceLi->AddIsotope(Li7, 0.02);
  miceLiH->AddElement(elMiceLi, 1);
  G4Element* elH = man->FindOrBuildElement("H");
  miceLiH->AddElement(elH, 1);
  materials_list->addMaterial( miceLiH, name );

  // BC600, here represented only as bisphenol (rho_bisphenol=1.20gcm-3)
  density = 1.18*g/cm3; // refractive index = 1.56 
  name = "BC600";
  G4Material* bc600 = new G4Material(name, density, nComp=3);
  G4Element* elC = man->FindOrBuildElement("C");
  bc600->AddElement(elC, 15);
  bc600->AddElement(elH, 16);
  G4Element* elO = man->FindOrBuildElement("O");
  bc600->AddElement(elO,  2);		//ME - Tamas thinks this should be 2, not 12!!!
  materials_list->addMaterial( bc600, name );

  // Stainless steel, using generic 304 
  density = 8.06*g/cm3;
  name = "STEEL304";
  // Ignoring P, S and other contaminants
  G4Material* steel304 = new G4Material(name, density, nComp=6, kStateSolid);
  G4Material* matFe = man->FindOrBuildMaterial("G4_Fe");
  G4Material* matC  = man->FindOrBuildMaterial("G4_C");
  G4Material* matMn = man->FindOrBuildMaterial("G4_Mn");
  G4Material* matSi = man->FindOrBuildMaterial("G4_Si");
  G4Material* matCr = man->FindOrBuildMaterial("G4_Cr");
  G4Material* matNi = man->FindOrBuildMaterial("G4_Ni");
  steel304->AddMaterial(matFe, 0.7092);
  steel304->AddMaterial(matC,  0.0008);
  steel304->AddMaterial(matMn, 0.02);
  steel304->AddMaterial(matSi, 0.01);
  steel304->AddMaterial(matCr, 0.18);
  steel304->AddMaterial(matNi, 0.08);
  materials_list->addMaterial( steel304, name );


  // Polyacetal for CKOV1 Window
  density = 1.41*g/cm3;
  name = "POLYACETAL";
  G4Material* polyacetal = new G4Material(name,density, nComp=3, kStateSolid);
  polyacetal->AddElement(elC, 37.5*perCent);
  polyacetal->AddElement(elH, 12.5*perCent);
  polyacetal->AddElement(elO, 50.0*perCent);
  materials_list->addMaterial( polyacetal, name );

  const G4int num = 2;
  G4double PhotonEnergy[num] = {1.7*eV, 4.7*eV};
  G4double PhotonEnergyAir[num] = {0.5*eV, 5.0*eV};
  // B270 for CKOV1 Mirror
  density = 2.44*g/cm3;
  name = "B270";
  G4Material* b270 = new G4Material(name,density, nComp=6, kStateSolid);
  G4Element* elSi = man->FindOrBuildElement("Si");
  G4Element* elB = man->FindOrBuildElement("B");
  G4Element* elNa = man->FindOrBuildElement("Na");
  G4Element* elK = man->FindOrBuildElement("K");
  G4Element* elCa = man->FindOrBuildElement("Ca");
  b270->AddElement(elSi, 31.9*perCent);
  b270->AddElement(elO,  48.5*perCent);
  b270->AddElement(elB,   3.3*perCent);
  b270->AddElement(elNa,  5.7*perCent);
  b270->AddElement(elK,   9.9*perCent);
  b270->AddElement(elCa,  0.7*perCent);

  materials_list->addMaterial( b270, name );

	//Optical Air mounted in the CKOV
  G4double a;  // atomic mass
  G4double z;
  G4int nelements;

  G4Element* N = new G4Element("Nitrogen", "N", z=7 , a=14.01*g/mole);
  G4Element* O = new G4Element("Oxygen"  , "O", z=8 , a=16.00*g/mole);
  name = "OptAIR";
  G4Material* OptAir = new G4Material(name, density=1.29e-3*mg/cm3, nelements=2);
  OptAir->AddElement(N, 70.*perCent);
  OptAir->AddElement(O, 30.*perCent);
  materials_list->addMaterial( OptAir, name );

  G4double RefractiveIndexAir[num] = { 1.00, 1.00 };
  G4MaterialPropertiesTable* AirMPT = new G4MaterialPropertiesTable();
  AirMPT->AddProperty("RINDEX", PhotonEnergyAir, RefractiveIndexAir, num);
  OptAir->SetMaterialPropertiesTable(AirMPT);

	//Optical Glass mounted in Glass Window for CKOV
  G4int natoms;
  G4double RefractiveIndexGlass[num] = { 1.4, 1.4 };
  G4double PhotonEnergyGlass[15] = { 1.5*eV, 2.6*eV,2.8*eV,3.0*eV,3.2*eV,3.3*eV,3.4*eV,3.5*eV,3.6*eV,3.7*eV,3.9*eV,4.0*eV,4.2*eV,4.4*eV,5.0*eV };
  G4double Absorption2[15] =       { 10.0*cm,9.0*cm,8.0*cm,8.0*cm,7.5*cm,6.2*cm,6.2*cm,4.7*cm,3.2*cm,1.9*cm,0.6*cm,0.3*cm,.15*cm,.12*cm,.13*cm };
  density = 2.6*g/cm3;
  name="OptGlass";
  G4Material* OptGlass = new G4Material(name, density,nelements=2);
  OptGlass->AddElement(elSi, natoms=1);
  OptGlass->AddElement(O,  natoms=2);
  G4MaterialPropertiesTable* GlassMPT1 = new G4MaterialPropertiesTable();
  GlassMPT1->AddProperty("RINDEX", PhotonEnergy, RefractiveIndexGlass, num);
  GlassMPT1->AddProperty("ABSLENGTH", PhotonEnergyGlass, Absorption2, 15);
  OptGlass->SetMaterialPropertiesTable(GlassMPT1);
  materials_list->addMaterial( OptGlass, name );

	//Optical Glass mounted in PMT Windows for CKOV
  name="OptPMTGlass";
  G4Material* OptGlassPMT = new G4Material(name, density,nelements=2);
  OptGlassPMT->AddElement(elSi, natoms=1);
  OptGlassPMT->AddElement(O,  natoms=2);
  G4MaterialPropertiesTable* GlassMPT2 = new G4MaterialPropertiesTable();
  GlassMPT2->AddProperty("RINDEX", PhotonEnergy, RefractiveIndexGlass, num);
  OptGlassPMT->SetMaterialPropertiesTable(GlassMPT2);
  materials_list->addMaterial( OptGlassPMT, name );

  // Aerogel mounted in Vessel1 for CKOV1
  density = 0.251*g/cm3;
  name = "AEROGEL_1";
  G4Material* aero1 = new G4Material(name,density, nComp=2, kStateSolid);
  aero1->AddElement(elSi, 46.7*perCent);
  aero1->AddElement(elO,  53.3*perCent);

  G4double Absorption1[num] = {245.0*mm, 245.0*mm};
  G4double RefractiveIndex1[num] = {1.08, 1.08};

  G4double RayleighPhotE[100];
  G4double RayleighSL[100];

	double C = 1e-24*(m3);
//	double C = run.DataCards->fetchValueDouble("AerogelClarity");
	RayleighPhotE[0] = 0.;
	RayleighSL[0]    = 0.;
	for(unsigned int i=1;i<=63;i++)
	{
		RayleighPhotE[i] = ( 1.6 + i*5e-2 )*eV;
		RayleighSL[i]    = pow( c_light*h_Planck/RayleighPhotE[i], 4 )/C;
	}

  G4MaterialPropertiesTable* a1_mt = new G4MaterialPropertiesTable();
  a1_mt->AddProperty("RINDEX",       PhotonEnergy, RefractiveIndex1,num);
  a1_mt->AddProperty("ABSLENGTH",    PhotonEnergy, Absorption1,    num);
  a1_mt->AddProperty("RAYLEIGH",     RayleighPhotE, RayleighSL,63);
  aero1->SetMaterialPropertiesTable(a1_mt);

  materials_list->addMaterial( aero1, name );

  // Aerogel mounted in Vessel2 for CKOV1
  density = 0.430*g/cm3;
  name = "AEROGEL_2";
  G4Material* aero2 = new G4Material(name,density, nComp=2, kStateSolid);
  aero2->AddElement(elSi, 46.7*perCent);
  aero2->AddElement(elO,  53.3*perCent);
  G4double RefractiveIndex2[num] = {1.12, 1.12};

  G4MaterialPropertiesTable* a2_mt = new G4MaterialPropertiesTable();
  a2_mt->AddProperty("RINDEX",       PhotonEnergy, RefractiveIndex2,num);
  a2_mt->AddProperty("ABSLENGTH",    PhotonEnergy, Absorption1,    num);
  a2_mt->AddProperty("RAYLEIGH",     RayleighPhotE, RayleighSL,63);
  aero2->SetMaterialPropertiesTable(a2_mt);
  materials_list->addMaterial( aero2, name );

	 // Matsushita Aerogel 103a tested at Fermi
  density = 0.113*g/cm3;
  name = "AEROGEL_103a";
  G4Material* aero103 = new G4Material(name,density, nComp=2, kStateSolid);
  aero103->AddElement(elSi, 46.7*perCent);
  aero103->AddElement(elO,  53.3*perCent);
  materials_list->addMaterial( aero103, name );
  
  // Matsushita Aerogel 107a tested at Fermi 
  density = 0.261*g/cm3;
  name = "AEROGEL_107a";
  G4Material* aero107 = new G4Material(name,density, nComp=2, kStateSolid);
  aero107->AddElement(elSi, 46.7*perCent);
  aero107->AddElement(elO,  53.3*perCent);
  materials_list->addMaterial( aero107, name );
  
  // Matsushita Aerogel 112a tested at Fermi 
  density = 0.371*g/cm3;
  name = "AEROGEL_112a";
  G4Material* aero112 = new G4Material(name,density, nComp=2, kStateSolid);
  aero112->AddElement(elSi, 46.7*perCent);
  aero112->AddElement(elO,  53.3*perCent);
  materials_list->addMaterial( aero112, name );
  
  // Freeman RenCast6400-1 Polyurethane used in tracker station construction
  density = 1.04*g/cm3;
  name = "RenCast6400";
  G4Material* renCast6400 = new G4Material(name,density, nComp=5, kStateSolid);
  G4Element* elN = man->FindOrBuildElement("N");
  G4Element* elP = man->FindOrBuildElement("P");
  renCast6400->AddElement(elC, 51.15*perCent);
  renCast6400->AddElement(elH, 34.95*perCent);
  renCast6400->AddElement(elO,  7.07*perCent);
  renCast6400->AddElement(elN,  6.76*perCent);
  renCast6400->AddElement(elP,  0.07*perCent);
  materials_list->addMaterial( renCast6400, name );

  // Be with PDG properties
  density = 1.848 * g/cm3;//at r.t
  name = "PDG_Be";
  G4Material* pdgBe = new G4Material(name,density, nComp=1, kStateSolid);
  G4Element* elBe = man->FindOrBuildElement("Be");
  pdgBe->AddElement(elBe, 100*perCent);
  materials_list->addMaterial( pdgBe, name );

 // lH2,Liquid Hydrogen with PDG properties
  density = 0.0708 * g/cm3; //at what T p?
  name = "PDG_lH2";
  G4double temp=20.39 * kelvin;//in K
  G4Material* pdglH = new G4Material(name,density, nComp=1, kStateLiquid,temp);
  pdglH->AddElement(elH, 100*perCent);
  materials_list->addMaterial( pdglH, name );

// lHe, Liquid Helium with PDG properties
  G4Element* elHe = man->FindOrBuildElement("He");
  density = 0.1249 * g/cm3; // at T p
  name = "lHe";
  temp=4.224 * kelvin;
  G4Material* pdglHe = new G4Material(name,density, nComp=1, kStateLiquid, temp);
  pdglHe->AddElement(elHe, 100*perCent);
  materials_list->addMaterial( pdglHe, name);

 // gaseous Neon
  G4Element* elNe = man->FindOrBuildElement("Ne");
  density = 0.0104 * g/cm3; //at stp
  name = "gNe";
  temp=27.00 * kelvin;//in K
  G4Material* pdggNe = new G4Material(name,density, nComp=1, kStateGas, temp);
  pdggNe->AddElement(elNe, 100*perCent);
  materials_list->addMaterial( pdggNe, name );

  // Al with PDG properties
  density = 2.700 * g/cm3;//at r.t
  name = "PDG_Al";
  G4Material* pdgAl = new G4Material(name,density, nComp=1, kStateSolid);
  G4Element* elAl = man->FindOrBuildElement("Al");
  pdgAl->AddElement(elAl, 100*perCent);
  materials_list->addMaterial( pdgAl, name );

 // Cu with PDG properties
  density = 8.960 * g/cm3;//at r.t
  name = "PDG_Cu";
  G4Material* pdgCu = new G4Material(name,density, nComp=1, kStateSolid);
  G4Element* elCu = man->FindOrBuildElement("Cu");
  pdgCu->AddElement(elCu, 100*perCent);
  materials_list->addMaterial( pdgCu, name );

 // Fe with PDG properties
  density = 7.87 * g/cm3;//at r.t
  name = "PDG_Fe";
  G4Material* pdgFe = new G4Material(name,density, nComp=1, kStateSolid);
  G4Element* elFe = man->FindOrBuildElement("Fe");
  pdgFe->AddElement(elFe, 100*perCent);
  materials_list->addMaterial( pdgFe, name );

 // Pb with PDG properties
  density = 11.35 * g/cm3;//at r.t
  name = "PDG_Pb";
  G4Material* pdgPb = new G4Material(name,density, nComp=1, kStateSolid);
  G4Element* elPb = man->FindOrBuildElement("Pb");
  pdgPb->AddElement(elPb, 100*perCent);
  materials_list->addMaterial( pdgPb, name );


 // AIR with PDG properties
  density = 1.205e-3 * g/cm3;//at r.t
  name = "PDG_AIR";
  G4Material* pdgAIR = new G4Material(name,density, nComp=1, kStateGas);
  G4Material* mAIR = man->FindOrBuildMaterial("G4_AIR");
  pdgAIR->AddMaterial(mAIR, 100*perCent);
  materials_list->addMaterial( pdgAIR, name );


  // BRASS 
  density = 8.4* g/cm3;
  name = "BRASS";
  G4Material* brass = new G4Material(name, density, nComp=3, kStateSolid);
  G4Element* elZn = man->FindOrBuildElement("Zn");
  brass->AddElement(elCu, 58*perCent);
  brass->AddElement(elZn, 39*perCent);
  brass->AddElement(elPb,  3*perCent);
  materials_list->addMaterial( brass, name );

  // POLYURETHANE
  density = 1.220 * g/cm3;
  name = "POLYURETHANE";
  G4Material* TufsetPU = new G4Material(name, density, nComp=4, kStateSolid);
  // G4Element* elN = man->FindOrBuildElement("N");
  TufsetPU->AddElement(elC, 64.38*perCent);
  TufsetPU->AddElement(elH,  9.01*perCent);
  TufsetPU->AddElement(elN,  6.01*perCent);
  TufsetPU->AddElement(elO, 20.60*perCent);
  materials_list->addMaterial( TufsetPU, name );

  // TUFNOL 

  // now fill in the other maps of the material properties

  const std::map<std::string,G4Material*> _materials = materials_list->materialMap();

  for( std::map<std::string,G4Material*>::const_iterator it = _materials.begin(); it != _materials.end(); ++it )
  {
    std::string chemical = it->second->GetChemicalFormula();
    if( chemical == "" || chemical[0] == ' ' ) chemical = "Not_Defined";

    double density = it->second->GetDensity();
    std::string state = "undefined";

    switch( it->second->GetState() )
    {
      case kStateSolid : { state = "Solid"; break; }
      case kStateLiquid : { state = "Liquid"; break; }
      case kStateGas : { state = "Gas"; break; }
      default : break;
    }

    double temp = it->second->GetTemperature();
    double press = it->second->GetPressure();
    double edens = it->second->GetElectronDensity();
    double X0 = it->second->GetRadlen();
    double L0 = it->second->GetNuclearInterLength();
//    double ZNum = it->second->GetZ();
//    double ANum = it->second->GetA();
    double meanExE = it->second->GetIonisation()->GetMeanExcitationEnergy();
    double densityCor = it->second->GetIonisation()->GetAdensity();
    materials_list->updateProperties( it->first, chemical, density, state, temp, press, edens, X0, L0, -1, -1, meanExE, densityCor );
//    materials_list->updateProperties( it->first, chemical, density, state, temp, press, edens, X0, L0, ZNum, ANum, meanExE, densityCor );
  }
  return materials_list;
}

