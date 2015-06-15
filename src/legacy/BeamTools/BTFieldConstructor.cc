// MAUS WARNING: THIS IS LEGACY CODE.
//BTField is the parent of all fields in the beamtools package
//Abstract Base Class
//Has simply a GetFieldValue method

#include "Config/MiceModule.hh"
#include "src/legacy/Interface/STLUtils.hh"

#include "src/common_cpp/FieldTools/SectorMagneticFieldMap.hh"
#include "src/common_cpp/FieldTools/DerivativesSolenoid.hh"

#include "BeamTools/BTFieldConstructor.hh"
#include "BeamTools/BTMultipole.hh"
#include "BeamTools/BTSolenoid.hh"
#include "BeamTools/BTFieldGroup.hh"
#include "BeamTools/BTPhaser.hh"
#include "BeamTools/BTMagFieldMap.hh"
#include "BeamTools/BTConstantField.hh"
#include "BeamTools/BTCombinedFunction.hh"
#include "BeamTools/BT3dFieldMap.hh"
#include "BeamTools/BTPillBox.hh"
#include "BeamTools/BTRFFieldMap.hh"
#include "src/legacy/BeamTools/BTFieldConstructor.hh"

#include "src/common_cpp/Utils/Exception.hh"

std::string       BTFieldConstructor::_defaultSolenoidMode = "";
const int         BTFieldConstructor::_numberOfFieldTypes  = 14;
const std::string BTFieldConstructor::_fieldTypes[15] 
           = {"Solenoid","PillBox","MagneticFieldMap", "Quadrupole", "RFFieldMap", 
              "RectangularField", "CylindricalField", "Dipole1", "Dipole2", "Multipole", "CombinedFunction", "DerivativesSolenoid", 
              "FieldAmalgamation","SectorMagneticFieldMap",""};
int               BTFieldConstructor::_fieldNameIndex = 0;

BTFieldConstructor::BTFieldConstructor() :
                                              _magneticField(new BTFieldGroup()),
                                              _electroMagneticField(new BTFieldGroup()),
                                              _needsPhases(false), _amalgamatedFields(0)
{
	if(GetGridDefault()[0]<2 || GetGridDefault()[1]<2 || GetGridDefault()[2]<2) 
		throw(MAUS::Exception(MAUS::Exception::recoverable, "FieldGridX, FieldGridY, FieldGridZ datacards must be > 1", "BTFieldConstructor::BTFieldConstructor"));
	SetGridSize( BTFieldGroup::GetGridDefault() );
	_electroMagneticField->AddField(_magneticField, Hep3Vector(0,0,0));
	AddField(_electroMagneticField, Hep3Vector(0,0,0));
}


BTFieldConstructor::BTFieldConstructor(MiceModule * rootModule) :
                                              _magneticField(new BTFieldGroup()),
                                              _electroMagneticField(new BTFieldGroup()),
                                              _needsPhases(false), _amalgamatedFields(0)
{
  _electroMagneticField->AddField(_magneticField, Hep3Vector(0,0,0));
  AddField(_electroMagneticField, Hep3Vector(0,0,0));
  BuildFields(rootModule);
}

BTFieldConstructor::~BTFieldConstructor() {
    MAUS::SectorMagneticFieldMap::ClearFieldCache();
}

void BTFieldConstructor::BuildFields(MiceModule * rootModule)
{
  delete BTPhaser::GetInstance();
	SetDefaults();
	if(GetGridDefault()[0]<2 || GetGridDefault()[1]<2 || GetGridDefault()[2]<2) 
		throw(MAUS::Exception(MAUS::Exception::recoverable, "FieldGridX, FieldGridY, FieldGridZ datacards must be > 1", "BTFieldConstructor::BTFieldConstructor"));
	SetGridSize( BTFieldGroup::GetGridDefault() );
	_magneticField       ->SetGridSize( BTFieldGroup::GetGridDefault() );
	_electroMagneticField->SetGridSize( BTFieldGroup::GetGridDefault() );
	try {
  	int ignoredAmalgamated = 0;
  	_needsPhases = false;
  	BTSolenoid::ClearStaticMaps();
  	std::vector<const MiceModule*> daughterModules;
  	for(int fields = 0; fields < _numberOfFieldTypes; fields++) {
  		daughterModules = rootModule->findModulesByPropertyString( "FieldType", _fieldTypes[fields] );
  		for(unsigned int i=0; i<daughterModules.size(); i++) {
  		  try {
    			BTField * newField;
    			const MiceModule * theModule   = daughterModules[i];
    			if (daughterModules[i]->propertyExistsThis("IsAmalgamated", "bool")) {
    				if (daughterModules[i]->propertyBoolThis("IsAmalgamated")) { //don't use, the BTFieldAmalgamation should deal with it
    				  ignoredAmalgamated++; newField = NULL;
    				} else {
    				  newField = GetField(theModule);  // not amalgamated, use as normal
    				}
    			} else {
    			  newField    = GetField(theModule); //not amalgamated, use as normal
    			}
    			Hep3Vector         position    = theModule->globalPosition();
    			HepRotation        rotation    = theModule->globalRotation();
    			double             scaleFactor = theModule->globalScaleFactor();
    			if ((newField!=NULL) && (newField->DoesFieldChangeEnergy())) {
    				_electroMagneticField->AddField(newField, position, rotation, scaleFactor, false); //make sure that I call Close at the end!
    			} else {
    				_magneticField->AddField(newField, position, rotation, scaleFactor, false);
    		  }
    			SetName(newField, theModule);
    		} catch (MAUS::Exception exc) {
    		  std::cerr << "Error while loading fields from module "
    		            << daughterModules[i]->fullName() << std::endl;
    		  exc.Print(); exit(1);
    		} catch (...) {
    		  std::cerr << "Unhandled exception while loading fields from module "
    		            << daughterModules[i]->fullName() << " - bailing"
    		            << std::endl;
    		  exit(1);
    		}
  		}
  	}
    _magneticField->Close();
    _electroMagneticField->Close();

  	WriteFieldMaps();
  	if(_amalgamatedFields != ignoredAmalgamated) {
  		std::stringstream in;
  		in << "Error - detected " << ignoredAmalgamated << " fields with PropertyBool IsAmalgamated 1, but only " 
  		   << _amalgamatedFields << " in Amalgamation MiceModules";
  		throw(MAUS::Exception(MAUS::Exception::recoverable, in.str(), "BTFieldConstructor()"));
  	}
	} catch (MAUS::Exception exc) {
	  exc.Print();
	  exit(1);
	} catch (...) {
	  std::cerr << "Unhandled exception while loading fields - bailing"
	            << std::endl;
	  exit(1);
	}
}

bool BTFieldConstructor::ModuleHasElectromagneticField(const MiceModule * theModule)
{
	if(theModule->propertyExistsThis("CavityMode", "string") ) return true;
	return false;
}

BTField * BTFieldConstructor::GetField(const MiceModule * theModule)
{
	std::string field = theModule->propertyStringThis("FieldType");
	if(field == "Solenoid")
		return GetSolenoid(theModule);
	if(field == "DerivativesSolenoid")
		return GetDerivativesSolenoid(theModule);
	else if(field == "PillBox")
		return GetRFCavity(theModule);
	else if(field == "MagneticFieldMap")
		return GetMagFieldMap(theModule);
	else if(field == "SectorMagneticFieldMap")
		return GetSectorMagFieldMap(theModule);
	else if(field == "RFFieldMap")
		return GetRFCavity(theModule);
	else if(field == "RectangularField" || field == "CylindricalField")
		return GetConstantField(theModule);
	else if(field == "Multipole")
		return GetMultipole(theModule);
	else if(field == "CombinedFunction")
		return GetCombinedFunction(theModule);
	else if(field == "FieldAmalgamation")
		return GetFieldAmalgamation(theModule);
	else
	{
		Squeak::mout(Squeak::warning) << "Unrecognised field of type " << theModule->propertyStringThis("FieldType") << std::endl;
		return NULL;
	}

}


BTField * BTFieldConstructor::GetSolenoid(const MiceModule * theModule)
{
	//If in read mode, read in a file
	std::string solenoidMode=_defaultSolenoidMode;
	std::string interpolation = "LinearCubic";
	if(theModule->propertyExistsThis( "InterpolationAlgorithm", "string" ) )
		interpolation = theModule->propertyStringThis("InterpolationAlgorithm");
	std::string filename = (theModule->propertyStringThis("FileName"));
	if(theModule->propertyExistsThis( "FieldMapMode", "string"))
		solenoidMode = (theModule->propertyStringThis("FieldMapMode"));
	if(solenoidMode == "Read")
	{
		return (new BTSolenoid(theModule->propertyStringThis("FileName").c_str(), interpolation) );
	}
	else if(solenoidMode != "Write" && solenoidMode != "Analytic" && solenoidMode != "WriteDynamic")
	{
		std::cerr << "Unrecognised field map mode " << solenoidMode << " in module " << theModule->name() << " - aborting" << std::endl;
		exit (1);
	}

	BTSolenoid * newSolenoid = new BTSolenoid();

	//Overload defaults if required
	double fieldTolerance = -1;
	if(theModule->propertyExistsThis( "ZExtentFactor", "double") )
		newSolenoid->SetZExtentFactor(theModule->propertyDoubleThis("ZExtentFactor"));
	if(theModule->propertyExistsThis( "RExtentFactor", "double") )
		newSolenoid->SetRExtentFactor(theModule->propertyDoubleThis("RExtentFactor"));
	if(theModule->propertyExistsThis( "NumberOfZCoords", "int") )
		newSolenoid->SetNumberOfZCoords(theModule->propertyIntThis("NumberOfZCoords"));
	if(theModule->propertyExistsThis( "NumberOfRCoords", "int") )
		newSolenoid->SetNumberOfZCoords(theModule->propertyIntThis("NumberOfRCoords"));
	if(theModule->propertyExistsThis( "NumberOfSheets", "int") )
		newSolenoid->SetNumberOfSheets(theModule->propertyIntThis("NumberOfSheets"));
	if(solenoidMode == "WriteDynamic")
        {
		if(theModule->propertyExistsThis( "FieldTolerance", "double") )
                {
			fieldTolerance = theModule->propertyDoubleThis("FieldTolerance");
 		}
		else
		{
			theModule->printTree(0);
			throw(MAUS::Exception(MAUS::Exception::recoverable, "Solenoid mode is WriteDynamic but no tolerance set in module "+theModule->name(), 
			                                  "BTFieldConstructor::GetSolenoid") );
		}
	}

	double length=(theModule->propertyDoubleThis("Length"));
	double thickness=(theModule->propertyDoubleThis("Thickness"));
	double innerRadius=(theModule->propertyDoubleThis("InnerRadius"));
  double currentDensity = 0.;
  if (theModule->propertyExistsThis("CurrentDensity", "double")) {
    	currentDensity = theModule->propertyDoubleThis("CurrentDensity");
  } else if (theModule->propertyExistsThis("Current", "double") &&
             theModule->propertyExistsThis("NumberOfTurns", "int")) {
      double current = theModule->propertyDoubleThis("Current");
      current *= theModule->propertyIntThis("NumberOfTurns");
      currentDensity = current/length/thickness;
  } else {
      throw(MAUS::Exception(MAUS::Exception::recoverable,
            "Either (Current and NumberOfTurns) or CurrentDensity should be defined in module "+theModule->name(),
            "BTFieldConstructor::GetSolenoid"));
  }
	if(solenoidMode == "Analytic")
	{
		newSolenoid->SetIsAnalytic(true);
		newSolenoid->BuildSheets(length,thickness,innerRadius,currentDensity);
	}
	else
		newSolenoid->BuildSheets(length,thickness,innerRadius,currentDensity, fieldTolerance, filename, interpolation);
	return newSolenoid;
}

BTField * BTFieldConstructor::GetRFCavity(const MiceModule * theModule)
{
	BTPillBox * newPillBox = NULL;

	double length=0, energyGain=0, frequency=0, timeDelay=0;

	std::string fieldMapFile       = "";
	std::string fieldMapType       = "";
	std::string fieldDuringPhasing = "";
	std::string pillBoxMode        = theModule->propertyStringThis("CavityMode");

	fieldDuringPhasing = theModule->propertyStringThis("FieldDuringPhasing");
	if(theModule->propertyStringThis("FieldType") == "RFFieldMap")
	{
		fieldMapFile = theModule->propertyStringThis("FileName");
		if(theModule->propertyExistsThis("FileType","string"))
			fieldMapType = theModule->propertyStringThis("FileType");
	}


	length = theModule->propertyDoubleThis("Length");
	if(pillBoxMode == "Electrostatic")
	{
		energyGain    = theModule->propertyDoubleThis("PeakEField");
		double radius = theModule->propertyDoubleThis("FieldRadius");
		newPillBox = new BTPillBox(length, energyGain, radius);
		if(theModule->propertyStringThis("FieldType") == "RFFieldMap")
			throw(MAUS::Exception(MAUS::Exception::recoverable, "Can't have electrostatic RFFieldMap", "BTFieldConstructor::GetRFCavity"));
	}
	else if(pillBoxMode == "Unphased" || pillBoxMode == "AutomaticPhasing")
	{
		frequency = theModule->propertyDoubleThis("Frequency");
		if(BTPillBox::StringToPhaseModel(fieldDuringPhasing) == BTPillBox::bestGuess)
			energyGain = theModule->propertyDoubleThis("EnergyGain");
		else    energyGain = theModule->propertyDoubleThis("PeakEField");
		if(theModule->propertyString("FieldType") == "PillBox")
			newPillBox = new BTPillBox(frequency, length, energyGain, fieldDuringPhasing);
		else if(theModule->propertyStringThis("FieldType") == "RFFieldMap")
			newPillBox = new BTRFFieldMap(frequency, length, energyGain, fieldDuringPhasing, fieldMapFile, fieldMapType);
		_needsPhases = true;
    BTPhaser::FieldForPhasing * phase = new BTPhaser::FieldForPhasing();
    phase->name = theModule->name();
    phase->plane_position = theModule->globalPosition();
    phase->rotation = theModule->globalRotation();
    phase->radius = -1.;  // bug - phasing broken in circular geometry
    BTPhaser::GetInstance()->SetFieldForPhasing(phase);
	}
	else
	{
		frequency = theModule->propertyDoubleThis("Frequency");
		energyGain = theModule->propertyDoubleThis("PeakEField");
		timeDelay  = theModule->propertyDoubleThis("TimeDelay");
		if(theModule->propertyStringThis("FieldType") == "PillBox")
			newPillBox = new BTPillBox(frequency, length, energyGain, timeDelay);
		else if(theModule->propertyStringThis("FieldType") == "RFFieldMap")
			newPillBox = new BTRFFieldMap(frequency, length, energyGain, timeDelay, fieldMapFile, fieldMapType);
	}
	if(newPillBox==NULL)
		throw(MAUS::Exception(MAUS::Exception::recoverable, "Unrecognised CavityMode", "BTFieldConstructor::GetRFCavity" ));
	if(theModule->propertyExistsThis("ReferenceParticlePhase", "double"))
		newPillBox->SetRefParticlePhase(theModule->propertyDoubleThis("ReferenceParticlePhase"));
	return newPillBox;
}

BTField * BTFieldConstructor::GetMagFieldMap(const MiceModule * theModule)
{
	std::string fieldMapMode = theModule->propertyStringThis("FieldMapMode");
	std::string filetype="g4micetext";
	if(theModule->propertyExistsThis( "FileType", "string"))
		filetype = (theModule->propertyStringThis("FileType"));

	if(fieldMapMode=="Write" && filetype != "g4bl3dGrid")
	{
		_fieldMapsForWriting.push_back(theModule);
		return NULL;
	}
	else if(fieldMapMode != "Read")
	{
		std::cerr << "Unrecognised field map mode " << fieldMapMode << " in module "
		     << theModule->name() << " - aborting" << std::endl;
		exit (1);
	}

	std::string filename = (theModule->propertyStringThis("FileName"));
	if(filetype == "g4bl3dGrid") 
	{
		std::string symmetry = "None";
		if(theModule->propertyExistsThis( "Symmetry", "string"))
			symmetry     = theModule->propertyStringThis("Symmetry");
		return new BT3dFieldMap("", filename, filetype, symmetry);
	}
	return new BTMagFieldMap(filename, filetype, "interpolation");
}

BTField * BTFieldConstructor::GetSectorMagFieldMap
                                                (const MiceModule * theModule) {
    std::string filename = theModule->propertyStringThis("FileName");
    std::string filetype = theModule->propertyStringThis("FileType");
    std::string symmetry = "None";
	if(theModule->propertyExistsThis( "Symmetry", "string"))
		symmetry = theModule->propertyStringThis("Symmetry");
    std::vector<double> units(6, 1.);
    for (int i = 0; i < 6; ++i) {
        std::string name = "Unit"+STLUtils::ToString(i+1);
	    if(theModule->propertyExistsThis(name, "double"))
		    units[i] = theModule->propertyDoubleThis(name);
    }

    return new MAUS::SectorMagneticFieldMap(filename, filetype, units, symmetry);
}

BTField * BTFieldConstructor::GetConstantField(const MiceModule * theModule)
{
	CLHEP::Hep3Vector field = theModule->propertyHep3VectorThis("ConstantField");
	if(theModule->propertyStringThis("FieldType") == "RectangularField")
	{
		double width  = theModule->propertyDoubleThis("Width");
		double height = theModule->propertyDoubleThis("Height");
		double length = theModule->propertyDoubleThis("Length"); 
		return new BTConstantField(length, width, height, field);
	}
	else
	{
		double radius = theModule->propertyDoubleThis("FieldRadius");
		double length = theModule->propertyDoubleThis("Length"); 
		return new BTConstantField(length, radius, field);
	}
	
}

BTField * BTFieldConstructor::GetMultipole(const MiceModule * theModule) {
	double height  = theModule->propertyDoubleThis("Height");
	double width   = theModule->propertyDoubleThis("Width");
	double length  = theModule->propertyDoubleThis("Length");
	int    pole    = theModule->propertyIntThis("Pole");
  double field   = 0.;
	std::string geometry = "";
	if(theModule->propertyExistsThis("CurvatureModel", "string"))
		geometry  = theModule->propertyStringThis("CurvatureModel");
  if (geometry != "MomentumBased")
    field   = theModule->propertyDoubleThis("FieldStrength");

  if (height <= 0) 
    throw(MAUS::Exception(MAUS::Exception::recoverable,
        "Multipole height must be more than 0 in module "+theModule->fullName(), 
        "BTFieldConstructor::GetMultipole"));
  if (width <= 0) 
    throw(MAUS::Exception(MAUS::Exception::recoverable,
         "Multipole width must be more than 0 in module "+theModule->fullName(), 
         "BTFieldConstructor::GetMultipole"));
  if (length <= 0) 
    throw(MAUS::Exception(MAUS::Exception::recoverable,
        "Multipole length must be more than 0 in module "+theModule->fullName(),
        "BTFieldConstructor::GetMultipole"));
  if (pole <= 0) 
    throw(MAUS::Exception(MAUS::Exception::recoverable,
          "Multipole pole must be more than 0 in module "+theModule->fullName(),
          "BTFieldConstructor::GetMultipole"));
  // MaxEndPole is also called in end field calculation
  int    max_p   = 0;
  if (theModule->propertyExistsThis("MaxEndPole", "int"))
  	max_p = theModule->propertyIntThis("MaxEndPole") - pole;
  BTMultipole::EndFieldModel* ef = GetEndFieldModel(theModule, pole);
  // curvature model
	double radiusVariable  = 0;
	if(theModule->propertyExistsThis("ReferenceCurvature", "double"))
		radiusVariable = theModule->propertyDoubleThis("ReferenceCurvature");
	if(geometry=="MomentumBased")
	{
		if(theModule->propertyExistsThis("ReferenceMomentum", "double"))
			radiusVariable = theModule->propertyDoubleThis("ReferenceMomentum");
		else 
			throw(MAUS::Exception(MAUS::Exception::nonRecoverable, 
                   "Did not define multipole reference momentum",
                   "BTFieldConstructor::GetMultipole(const MiceModule*)"));
		if(theModule->propertyExistsThis("BendingAngle", "double"))
			field          = theModule->propertyDoubleThis("BendingAngle");
		else 
			throw(MAUS::Exception(MAUS::Exception::nonRecoverable,
                   "Did not define multipole bending angle",
                   "BTFieldConstructor::GetMultipole(const MiceModule*)"));
	}
	BTMultipole* m = new BTMultipole();
  m->Init(pole, field, length, height, width, geometry,
                                                     radiusVariable, ef, max_p);   
  return m;
}

BTField * BTFieldConstructor::GetCombinedFunction
                                                (const MiceModule * theModule) {
	double height  = theModule->propertyDoubleThis("Height");
	double width   = theModule->propertyDoubleThis("Width");
	double length  = theModule->propertyDoubleThis("Length");
  double field   = theModule->propertyDoubleThis("BendingField");
	int    pole    = theModule->propertyIntThis("Pole");
  double index   = theModule->propertyDoubleThis("FieldIndex");

  if (height <= 0) 
    throw(MAUS::Exception(MAUS::Exception::recoverable,
        "CombinedFunction Height must be more than 0 in module "+theModule->fullName(), 
        "BTFieldConstructor::GetCombinedFunction"));
  if (width <= 0) 
    throw(MAUS::Exception(MAUS::Exception::recoverable,
         "CombinedFunction Width must be more than 0 in module "+theModule->fullName(), 
         "BTFieldConstructor::GetCombinedFunction"));
  if (length <= 0) 
    throw(MAUS::Exception(MAUS::Exception::recoverable,
        "CombinedFunction Length must be more than 0 in module "+theModule->fullName(),
        "BTFieldConstructor::GetCombinedFunction"));
  if (pole <= 0) 
    throw(MAUS::Exception(MAUS::Exception::recoverable,
          "CombinedFunction Pole must be more than 0 in module "+theModule->fullName(),
          "BTFieldConstructor::GetCombinedFunction"));
  if (index <= 0) 
    throw(MAUS::Exception(MAUS::Exception::recoverable,
          "CombinedFunction FieldIndex must be more than 0 in module "+theModule->fullName(),
          "BTFieldConstructor::GetCombinedFunction"));

  // MaxEndPole is also called in end field calculation
  int max_p = 0;
  if (theModule->propertyExistsThis("MaxEndPole", "int"))
  	max_p = theModule->propertyIntThis("MaxEndPole") - pole;
  BTMultipole::EndFieldModel* ef = GetEndFieldModel(theModule, pole);
  // curvature model
	std::string geometry = "";
	double radiusVariable  = 0;
	if(theModule->propertyExistsThis("CurvatureModel", "string"))
		geometry  = theModule->propertyStringThis("CurvatureModel");
	if(theModule->propertyExistsThis("ReferenceCurvature", "double"))
		radiusVariable = theModule->propertyDoubleThis("ReferenceCurvature");
	if(geometry=="MomentumBased")
	{
		if(theModule->propertyExistsThis("ReferenceMomentum", "double"))
			radiusVariable = theModule->propertyDoubleThis("ReferenceMomentum");
		else 
			throw(MAUS::Exception(MAUS::Exception::nonRecoverable,
                   "Did not define multipole reference momentum",
                   "BTFieldConstructor::GetMultipole(const MiceModule*)"));
		if(theModule->propertyExistsThis("BendingAngle", "double")) 
			field          = theModule->propertyDoubleThis("BendingAngle");
		else 
			throw(MAUS::Exception(MAUS::Exception::nonRecoverable,
                   "Did not define multipole bending angle",
                   "BTFieldConstructor::GetMultipole(const MiceModule*)"));
	}
	return new BTCombinedFunction(pole, field, index, length, height, width,
                                geometry, radiusVariable, ef, max_p);
}

BTMultipole::EndFieldModel* BTFieldConstructor::GetEndFieldModel
                                             (const MiceModule* mod, int pole) {
  if (!mod->propertyExistsThis("EndFieldType", "string")) return NULL;
  if (mod->propertyStringThis("EndFieldType") == "HardEdged") return NULL;
  if (mod->propertyStringThis("EndFieldType") == "Tanh") {
    double l     = mod->propertyDoubleThis("EndLength");
    double x0    = mod->propertyDoubleThis("CentreLength");
    int    max_p = mod->propertyIntThis("MaxEndPole")-pole;
    if (l <= 0)
      throw(MAUS::Exception(MAUS::Exception::recoverable,
        "Tanh EndLength must be more than 0 in module "+mod->fullName(), 
        "BTFieldConstructor::GetEndFieldModel"));
    if (x0 <= 0)
      throw(MAUS::Exception(MAUS::Exception::recoverable,
       "Tanh CentreLength must be more than 0 in module "+mod->fullName(), 
       "BTFieldConstructor::GetEndFieldModel"));
    if (max_p < 0)
      throw(MAUS::Exception(MAUS::Exception::recoverable,
       "Tanh MaxEndPole must be >= Pole in module "+mod->fullName(),
       "BTFieldConstructor::GetEndFieldModel"));
    return new BTMultipole::TanhEndField(x0, l, max_p);
  }
  if (mod->propertyStringThis("EndFieldType") == "Enge") {
		double l     = mod->propertyDoubleThis("EndLength");
		double x0    = mod->propertyDoubleThis("CentreLength");
		int    max_p = mod->propertyIntThis("MaxEndPole") - pole;
    if (l <= 0)
      throw(MAUS::Exception(MAUS::Exception::recoverable,
        "Enge EndLength must be more than 0 in module "+mod->fullName(), 
        "BTFieldConstructor::GetEndFieldModel"));
    if (x0 <= 0)
      throw(MAUS::Exception(MAUS::Exception::recoverable,
       "Enge CentreLength must be more than 0 in module "+mod->fullName(),
       "BTFieldConstructor::GetEndFieldModel"));
    if (max_p < 0)
      throw(MAUS::Exception(MAUS::Exception::recoverable,
       "Enge MaxEndPole must be >= Pole in module "+mod->fullName(),
       "BTFieldConstructor::GetEndFieldModel"));
    std::vector<double> enge;
		bool propertyExists = true;
		int  engeNumber     = 0;
		while(propertyExists) {
			++engeNumber;
			std::stringstream property;
			property << "Enge" << engeNumber;
			if(mod->propertyExistsThis(property.str(), "double"))
				enge.push_back( mod->propertyDoubleThis(property.str()) );
			else propertyExists = false;
		}
		if (engeNumber == 1)
      throw(MAUS::Exception(MAUS::Exception::recoverable,
          "Must define at least one Enge parameter for Enge end field",
          "BTFieldConstructor::GetEndFieldModel"));
		return new BTMultipole::EngeEndField
                                       (enge, x0, l, max_p);
  }
  throw(MAUS::Exception(MAUS::Exception::recoverable,
          "Failed to recognise EndFieldModel "+
          mod->propertyStringThis("EndFieldType")+" in module "+mod->fullName(),
          "BTFieldConstructor::GetEndFieldModel"));
}

BTField * BTFieldConstructor::GetDerivativesSolenoid(const MiceModule * theModule)
{
  double      peak_field = theModule->propertyDoubleThis("PeakField");
  double      z_max = theModule->propertyDoubleThis("ZMax");
  double      r_max = theModule->propertyDoubleThis("RMax");
  int         order = theModule->propertyIntThis   ("MaxEndPole");
  MAUS::EndFieldModel* end_field = GetEndFieldModel(theModule, 0);
  if (end_field == NULL) {  // hard edged == NULL
      throw(MAUS::Exception(MAUS::Exception::recoverable,
                   "DerivativesSolenoid must have soft edged end field",
                   "BTFieldConstructor::GetDerivativesSolenoid"));
  }
  MAUS::DerivativesSolenoid* sol = new MAUS::DerivativesSolenoid
                                   (peak_field, r_max, z_max, order, end_field);
  return sol;
}

BTField * BTFieldConstructor::GetFieldAmalgamation(const MiceModule * theModule)
{
//  BTFieldAmalgamation(double r_max_, double length_, double z_step_, double r_step_, std::string interpolation_, field_type type_=solenoid);
  double r_max  = theModule->propertyDoubleThis("RMax");
  double length = theModule->propertyDoubleThis("Length");
  double z_step = theModule->propertyDoubleThis("ZStep");
  double r_step = theModule->propertyDoubleThis("RStep");
  std::string interpolation = "LinearCubic";
  if(theModule->propertyExistsThis( "InterpolationAlgorithm", "string" ) )
    interpolation = theModule->propertyStringThis("InterpolationAlgorithm");

  BTFieldAmalgamation* field = new BTFieldAmalgamation(r_max, length, z_step, r_step, interpolation);
  std::vector<const MiceModule*> daughterModules = theModule->findModulesByPropertyExists( "bool", "IsAmalgamated" );
  for(unsigned int i=0; i<daughterModules.size(); i++)
  {
    if(daughterModules[i]->propertyExistsThis("IsAmalgamated", "bool") && daughterModules[i]->propertyExistsThis("FieldType", "string"))
    {
      if(daughterModules[i]->propertyBoolThis("IsAmalgamated")) //include in BTFieldAmalgamation should deal with it
      {
        BTField *   newField    = GetField(daughterModules[i]);  //not amalgamated, use as normal
        Hep3Vector  position    = daughterModules[i]->relativePosition(theModule);//daughterModules[i]);
        double      scaleFactor = daughterModules[i]->globalScaleFactor()/theModule->globalScaleFactor(); //scale factor relative to this
//        std::cout << "AMALGAMATE " << theModule->name() << " gscale " << daughterModules[i]->globalScaleFactor() << " scale " << scaleFactor << std::endl;
        if(newField==NULL)  {;} //error do nothing
        else field->AddField(newField, position, scaleFactor);
        _amalgamatedFields++;
      }
    }
  }
  field->AmalgamateThis();
  return field;
}



void BTFieldConstructor::SetDefaults()
{
	MagnetParameters* MagParams = MagnetParameters::getInstance();
	RFParameters* RFParams = RFParameters::getInstance();
	//Defaults for solenoids
	//"useFiles" for backward compatibility
	if(MagParams->SolDataFiles()==("useFiles"))
		_defaultSolenoidMode = "Read";
	else _defaultSolenoidMode = MagParams->SolDataFiles();
	BTSolenoid::SetStaticVariables(MagParams->NumberNodesRGrid(), MagParams->NumberNodesZGrid(),
	                               MagParams->DefaultNumberOfSheets(), MagParams->SolzMapExtendFactor(),
	                               MagParams->SolrMapExtendFactor());
	//Defaults for Pill box Cavities
	BTPillBox::SetStaticRefParticlePhase (RFParams->rfAccelerationPhase());
	//use default for now BTPillBox::SetStaticPhaseModel(RFParams->phaseModel());
	//Defaults for Quadrupoles
	vector<double> AVec = MagParams->QuadrupoleFringeParameters();
	if(AVec.size()==0) AVec = vector<double>(6);
	BTPhaser::GetInstance()->SetPhaseTolerance (RFParams->rfPhaseTolerance());
	int gridSpace[3] = {MagParams->FieldGridX(),MagParams->FieldGridY(),MagParams->FieldGridZ()};
	BTFieldGroup::SetGridDefault(std::vector<int>(gridSpace, gridSpace+3));
}



void BTFieldConstructor::WriteFieldMaps()
{
	for(unsigned int i=0; i<_fieldMapsForWriting.size(); i++)
	{
		const MiceModule * theModule = _fieldMapsForWriting[i];
		std::string filename = theModule->propertyStringThis("FileName");
		double zMin = theModule->propertyDoubleThis("ZMin");
		double zMax = theModule->propertyDoubleThis("ZMax");
		double rMin = theModule->propertyDoubleThis("RMin");
		double rMax = theModule->propertyDoubleThis("RMax");
		double dz = theModule->propertyDoubleThis("ZStep");
		double dr = theModule->propertyDoubleThis("RStep");

		BTMagFieldMap * fieldMap = new BTMagFieldMap(_magneticField);
		fieldMap->WriteG4MiceTextMap( filename, zMin,zMax,dz,rMin,rMax,dr);
	}
}

void BTFieldConstructor::SetName(BTField* newField, const MiceModule* theModule) const
{
  if(!newField) return;
  _fieldNameIndex++;
  std::stringstream ss;
  ss << "Field" << _fieldNameIndex;
  newField->BTField::SetName( ss.str() );
  if(theModule == NULL) return;
  else if(theModule->propertyExistsThis( "FieldName", "string" ) )
    newField->BTField::SetName( theModule->propertyStringThis("FieldName") );  

}



