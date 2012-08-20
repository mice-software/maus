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
Check MAUS_ROOT_DIR path is okay for building
"""

import os
import sys
import unittest

def has_whitespace():
    """
    Check for ' ' in MAUS_ROOT_DIR path (no tab checking)
    """
    mrd = os.environ["MAUS_ROOT_DIR"]
    if mrd.find(' ') > -1:
        print """
FATAL: Found white space in $MAUS_ROOT_DIR. Your $MAUS_ROOT_DIR was
FATAL:            """+mrd
        return True
    return False

def has_symbolic_links():
    """
    Check for symbolic links in MAUS_ROOT_DIR path
    """
    mrd = os.path.normpath(os.environ["MAUS_ROOT_DIR"])
    if os.path.realpath(mrd) != mrd:
        print """
FATAL: Found a symbolic link in $MAUS_ROOT_DIR. Your $MAUS_ROOT_DIR was
FATAL:            """+mrd+"""
FATAL: and your absolute path is
FATAL:            """+os.path.abspath(mrd)
        return True
    return False

class TestCheckPath(): #unittest.TestCase): # pylint: disable=R0904
    """
    Tests
    """

    def test_has_symbolic_links(self):
        """
        Tests for has_symbolic_links()
        """
        symlink = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp", "symlink")
        if os.path.exists(symlink):
            os.remove(symlink)
        os.symlink(os.environ["MAUS_ROOT_DIR"], symlink)
        os.environ["MAUS_ROOT_DIR"] = symlink
        assert(has_symbolic_links())
        os.environ["MAUS_ROOT_DIR"] = "/usr/"
        assert(not has_symbolic_links())
        os.environ["MAUS_ROOT_DIR"] = "/usr"
        assert(not has_symbolic_links())

    def test_has_white_space(self):
        """
        Tests for has_white_space()
        """
        os.environ["MAUS_ROOT_DIR"] = " bobjim"
        assert(has_whitespace())
        os.environ["MAUS_ROOT_DIR"] = "bobjim"
        assert( not has_whitespace())   

def main():
    """
    Check MAUS_ROOT_DIR environment variable for whitespace or symbolic links
    """
    if has_symbolic_links() or has_whitespace():
        sys.exit(1)

if __name__ == "__main__":
    main()
else:
    # unittest for some reason doesn't work
    TestCheckPath().test_has_symbolic_links()
    TestCheckPath().test_has_white_space()


