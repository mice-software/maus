#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

#pylint: disable = C0103

"""
Tests for MapPyGroup
"""

import unittest
import json

import MapPyGroup

class TestMapPyGroup(unittest.TestCase): #pylint: disable=R0904
    """
    Tests for MapPyGroup
    """
    def setUp(self): #pylint: disable = C0103
        self.test_group = MapPyGroup.MapPyGroup()
        self.tests = [TestMap(), TestMap(), TestMap()]
        for i, identifier in enumerate(["x", "y", "z"]):
            self.test_group.append(self.tests[i])
            self.tests[i].key = identifier

    def test_init(self):
        """Test MapPyGroup init"""
        test_group_2 = MapPyGroup.MapPyGroup(self.tests)
        self.assertEqual(test_group_2._workers, self.tests) #pylint: disable = W0212, C0301

    def test_get_worker_names(self):
        """Test MapPyGroup get_worker_names"""
        for name in self.test_group.get_worker_names():
            self.assertEqual(name, "TestMap")

    def test_append_bad_class(self):
        """Test MapPyGroup append fails for incorrect maps"""
        self.assertRaises(TypeError, self.test_group.append, TestMapNoBirth())
        self.assertRaises(TypeError, self.test_group.append, TestMapNoProcess())
        self.assertRaises(TypeError, self.test_group.append, TestMapNoDeath())
        self.assertRaises \
                     (TypeError, self.test_group.append, TestMapBirthWrongSig())
        self.assertRaises(TypeError, self.test_group.append, TestMapNoProcess())
        self.assertRaises \
                   (TypeError, self.test_group.append, TestMapProcessWrongSig())
        self.assertRaises(TypeError, self.test_group.append, TestMapNoDeath())
        self.assertRaises \
                     (TypeError, self.test_group.append, TestMapDeathWrongSig())

    def test_birth_okay(self):
        """Test MapPyGroup birth"""
        self.assertTrue(self.test_group.birth(""))
        for test in self.tests:
            self.assertTrue(test.has_birthed)

    def test_birth_fail(self):
        """Test MapPyGroup birth"""
        self.tests[1].birth_return_value = False
        self.assertFalse(self.test_group.birth(""))
        for test in self.tests:
            self.assertTrue(test.has_deathed)
        self.assertTrue(self.tests[0].has_birthed)
        self.assertTrue(self.tests[1].has_birthed)
        self.assertFalse(self.tests[2].has_birthed)

    def test_process_all_ran(self):
        """Test MapPyGroup process all maps run"""
        out = self.test_group.process("{}")
        test_output = json.loads(out)
        for item in ["x", "y", "z"]: # check they all ran
            self.assertTrue(test_output[item], item)

    def test_process_run_order(self):
        """Test MapPyGroup process all maps run in correct order"""
        for i, test in enumerate(self.tests):
            test.key = "w"
            test.value = i
        out = self.test_group.process("{}")
        test_output = json.loads(out)
        self.assertEqual(test_output["w"], 2)

    def test_death_okay(self):
        """Test MapPyGroup death"""
        self.assertTrue(self.test_group.death())
        for test in self.tests:
            self.assertTrue(test.has_deathed)
        
    def test_death_fail(self):
        """Test MapPyGroup death"""
        self.tests[1].death_return_value = False
        self.assertFalse(self.test_group.death())
        self.assertTrue(self.tests[0].has_deathed)
        self.assertTrue(self.tests[2].has_deathed)
        del(self.test_group)

    def test_del(self):
        """TestMapPyGroup __del__"""
        del(self.test_group)
        for test in self.tests:
            self.assertTrue(test.has_deathed)


class TestMap:
    """
    test class

    Dummy birth, process, death, methods. Determine at runtime whether they are
    successful or not. Also ability to overwrite function calls at runtime.
    """
    def __init__(self): #pylint: disable=W0613, R0201, C0111
        self.key = "a"
        self.value = True
        self.has_birthed = False
        self.has_deathed = False
        self.birth_return_value = True
        self.death_return_value = True

    def birth(self, arg): #pylint: disable=W0613, R0201, C0111, E0202
        self.has_birthed = True
        return self.birth_return_value

    def process(self, arg): #pylint: disable=W0613, R0201, C0111, E0202
        arg_json = json.loads(arg)
        arg_json[self.key] = self.value
        return json.dumps(arg_json)

    def death(self): #pylint: disable=W0613, R0201, C0111, E0202
        self.has_deathed = self.death_return_value
        return self.death_return_value

class TestMapNoBirth:
    """
    TestMap with missing birth()
    """
    def __init__(self): #pylint: disable=W0613, R0201, C0111
        pass

#    def birth(self, arg): #pylint: disable=W0613, R0201, C0111
#        pass

    def process(self, arg): #pylint: disable=W0613, R0201, C0111
        pass

    def death(self): #pylint: disable=W0613, R0201, C0111
        pass

class TestMapBirthWrongSig:
    """
    TestMap with birth() has wrong call signature
    """
    def __init__(self): #pylint: disable=W0613, R0201, C0111
        pass

    def birth(self): #pylint: disable=W0613, R0201, C0111
        pass

    def process(self, arg): #pylint: disable=W0613, R0201, C0111
        pass

    def death(self): #pylint: disable=W0613, R0201, C0111
        pass


class TestMapNoProcess:
    """
    TestMap with missing process()
    """
    def __init__(self): #pylint: disable=W0613, R0201, C0111
        pass

    def birth(self, arg): #pylint: disable=W0613, R0201, C0111
        pass

#    def process(self, arg): #pylint: disable=W0613, R0201, C0111
#        pass

    def death(self): #pylint: disable=W0613, R0201, C0111
        pass

class TestMapProcessWrongSig:
    """
    TestMap with process() has wrong call signature
    """
    def __init__(self): #pylint: disable=W0613, R0201, C0111
        pass

    def birth(self, arg): #pylint: disable=W0613, R0201, C0111
        pass

    def process(self): #pylint: disable=W0613, R0201, C0111
        pass

    def death(self): #pylint: disable=W0613, R0201, C0111
        pass


class TestMapNoDeath:
    """
    TestMap with missing death()
    """
    def __init__(self): #pylint: disable=W0613, R0201, C0111
        pass

    def birth(self, arg): #pylint: disable=W0613, R0201, C0111
        pass

    def process(self, arg): #pylint: disable=W0613, R0201, C0111
        pass

#    def death(self): #pylint: disable=W0613, R0201, C0111
#        pass

class TestMapDeathWrongSig:
    """
    TestMap with death() has wrong call signature
    """
    def __init__(self): #pylint: disable=W0613, R0201, C0111
        pass

    def birth(self, arg): #pylint: disable=W0613, R0201, C0111
        pass

    def process(self, arg): #pylint: disable=W0613, R0201, C0111
        pass

    def death(self, arg): #pylint: disable=W0613, R0201, C0111
        pass

if __name__ == "__main__":
    unittest.main()


