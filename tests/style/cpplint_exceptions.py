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
import glob

exceptions = {
}

cm = os.path.join('src','common')
tst = os.path.join('tests', 'cpp_unit')

exceptions[os.path.join(cm, 'Simulation','VirtualPlanes.cc')] = [
('#include "G4Track.hh"', 'cpplint confused with maus header', 'rogers'),
('#include "G4Step.hh"', 'cpplint confused with maus header', 'rogers'),
('#include "G4StepPoint.hh"', 'cpplint confused with maus header', 'rogers')
]

exceptions[os.path.join(tst, 'Simulation', 'VirtualPlaneTest.cc')] = [
('#include "G4ParticleTable.hh"', 'cpplint confused with maus header', 'rogers'),
('#include "G4Step.hh"', 'cpplint confused with maus header', 'rogers'),
('#include "G4StepPoint.hh"', 'cpplint confused with maus header', 'rogers')
]



#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ParticleTable.hh"

