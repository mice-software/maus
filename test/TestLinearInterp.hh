#include <cxxtest/TestSuite.h>
#include <LinearInterp.hh>
#include <string_utilities.hpp>

using namespace std;

class TestLinearInterp : public CxxTest::TestSuite
{
 public:
  vector<float> makeVector(int npoints, float array[])
  {
    vector<float> v(npoints);
    for (int i = 0; i < npoints; i++)
      v[i] = array[i];
    return v;
  }

  void testOddPoints()
  {
    const int npoints = 3;
    float x[npoints] = { 1.0, 2.0, 3.0 };
    float y[npoints] = { 2.0, 4.0, 6.0 };

    LinearInterp<float> line(npoints, x, y);
    LinearInterp<float> linevect(makeVector(npoints, x), 
				 makeVector(npoints, y));
    
    const int testpoints = 100;
    for (int i = 0; i < testpoints; i++) {
      float xeval = 1.0 + i * 2.0 / testpoints;

      TS_ASSERT_DELTA(line(xeval), 2.0 * xeval, 1e-5);
      TS_ASSERT_DELTA(linevect(xeval), 2.0 * xeval, 1e-5);
    }
  };

  void testEvenPoints()
  {
    const int npoints = 4;
    float x[npoints] = { 1.0, 2.0, 3.0, 4.0 };
    float y[npoints] = { 2.0, 4.0, 6.0, 8.0 };

    LinearInterp<float> line(npoints, x, y);
    LinearInterp<float> linevect(makeVector(npoints, x), 
				 makeVector(npoints, y));
    
    
    const int testpoints = 100;
    for (int i = 0; i < testpoints; i++) {
      float xeval = 1.0 + i * 3.0 / testpoints;

      TS_ASSERT_DELTA(line(xeval), 2.0 * xeval, 1e-5);
      TS_ASSERT_DELTA(linevect(xeval), 2.0 * xeval, 1e-5);
    }
  };

  void testLowRange()
  {
    const int npoints = 4;
    float x[npoints] = { 1.0, 2.0, 3.0, 4.0 };
    float y[npoints] = { 2.0, 4.0, 6.0, 8.0 };

    LinearInterp<float> line(npoints, x, y);
    TS_ASSERT_THROWS_EQUALS(line(0.0), LinearInterp<float>::RangeError &r,
			    r, LinearInterp<float>::RangeError(1.0, 4.0, 0.0));
  };

  void testHighRange()
  {
    const int npoints = 4;
    float x[npoints] = { 1.0, 2.0, 3.0, 4.0 };
    float y[npoints] = { 2.0, 4.0, 6.0, 8.0 };

    LinearInterp<float> line(npoints, x, y);
    TS_ASSERT_THROWS_EQUALS(line(5.0), LinearInterp<float>::RangeError &r,
			    r, LinearInterp<float>::RangeError(1.0, 4.0, 5.0));
  };

  void testDiscontinuity()
  {
    // Monotonically decreasing function should be decreasing always
    // Get parameters from database
    RATDB db;
    db.LoadAll(string(getenv("GLG4DATA")));
    RATDBLinkPtr libd = db.GetLink("IBD");

    float Emin = libd->GetF("emin");
    float Emax = libd->GetF("emax");
    LinearInterp<float> func(libd->GetFArray("spec_e"),
			     libd->GetFArray("spec_flux"));
    float xlast = -1.0;
    float last = 50.0;
    for (int i = 0; i < 1000; i++) {
      float xtest = Emin + (Emax - Emin) * i / 1000;
      float funcval = func(xtest);
      TSM_ASSERT_LESS_THAN(dformat("x = %f, %f", xtest, xlast).c_str(),
			   funcval, last);
			   
      last = funcval;
      xlast = xtest;
    }
  };
};
