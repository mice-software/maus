#!/usr/bin/env python

# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
#

"""
  The analysis module contains useful functions and classes for those wishing 
  to perform some analysis of MAUS data (MC and Recon) writting in python.

  Useful fitting and plotting tools are included, including some shortcuts for
  the detector reconstruction data and some file I/O related methods.

  Please append and extend this as appropriate.
"""

import analysis.tools
import analysis.covariances
import analysis.hit_types
import analysis.inspectors
import analysis.scifi_lookup

