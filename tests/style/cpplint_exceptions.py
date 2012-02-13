"""
test_cpp_style can allow exceptions in certain circumstances:

* exceptions dict is a dict like 
  exceptions = { 
    file_name:[
      ("text of line where exception occurs", "reason for exception", "name")
    ]
  }
  I enumerate exceptions using the line where the exception occurs so that if
  someone moves code around the exception is still valid. If the actual line 
  where the exception occurs changes, we have to re-enter the exception.

  Please use these exceptions in moderation - i.e. try to work within the style
  guide wherever possible. In general, we make exceptions only when there is a
  good technical reason (typically a non-compliant interface with an external
  library).

  Please comment up why you think the exception is valid. If there is no  good
  reason for the exception, it may get deleted...
"""

# pylint: disable=C0301

import os

exceptions = {
}

cpp_cm = os.path.join('src','common_cpp')
legacy = os.path.join('src','legacy')
tst = os.path.join('tests', 'cpp_unit')

exceptions[os.path.join(cpp_cm, 'Simulation','MAUSRunAction.hh')] = [
('#include "globals.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4UserRunAction.hh"', 'cpplint confused with g4 header', 'rogers'),
]

exceptions[os.path.join(cpp_cm, 'Simulation','MAUSRunAction.cc')] = [
('#include "G4Run.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4VVisManager.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4UImanager.hh"', 'cpplint confused with g4 header', 'rogers'),
]

exceptions[os.path.join(cpp_cm, 'Simulation','MAUSPhysicsList.hh')] = [
('#include "G4VModularPhysicsList.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4UImanager.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4UserSpecialCuts.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4StepLimiter.hh"', 'cpplint confused with g4 header', 'rogers'),
]

exceptions[os.path.join(cpp_cm, 'Simulation','MAUSPhysicsList.cc')] = [
('#include "G4UImanager.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4ProcessTable.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4ProcessVector.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4PhysListFactory.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "globals.hh"', 'cpplint confused with g4 header', 'rogers'),
]

exceptions[os.path.join(cpp_cm, 'Simulation','VirtualPlanes.cc')] = [
('#include "G4Track.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4Step.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4StepPoint.hh"', 'cpplint confused with g4 header', 'rogers')
]

exceptions[os.path.join(tst, 'Simulation', 'VirtualPlaneTest.cc')] = [
('#include "G4ParticleTable.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4Step.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4StepPoint.hh"', 'cpplint confused with g4 header', 'rogers')
]

exceptions[os.path.join(cpp_cm, 'Simulation','MAUSPrimaryGeneratorAction.hh')] = [
('#include "G4ParticleGun.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4ParticleTable.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4VUserPrimaryGeneratorAction.hh"  // inherit from', 'cpplint confused with maus header', 'rogers'),
('    long int seed;', 'CLHEP also uses long int for its random seed - if we want to be compatible we should follow their lead', 'rogers'),
('#include "G4Track.hh"  //  arg to tracking action', 'cpplint confused by g4 header', 'rogers'),
('#include "G4UserTrackingAction.hh"  //  inherit from', 'cpplint confused by g4 header', 'rogers')
]

exceptions[os.path.join(cpp_cm, 'Simulation','MAUSPrimaryGeneratorAction.cc')] = [
('#include "G4Event.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4PrimaryVertex.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4Track.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4ios.hh"', 'cpplint confused by g4 header', 'rogers')
]

exceptions[os.path.join(cpp_cm, 'Simulation','MAUSSteppingAction.hh')] = [
('#include "G4Step.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4StepPoint.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4Track.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4UserSteppingAction.hh"', 'cpplint confused by g4 header', 'rogers'),
]

exceptions[os.path.join(cpp_cm, 'Simulation','MAUSTrackingAction.hh')] = [
('#include "G4Track.hh"  //  arg to tracking action', 'cpplint confused by g4 header', 'rogers'),
('#include "G4UserTrackingAction.hh"  //  inherit from', 'cpplint confused by g4 header', 'rogers')
]

exceptions[os.path.join(cpp_cm, 'Simulation','MAUSVisManager.hh')] = [
('#include "G4VisManager.hh"', 'cpplint confused by g4 header', 'rogers'),
]

exceptions[os.path.join(cpp_cm, 'Simulation','MAUSEventAction.hh')] = [
('#include "G4UserEventAction.hh"', 'cpplint confused by g4 header', 'rogers'),
]

exceptions[os.path.join(cpp_cm, 'Maths','Complex.hh')] = [
('MAUS::complex& operator+=(MAUS::complex& lhs, const double         rhs);',
 'I guess cpplint is confused by the fact that MAUS::complex is not a class but has math operators', 'lane'),
('MAUS::complex& operator-=(MAUS::complex& lhs, const double         rhs);',
 'I guess cpplint is confused by the fact that MAUS::complex is not a class but has math operators', 'lane'),
('MAUS::complex& operator*=(MAUS::complex& lhs, const double         rhs);',
 'I guess cpplint is confused by the fact that MAUS::complex is not a class but has math operators', 'lane'),
('MAUS::complex& operator/=(MAUS::complex& lhs, const double         rhs);',
 'I guess cpplint is confused by the fact that MAUS::complex is not a class but has math operators', 'lane')
]

exceptions[os.path.join(cpp_cm, 'Maths','HermitianMatrix.hh')] = [
('  HermitianMatrix(const Matrix<complex>& original_instance);',
 'cpplint confused by STL copy constructor', 'lane')
]

exceptions[os.path.join(cpp_cm, 'Maths','Matrix.hh')] = [
('  Matrix(const Matrix<double>& original_instance)',
 'cpplint confused by STL copy constructor', 'lane'),
('  Matrix(const Matrix<complex>& original_instance)',
 'cpplint confused by STL copy constructor', 'lane')
]

exceptions[os.path.join(cpp_cm, 'Maths','Vector.hh')] = [
('  Vector(const Vector<double>& original_instance)',
 'cpplint confused by STL copy constructor', 'lane'),
('  Vector(const Vector<complex>& original_instance)',
 'cpplint confused by STL copy constructor', 'lane')
]

exceptions[os.path.join(tst, 'Maths', 'PolynomialVectorTest.cc')] = [
('  } catch (Squeal squee) {}', '"catch" is a keyword like "for" and "if", not a function', 'lane')
]

exceptions[os.path.join(tst, 'Maths', 'VectorTest.cc')] = [
('  } catch (Squeal squeal) {}', '"catch" is a keyword like "for" and "if", not a function', 'lane')
]

exceptions[os.path.join(tst, 'Optics', 'PhaseSpaceVectorTest.cc')] = [
('  } catch (Squeal squeal) {}', '"catch" is a keyword like "for" and "if", not a function', 'lane')
]

exceptions[os.path.join(tst, 'Optics', 'CovarianceMatrixTest.cc')] = [
('  } catch (Squeal squeal) {}', '"catch" is a keyword like "for" and "if", not a function', 'lane')
]
