"""
Datastructure items should all the MAUS_VERSIONED_CLASS_DEF macro
"""

import unittest
import subprocess
import os

def is_header(a_path):
    """
    Return true if it is a header file
    """
    return a_path[-3:] == ".hh" and a_path[-6:] != "inl.hh"

EXCLUSIONS = [
"src/common_cpp/DataStructure/LinkDef.hh",
"src/common_cpp/DataStructure/Global/ReconEnums.hh",
]

def is_ignored(a_path):
    """
    Return true if file is in the exclusion list
    """
    strip_path = a_path[len(os.environ["MAUS_ROOT_DIR"])+1:]
    return strip_path in EXCLUSIONS

class TestDataStructure(unittest.TestCase): # pylint: disable=R0904
    """
    Check data structure vs ROOT integrity
    """
    def test_data_structure(self):
        """
        @brief walk the directory structure from target_dir
        """
        target_dir = "${MAUS_ROOT_DIR}/src/common_cpp/DataStructure"
        target_dir = os.path.expandvars(target_dir)
        for this_dir, subdirs, files in os.walk(target_dir):
            print 'In', this_dir, 'with', len(subdirs), 'subdirectories'
            for file_name in files:
                path = os.path.join(this_dir, file_name)
                if is_header(path) and not is_ignored(path):
                    print '   ', path
                    out = subprocess.check_output(['grep',
                                                   'MAUS_VERSIONED_CLASS_DEF',
                                                    path])
                    self.assertNotEqual(out, '')

if __name__ == "__main__":
    unittest.main()

