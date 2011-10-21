"""
Prototypes of MAUS-related Celery tasks.
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

from celery import registry
from celery.task import task
from celery.task import Task
import time

import MAUS

@task
def maus_transform_naive(json_config_doc, transform, spill):
    """
    Configure the transform then apply the transform to 
    the spill and return the new spill.
    @param self Object reference.
    @param transform Serialised transform to apply.
    @param json_config_doc JSON configuration document.
    @param spill JSON document string holding spill.
    @return JSON document string holding new spill.
    """
    print "Invoking execute_transform_naive" 
    transform.birth(json_config_doc)
    return transform.process(spill)

class MausTransformNaive(Task): # pylint:disable=R0903
    """ Task that applies a serialized transform. """

    def run(self, json_config_doc, transform, spill): 
        """
        Configure the transform then apply the transform to 
        the spill and return the new spill.
        @param self Object reference.
        @param transform Serialised transform to apply.
        @param json_config_doc JSON configuration document.
        @param spill JSON document string holding spill.
        @return JSON document string holding new spill.
        """
        print "%s.run(): " % type(self).__name__ 
        print "  Start : %s" % time.ctime()
        transform.birth(json_config_doc)
        spill = transform.process(spill)
        print "  End : %s" % time.ctime()
        return spill
registry.tasks[MausTransformNaive.name] # pylint:disable=W0104, E1101

class MausTransformTask(Task):
    """
    Super-class of MAUS transform tasks.
    """

    abstract = True

    def __init__(self):
        """ 
        Initialise transform to None.
        @param self Object reference.
        """
        self._transform = None

    def _setup_transform(self):
        """ 
        Set up transform to be applied by task. 
        Sub-classes must override this method.
        @param self Object reference.
        """
        pass

    def birth(self, json_config_doc):
        """
        Setup transform then invoke birth with the JSON
        configuration document.
        @param self Object reference.
        @param json_config_doc JSON configuration document.       
        """
        self._setup_transform()
        if self._transform != None:
            self._transform.birth(json_config_doc)

    def run(self, spill):
        """
        Apply the transform to the spill and return the new spill.
        @param self Object reference.
        @param spill JSON document string holding spill.
        @return JSON document string holding new spill.
        """
        print "%s.run(): " % type(self).__name__ 
        print "  Start : %s" % time.ctime()
        spill = self._transform.process(spill)
        print "  End : %s" % time.ctime()
        return spill

class MausTransformDoNothing(MausTransformTask):
    """ Task that applies a MapPyDoNothing transform. """

    def _setup_transform(self):
        """ 
        Set up transform to be applied by task. 
        Sub-classes must override this method.
        @param self Object reference.
        """
        self._transform = MAUS.MapPyGroup()
        self._transform.append(MAUS.MapPyDoNothing())  

registry.tasks[MausTransformDoNothing.name] # pylint:disable=W0104, E1101

class MausTransformSimulate(MausTransformTask):
    """ 
    Task that applies a MapPyBeamMaker->MapCppSimulation->
    MapCppTOFDigitization->MapCppTrackerDigitization
    transform. 
    """

    def _setup_transform(self):
        """ 
        Set up transform to be applied by task. 
        Sub-classes must override this method.
        @param self Object reference.
        """
        self._transform = MAUS.MapPyGroup()
        self._transform.append(MAUS.MapPyBeamMaker())
        self._transform.append(MAUS.MapCppSimulation())
        self._transform.append(MAUS.MapCppTOFDigitization())
        self._transform.append(MAUS.MapCppTrackerDigitization())

registry.tasks[MausTransformSimulate.name] # pylint:disable=W0104, E1101
