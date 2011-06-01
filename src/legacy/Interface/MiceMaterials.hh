// MAUS WARNING: THIS IS LEGACY CODE.
// MiceMaterials.hh
//
// A map of G4Material pointers to strings which can be accessed through the MICE Run for use in the
// GEANT4 representation of a particular MICE configuration
//
// 25th May 2006
//
// Malcolm Ellis

#ifndef INTERFACE_MICEMATERIALS_HH
#define INTERFACE_MICEMATERIALS_HH 1

#include <cstdlib>
#include <string>
#include <map>

class G4Material;
class G4IonisParamMat;

class MiceMaterials
{
  public :

    MiceMaterials()				{};

    MiceMaterials( bool );

    ~MiceMaterials()				{};

    void		addMaterial( G4Material*, std::string );

    G4Material*		materialByName( std::string ) const;

    const std::map<std::string,G4Material*>&	materialMap() const	{ return _materials; };

    void		listMaterials() const;

    std::string		chemicalFomula( std::string ) const;

    double		density( std::string ) const;

    std::string		state( std::string ) const;

    double		temperature( std::string ) const;

    double		pressure( std::string ) const;

    double		electronDensity( std::string ) const;

    double		radiationLength( std::string ) const;
   
    double		zNumber( std::string ) const;

    double		aNumber( std::string ) const;
    
    double		meanExcitationEnergy( std::string ) const;
    
    double		densityCorrection( std::string ) const;
    
    double		interactionLength( std::string ) const;

    double		dEdx( std::string ) const;

    double		dEdxLength( std::string ) const;

    void		updateProperties( std::string, std::string, double, std::string, double, double, double, double, double, double, double, double, double );

  private :

    std::map<std::string,G4Material*>	_materials;

    std::map<std::string,std::string>	_formula;
    std::map<std::string,double>	_density;
    std::map<std::string,std::string>	_state;
    std::map<std::string,double>	_temperature;
    std::map<std::string,double>	_pressure;
    std::map<std::string,double>	_electronDensity;
    std::map<std::string,double>	_radiationLength;
    std::map<std::string,double>	_interactionLength;
    std::map<std::string,double>	_zNumber;
    std::map<std::string,double>	_aNumber;
    std::map<std::string,double>	_meanExcitationEnergy;
    std::map<std::string,double>	_densityCorrection;
};

#endif
