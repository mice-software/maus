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

import time

from celery import registry
from celery.task import task
from celery.task import Task
from celery.worker.control import Panel

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

    def __init__(self):
        """ 
        Invoke superclass.
        @param self Object reference.
        """
        Task.__init__(self)

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

class MausGenericTransform(MausTransformTask):
    """ Task that applies a transform that is set by a caller. """

    def set_transforms(self, transforms):
        """
        Update the transform to be applied to consist of those in 
        the given list.
        @param self Object reference.
        @param self List of transform class names 
        e.g. "MAUS.PyDoNothing".
        """
        self._transform = MAUS.MapPyGroup()
        for transform in transforms:
            print transform
            self._transform.append(transform())

registry.tasks[MausGenericTransform.name] # pylint:disable=W0104, E1101

@Panel.register
def maus_reconfigure_worker(panel, config_doc):
    """
    Reconfigure all MAUS workers that support birth with the
    given JSON configuration document.
    @param panel Celery ControlPanel.
    @param config_doc JSON configuration document.
    @returns status message.
    """
    panel.logger.info("MAUS reconfigure_worker invoked!")
    panel.logger.info("Configuration doc: %s" % config_doc)
    for celery_task_name in registry.tasks:
        celery_task = registry.tasks[celery_task_name]
        if hasattr(celery_task, "birth"):
            print "MAUS task: %s. Invoking birth()" % celery_task_name
            celery_task.birth(config_doc)
    print "MAUS tasks reconfigured"
    return {"ok": "maus_worker_reconfigured"}

@Panel.register
def maus_set_worker_transforms(panel, transforms):
    """
    Reconfigure all MAUS workers that support set_transforms with the
    given list of transform classes.
    @param panel Celery ControlPanel.
    @param transforms List of transform classes.
    @returns status message.
    """
    panel.logger.info("MAUS maus_set_worker_transforms invoked!")
    panel.logger.info("Transforms: %s" % transforms)
    for celery_task_name in registry.tasks:
        celery_task = registry.tasks[celery_task_name]
        if hasattr(celery_task, "set_transforms"):
            print "  MAUS task: %s. Invoking set_transform()" \
                % celery_task_name
            celery_task.set_transforms(eval(transforms))
    print "MAUS tasks transforms updated"
    return {"ok": "maus_worker_transforms"}
