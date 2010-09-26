#include <cxxtest/TestSuite.h>
#include <RATDB.hh>
#include <IBDgen.hh>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>

using namespace std;

class TestIBDgen : public CxxTest::TestSuite
{
 public:
  static TestIBDgen *createSuite() { return new TestIBDgen(); }
  static void destroySuite( TestIBDgen *suite ) { delete suite; }

  TFile *f;
  float Emax;
  float Emin;

  TestIBDgen() {
    RATDB *db = RATDB::Get();
    db->Clear();
    db->LoadAll(string(getenv("GLG4DATA")));
    f = TFile::Open("TestIBDgen.root", "RECREATE");

    RATDBLinkPtr libd = db->GetLink("IBD");

    Emax = libd->GetF("emax");
    Emin = libd->GetF("emin");
  };

  ~TestIBDgen() {
    f->Close();
  };


  void testCrossSection()
  {
    const int e_points = 100;
    const int costheta_points = 20;

    f->cd();
    TH2F *h = new TH2F("xs", "Inverse beta decay cross-section", 
		       e_points, Emin, Emax, costheta_points, -1.0, 1.0);
    h->GetXaxis()->SetTitle("Energy (MeV)");
    h->GetYaxis()->SetTitle("cos(#theta)_{lab} e^{+}");
 
    for (int i = 0; i < e_points; i++) {
      for (int j = 0; j < costheta_points; j++) {
	double E = Emin + (Emax - Emin) * i / e_points;
	double costheta = -1.0 + 2.0 * j / costheta_points;

	double xs;
	TS_ASSERT_THROWS_NOTHING(xs = IBDgen::CrossSection(E, costheta));
	h->SetBinContent(i+1, j+1, xs);
      }
    }

    h->Write();
  };

  void testFlux()
  {
    IBDgen ibd;
    const int e_points = 100;

    f->cd();
    TH1F *h = new TH1F("flux", "Nu flux", e_points, Emin, Emax);
    h->GetXaxis()->SetTitle("Energy (MeV)");
 
    for (int i = 0; i < e_points; i++) {
      double E = Emin + (Emax - Emin) * i / e_points;
      
      TS_ASSERT_THROWS_NOTHING(h->SetBinContent(i+1, ibd.Flux(E)));
    }

    h->Write();
  };


  void testGenInteraction()
  {
    f->cd();
    TH2F *h = new TH2F("e_cos", "Energy/cos(#theta)_{lab} distribution", 
		       20, Emin, Emax, 20, -1.0, 1.0);
    h->GetXaxis()->SetTitle("Energy (MeV)");
    h->GetYaxis()->SetTitle("cos(#theta) e^{+}");

    IBDgen ibd;

    for (int i=0; i < 5000; i++) {
      float E, CosThetaLab;
      ibd.GenInteraction(E, CosThetaLab);

      h->Fill(E, CosThetaLab);
    }

    h->Write();
  };

  void testMomentumConserve()
  {
    IBDgen ibd;

    Hep3Vector nu_dir(0.0, 0.0, 1.0);
    HepLorentzVector nu, eplus, n;

    for (int i=0; i < 1000; i++) {
      ibd.GenEvent(nu_dir, nu, eplus, n);
      
      Hep3Vector diff = nu.vect() - eplus.vect() - n.vect();
      TS_ASSERT_DELTA(diff.mag(), 0.0, 1e-5);
    }
  };

  void testGenInteractionRange()
  {
    IBDgen ibd;

    for (int i=0; i < 100; i++) {
      float E, CosThetaLab;
      ibd.GenInteraction(E, CosThetaLab);

      TS_ASSERT_LESS_THAN(E, Emax);
      TS_ASSERT_LESS_THAN_EQUALS(Emin, E);
      
      TS_ASSERT_LESS_THAN_EQUALS(CosThetaLab, 1.0);
      TS_ASSERT_LESS_THAN_EQUALS(-1.0, CosThetaLab);
    }
  };


  void testCosTheta() {
    // Verify that cos(theta) distribution from GenInteraction()
    // is preserved by GenEvent()  (checks rotation matrix)
    TH1F *genInter = new TH1F("cos_int", 
			      "cos(#theta)_{lab} from GenInteraction()",
			      50, -1.0, 1.0);

    TH1F *genEv = new TH1F("cos_ev", 
			   "cos(#theta)_{lab} from GenEvent()",
			   50, -1.0, 1.0);
    IBDgen ibd;

    for (int i=0; i < 10000; i++) {
      float E, CosThetaLab;
      ibd.GenInteraction(E, CosThetaLab);
      genInter->Fill(CosThetaLab);

      Hep3Vector nu_dir(0.0, 1.0, 0.0);
      HepLorentzVector nu, eplus, n; 
      ibd.GenEvent(nu_dir, nu, eplus, n);
      
      genEv->Fill(nu.vect().cosTheta(eplus.vect()));
    }

    // KS-test is pretty harsh, going to be sloppy here.
    // Should only fail 0.01% of the time
    TS_ASSERT_LESS_THAN(0.0001, genInter->KolmogorovTest(genEv));

    genInter->Write();
    genEv->Write();
  };

};
