// MAUS WARNING: THIS IS LEGACY CODE.
//! Data Cards : clone from Geant3 data cards, except:
//! Only user data cards: i.e. specific to the problem.
//! No generic and mysterious "geom" array. One must define
//! these key words explicitly.
//!
//! As suggest - and prototyped - by Mark Fischler, this is
//! based on the STL Map class.
//! This class is a expected to be used as a global singleton.
//!
//! Modified to add 3vector support - Ed Mckigney Aug 21 2002
//! Modified to add std::vector support - Steve Kahn Nov 12 2002
#ifndef dataCards_h
#define dataCards_h 1
#include "Interface/MICEUnits.hh"
#include <string>
#include <map>
#include <vector>
#include <cstdlib>

#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;

typedef std::map < std::string , double > InputDataCardsDouble;
typedef std::map < std::string , Hep3Vector > InputDataCards3Vector;
typedef std::map < std::string , std::string > InputDataCardsString;
typedef std::map<std::string, std::vector<double> > InputDataCardsVector;
typedef std::map<std::string, int >  InputDataCardsInt;

class dataCards
{
  public:
    dataCards(const std::string&);
    dataCards(int);
    dataCards();
    ~dataCards();

    std::string getClient();
    int readKeys(const char* fileName);
    int readKeys(std::istream& in);

    double fetchValueDouble(const std::string& key);
    int fetchValueInt(const std::string& key);
    Hep3Vector fetchValue3Vector(const std::string& key);
    std::string fetchValueString(const std::string& key);
    std::vector<double>  fetchValueVector(const std::string&);
    void printValue(const std::string& key);
    friend std::ostream &operator<<(std::ostream &o, const dataCards& d);

    void AddDataCard( std::string aName, std::string aDefaultValue = "" );
    void AddDataCard( std::string aName, int aDefaultValue );
    void AddDataCard( std::string aName, double aDefaultValue );
    void AddDataCard( std::string aName, std::vector< double > aDefaultValue );
    void AddDataCard( std::string aName, const unsigned int aSize, double * aDefaultValue );

    /*
    How to use the above:

    1.) Adding string property 'foo' with empty default value
    AddDataCard( "foo" );

    2.) Adding string property 'foo' with default value 'bar'
    AddDataCard( "foo", "bar" );

    3.) Adding integer property 'foo' with default value 0
    AddDataCard( "foo", 0 );

    4.) Adding double property 'foo' with default value 0.0 --> you should explicitly appoint the type to double by adding a decimal point
    AddDataCard( "foo", 0.0 );

    5.1.) Adding vector of double
    vector< double > bar;
    bar.push_back( 3.14 );
    bar.push_back( -273.15 ); 
    AddDataCard( "foo", bar );

    5.2.) Adding vector of double
    double * bar;
    bar = new double[ 2 ];
    bar[ 0 ] = 3.14;
    bar[ 1 ] = -273.15;
    AddDataCard( "foo", 2, bar );	
    */

  private:

    // return the CLHEP SystemOf Units constant that corresponds to the
    // name passed as a string; card name is passed to give a good error
    // message on fail
    double getUnits( std::string card, std::string unit  );
    void fillCards(int);
    void initialiseUnits();
    void initializeApplicationCodes();

    InputDataCardsDouble cd;
    InputDataCards3Vector c3v;
    InputDataCardsString cs;
    InputDataCardsVector cv;
    InputDataCardsInt  ci;
    void fillVector(const char *, const int&, const double*);
    MICEUnits units;
    std::map<std::string,int> appCode;
    std::string client;
};

#endif
