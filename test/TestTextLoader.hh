#include <cxxtest/TestSuite.h>
#include <cxxtest/ValueTraits.h>
#include <RATDB_TextLoader.hh>
#include <string>

using namespace std;

/************************* Parser Tests **********************/

namespace CxxTest
{
  CXXTEST_TEMPLATE_INSTANTIATION
    class ValueTraits<ParseError>
  {
    char _s[256];

  public:
    ValueTraits( const ParseError &p) { 
      sprintf(_s, "ParseError(%d, %d, \"%s\", \"%s\")", p.linenum, p.colnum,
	      p.line.c_str(), p.message.c_str());
    };

    const char *asString() const { return _s; };

  };
}

class TestParseError : public CxxTest::TestSuite
{
 public:
  
  void testEqual()
  {
    ParseError p(2, 3, "foo bar", "my test");
    p.colnum = 5;

    TS_ASSERT_EQUALS(p, ParseError(2, 5, "foo bar", "my test"));
  };

  void testUnequal()
  {
    ParseError p(2, 3, "foo", "bar");
    p.colnum = 5;

    TS_ASSERT_DIFFERS(p, ParseError(2, 3, "foo", "bar"));
  };
};


/*************************Tokenizer Tests************************/

// More pretty printing help
CXXTEST_ENUM_TRAITS( Tokenizer::Type,
		     CXXTEST_ENUM_MEMBER(Tokenizer::TYPE_IDENTIFIER) 
		     CXXTEST_ENUM_MEMBER(Tokenizer::TYPE_INTEGER)
		     CXXTEST_ENUM_MEMBER(Tokenizer::TYPE_FLOAT)
		     CXXTEST_ENUM_MEMBER(Tokenizer::TYPE_DOUBLE)
		     CXXTEST_ENUM_MEMBER(Tokenizer::TYPE_STRING)
		     CXXTEST_ENUM_MEMBER(Tokenizer::TYPE_SYMBOL)
		     CXXTEST_ENUM_MEMBER(Tokenizer::TYPE_EOF)
		     CXXTEST_ENUM_MEMBER(Tokenizer::TYPE_ERROR)
		     CXXTEST_ENUM_MEMBER(Tokenizer::TYPE_EOF_ERROR) )


class TestTokenizer : public CxxTest::TestSuite 
{
 public:
  
  void testEmpty()
  {
    Tokenizer t("");
    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_EOF);
  };

  void testSymbols()
  {
    string symbols = "{}[],:";
    Tokenizer t(symbols);
    
    for (unsigned int i=0; i < symbols.size(); i++) {
      TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_SYMBOL);
      TS_ASSERT_EQUALS(t.Token(), symbols.substr(i, 1));
    }
  };

  void testInteger()
  {
    Tokenizer t("1 17\n 35");
    
    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_INTEGER);
    TS_ASSERT_EQUALS(t.Token(), "1");
    TS_ASSERT_EQUALS(t.AsInt(), 1);

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_INTEGER);
    TS_ASSERT_EQUALS(t.Token(), "17");
    TS_ASSERT_EQUALS(t.AsInt(), 17);

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_INTEGER);
    TS_ASSERT_EQUALS(t.Token(), "35");
    TS_ASSERT_EQUALS(t.AsInt(), 35);
  };

  void testHexInteger()
  {
    Tokenizer t("0xef 0xFFFF 0xdeadbeef");

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_INTEGER);
    TS_ASSERT_EQUALS(t.Token(), "0xef");
    TS_ASSERT_EQUALS(t.AsInt(), 0xef);

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_INTEGER);
    TS_ASSERT_EQUALS(t.Token(), "0xFFFF");
    TS_ASSERT_EQUALS(t.AsInt(), 0xffff);

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_INTEGER);
    TS_ASSERT_EQUALS(t.Token(), "0xdeadbeef");
    TS_ASSERT_EQUALS(t.AsInt(), 0xdeadbeef);
  };

  void testFloat()
  {
    Tokenizer t("1.3 1. 1.0e14 0.1 0.23456789");

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_FLOAT);
    TS_ASSERT_EQUALS(t.Token(), "1.3");
    TS_ASSERT_DELTA(t.AsFloat(), 1.3, 1e-4);

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_FLOAT);
    TS_ASSERT_EQUALS(t.Token(), "1.");
    TS_ASSERT_DELTA(t.AsFloat(), 1.0, 1e-4);

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_FLOAT);
    TS_ASSERT_EQUALS(t.Token(), "1.0e14");
    TS_ASSERT_DELTA(t.AsFloat(), 1.0e14, 1e7);

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_FLOAT);
    TS_ASSERT_EQUALS(t.Token(), "0.1");
    TS_ASSERT_DELTA(t.AsFloat(), 0.1, 1e-4);

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_FLOAT);
    TS_ASSERT_EQUALS(t.Token(), "0.23456789");
    TS_ASSERT_DELTA(t.AsFloat(), .23456789, 1e-5);
  };

  void testDouble()
  {
    Tokenizer t("1.3d 1.d 1.0d14 0.1d 0.23456789d");

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_DOUBLE);
    TS_ASSERT_EQUALS(t.Token(), "1.3d");
    TS_ASSERT_DELTA(t.AsDouble(), 1.3, 1e-9);

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_DOUBLE);
    TS_ASSERT_EQUALS(t.Token(), "1.d");
    TS_ASSERT_DELTA(t.AsDouble(), 1.0, 1e-9);

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_DOUBLE);
    TS_ASSERT_EQUALS(t.Token(), "1.0d14");
    TS_ASSERT_DELTA(t.AsDouble(), 1.0e14, 1.0);

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_DOUBLE);
    TS_ASSERT_EQUALS(t.Token(), "0.1d");
    TS_ASSERT_DELTA(t.AsDouble(), 0.1, 1e-9);

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_DOUBLE);
    TS_ASSERT_EQUALS(t.Token(), "0.23456789d");
    TS_ASSERT_DELTA(t.AsDouble(), .23456789, 1e-9);
  };

  void testString()
  {
    Tokenizer t("\"double quote\"  'single quote'");

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_STRING);
    TS_ASSERT_EQUALS(t.Token(), "double quote");

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_STRING);
    TS_ASSERT_EQUALS(t.Token(), "single quote");
  };

  void testStringEscape()
  {
    Tokenizer t("\"\\n\\b\\\\s\\/\"");

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_STRING);
    TS_ASSERT_EQUALS(t.Token(), "\n\b\\s/");
  };

  void testStringNoClose()
  {
    Tokenizer t("\"fkdaljal;");
    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_EOF_ERROR);
  };

  void testIdentifier()
  {
    Tokenizer t("iden id1 __foo_  _1fds");

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_IDENTIFIER);
    TS_ASSERT_EQUALS(t.Token(), "iden");

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_IDENTIFIER);
    TS_ASSERT_EQUALS(t.Token(), "id1");

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_IDENTIFIER);
    TS_ASSERT_EQUALS(t.Token(), "__foo_");

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_IDENTIFIER);
    TS_ASSERT_EQUALS(t.Token(), "_1fds");
  };

  void testCommentLong()
  {
    Tokenizer t("iden /* foo\nmore\ntest\nstuff */ bar");

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_IDENTIFIER);
    TS_ASSERT_EQUALS(t.Token(), "iden");

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_IDENTIFIER);
    TS_ASSERT_EQUALS(t.Token(), "bar");
  };

  void testCommentLine()
  {
    Tokenizer t("iden// this is a comment\nbar");

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_IDENTIFIER);
    TS_ASSERT_EQUALS(t.Token(), "iden");

    TS_ASSERT_EQUALS(t.Next(), Tokenizer::TYPE_IDENTIFIER);
    TS_ASSERT_EQUALS(t.Token(), "bar");
  }

  void testRaiseError()
  {
    Tokenizer t("foo bar");
    t.Next();
    TS_ASSERT_THROWS_EQUALS(t.RaiseError("my test"), ParseError &p,
			    p, ParseError(1, 3, "foo bar\n", "my test"));
  };
    
};

/****************************Test Parser ****************************/

class TestParser: public CxxTest::TestSuite
{
 public:
  
  void testEmpty() {
    Parser p("");
    RATDBTable *t;

    try {
      t = p.Next();
      TS_ASSERT_EQUALS(t, static_cast<RATDBTable *>(0));
    } catch ( const ParseError &p) {
      TS_FAIL(p.GetFull());
    }
  };

  void testNoFields() {
    Parser p("{}");
    RATDBTable *t;

    try {
      t = p.Next();
      TS_ASSERT_DIFFERS(t, static_cast<RATDBTable *>(0));
    } catch ( const ParseError &p) {
      TS_FAIL(p.GetFull());
    }
  };

  void testInt() {
    Parser p("{foo : 13}");
    RATDBTable *t;
    try {
      t = p.Next();
      TS_ASSERT_DIFFERS(t, static_cast<RATDBTable *>(0));
      TS_ASSERT_EQUALS(t->GetI("foo"), 13);
    } catch ( const ParseError &p) {
      TS_FAIL(p.GetFull());
    }
  };

  void testFloat() {
    Parser p("{foo : 1.3}");
    RATDBTable *t;
    try {
      t = p.Next();
      TS_ASSERT_DIFFERS(t, static_cast<RATDBTable *>(0));
      TS_ASSERT_DELTA(t->GetF("foo"), 1.3, 1e-5);
    } catch ( const ParseError &p) {
      TS_FAIL(p.GetFull());
    }
  };

  void testDouble() {
    Parser p("{foo : 1.3d}");
    RATDBTable *t;
    try {
      t = p.Next();
      TS_ASSERT_DIFFERS(t, static_cast<RATDBTable *>(0));
      TS_ASSERT_DELTA(t->GetD("foo"), 1.3, 1e-12);
    } catch ( const ParseError &p) {
      TS_FAIL(p.GetFull());
    }
  };

  void testString() {
    Parser p("{foo : \"this is a test\"}");
    RATDBTable *t;
    try {
      t = p.Next();
      TS_ASSERT_DIFFERS(t, static_cast<RATDBTable *>(0));
      TS_ASSERT_EQUALS(t->GetS("foo"), "this is a test");
    } catch ( const ParseError &p) {
      TS_FAIL(p.GetFull());
    }
  };

  void testMany() {
    Parser p("{int: 13,float:0.1,\ndouble : 1.2d\n,string : \"abc\"}");
    RATDBTable *t;
    try {
      t = p.Next();
      TS_ASSERT_DIFFERS(t, static_cast<RATDBTable *>(0));
      TS_ASSERT_EQUALS(t->GetI("int"), 13);
      TS_ASSERT_DELTA(t->GetF("float"), 0.1, 1e-5);
      TS_ASSERT_DELTA(t->GetD("double"), 1.2, 1e-12);
      TS_ASSERT_EQUALS(t->GetS("string"), "abc");
    } catch ( const ParseError &p) {
      TS_FAIL(p.GetFull());
    }
  };

  void testIntegerArray() {
    Parser p("{foo : [1, 2, 3, 4]}");
    vector<int> a(4);
    a[0] = 1; a[1] = 2; a[2] = 3; a[3] = 4;

    RATDBTable *t;
    try {
      t = p.Next();
      TS_ASSERT_DIFFERS(t, static_cast<RATDBTable *>(0));
      TS_ASSERT_EQUALS(t->GetIArray("foo"), a);
    } catch ( const ParseError &p) {
      TS_FAIL(p.GetFull());
    }
  };

  void testFloatArray() {
    Parser p("{foo : [1.2, 2.2, 3.3, 4.5]}");
    vector<float> a(4);
    a[0] = 1.2; a[1] = 2.2; a[2] = 3.3; a[3] = 4.5;

    RATDBTable *t;
    try {
      t = p.Next();
      TS_ASSERT_DIFFERS(t, static_cast<RATDBTable *>(0));
      TS_ASSERT_EQUALS(t->GetFArray("foo"), a);
    } catch ( const ParseError &p) {
      TS_FAIL(p.GetFull());
    }
  };

  void testDoubleArray() {
    Parser p("{foo : [1.2d, 2.2d, 3.3d, 4.5d]}");
    vector<double> a(4);
    a[0] = 1.2; a[1] = 2.2; a[2] = 3.3; a[3] = 4.5;

    RATDBTable *t;
    try {
      t = p.Next();
      TS_ASSERT_DIFFERS(t, static_cast<RATDBTable *>(0));
      TS_ASSERT_EQUALS(t->GetDArray("foo"), a);
    } catch ( const ParseError &p) {
      TS_FAIL(p.GetFull());
    }
  };

  void testStringArray() {
    Parser p("{foo : ['a', 'b', 'c', 'd']}");
    vector<string> a(4);
    a[0] = "a"; a[1] = "b"; a[2] = "c"; a[3] = "d";

    RATDBTable *t;
    try {
      t = p.Next();
      TS_ASSERT_DIFFERS(t, static_cast<RATDBTable *>(0));
      TS_ASSERT_EQUALS(t->GetSArray("foo"), a);
    } catch ( const ParseError &p) {
      TS_FAIL(p.GetFull());
    }
  };

  void testMixed() {
    Parser p("{foo : ['a', 1.3, 'b', 'c'] }");

    RATDBTable *t;
    TS_ASSERT_THROWS_EQUALS( t = p.Next(), const ParseError &e,
			     e.message, 
			     string("Mixed types in array not allowed"));
  };

};


class TestTextLoader : public CxxTest::TestSuite
{
 public:

  void testLoad()
  {
    try {
      list<RATDBTable *> tables;
      tables = RATDB_TextLoader("test.ratdb");
      TS_ASSERT_EQUALS(tables.size(), 2);

      // Spot check some values
      list<RATDBTable *>::iterator i = tables.begin();
      TS_ASSERT_EQUALS((*i)->GetS("name"), "MEDIA");
      TS_ASSERT_EQUALS((*i)->GetS("index"), "vacuum");
      TS_ASSERT_EQUALS((*i)->GetS("media_name"), "Vacuum");
      TS_ASSERT_EQUALS((*i)->GetI("optical_flag"), 1.0);
      TS_ASSERT_DELTA((*i)->GetF("refractive_index"), 1.0, 1e-5);
      TS_ASSERT_DELTA((*i)->GetFArray("absorb_coeff_data_abs")[1], 
		      1.0e-6, 1e-10);
      
    
    } catch (ParseError &p) {
      TS_FAIL("Parse error");
    }
  };

  
};
