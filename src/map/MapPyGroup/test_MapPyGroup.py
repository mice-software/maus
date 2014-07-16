"""
Tests for MapPyGroup module.
"""
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

# pylint: disable=C0103

import json
from types import ListType
import unittest

import maus_cpp.globals 
import maus_cpp.converter
import Configuration
import ErrorHandler
from MapPyDoNothing import MapPyDoNothing
from MapPyGroup import MapPyGroup
from MapPyGroup import MapPyGroupBirthException
from MapPyGroup import MapPyGroupDeathException
from MapPyPrint import MapPyPrint
from MapPyTestMap import MapPyTestMap



class MapPyGroupTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for MapPyGroup.
    """

    def setUp(self):
        """ 
        Reset the ErrorHandler to "none".
        @param self Object reference.
        """
        ErrorHandler.DefaultHandler().on_error='none'
        if not maus_cpp.globals.has_instance():
            maus_cpp.globals.birth(
              Configuration.Configuration().getConfigJSON()
            )

    def test_init_default(self):
        """
        Test with default constructor where group contains 0 workers.
        @param self Object reference.
        """
        group = MapPyGroup()
        names = group.get_worker_names()
        self.assertEquals(0, len(names), "Unexpected number of workers")
        group.birth("{}")
        group.process("{}")
        group.death()

    def test_init_list(self):
        """
        Test with constructor given an initial list of workers.
        @param self Object reference.
        """
        workers = [MapPyTestMap(), MapPyDoNothing(), MapPyPrint()]
        group = MapPyGroup(workers)
        names = group.get_worker_names()
        self.assertEquals(len(workers), len(names), 
            "Unexpected number of workers")
        for i in range(len(workers)):
            self.assertEquals(workers[i].__class__.__name__,
                              names[i],
                              "Unexpected name")

    def test_init_non_list(self):
        """
        Test constructor when it's given a non-list.
        @param self Object reference.
        """
        with self.assertRaisesRegexp(TypeError,
            ".*"):
            MapPyGroup("bad_argument")

    def test_get_worker_names(self):
        """
        Test get_worker_names.
        @param self Object reference.
        """
        # Create nested group.
        workers = [MapPyTestMap(), MapPyDoNothing(), MapPyPrint()]
        group = MapPyGroup(workers)
        group = MapPyGroup([MapPyDoNothing(), group, MapPyTestMap()])
        workers = ["MapPyDoNothing", 
            ["MapPyTestMap", "MapPyDoNothing", "MapPyPrint"], 
            "MapPyTestMap"]
        names = group.get_worker_names()
        self.assertEquals(len(workers), len(names), 
            "Unexpected number of names")
        # This does a start-to-end comparison of the lists.
        while len(workers) != 0:
            expected = workers.pop(0)
            actual = names.pop(0)
            if isinstance(actual, ListType):
                self.assertEquals(len(expected), len(actual),
                    "Unexpected number of names in sub-list")
                expected.extend(workers) 
                workers = expected
                names = actual.extend(names)
                names = actual
            else:
                self.assertEquals(expected, actual, "Unexpected name")

    def execute_bad_append(self, worker):
        """
        Test appending the given worker to the group and check than
        a TypeError is raised.
        @param self Object reference.
        @param worker Worker.
        """
        with self.assertRaisesRegexp(TypeError, ".*"):
            MapPyGroup().append(worker)

    def test_append_worker_no_birth(self):
        """
        Test appending a worker that has no birth function.
        @param self Object reference.
        """
        class TestWorker: # pylint:disable = C0111, W0232, R0903
            def __init__(self):
                pass
        self.execute_bad_append(TestWorker())

    def test_append_worker_bad_birth(self):
        """
        Test appending a worker that has a birth function with a
        bad signature.
        @param self Object reference.
        """
        class TestWorker: # pylint:disable = C0111, W0232, R0903
            def birth(self):
                pass
        self.execute_bad_append(TestWorker())

    def test_append_worker_no_process(self):
        """
        Test appending a worker that has no process function.
        @param self Object reference.
        """
        class TestWorker: # pylint:disable = C0111, W0232, R0903
            def birth(self, json_document):
                pass
        self.execute_bad_append(TestWorker())

    def test_append_worker_bad_process(self):
        """
        Test appending a worker that has a process function with a
        bad signature.
        @param self Object reference.
        """
        class TestWorker: # pylint:disable = C0111, W0232
            def birth(self, json_document):
                pass
            def process(self):
                pass
        self.execute_bad_append(TestWorker())
        
    def test_append_worker_no_death(self):
        """
        Test appending a worker that has no death function.
        @param self Object reference.
        """
        class TestWorker: # pylint:disable = C0111, W0232
            def birth(self, json_document):
                pass
            def process(self, spill):
                pass
        self.execute_bad_append(TestWorker())
        
    def test_append_worker_bad_death(self):
        """
        Test appending a worker that has a death function with a
        bad signature.
        @param self Object reference.
        """
        class TestWorker: # pylint:disable = C0111, W0232
            def birth(self, json_document):
                pass
            def process(self, spill):
                pass
        self.execute_bad_append(TestWorker())
 
    def test_birth(self):
        """
        Test calling birth and check that all workers have their
        birth function called and configuration set.
        @param self Object reference.
        """
        workers = \
            [MapPyTestMap("A"), MapPyTestMap("B"), MapPyTestMap("C")]
        group = MapPyGroup(workers)
        result = group.birth("""{"worker_key":"birth"}""")
        self.assertTrue(result, "birth unexpectedly returned False")
        for worker in workers:
            self.assertTrue(worker.birth_called, 
                "birth wasn't called for worker %s" % worker.map_id)
            self.assertEquals("birth", 
              worker.config_value,
              "Worker %s has unexpected configuration value %s" \
              % (worker.map_id, worker.config_value))

    def test_birth_false(self):
        """
        Test calling birth where one worker's birth function fails.
        @param self Object reference.
        """
        workers = [MapPyDoNothing(), MapPyDoNothing(), MapPyTestMap()]
        group = MapPyGroup(workers)
        # Force a worker in group to fail
        result = group.birth("""{"birth_result":%s}""" % \
            MapPyTestMap.FAIL)
        self.assertFalse(result, "birth unexpectedly returned True")

    def test_birth_exception(self):
        """
        Test calling birth where one worker's birth function 
        throws an exception.
        @param self Object reference.
        """
        workers = [MapPyDoNothing(), MapPyDoNothing(), MapPyTestMap()]
        group = MapPyGroup(workers)
        # Force a worker in group to fail
        result = group.birth("""{"birth_result":%s}""" % \
            MapPyTestMap.EXCEPTION)
        self.assertFalse(result, "birth unexpectedly returned True")

    def test_birth_exception_raise(self):
        """
        Test calling death where one worker's birth function 
        throws an exception and the ErrorHandler is set to
        "raise"
        @param self Object reference.
        """
        ErrorHandler.DefaultHandler().on_error='raise'
        workers = [MapPyDoNothing(), MapPyDoNothing(), MapPyTestMap()]
        group = MapPyGroup(workers)
        # Force a worker in group to fail
        with self.assertRaisesRegexp(MapPyGroupBirthException, ".*"):
            group.birth("""{"birth_result":%s}""" % \
                MapPyTestMap.EXCEPTION)

    def test_birth_death_exception(self):
        """
        Test calling birth where one worker's birth function 
        throws an exception and other worker's death functions
        also throw exceptions.
        @param self Object reference.
        """
        ErrorHandler.DefaultHandler().on_error='raise'
        # Create worker that fails on death.
        class TestWorker: # pylint:disable = C0111, W0232
            def birth(self, json_document):
                pass
            def process(self, spill):
                pass
            def death(self): # pylint:disable = R0201
                raise ValueError("Test")
        workers = [MapPyDoNothing(), TestWorker(), MapPyTestMap()]
        group = MapPyGroup(workers)
        # Force 3rd worker to fail on birth.
        with self.assertRaisesRegexp(MapPyGroupBirthException, ".*"):
            group.birth("""{"birth_result":%s}""" % \
                MapPyTestMap.EXCEPTION)

    def test_process(self):
        """
        Test calling process and check that all workers have their
        process function called and process the spill.
        @param self Object reference.
        """
        workers = \
            [MapPyTestMap("A"), MapPyTestMap("B"), MapPyTestMap("C")]
        group = MapPyGroup(workers)
        spill_doc = group.process("{}")
        spill = json.loads(spill_doc)
        for worker in workers:
            self.assertTrue(worker.process_called, 
                "process wasn't called for worker %s" % worker.map_id)
            self.assertTrue(spill.has_key("processed"),
                "Spill does not contain processed key")
            self.assertTrue(worker.map_id in spill["processed"],
                "Spill does not contain map_id for worker %s" \
                % worker.map_id)

    def test_death(self):
        """
        Test calling death and check all workers have their death
        functions called.
        @param self Object reference.
        """
        workers = \
            [MapPyTestMap("A"), MapPyTestMap("B"), MapPyTestMap("C")]
        group = MapPyGroup(workers)
        result = group.death()
        self.assertTrue(result, "death unexpectedly returned False")
        for worker in workers:
            self.assertTrue(worker.death_called, 
                "death wasn't called for worker %s" % worker.map_id)

    def test_death_false(self):
        """
        Test calling death where one worker's death function fails.
        @param self Object reference.
        """
        workers = [MapPyDoNothing(), MapPyDoNothing(), MapPyTestMap()]
        group = MapPyGroup(workers)
        # Force a worker in group to fail
        workers[2].birth("""{"death_result":%s}""" % MapPyTestMap.FAIL)
        result = group.death()
        self.assertFalse(result, "death unexpectedly returned True")

    def test_death_exception(self):
        """
        Test calling death where one worker's death function 
        throws an exception.
        @param self Object reference.
        """
        workers = [MapPyDoNothing(), MapPyDoNothing(), MapPyTestMap()]
        group = MapPyGroup(workers)
        # Force a worker in group to fail
        workers[2].birth("""{"death_result":%s}""" % MapPyTestMap.EXCEPTION)
        result = group.death()
        self.assertFalse(result, "death unexpectedly returned True")

    def test_death_exception_raise(self):
        """
        Test calling death where one worker's death function 
        throws an exception and the ErrorHandler is set to
        "raise"
        @param self Object reference.
        """
        ErrorHandler.DefaultHandler().on_error='raise'
        workers = [MapPyDoNothing(), MapPyDoNothing(), MapPyTestMap()]
        group = MapPyGroup(workers)
        # Force a worker in group to fail
        workers[2].birth("""{"death_result":%s}""" % MapPyTestMap.EXCEPTION)
        with self.assertRaisesRegexp(MapPyGroupDeathException, ".*"):
            group.death()

    def test_del(self):
        """
        Test calling __del__ and check all workers have their death
        functions called.
        @param self Object reference.
        """
        workers = \
            [MapPyTestMap("A"), MapPyTestMap("B"), MapPyTestMap("C")]
        MapPyGroup(workers)
        for worker in workers:
            self.assertTrue(worker.death_called, 
                "death wasn't called for worker %s" % worker.map_id)

    def test_conversion(self):      
        """Test MapPyGroup handles conversions appropriately"""
        class MapPyGroupConvert: # pylint:disable = C0111, W0232, R0201 
            def __init__(self):
                pass
            def birth(self, _json):
                pass
            def death(self):
                pass
            def process(self, data):
                data = maus_cpp.converter. json_repr(data)
                if type(data) != type({}):
                    raise ValueError("Expected data with type dict")
                return data
            can_convert = True

        class MapPyGroupNoConvert: # pylint:disable = C0111, W0232, R0201 
            def __init__(self):
                pass
            def birth(self, _json):
                pass
            def death(self):
                pass
            def process(self, data):
                if type(data) != type(""):
                    raise ValueError("Expected data with type string")
                return data

        workers = [MapPyGroupNoConvert(), MapPyGroupConvert(), 
                   MapPyGroupNoConvert(), MapPyGroupConvert()]
        my_group = MapPyGroup(workers)
        output = my_group.process("{}")
        self.assertEqual(output, {})



if __name__ == '__main__':
    unittest.main()
