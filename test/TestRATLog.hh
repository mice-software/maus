#include <cxxtest/TestSuite.h>
#include <cxxtest/GlobalFixture.h>
#include "RATLog.hh"

using namespace std;


class Fixture1 : public CxxTest::GlobalFixture
{
public:
  bool setUpWorld() { 
    RATLog::Init("testrat.log", RATLog::DETAIL, RATLog::DEBUG);
    return true;
  };
};

// Ensure that RATLog isn't trapping output from other tests
static Fixture1 init_fixture;

class TestRATLog : public CxxTest::TestSuite
{
public:
  void testNormal() {
    RATLog::Init("test.log", RATLog::INFO, RATLog::DETAIL);

    warn << "You should see this warning" << newline;
    info << "You should see this info line" << newline;
    detail << "This detail line should only be in the log file" << newline;
    debug << "This should not be visible anywhere" << newline;
  };

  void testDebug() {
    RATLog::Init("testdebug.log", RATLog::INFO, RATLog::DEBUG);

    warn << "You should see this warning" << newline;
    info << "You should see this info line" << newline;
    detail << "This detail line should only be in the log file" << newline;
    debug << "This debug line should also be in the log file" << newline;
  };

  // Leave this test commented out since it terminates the program
//   void testDie() {
//     RATLog::Init("testdie.log", RATLog::INFO, RATLog::DETAIL);

//     RATLog::Die("terminating immediately");
//     warn << "You should never see this." << newline;
//   };

};
   
 
