"""
Test class for mauscelery.mausworker module. Mock objects are used to
mimic a process pool for asynchronous function calls.
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

from datetime import datetime
import json
import logging
import unittest

from Configuration import Configuration
import mauscelery.mausworker
from mauscelery.state import MausConfiguration
from mauscelery.mausprocess import process_birth
from mauscelery.mausprocess import process_death
from mauscelery.mausworker import birth
from mauscelery.mausworker import death
from mauscelery.mausworker import get_maus_configuration
from mauscelery.mausworker import worker_init_callback

class TestPanel: # pylint: disable=R0903
    """
    Mock test class for Celery panel.
    """

    def __init__(self):
        """ 
        Constructor. Create a TestConsumer attribute.
        @param self Object reference.
        """
        self.consumer = TestConsumer()

class TestConsumer: # pylint: disable=R0903
    """
    Mock test class for Celery consumer.
    """

    def __init__(self):
        """ 
        Constructor. Create a TestPool attribute.
        @param self Object reference.
        """
        self.pool = TestPool()

class TestPool: # pylint: disable=R0903
    """
    Mock test class for Celery pool.
    """

    def __init__(self):
        """ 
        Constructor. Create a process ID list, result, and
        both asynchronous invocation and function success/fail
        attributes.  
        @param self Object reference.
        """
        self.pid = 123
        self.info = {}
        self.info["processes"] = [self.pid]
        self.result = None
        self.async_success = True
        self.function_success = True

    def set_async_success(self, success):
        """
        Set whether to pretend that an attempt to make an asynchronous
        function call succeeds or throws an exception. This allows
        mimicking of exceptions that might arise within a process
        pool. 
        @param self Object reference.
        """
        self.async_success = success

    def set_function_success(self, success):
        """
        Set whether to pretend that a function called asynchronously
        succeeds or throws an exception within a sub-process. This
        allows mimicking of cases when use of a processing pool
        succeeds but the function called fails in a sub-process.
        @param self Object reference.
        """
        self.function_success = success

    def apply_async(self, func, arguments):
        """ 
        Mock of the Celery Pool.apply_async method. If async_success
        is false then raise an exception otherwise return a
        TestAsyncResult reference and store this in an attribute.  
        @param self Object reference.
        @param func Function pointer.
        @param arguments. Arguments list.
        @throws Exception if async_success is False.
        """
        if (not self.async_success):
            raise Exception("Mock asynchronous invocation error")
        self.result = TestAsyncResult(self.pid, func, arguments, 
            self.function_success)
        return self.result

class TestAsyncResult: # pylint: disable=R0903
    """
    Mock test class for Celery asynchronous result.
    """

    def __init__(self, pid, func, arguments, function_success):
        """ 
        Constructor.
        @param self Object reference.
        @param pid Fake process ID.
        @param func Function pointer.
        @param arguments. Arguments list.
        """
        # Save function and arguments for validation.
        self.func = func
        self.arguments = arguments
        if (function_success):
            self.result = (pid, None)
        else:
            self.result = (pid, {"error":"ERROR", "message":"MESSAGE"})

    def get(self):
        """ 
        Return mock result. If function_success is True then this is:
        {pid, None) else it is (pid, 
        {"error":"ERROR", "message":"MESSAGE"}).
        @param self Object reference.
        @return mock result.
        """
        return self.result

class WorkerInitCallbackTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.mausworker.worker_init_callback 
    method.
    """

    def setUp(self): # pylint: disable=C0103
        """ 
        Reset MausConfiguration and set logging.
        @param self Object reference.
        """
        MausConfiguration.version = None
        # Configure lowest logging level so all logging statements
        # are executed.
        logger = logging.getLogger(mauscelery.mausworker.__name__)
        logger.setLevel(logging.DEBUG)

    def test_callback(self):
        """ 
        Invoke worker_process_callback.
        @param self Object reference.
        """
        worker_init_callback()
        configuration  = Configuration()
        config_doc = configuration.getConfigJSON()
        config_dictionary = json.loads(config_doc)
        self.assertEquals(config_dictionary["maus_version"],
            MausConfiguration.version,
            "Unexpected version")

class MausBroadcastTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.mausworker broadcast methods.
    """

    def setUp(self): # pylint: disable=C0103
        """ 
        Configure logging and reset the MausConfiguration.
        @param self Object reference.
        """
        # Configure lowest logging level so all logging statements
        # are executed.
        logger = logging.getLogger(mauscelery.mausworker.__name__)
        logger.setLevel(logging.DEBUG)
        # Get the current MAUS version.
        configuration  = Configuration()
        config_doc = configuration.getConfigJSON()
        config_dictionary = json.loads(config_doc)
        self.__version = config_dictionary["maus_version"]
        # Reset MAUS configuration.
        MausConfiguration.config_id = 0
        MausConfiguration.configuration = "{}"
        MausConfiguration.transform = "MapPyDoNothing"
        MausConfiguration.version = self.__version
        # Create mock Celery panel object.
        self.__panel = TestPanel()

    def validate_status(self, report, status = "ok"):
        """
        Validate the status report from a birth or death
        command. Expect one of:  
        @verbatim
        {'status': 'ok'}
        {u'error': [{u'error': CLASS, u'message': 
            MESSAGE},...], u'status': u'error'}
        @endverbatim
        @param self Object reference.
        @param report Status report.
        @param status Expected status.
        """
        self.assertTrue(report.has_key("status"), "Missing status entry")
        self.assertEquals(status, report["status"], 
            "Unexpected status value")
        if (status == "error"): 
            self.assertTrue(report.has_key("error"), "Missing error entry")
            errors = report["error"]
            self.assertTrue(len(errors) > 0, 
                "Expected a list of error information")

    def validate_configuration(self, configuration, transform,
        config_id = None):
        """
        Validate MausConfiguration has been updated.
        @param self Object reference.
        @param configuration Expected configuration.
        @param transform Expected transform specification.
        @param config_id Expected configuration ID.
        """
        self.assertEquals(config_id, 
            MausConfiguration.config_id,
            "Unexpected config_id value")
        self.assertEquals(configuration,
            MausConfiguration.configuration,
            "Unexpected configuration value")
        self.assertEquals(transform,  
            MausConfiguration.transform,
            "Unexpected transform value")
        self.assertEquals(self.__version, 
            MausConfiguration.version,
            "Unexpected version value")

    def test_birth(self):
        """
        Test birth.
        @param self Object reference.
        """
        config_id = datetime.now().microsecond
        transform = "MapPyPrint"
        configuration = """{"TOFconversionFactor":%s, "maus_version":"%s"}""" \
            % (config_id, self.__version)
        result = birth(self.__panel, config_id, transform, configuration)
        # Check the status and that the configuration has been 
        # updated.  
        self.validate_status(result)
        self.validate_configuration(configuration, transform, config_id)
        # Check expected function and arguments were passed:
        self.assertEquals(process_birth, 
            self.__panel.consumer.pool.result.func,
            "An unexpected function was passed to be invoked")
        (check_set, check_config_id, check_transform, check_config) = \
            self.__panel.consumer.pool.result.arguments
        self.assertTrue(isinstance(check_set, set),
             "Expected a set to be passed")
        self.assertEquals(check_config_id, config_id,
            "Unexpected config_id was passed to be invoked")
        self.assertEquals(check_transform, transform,
            "Unexpected transform was passed to be invoked")
        self.assertEquals(check_config, configuration,
            "Unexpected configuration was passed to be invoked")
        # Check that another birth with the same ID is a no-op. Use
        # different transform name and configuration to be sure.
        result = birth(self.__panel, config_id, transform, configuration)
        # Check the status and configuration.
        self.validate_status(result, "ok")
        self.validate_configuration(configuration, transform, config_id)

    def test_birth_bad_config_json(self):
        """
        Test birth with an invalid JSON configuration document. 
        @param self Object reference.
        """
        config_id = datetime.now().microsecond
        configuration = """{"TOFconversionFactor":BADJSON"""
        result = birth(self.__panel, config_id, "MapPyPrint", configuration)
        # Check the status specifies an error.
        self.validate_status(result, "error")

    def test_birth_bad_version(self):
        """
        Test birth with a mismatched MAUS version. 
        @param self Object reference.
        """
        config_id = datetime.now().microsecond
        configuration = """{"maus_version":"BAD"}"""
        result = birth(self.__panel, config_id, "MapPyPrint", configuration)
        # Check the status specifies an error.
        self.validate_status(result, "error")

    def test_birth_function_exception(self):
        """
        Test birth where birth throws an exception.
        @param self Object reference.
        """
        # Configure so that birth throws an exception.
        self.__panel.consumer.pool.set_function_success(False)
        config_id = datetime.now().microsecond
        configuration = """{"maus_version":"%s"}""" % self.__version
        result = birth(self.__panel, config_id, "MapPyPrint", configuration)
        # Check the status specifies an error.
        self.validate_status(result, "error")

    def test_birth_pool_exception(self):
        """
        Test birth where the pool throws an exception.
        @param self Object reference.
        """
        # Configure so that the pool throws an exception.
        self.__panel.consumer.pool.set_async_success(False)
        config_id = datetime.now().microsecond
        configuration = """{"maus_version":"%s"}""" % self.__version
        result = birth(self.__panel, config_id, "MapPyPrint", configuration)
        # Check the status specifies an error.
        self.validate_status(result, "error")

    def test_death(self):
        """
        Test death.
        @param self Object reference.
        """
        result = death(self.__panel)
        # Check the status.
        self.validate_status(result)
        # Check expected function and arguments were passed:
        self.assertEquals(process_death, 
            self.__panel.consumer.pool.result.func,
            "An unexpected function was passed to be invoked")
        (check_set,) = self.__panel.consumer.pool.result.arguments
        self.assertTrue(isinstance(check_set, set),
             "Expected a set to be passed")

    def test_death_function_exception(self):
        """
        Test death where death throws an exception.
        @param self Object reference.
        """
        # Configure so that death throws an exception.
        self.__panel.consumer.pool.set_function_success(False)
        result = death(self.__panel)
        # Check the status specifies an error.
        self.validate_status(result, "error")

    def test_death_pool_exception(self):
        """
        Test death where the pool throws an exception.
        @param self Object reference.
        """
        # Configure so that the pool throws an exception.
        self.__panel.consumer.pool.set_async_success(False)
        result = death(self.__panel)
        # Check the status specifies an error.
        self.validate_status(result, "error")

    def test_get_maus_configuration(self):
        """ 
        Test get_maus_configuration.
        @param self Object reference.
        """
        MausConfiguration.transform = "MapPyTestMap"
        MausConfiguration.configuration = "{test}"
        MausConfiguration.version = "MAUS release version: 123"
        config = get_maus_configuration(None)
        self.assertTrue(config.has_key("config_id"), 
            "Missing config_id key")
        self.assertEquals(MausConfiguration.config_id, 
            config["config_id"], "Unexpected config_id")
        self.assertTrue(config.has_key("configuration"), 
            "Missing configuration key")
        self.assertEquals(MausConfiguration.configuration, 
            config["configuration"], "Unexpected configuration")
        self.assertTrue(config.has_key("transform"),
            "Missing transform")
        self.assertEquals(MausConfiguration.transform,
            config["transform"], "Unexpected transform")
        self.assertEquals(MausConfiguration.version,
            config["version"], "Unexpected version")

if __name__ == '__main__':
    unittest.main()
