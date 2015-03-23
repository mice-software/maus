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

"""
Check that celery is alive on the test server. Note that this should not be run
as part of the regular test script - only should be run by online machine (and
online-capable machines)
"""

import os 
import unittest
import pymongo
import pymongo.errors
import subprocess
import time
import sys 
import signal 

def kill_maus_apps():
    """
    Kill any lurking maus_apps
    """
    # columns = 1024 stops ps from truncating the line (and we fail to detect
    # the maus-app call)
    ps_out =  subprocess.check_output(['ps', '-e', '-F', '--columns=1024'])
    pids = []
    for line in ps_out.split('\n')[1:]:
        if line.find('manage.py') > -1 and line.find('runserver') > -1:
            words = line.split()
            pids.append(int(words[1]))
            print "Found lurking maus-apps process", pids[-1]
    for a_pid in pids:
        print "SIGINT", a_pid
        try:
            os.kill(a_pid, signal.SIGINT)
        except OSError:
            sys.excepthook(*sys.exc_info())
    time.sleep(1)
    for a_pid in pids:
        print "SIGKILL", a_pid
        try:
            os.kill(a_pid, signal.SIGKILL)
        except OSError:
            sys.excepthook(*sys.exc_info())
    time.sleep(1)

class OnlineOkayTest(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Check that celery nodes are available and RabbitMQ is alive or fail
    """
    def test_celery_is_alive(self):
        """
        _test_online_okay: Check that celery can be accessed 

        Fail an assertion if celery.task.control.inspect() fails (RabbitMQ is
        down) or no active nodes
        """
        try:
            proc = subprocess.Popen(['celeryd', '-lINFO', '-c2', '--purge'])
            time.sleep(5)
            proc_alive = proc.poll()
        except Exception: #pylint: disable=W0703
            sys.excepthook(*sys.exc_info())
            self.assertTrue(False)
        finally:
            if proc.poll() == None:
                proc.send_signal(signal.SIGKILL)
        self.assertEqual(proc_alive, None)

    def test_mongodb_is_alive(self):
        """
        _test_online_okay: Check that mongodb can be accessed

        Fail an assertion if mongodb can't be contacted on localhost        
        """
        try:
            test_connx = pymongo.Connection("localhost", 27017)
        except pymongo.errors.AutoReconnect: # pylint: disable=W0702
            self.assertTrue(False, "MongoDB server is not accessible")
        test_connx.disconnect()

    def test_maus_app(self):
        """
        _test_online_okay: Check that maus web-app is in the environment
        """
        kill_maus_apps()
        # should throw an exception if undefined
        os.environ['MAUS_WEB_DIR'] # pylint: disable=W0104
        os.environ['MAUS_WEB_MEDIA_RAW'] # pylint: disable=W0104
        maus_web = os.path.join(
                      os.path.expandvars('$MAUS_WEB_DIR/src/mausweb/manage.py'))
        proc = subprocess.Popen(
                            ['python', maus_web, 'runserver', 'localhost:9000'])
        time.sleep(5)
        kill_maus_apps()
        time.sleep(5)
        self.assertEquals(proc.poll(), 0) # pylint: disable=E1101

    def test_input_cpp_daq_online(self): # pylint:disable=R0201
        """
        _test_online_okay: Check that maus daq online library imports okay
        """
        # skip InputCppDAQOnlineTest if unpacker version is StepI
        if (os.environ['MAUS_UNPACKER_VERSION'] == "StepI"):
            unittest.TestCase.skipTest(self,
                                       "Skip - Not testing online for StepI")
        from MAUS import InputCppDAQOnlineData # pylint:disable=E0611,W0612

if __name__ == "__main__":
    unittest.main()


