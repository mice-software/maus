"""
Tests for Celery workers configured for MAUS. This provides tests for
"broadcast" commands to reconfigure the Celery worker and also for
executing transforms on spills.
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

# pylint: disable=E1101
# pylint: disable=C0103

import json
import unittest
import subprocess
import signal
import time

from datetime import datetime
 
import celery
from celery.task.control import broadcast #pylint: disable=E0611, F0401
from celery.task.control import inspect #pylint: disable=E0611, F0401

import maus_cpp
from Configuration import Configuration
from MapPyTestMap import MapPyTestMap
from mauscelery.tasks import execute_transform

class MausCeleryWorkerTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for Celery workers configured for MAUS.
    """

    proc = None
    default_config = None

    @classmethod
    def setUpClass(cls):
        cls.proc = subprocess.Popen(['celeryd', '-lINFO', '-c2', '--purge'])
        time.sleep(1)

    @classmethod
    def tearDownClass(cls):
        if cls.proc != None:
            print "Killing celeryd process", cls.proc.pid
            cls.proc.send_signal(signal.SIGKILL)

    def setUp(self):
        """ 
        Check for at least one active Celery worker else skip the
        test.
        @param self Object reference.
        """
        self.__inspection = inspect()
        try:
            active_nodes = self.__inspection.active()
        except Exception: # pylint:disable = W0703
            unittest.TestCase.skipTest(self, 
                                       "Skip - RabbitMQ seems to be down")
        if (active_nodes == None):
            unittest.TestCase.skipTest(self, 
                                       "Skip - No active Celery workers")
        # Get the current MAUS version.
        configuration  = Configuration()
        self.config_doc = configuration.getConfigJSON()
        config_dictionary = json.loads(self.config_doc)
        self.__version = config_dictionary["maus_version"]
        # Reset the worker. Invoke twice in case the first attempt
        # fails due to mess left by previous test.
        self.reset_worker()
        self.reset_worker()
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        maus_cpp.globals.birth(self.config_doc)

    def reset_worker(self): # pylint:disable = R0201
        """
        Reset the Celery workers, via a broadcast, to have no
        configuration and to use MapPyDoNothing. 
        @param self Object reference.
        @return status of update.
        """
        config_id = datetime.now().microsecond
        broadcast("birth", 
            arguments={"configuration":self.config_doc,
                       "transform":"MapPyDoNothing", 
                       "config_id":config_id}, reply=True)

    def birth(self, config_id, configuration = None,
              transform = "MapPyDoNothing", merge_configuration = False): # pylint:disable = R0201, C0301
        """
        Configure the Celery workers, via a broadcast.
        @param self Object reference.
        @param config_id Configuration ID.
        @param configuration Configuration.
        @param transform Transform specification.
        @return status of update.
        """
        if configuration == None:
            configuration = self.config_doc
        elif merge_configuration:
            my_config = json.loads(self.config_doc)
            config_user = json.loads(configuration)
            for key in config_user:
                my_config[key] = config_user[key]
            configuration = json.dumps(my_config)
        return broadcast("birth", 
            arguments={"configuration":configuration, \
                       "transform":transform, 
                       "config_id":config_id,
                       "run_number":1}, reply=True)

    def validate_configuration(self, configuration, transform,
        config_id = None):
        """
        Validate workers have the given configuration using
        set_maus_configuration and the same MAUS version as the
        test class.
        @param self Object reference.
        @param configuration Expected configuration.
        @param transform Expected transform specification.
        @param config_id Expected configuration ID.
        """
        result = broadcast("get_maus_configuration", reply=True)
        print "get_maus_configuration: %s " % result
        # Use built-in Celery worker inspection command to get
        # worker names.
        check_workers = self.__inspection.stats()
        check_worker_names = check_workers.keys()
        self.assertEquals(len(check_worker_names), len(result), 
            "Number of worker entries does not match that expected")
        for worker in result:
            worker_name = worker.keys()[0]
            self.assertTrue(worker_name in check_worker_names, 
                "Cannot find entry for worker %s" % worker_name)
            worker_config = worker[worker_name]
            self.assertTrue(worker_config.has_key("config_id"),
                "Configuration has no config_id entry")
            if (config_id != None):
                self.assertEquals(config_id, worker_config["config_id"],
                    "Unexpected config_id value")
            self.assertTrue(worker_config.has_key("configuration"),
                "Configuration has no configuration entry")
            self.assertEquals(configuration,
                worker_config["configuration"],
                "Unexpected configuration value\n\n%s\n\n%s" % (configuration,
                worker_config["configuration"]))
            self.assertTrue(worker_config.has_key("transform"),
                "Configuration has no transform entry")
            self.assertEquals(transform, worker_config["transform"],
                "Unexpected transform value")
            self.assertTrue(worker_config.has_key("version"),
                "Configuration has no version entry")
            self.assertEquals(self.__version, 
                worker_config["version"],
                "Unexpected version value")

    def validate_status(self, report, status = "ok"):
        """
        Validate the status report from a birth or death
        command. Expect one of:
        @verbatim
        [{u'w1': {'status': 'ok'}},...]
        [{u'w1': {u'error': [{u'error': CLASS, u'message': 
            MESSAGE},...], u'status': u'error'}},...]
        @endverbatim
        @param self Object reference.
        @param report Status report.
        @param status Expected status.
        """
        self.assertTrue(len(report) > 0, 
            "Expected at least one worker to respond")
        # Get the worker names.
        check_workers = self.__inspection.ping().keys()
        # Sort so both are ordered by worker names.
        check_workers.sort()
        report.sort()
        while len(report) > 0:
            worker = report.pop()
            worker_name = worker.keys()[0]
            check_worker = check_workers.pop()
            self.assertEquals(check_worker, worker_name,
                              "Unexpected worker name")
            worker_status = worker[worker_name]
            self.assertTrue(worker_status.has_key("status"), 
                "Missing status entry")
            self.assertEquals(status, worker_status["status"], 
                "Unexpected status value")
            if (status == "error"): 
                self.assertTrue(worker_status.has_key("error"), 
                    "Missing error entry")
                errors = worker_status["error"]
                self.assertTrue(len(errors) > 0, 
                    "Expected a list of error information")

    def test_birth(self):
        """
        Test birth broadcast command. 

        Note we also test mausworker.get_maus_configuration here.

        @param self Object reference.
        """
        config_id = datetime.now().microsecond
        transform = "MapPyPrint"
        configuration = json.loads(self.config_doc)
        configuration["TOFconversionFactor"] = 1
        configuration = json.dumps(configuration)
        result = self.birth(config_id, configuration, transform)
        print "birth(OK): %s " % result
        # Check the status and that the configuration has been 
        # updated.  
        self.validate_status(result)
        self.validate_configuration(configuration, transform, config_id)
        # Check that another birth with the same ID is a no-op. Use
        # different transform name and configuration to be sure.
        result = self.birth(config_id, configuration, transform)
        print "birth(OK): %s " % result
        # Check the status and configuration are OK.
        self.validate_status(result)
        self.validate_configuration(configuration, transform, config_id)

    def test_birth_bad_config_json(self):
        """
        Test birth broadcast command with an invalid JSON
        configuration document. 
        @param self Object reference.
        """
        config_id = datetime.now().microsecond
        transform = "MapPyPrint"
        configuration = """{"TOFconversionFactor":BADJSON"""
        result = self.birth(config_id, configuration, transform)
        print "birth(bad JSON document): %s " % result
        self.validate_status(result, "error")

    def test_birth_bad_version(self):
        """
        Test birth broadcast command with a mismatched MAUS version. 
        @param self Object reference.
        """
        config_id = datetime.now().microsecond
        transform = "MapPyPrint"
        configuration = """{"maus_version":"BAD"}"""
        result = self.birth(config_id, configuration, transform)
        print "birth(bad version): %s " % result
        self.validate_status(result, "error")

    def test_birth_bad_transform(self):
        """
        Test birth broadcast command with an invalid transform name.
        @param self Object reference.
        """
        config_id = datetime.now().microsecond
        transform = "MapPyUnknown"
        configuration = """{"maus_version":"%s"}""" % self.__version
        result = self.birth(config_id, configuration, transform)
        print "birth(bad transform name): %s " % result
        self.validate_status(result, "error")

    def test_birth_error(self):
        """
        Test birth broadcast command where the transform returns
        False when birth is called.
        @param self Object reference.
        """
        # Set up a transform that will fail when it is birthed.
        config_id = datetime.now().microsecond
        transform = "MapPyTestMap"
        configuration = """{"birth_result":%s, "maus_version":"%s"}""" \
            % (MapPyTestMap.EXCEPTION, self.__version)
        result = self.birth(config_id, configuration, transform)
        print "birth(transform.birth exception): %s " % result
        # Check the status specifies an error.
        self.validate_status(result, "error")

    def test_death_before_birth_error(self):
        """
        Test death broadcast command where the transform returns
        False when death is called prior to a birth command.
        @param self Object reference.
        """
        # Set up a transform that will fail when it is deathed.
        config_id = datetime.now().microsecond
        transform = "MapPyTestMap"
        configuration = """{"death_result":%s, "maus_version":"%s"}""" \
            % (MapPyTestMap.EXCEPTION, self.__version)
        result = self.birth(config_id, configuration, transform)
        # Check the status is OK.
        self.validate_status(result)
        # Now create a new transform thereby causing the death failure
        # of the current one.
        config_id = datetime.now().microsecond
        transform = "MapPyDoNothing"
        configuration = """{"maus_version":"%s"}""" % self.__version
        result = self.birth(config_id, configuration, transform)
        print "birth(transform.death exception): %s " % result
        # Check the status specifies an error.
        self.validate_status(result, "error")
        # Try again with the same configuration ID. Expect
        # success this time.
        result = self.birth(config_id, configuration, transform)
        # Check the status and that the configuration has been
        # updated. 
        self.validate_status(result)
        self.validate_configuration(configuration, transform, config_id)

    def test_death(self):
        """
        Test death broadcast command.
        @param self Object reference.
        """
        result = broadcast("death", arguments={"run_number":1}, reply=True)
        result.sort()
        print "death: %s " % result
        self.validate_status(result)
        # Expect subsequent attempt to succeed.
        result = broadcast("death", arguments={"run_number":1}, reply=True)
        self.validate_status(result)

    def test_death_exception(self):
        """
        Test death broadcast command where the transform throws
        an exception when death is called.
        @param self Object reference.
        """
        # Set up a transform that will fail when it is deathed.
        config_id = datetime.now().microsecond
        transform = "MapPyTestMap"
        configuration = """{"death_result":%s, "maus_version":"%s"}""" \
            % (MapPyTestMap.EXCEPTION, self.__version)
        result = self.birth(config_id, configuration, transform)
        # Check the status is OK.
        self.validate_status(result)
        # Now death the transform.
        result = broadcast("death", arguments={"run_number":1}, reply=True)
        print "death(transform.death exception): %s " % result
        self.validate_status(result, "error")
        # Expect subsequent attempt to succeed.
        result = broadcast("death", arguments={"run_number":1}, reply=True)
        self.validate_status(result)

    def test_process(self):
        """
        Test process command.
        @param self Object reference.
        """
        config_id = datetime.now().microsecond
        transform = "MapPyTestMap"
        configuration = """{"maus_version":"%s"}""" % self.__version
        result = self.birth(config_id, configuration, transform)
        self.validate_status(result)
        # Call process.
        result = execute_transform.delay("{}", 1)
        # Wait for it to complete.
        result.wait()
        self.assertTrue(result.successful(), "Expected success")
        spill = json.loads(result.result)
        self.assertTrue(spill.has_key("processed"), 
            "Spill does not seem to have been updated")

    def test_process_after_death(self):
        """
        Test process command fails after death has been called.
        @param self Object reference.
        """
        config_id = datetime.now().microsecond
        transform = "MapPyTestMap"
        configuration = """{"maus_version":"%s"}""" % self.__version
        result = self.birth(config_id, configuration, transform)
        self.validate_status(result)
        result = broadcast("death", arguments={"run_number":1}, reply=True)
        self.validate_status(result)
        # Call process.
        result = execute_transform.delay("{}", 1)
        # Wait for it to complete.
        try:
            result.wait()
        except Exception:  # pylint:disable = W0703
            pass
        self.assertTrue(result.failed(), "Expected failure")

    def test_process_timeout(self):
        """
        Test process timeout.
        @param self Object reference.
        """
        config_id = datetime.now().microsecond
        transform = "MapPyTestMap"
        # check that process does not time out when execution is quick
        configuration = """{"maus_version":"%s", "process_delay":9.0}""" \
                                                                % self.__version
        result = self.birth(config_id, configuration, transform, True)
        self.validate_status(result)
        # Call process.
        result = execute_transform.delay("{}", 1)
        # Wait for it to complete.
        print "Executing delayed transform"
        result.wait()

        # check that process does time out when execution is slow
        configuration = """{"maus_version":"%s", "process_delay":15.0}""" \
                                                                % self.__version
        result = self.birth(config_id, configuration, transform, True)
        self.validate_status(result)
        # Call process. Check that time_limit flag is ignored
        # (true for celery version < 3); if we move to a new version of celery,
        # this should fail; and we should change the timeout to be soft coded
        result = execute_transform.apply_async(["{}", 1], time_limit=20.)
        # Wait for it to complete.
        try:
            print "Executing delayed transform"
            result.wait()
            self.assertTrue(False, 'Should have failed')
        except celery.exceptions.TimeLimitExceeded:  # pylint:disable = W0703
            pass
        # should have failed
        self.assertEqual(result.state, 'FAILURE')

if __name__ == '__main__':
    unittest.main()
