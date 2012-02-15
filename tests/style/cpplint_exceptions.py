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

import os

exceptions = { #pylint: disable=C0103
}

CPP_CM = os.path.join('src','common_cpp')
LEGACY = os.path.join('src','LEGACY')
TST = os.path.join('tests', 'cpp_unit')

exceptions[os.path.join(CPP_CM, 'Simulation','MAUSRunAction.hh')] = [
('#include "globals.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4UserRunAction.hh"', 'cpplint confused with g4 header', 'rogers'),
]

exceptions[os.path.join(CPP_CM, 'Simulation','MAUSRunAction.cc')] = [
('#include "G4Run.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4VVisManager.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4UImanager.hh"', 'cpplint confused with g4 header', 'rogers'),
]

exceptions[os.path.join(CPP_CM, 'Simulation','MAUSPhysicsList.hh')] = [
('#include "G4VModularPhysicsList.hh"', 'cpplint confused with g4 header',
                                                                      'rogers'),
('#include "G4UImanager.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4UserSpecialCuts.hh"', 'cpplint confused with g4 header',
                                                                      'rogers'),
('#include "G4StepLimiter.hh"', 'cpplint confused with g4 header', 'rogers'),
]

exceptions[os.path.join(CPP_CM, 'Simulation','MAUSPhysicsList.cc')] = [
('#include "G4UImanager.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4ProcessTable.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4ProcessVector.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4PhysListFactory.hh"', 'cpplint confused with g4 header',
                                                                      'rogers'),
('#include "globals.hh"', 'cpplint confused with g4 header', 'rogers'),
]

exceptions[os.path.join(CPP_CM, 'Simulation','VirtualPlanes.cc')] = [
('#include "G4Track.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4Step.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4StepPoint.hh"', 'cpplint confused with g4 header', 'rogers')
]

exceptions[os.path.join(TST, 'Simulation', 'VirtualPlaneTest.cc')] = [
('#include "G4ParticleTable.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4Step.hh"', 'cpplint confused with g4 header', 'rogers'),
('#include "G4StepPoint.hh"', 'cpplint confused with g4 header', 'rogers')
]

exceptions[os.path.join(CPP_CM, 'Simulation',
                                           'MAUSPrimaryGeneratorAction.hh')] = [
('#include "G4ParticleGun.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4ParticleTable.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4VUserPrimaryGeneratorAction.hh"  // inherit from',
                                 'cpplint confused with maus header', 'rogers'),
('    long int seed;', 'CLHEP also uses long int for its random seed - if we want to be compatible we should follow their lead', 'rogers'), #pylint: disable=C0301
('#include "G4Track.hh"  //  arg to tracking action',
'cpplint confused by g4 header', 'rogers'),
('#include "G4UserTrackingAction.hh"  //  inherit from',
'cpplint confused by g4 header', 'rogers')
]

exceptions[os.path.join(CPP_CM, 'Simulation',
                                           'MAUSPrimaryGeneratorAction.cc')] = [
('#include "G4Event.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4PrimaryVertex.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4Track.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4ios.hh"', 'cpplint confused by g4 header', 'rogers')
]

exceptions[os.path.join(CPP_CM, 'Simulation','MAUSSteppingAction.hh')] = [
('#include "G4Step.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4StepPoint.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4Track.hh"', 'cpplint confused by g4 header', 'rogers'),
('#include "G4UserSteppingAction.hh"', 
                                     'cpplint confused by g4 header', 'rogers'),
]

exceptions[os.path.join(CPP_CM, 'Simulation','MAUSTrackingAction.hh')] = [
('#include "G4Track.hh"  //  arg to tracking action', 
                                     'cpplint confused by g4 header', 'rogers'),
('#include "G4UserTrackingAction.hh"  //  inherit from',
                                      'cpplint confused by g4 header', 'rogers')
]

exceptions[os.path.join(CPP_CM, 'Simulation','MAUSVisManager.hh')] = [
('#include "G4VisManager.hh"', 'cpplint confused by g4 header', 'rogers'),
]

exceptions[os.path.join(CPP_CM, 'Simulation','MAUSEventAction.hh')] = [
('#include "G4UserEventAction.hh"', 'cpplint confused by g4 header', 'rogers'),
]

exceptions[os.path.join(CPP_CM, 'Maths','Complex.hh')] = [
('MAUS::complex& operator+=(MAUS::complex& lhs, const double         rhs);',
 'cpplint confused by MAUS::complex not a class but has math operators',
 'lane'),
('MAUS::complex& operator-=(MAUS::complex& lhs, const double         rhs);',
 'cpplint confused by MAUS::complex not a class but has math operators',
 'lane'),
('MAUS::complex& operator*=(MAUS::complex& lhs, const double         rhs);',
 'cpplint confused by MAUS::complex not a class but has math operators',
 'lane'),
('MAUS::complex& operator/=(MAUS::complex& lhs, const double         rhs);',
 'cpplint confused by MAUS::complex not a class but has math operators',
 'lane'),
]

exceptions[os.path.join(CPP_CM, 'Maths','HermitianMatrix.hh')] = [
('  HermitianMatrix(const Matrix<complex>& original_instance);',
 'cpplint confused by STL copy constructor', 'lane')
]

exceptions[os.path.join(CPP_CM, 'Maths','Matrix.hh')] = [
('  Matrix(const Matrix<double>& original_instance)',
 'cpplint confused by STL copy constructor', 'lane'),
('  Matrix(const Matrix<complex>& original_instance)',
 'cpplint confused by STL copy constructor', 'lane')
]

exceptions[os.path.join(CPP_CM, 'Maths','Vector.hh')] = [
('  Vector(const Vector<double>& original_instance)',
 'cpplint confused by STL copy constructor', 'lane'),
('  Vector(const Vector<complex>& original_instance)',
 'cpplint confused by STL copy constructor', 'lane')
]

exceptions[os.path.join(TST, 'Maths', 'PolynomialVectorTest.cc')] = [
('  } catch (Squeal squee) {}',
 '"catch" is a keyword like "for" and "if", not a function', 'lane')
]

exceptions[os.path.join(TST, 'Maths', 'VectorTest.cc')] = [
('  } catch (Squeal squeal) {}',
 '"catch" is a keyword like "for" and "if", not a function', 'lane')
]

exceptions[os.path.join(TST, 'Optics', 'PhaseSpaceVectorTest.cc')] = [
('  } catch (Squeal squeal) {}',
 '"catch" is a keyword like "for" and "if", not a function', 'lane')
]

exceptions[os.path.join(TST, 'Optics', 'CovarianceMatrixTest.cc')] = [
('  } catch (Squeal squeal) {}',
 '"catch" is a keyword like "for" and "if", not a function', 'lane')
]
