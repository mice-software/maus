#include <cxxtest/TestSuite.h>
#include "RATDB.hh"
#include "RATDBManager.hh"
#include <G4UImanager.hh>

using namespace std;

class TestRATDBManager : public CxxTest::TestSuite
{
public:
  RATDB *db;
  RATDBManager *dbm;
  G4UImanager *ui;
  RATDBLinkPtr ltest1;

  void setUp() {
    db = new RATDB();
    dbm = new RATDBManager(db);
    ltest1 = db->GetLink("TEST", "foo");
    ui = G4UImanager::GetUIpointer();
  };

  void tearDown() {
    delete dbm;
    delete db;
  };

  void testLoad() {
    ui->ApplyCommand("/rat/db/load test2.ratdb");
    TS_ASSERT_EQUALS(ltest1->GetI("myint"), 17);
  };

  void testSet() {
    ui->ApplyCommand("/rat/db/set TEST[foo] myint 16");
    TS_ASSERT_EQUALS(ltest1->GetI("myint"), 16);
    ui->ApplyCommand("/rat/db/set TEST[foo] myfloat 1.6");
    TS_ASSERT_DELTA(ltest1->GetF("myfloat"), 1.6, 1e-5);
    ui->ApplyCommand("/rat/db/set TEST[foo] mydouble 16.5d-3");
    TS_ASSERT_DELTA(ltest1->GetD("mydouble"), 16.5e-3, 1e-10);
    ui->ApplyCommand("/rat/db/set TEST[foo] mystring 'ratratrat'");
    TS_ASSERT_EQUALS(ltest1->GetS("mystring"), "ratratrat");
  }

  void testSetAfterLoad() {
    testLoad();
    testSet();
  }

};
   
 
