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

from MapPyGroup import MapPyGroup

class MapDummy: # pylint:disable = R0902
    """
    Simple map class for testing purposes.
    """

    def __init__(self, map_id = ""):
        """
        Constructor.
        @param self Object reference.
        @param map_id ID of this class.
        """
        self.map_id = map_id
        # Configuration value.
        self.config_value = ""
        # Flags to indicate if these functions were called.
        self.birth_called = False
        self.process_called = False
        self.death_called = False
        # Flags holding result of these function calls.
        self.birth_result = True
        self.process_result = True
        self.death_result = True

    def birth(self, json_doc):
        """
        Birth the mapper. If a worker_key exists in the
        JSON document then its value is saved in 
        self.config_value.
        @param self Object reference.
        @param json_doc JSON configuration document.
        @return current value of self.birth_result.
        """
        config = json.loads(json_doc)
        if (config.has_key("worker_key")):
            self.config_value = config["worker_key"]
        self.birth_called = True
        return self.birth_result

    def process(self, spill_doc):
        """
        Process a spill. Add self.map_id to the spill with key self.map_id
        and value "Processed".
        @param self Object reference.
        @param spill JSON spill document.
        @return updated spill document.
        """
        spill = json.loads(spill_doc)
        spill[self.map_id] = "Processed"
        self.process_called = True
        return json.dumps(spill)

    def death(self):
        """
        Death the mapper.
        @param self Object reference.
        @return current value of self.death_result.
        """
        self.death_called = True
        return self.death_result

class MapDummyOne(MapDummy):
    """
    Sub-class of MapDummy - used to discriminate workers in MapPyGroup.
    """
    pass

class MapDummyTwo(MapDummy):
    """
    Sub-class of MapDummy - used to discriminate workers in MapPyGroup.
    """
    pass

class MapDummyThree(MapDummy):
    """
    Sub-class of MapDummy - used to discriminate workers in MapPyGroup.
    """
    pass

class MapPyGroupTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for MapPyGroup.
    """

    def setUp(self):
        """ 
        Create a MapPyGroup for testing. The group has form:
        @verbatim
        MapPyGroup
          MapDummyOne
          MapPyGroup
            MapDummyOne
            MapDummyTwo
          MapPyGroup
            MapDummyThree
          MapDummyTwo
        @endverbatim
        @param self Object reference.
        """
        # Map group.
        self.__group = MapPyGroup()
        self.__workers = []
        self.__fail_worker = None

        dummy = MapDummyOne("A")
        self.__group.append(dummy)
        self.__workers.append(dummy)

        workers = []
        group = MapPyGroup()
        self.__group.append(group)
        dummy = MapDummyOne("B")
        group.append(dummy)
        workers.append(dummy)
        dummy = MapDummyTwo("C")
        self.__fail_worker = dummy # Use this worker to force failures.
        group.append(dummy)
        workers.append(dummy)
        self.__workers.append(workers)

        group = MapPyGroup()
        self.__group.append(group)
        dummy = MapDummyThree("D")
        group.append(dummy)
        self.__workers.append([dummy])

        dummy = MapDummyTwo("E")
        self.__group.append(dummy)
        self.__workers.append(dummy)

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
        workers = [MapDummyOne(), MapDummyTwo(), MapDummyThree()]
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
        with self.assertRaisesRegexp(AssertionError,
            ".*"):
            MapPyGroup("bad_argument")

    def test_get_worker_names(self):
        """
        Test get_worker_names.
        @param self Object reference.
        """
        names = self.__group.get_worker_names()
        self.assertEquals(len(self.__workers), len(names), 
            "Unexpected number of names")
        while len(self.__workers) != 0:
            expected = self.__workers.pop()
            actual = names.pop()
            if isinstance(actual, ListType):
                self.assertEquals(len(expected), len(actual),
                    "Unexpected number of names in sub-list")
                self.__workers.extend(expected)
                names.extend(actual)
            else:
                self.assertEquals(expected.__class__.__name__, actual,
                    "Unexpected name")

    def execute_bad_append(self, worker):
        """
        Test appending the given worker to the group and check than
        an AssertionError is raised.
        @param self Object reference.
        @param worker Worker.
        """
        with self.assertRaisesRegexp(AssertionError, ".*"):
            self.__group.append(worker)

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
            def birth(self):
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
            def process(self):
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
            def death(self, argument):
                pass
        self.execute_bad_append(TestWorker())
 
    def test_birth(self):
        """
        Test calling birth and check that all workers have their
        birth function called and configuration set.
        @param self Object reference.
        """
        result = self.__group.birth("""{"worker_key":"birth"}""")
        self.assertTrue(result, "birth unexpectedly returned False")
        while len(self.__workers) != 0:
            worker = self.__workers.pop()
            if isinstance(worker, ListType):
                self.__workers.extend(worker)
            else:
                self.assertTrue(worker.birth_called, 
                    "birth wasn't called for worker %s" % worker.map_id)
                self.assertEquals("birth", 
                  worker.config_value,
                  "Worker %s has unexpected configuration value %s" \
                  % (worker.map_id, worker.config_value))

    def test_bad_birth(self):
        """
        Test calling birth where one worker's birth function fails.
        @param self Object reference.
        """
        # Force a worker in group to fail
        self.__fail_worker.birth_result = False
        result = self.__group.birth("{}")
        self.assertFalse(result, "birth unexpectedly returned True")

    def test_process(self):
        """
        Test calling process and check that all workers have their
        process function called and process the spill.
        @param self Object reference.
        """
        spill_doc = self.__group.process("{}")
        spill = json.loads(spill_doc)
        while len(self.__workers) != 0:
            worker = self.__workers.pop()
            if isinstance(worker, ListType):
                self.__workers.extend(worker)
            else:
                self.assertTrue(worker.process_called, 
                    "process wasn't called for worker %s" % worker.map_id)
                self.assertTrue(spill.has_key(worker.map_id),
                    "Spill does not contain map_id for worker %s" \
                    % worker.map_id)

    def test_death(self):
        """
        Test calling death and check all workers have their death
        functions called.
        @param self Object reference.
        """
        result = self.__group.death()
        self.assertTrue(result, "death unexpectedly returned False")
        while len(self.__workers) != 0:
            worker = self.__workers.pop()
            if isinstance(worker, ListType):
                self.__workers.extend(worker)
            else:
                self.assertTrue(worker.death_called, 
                    "death wasn't called for worker %s" % worker.map_id)

    def test_bad_death(self):
        """
        Test calling death where one worker's death function fails.
        @param self Object reference.
        """
        # Force a worker in group to fail
        self.__fail_worker.death_result = False
        result = self.__group.death()
        self.assertFalse(result, "death unexpectedly returned True")

    def test_del(self):
        """
        Test calling __del__ and check all workers have their death
        functions called.
        @param self Object reference.
        """
        self.__group = None
        while len(self.__workers) != 0:
            worker = self.__workers.pop()
            if isinstance(worker, ListType):
                self.__workers.extend(worker)
            else:
                self.assertTrue(worker.death_called, 
                    "death wasn't called for worker %s" % worker.map_id)

if __name__ == '__main__':
    unittest.main()
