#include <cxxtest/TestSuite.h>
#include "RATDB.hh"

using namespace std;

class TestRATDB : public CxxTest::TestSuite
{
public:
  
  void setUp() {
    db = new RATDB();
  };

  void tearDown() {
    delete db;
  };
   
 
  void testGetDefault() {
    TS_ASSERT_DIFFERS(RATDB::Get(), static_cast<RATDB *>(0));
  };

  void testLoad() {
    TS_ASSERT_EQUALS(db->Load("test.ratdb"), 1);
    TS_TRACE("Should see error message next.");
    TS_ASSERT_EQUALS(db->Load("noexist.ratdb"), 0);
  };

  void testLoadAll() {
    TS_ASSERT_EQUALS(db->LoadAll("."), 1);
    TS_ASSERT_DIFFERS(db->GetDefaultTable("MEDIA", "vacuum"), 
		      static_cast<RATDBTable *>(0));
    TS_ASSERT_DIFFERS(db->GetDefaultTable("TEST", "foo"), 
		      static_cast<RATDBTable *>(0));
    TS_ASSERT_EQUALS(db->GetDefaultTable("NOEXIST", ""), 
		     static_cast<RATDBTable *>(0));
  };

  void testGetLink() {
    RATDBLinkPtr lmedia;
    TS_ASSERT_THROWS_NOTHING(db->Load("test.ratdb"));
    TS_ASSERT_THROWS_NOTHING(lmedia = db->GetLink("MEDIA"));
  };

  void testGetLinkGroup() {
    RATDBLinkGroup lmediag;
    TS_ASSERT_THROWS_NOTHING(db->Load("test.ratdb"));
    TS_ASSERT_THROWS_NOTHING(lmediag = db->GetLinkGroup("MEDIA"));
    
    TS_ASSERT_EQUALS(lmediag["vacuum"]->GetS("media_name"), "Vacuum");
    TS_ASSERT_EQUALS(lmediag["h2o"]->GetS("media_name"), "Light water");
  };


  void testNoExistUserSet() {
    db->Load("test2.ratdb");
    RATDBLinkPtr ltest0 = db->GetLink("TEST", "");

    // Verify we're looking at right index (TEST[1] has myint)
    TS_ASSERT_THROWS_EQUALS(ltest0->GetI("myint"), RATDBNotFoundError &r,
			    r, RATDBNotFoundError("TEST", "", "myint"));
    TS_ASSERT_THROWS_EQUALS(ltest0->GetS("foo"), RATDBNotFoundError &r,
			    r, RATDBNotFoundError("TEST", "", "foo"));

    // Implicitly setting index ""
    db->SetI("TEST", "myint", 17);

    // Verify link sees this new value
    TS_ASSERT_EQUALS(ltest0->GetI("myint"), 17);    
  };

  void testParseTableName() {
    string title;
    string index;

    TS_ASSERT_EQUALS(RATDB::ParseTableName("TEST", title, index), true);
    TS_ASSERT_EQUALS(title, string("TEST"));
    TS_ASSERT_EQUALS(index, "");

    TS_ASSERT_EQUALS(RATDB::ParseTableName("ABC[x]", title, index), true);
    TS_ASSERT_EQUALS(title, string("ABC"));
    TS_ASSERT_EQUALS(index, "x");

    TS_ASSERT_EQUALS(RATDB::ParseTableName("XXX[ab12]", title, index), true);
    TS_ASSERT_EQUALS(title, string("XXX"));
    TS_ASSERT_EQUALS(index, "ab12");

    TS_ASSERT_EQUALS(RATDB::ParseTableName("TE+", title, index), false);
    TS_ASSERT_EQUALS(RATDB::ParseTableName("124d", title, index), false);
    TS_ASSERT_EQUALS(RATDB::ParseTableName("XXX[ab1]A", title, index), false);
    TS_ASSERT_EQUALS(RATDB::ParseTableName("!", title, index), false);
  };

  RATDB *db;
};


class TestRATDBLink : public CxxTest::TestSuite
{
public:
  RATDB *db;
  RATDBLinkPtr ltest0;
  RATDBLinkPtr ltest1;
  

  void setUp() {
    db = new RATDB();
    ltest0 = db->GetLink("TEST", "");
    ltest1 = db->GetLink("TEST", "foo");
    db->Load("test2.ratdb");
  };

  void tearDown() {
    delete db;
  };

  void testGet() {
    TS_ASSERT_EQUALS(ltest1->GetI("myint"), 17); // user
    TS_ASSERT_DELTA(ltest1->GetF("myfloat"), 14.0, 1e-5); // user
    TS_ASSERT_DELTA(ltest1->GetD("mydouble"), 5.0, 1e-9); // default
    TS_ASSERT_EQUALS(ltest1->GetS("mystring"), "default"); // default
  };

  void testGetArray() {
    const vector<int> &ia = ltest1->GetIArray("ia");
    TS_ASSERT_EQUALS(ia[4], 5); // default

    const vector<float> &fa = ltest1->GetFArray("fa");
    TS_ASSERT_DELTA(fa[1], 4.0, 1e-5); // default

    const vector<double> &da = ltest1->GetDArray("da");
    TS_ASSERT_DELTA(da[2], 24.0, 1e-9); // user

    const vector<string> &sa = ltest1->GetSArray("sa");
    TS_ASSERT_EQUALS(sa[0], "alvin"); // user
  };

  void testSmartPtr() {
    for (int i = 0; i < 100; i++) {
      // Create a whole bunch of links
      RATDBLinkPtr ltemp = db->GetLink("TEST", "foo");
    }
    
    // All those links are now out of scope, so there should just
    // be the two links we started with
    TS_ASSERT_EQUALS(db->NumLinks(), 2);
  }
  
};
   
 
