#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
# 
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

# pylint: disable=C0103

"""
Test maus_cpp.mice_module
"""

import unittest
import os
import math

from maus_cpp.mice_module import MiceModule

class MiceModuleTestCase(unittest.TestCase): # pylint: disable=R0904
    """
    Test maus_cpp.mice_module
    """
    def setUp(self):
        """open test mice module"""
        self.test = os.path.expandvars(
        "${MAUS_ROOT_DIR}/tests/py_unit/test_maus_cpp/test_mice_modules_py.dat")
        self.mod = MiceModule(file_name=self.test)

    def test_init(self):
        """check initialise mice module"""
        try:
            mod = MiceModule() # pylint: disable=W0612
            self.assertTrue(False, "Should have thrown an error")
        except TypeError:
            pass
        try:
            mod = MiceModule(file_name="Test.dat", bob="")
            self.assertTrue(False, "Should have thrown an error")
        except TypeError:
            pass
        mod = MiceModule(file_name="Test.dat")

    def test_get_name(self):
        """Test MiceModule.get_name()"""
        self.assertEqual(self.mod.get_name(), "test_mice_modules_py.dat")
        for i, child in enumerate(self.mod.get_children()):
            self.assertEqual(child.get_name(), "TestMod"+str(i+1))

    def test_get_property(self):
        """Test mice_module get property"""
        self.mod.get_property(name="Str", type="String")
        try:
            self.mod.get_property("Str", "Not_String")
            self.assertTrue(False, "Should have thrown typeerror")
        except TypeError:
            pass

    def test_get_property_string(self):
        """Test mice_module get property string"""
        self.assertEqual(self.mod.get_property("Str", "StrinG"),
                         "Galactic")
        try:
            self.mod.get_property("str", "String")
            self.assertTrue(False, "Should have thrown keyerror")
        except KeyError:
            pass

    def test_get_property_double(self):
        """Test mice_module get property double"""
        self.assertAlmostEqual(self.mod.get_property("Dbl", "DoublE"),
                               100.)
        try:
            self.mod.get_property("dbl", "DoublE")
            self.assertTrue(False, "Should have thrown keyerror")
        except KeyError:
            pass

    def test_get_property_bool(self):
        """Test mice_module get property bool"""
        self.assertEqual(self.mod.get_property("BoolTrue", "BooL"),
                         True)
        self.assertEqual(self.mod.get_property("BoolFalse", "bOOl"),
                         False)
        try:
            self.mod.get_property("booltrue", "bOOl")
            self.assertTrue(False, "Should have thrown keyerror")
        except KeyError:
            pass

    def test_get_property_int(self):
        """Test mice_module get property int"""
        self.assertEqual(self.mod.get_property("Int", "InT"),
                         -1)
        try:
            self.mod.get_property("INT", "int")
            self.assertTrue(False, "Should have thrown keyerror")
        except KeyError:
            pass

    def test_get_property_hep3vector(self):
        """Test mice_module get property hep3vector"""
        h3v = self.mod.get_property("H3v", "HEP3VECTOR")
        self.assertAlmostEqual(h3v["x"], 1.)
        self.assertAlmostEqual(h3v["y"], 2.)
        self.assertAlmostEqual(h3v["z"], 3.)
        try:
            self.mod.get_property("h3v", "HEP3VECTOR")
            self.assertTrue(False, "Should have thrown keyerror")
        except KeyError:
            pass

    def test_set_property(self):
        """Test mice_module set property"""
        self.mod.set_property(name="SET", type="StrinG", value="XYZ")
        self.assertEqual(self.mod.get_property("SET", "string"), "XYZ")
        try:
            self.mod.get_property("Str", "Not_String")
            self.assertTrue(False, "Should have thrown typeerror")
        except TypeError:
            pass
        try:
            self.mod.set_property("SET", "string", 1.)
        except TypeError:
            pass
        self.mod.set_property("SET", "bool", 1)
        self.assertTrue(self.mod.get_property("SET", "bool"))
        self.mod.set_property("SET", "bool", 0)
        self.assertFalse(self.mod.get_property("SET", "bool"))
      
    def test_set_property_hep3vector(self):
        """Test mice_module set property for hep3vectors"""
        test_in = {"x":1., "y":2., "z":3.}
        self.mod.set_property(name="h3v_a", type="hep3vector", value=test_in)
        test_out = self.mod.get_property(name="h3v_a", type="hEP3vectOR") 
        for key in test_out.keys():
            self.assertAlmostEqual(test_in[key], test_out[key])
        try:
            self.mod.set_property(name="h3v_b", type="hEP3vectOR",
                                  value={"x":1., "y":1.})
            self.assertTrue(False)
        except KeyError:
            pass
        try:
            self.mod.set_property(name="h3v_a", type="hEP3vectOR",
                                  value={"x":"a", "y":1., "z":1.})
            self.assertTrue(False)
        except TypeError:
            pass

    def test_get_children(self):
        """Test MiceModule get_children"""
        children = self.mod.get_children()
        self.assertEqual(len(children), 3)
        ints = sorted([child.get_property('IV', 'int') for child in children])
        self.assertEqual(ints, [1, 2, 3])
        self.mod.get_children()[0].set_property('IV', 'int', 999)
        # this behaviour is expected - get_children() does a deep copy
        self.assertEqual \
                     (self.mod.get_children()[0].get_property('IV', 'int'), 1)

    def test_set_children(self):
        """Test MiceModule get_children"""
        test_children = [MiceModule(self.test) for i in range(2)]
        for i, child in enumerate(test_children):
            child.set_property('test', 'int', i)
        self.mod.set_children(test_children)
        for i, child in enumerate(self.mod.get_children()):
            self.assertEqual(child.get_property('test', 'int'), i)
        self.mod.set_children([]) # should be okay
        try:
            self.mod.set_children(10)
            self.assertTrue(False, 'Should have thrown typeerror')
        except TypeError:
            pass
        try:
            self.mod.set_children([10])
            self.assertTrue(False, 'Should have thrown typeerror')
        except TypeError:
            pass

    def test_print(self):
        """Test MiceModule print"""
        self.assertEqual(type(str(self.mod)), type(''))
        self.assertGreater(str(self.mod).find(self.mod.get_name()), -1)

    def test_reload_repeat_module2(self):
        """Check that we can reload RepeatModule2 directive"""
        self.test = os.path.expandvars(
        "${MAUS_ROOT_DIR}/tests/py_unit/test_maus_cpp/mice_mod_repeat.dat")
        for index in range(3): #pylint: disable=W0612
            self.mod = MiceModule(file_name=self.test)
            mod_list = self.mod.get_children()
            z_list = [mod.get_property("Position", "Hep3Vector")["z"] \
                                                            for mod in mod_list]
            for i, z in enumerate(sorted(z_list)):                
                self.assertAlmostEqual(i*1000., z)

    def _test_global_coordinates(self):
        """See comment in PyMiceModule.cc for why this doesnt work"""
        import maus_cpp.globals
        import maus_cpp.field
        import Configuration
        self.test = os.path.expandvars("${MAUS_ROOT_DIR}/tests/py_unit/"+\
                            "test_maus_cpp/test_mice_modules_global_coords.dat")
        mod = MiceModule(file_name=self.test)
        maus_cpp.globals.birth(Configuration.Configuration().getConfigJSON())
        maus_cpp.globals.set_monte_carlo_mice_modules(mod)
        print "NEW FIELDS"
        print maus_cpp.field.str(True)
        children = mod.get_children()
        child = children[0]
        for child in children:
            if child.get_name().find("TestMod4") > -1:
                break
        test_mod = child.get_children()[0]
        pos = test_mod.get_global_position()
        ref_pos = {"x":500., "y":1100., "z":1100.}
        for key in ref_pos:
            print pos, ref_pos
            self.assertAlmostEqual(pos[key], ref_pos[key])
        rot = test_mod.get_global_rotation()
        ref_rot = {"x":0., "y":1., "z":0.,
                   "angle":math.pi-math.radians(135.)*1e-9}
        for key in ref_rot:
            self.assertAlmostEqual(rot[key], ref_rot[key])


if __name__ == "__main__":
    unittest.main()

