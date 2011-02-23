#include "gtest/gtest.h" 

#include "src/common/Interface/TofHit.hh"


namespace {
bool TofHitEquality(TofHit th1, TofHit th2, double float_tolerance) {
  bool equal = true;
  equal &= th1.GetTrackID()   == th2.GetTrackID();
  equal &= th1.GetStripNo()   == th2.GetStripNo();
  equal &= th1.GetPlaneNo()   == th2.GetPlaneNo();
  equal &= th1.GetStationNo() == th2.GetStationNo();
  equal &= th1.GetPID()       == th2.GetPID();

  equal &= th1.GetVolumeName() == th2.GetVolumeName();

  equal &= fabs(th1.GetTime()   - th2.GetTime())   < float_tolerance;
  equal &= fabs(th1.GetEnergy() - th2.GetEnergy()) < float_tolerance;
  equal &= fabs(th1.GetMass()   - th2.GetMass())   < float_tolerance;
  equal &= fabs(th1.GetEdep()   - th2.GetEdep())   < float_tolerance;
  equal &= fabs(th1.GetPathLength() - th2.GetPathLength()) < float_tolerance;

  equal &= fabs( (th1.GetPosition() - th2.GetPosition()).mag() ) < float_tolerance;
  equal &= fabs( (th1.GetMomentum() - th2.GetMomentum()).mag() ) < float_tolerance;
  return equal;
}

class TofHitTest : public ::testing::Test
{
protected:
  TofHitTest () { 
    th_all.SetTrackID(1);
    th_all.SetStripNo(2);
    th_all.SetPlaneNo(3);
    th_all.SetStationNo(4);
    th_all.SetEdep(5.);
    th_all.SetPathLength(6.);
    th_all.SetPosition(ThreeVector(7., 8., 9.));
    th_all.SetMomentum(ThreeVector(10., 11., 12.));
    th_all.SetVolumeName("Tofstrip4320");
    th_all.SetTime(13.);
    th_all.SetEnergy(14.);
    th_all.SetPID(15);
    th_all.SetMass(16.);
  }
  virtual ~TofHitTest  () {}

  virtual void SetUp   () {}
  virtual void TearDown() {}

  TofHit th;
  TofHit th_all;

};



// Test accessors by set and then get
TEST_F(TofHitTest, TrackIDAccessors) {
  TofHit th;
  th.SetTrackID   (-1);
  EXPECT_EQ(th.GetTrackID(), -1);
}

TEST_F(TofHitTest, StripNoAccessors) {
  th.SetStripNo   (-2);
  EXPECT_EQ(th.GetStripNo(), -2);
}

TEST_F(TofHitTest, PlaneNoAccessors) {
  th.SetPlaneNo   (-3);
  EXPECT_EQ(th.GetPlaneNo(),-3);
}

TEST_F(TofHitTest, StationNoAccessors) {
  th.SetStationNo (-4);
  EXPECT_EQ(th.GetStationNo(), -4);
}

TEST_F(TofHitTest, EdepAccessors) {
  th.SetEdep      (-5);
  EXPECT_EQ(th.GetEdep(), -5);
}

TEST_F(TofHitTest, PathLengthAccessors) {
  th.SetPathLength(-6.);
  EXPECT_EQ(th.GetPathLength(), -6.);
}

TEST_F(TofHitTest, PositionAccessors) {
  th.SetPosition  (ThreeVector(-7., -8., -9.));
  EXPECT_EQ(th.GetPosition().x(),-7.);
  EXPECT_EQ(th.GetPosition().y(),-8.);
  EXPECT_EQ(th.GetPosition().z(),-9.);
}

TEST_F(TofHitTest, MomentumAccessors) {
  th.SetMomentum  (ThreeVector(-10., -11., -12.));
  EXPECT_EQ(th.GetMomentum().x(),-10.);
  EXPECT_EQ(th.GetMomentum().y(),-11.);
  EXPECT_EQ(th.GetMomentum().z(),-12.);
}

TEST_F(TofHitTest, VolumeNameAccessors) {
  th.SetVolumeName("Badname");
  EXPECT_EQ( th.GetVolumeName(), "Badname");
}

TEST_F(TofHitTest, TimeAccessors) {
  th.SetTime      (-13.);
  EXPECT_EQ( th.GetTime(), -13. );
}

TEST_F(TofHitTest, EnergyAccessors) {
  th.SetEnergy    (-14.);
  EXPECT_EQ( th.GetEnergy(), -14. );
}

TEST_F(TofHitTest, PIDAccessors) {
  th.SetPID       (-15);
  EXPECT_EQ( th.GetPID(), -15 );
}

TEST_F(TofHitTest, MassAccessors) {
  th.SetMass      (-16.);
  EXPECT_EQ( th.GetMass(), -16. );
}

TEST_F(TofHitTest, AddEdep) {
  th.SetEdep(1.);
  th.AddEdep(2.);
  EXPECT_DOUBLE_EQ(th.GetEdep(), 3.);
}

TEST_F(TofHitTest, AddPathLength) {
  th.SetPathLength(1.);
  th.AddPathLength(2.);
  EXPECT_DOUBLE_EQ(th.GetPathLength(), 3.);
}

//Hope no one ever wants to make a 0 > strip number > 10...
TEST_F(TofHitTest, DecodeVolumeName) { //GAK!
  th.SetVolumeName("TOFstrip10123");
  th.DecodeVolumeName();
  EXPECT_EQ(th.GetStationNo(), 10);
  EXPECT_EQ(th.GetPlaneNo(), 1); 
  EXPECT_EQ(th.GetStripNo(), 2);
}

TEST_F(TofHitTest, CopyConstructor) {
  TofHit th_copy(th);
  TofHit th_all_copy(th_all);
  EXPECT_TRUE(TofHitEquality(th, th_copy, 1e-9));
  EXPECT_TRUE(TofHitEquality(th_all, th_all_copy, 1e-9));
}


TEST_F(TofHitTest, EqualsAssignmentOperator) {
  TofHit th_copy; //if I put all on one line, gcc implicitly calls copy constructor
  th_copy = th;
  TofHit th_all_copy;
  th_all_copy = th_all;
  EXPECT_TRUE(TofHitEquality(th, th_copy, 1e-9));
  EXPECT_TRUE(TofHitEquality(th_all, th_all_copy, 1e-9));
}

TEST_F(TofHitTest, Destructor) {
  TofHit* th_p        = new TofHit();
  TofHit* th_all_copy = new TofHit(th_all);
  delete th_all_copy; //check memory is deallocated
  delete th_p;
}


} //namespace

