#include <cxxtest/TestSuite.h>
#include "RATGeoBuilder.hh"
#include <G4SolidStore.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4PhysicalVolumeStore.hh>
#include "BWDetectorConstruction.hh"
#include "RATLog.hh"

using namespace std;

class TestRATGeoBuilder : public CxxTest::TestSuite
{
 public:
  void testLoad() {
    BWDetectorConstruction bw;

    RATDB *db = RATDB::Get();
    db->Clear();
    db->LoadDefaults();
    db->Load("testsimple.geo");

    bw.ConstructMaterials();

    RATGeoBuilder geo;
    geo.ConstructAll("GEO");
    
    checkVolume("world");
    checkVolume("cave");
    checkVolume("tank");
    checkVolume("buffer");
    checkVolume("av");
    checkVolume("scint");
  };

  void checkVolume(string name) {
    bool found;

    found = false;
    G4SolidStore *sstore = G4SolidStore::GetInstance();
    for (G4SolidStore::iterator i = sstore->begin(); i != sstore->end(); i++)
      if ( (*i)->GetName() == G4String(name) ) {
	found = true;
	break;
      }	
    TSM_ASSERT("Solid " + name + " not found in store", found);

    found = false;
    G4LogicalVolumeStore *lstore = G4LogicalVolumeStore::GetInstance();
    for (G4LogicalVolumeStore::iterator i = lstore->begin(); i != lstore->end(); i++)
      if ( (*i)->GetName() == G4String(name) ) {
	found = true;
	break;
      }	
    TSM_ASSERT("Logical volume " + name + " not found in store", found);

    found = false;
    G4PhysicalVolumeStore *pstore = G4PhysicalVolumeStore::GetInstance();
    for (G4PhysicalVolumeStore::iterator i = pstore->begin(); i != pstore->end(); i++)
      if ( (*i)->GetName() == G4String(name) ) {
	found = true;
	break;
      }	
    TSM_ASSERT("Physical volume " + name + " not found in store", found);
  };
};
