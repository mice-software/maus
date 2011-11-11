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

from celery.signals import worker_process_init 

def maus_worker_process_init_handler(sender, signal): 
#    log = logging.getLogger(__name__) 
#    log.debug("Got signal worker_process_init, so starting TurboMail") 
#    from turbomail.adapters import tm_pylons 
#    tm_pylons.start_extension() 
    print "----WORKER PROCESS INIT HANDLER"
    print sender
    print signal
    print dir(signal)
    MausTransformTask.staticprint()
    return "XXX"
worker_process_init.connect(maus_worker_process_init_handler) 

class MausTransformTask(Task):
    """
    Super-class of MAUS transform tasks.
    """

    abstract = True
    staticcount = 0
    statictransforms = None

    @staticmethod
    def staticprint():
        print "STATICPRINT"
        MausTransformTask.staticcount = MausTransformTask.staticcount + 10

    @staticmethod
    def staticprint2(sender, signal):
        print "STATICPRINT2 %s" % sender

    @staticmethod
    def statictransforms(transforms):
        print "STATICTRANSFORM"
        MausTransformTask.statictransforms = transforms

    def __init__(self):
        """ 
        Initialise transform to None.
        @param self Object reference.
        """
        print dir(self)
        self.worker_process_init()
        print "%s.__init__(): id: %s " % (type(self).__name__, id(self))
        self._transform = None
        print "__init__():  self._transform %s" % self._transform
        self._config = None
        print "__init__ %d" % MausTransformTask.staticcount
        self._count = 0
        self.update_counts("__init__")

    def update_counts(self, function_name):
        print "%s %d %d" % (function_name, self._count, MausTransformTask.staticcount)
        self._count = self._count + 1
        MausTransformTask.staticcount = MausTransformTask.staticcount + 1
        print "%s %d %d" % (function_name, self._count, MausTransformTask.staticcount)
                
    def _setup_transform(self):
        """ 
        Set up transform to be applied by task. 
        Sub-classes must override this method.
        @param self Object reference.
        """
        print "%s._setup_transform(): id: %s " % (type(self).__name__, id(self))
        print(" _setup_transform task.id %r" % MausTransformTask.request.id)
        print "   self._transform %s" % self._transform
        self.update_counts("_setup_transform_")
        pass

    def birth(self, json_config_doc):
        """
        Setup transform then invoke birth with the JSON
        configuration document.
        @param self Object reference.
        @param json_config_doc JSON configuration document.       
        """
        print "%s.birth(): id: %s " % (type(self).__name__, id(self))
        self._setup_transform()
        self._config = json_config_doc
        print "   self._transform %s" % self._transform
        if self._transform != None:
            self._transform.birth(json_config_doc)
        self.update_counts("birth")

    def execute(self, request, pool, loglevel, logfile, **kwargs):
        print "---->%s.execute(): id: %s " % (type(self).__name__, id(self))
        self.update_counts("execute")
        Task.execute(self, request, pool, loglevel, logfile, **kwargs)
        print "<----execute()"

    def __call__(self, *args, **kwargs): 
        print "---->%s.__call__(): id: %s " % (type(self).__name__, id(self))
        return super(MausTransformTask, self).__call__(*args, **kwargs) 

    def run(self, spill):
        """
        Apply the transform to the spill and return the new spill.
        @param self Object reference.
        @param spill JSON document string holding spill.
        @return JSON document string holding new spill.
        """
        print "---->%s.run(): id: %s " % (type(self).__name__, id(self))
        print(" run() task.id %r" % MausTransformTask.request.id)
        print "  Start : %s" % time.ctime()
        print "   self._transform %s" % self._transform
        self.update_counts("run")
        mappers = MAUS.MapPyGroup()
        mappers.append(MAUS.MapPyDoNothing())  
        spill = mappers.process(spill)
#        spill = self._transform.process(spill)
        print "  End : %s" % time.ctime()
        print "<----run()"
        return spill

    def worker_process_init(self):
        print ">-----------------worker_process_init"
        self.worker_process_init()
        print "<-----------------worker_process_init"

from celery.signals import worker_process_init 

worker_process_init.connect(MausTransformTask.staticprint2)



class MausGenericTransform(MausTransformTask):
    """ Task that applies a transform that is set by a caller. """

    def worker_process_init(self):
        print "---------------------------------"

    def set_transforms(self, transforms):
        """
        Update the transform to be applied to consist of those in 
        the given list.
        @param self Object reference.
        @param self List of transform class names 
        e.g. "MAUS.PyDoNothing".
        """
        print "---->%s.set_transforms(): id: %s " % (type(self).__name__, id(self))
        print("set_transforms task.id %r" % MausTransformTask.request.id)
        print "self._transform %s" % self._transform
        self._transform = MAUS.MapPyGroup()
        for transform in transforms:
            print transform
            self._transform.append(transform())
        self._transform.birth(self._config)
        print "   self._transform %s" % self._transform
        self.update_counts("set_transforms")
        print "<----%s.set_transforms(): id: %s " % (type(self).__name__, id(self))

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
    panel.logger.info("---->maus_reconfigure_worker")
    panel.logger.info("Configuration doc: %s" % config_doc)
    for celery_task_name in registry.tasks:
        celery_task = registry.tasks[celery_task_name]
        if hasattr(celery_task, "birth"):
            print "----maus_reconfigure_worker: %s. Invoking birth()" % celery_task_name
            celery_task.birth(config_doc)
    print "MAUS tasks reconfigured"
    panel.logger.info("<----maus_reconfigure_worker")
    return {"ok": "maus_worker_reconfigured"}

@Panel.register
def maus_set_worker_transforms(panel, transforms, tmpvalue):
    """
    Reconfigure all MAUS workers that support set_transforms with the
    given list of transform classes.
    @param panel Celery ControlPanel.
    @param transforms List of transform classes.
    @returns status message.
    """
    panel.logger.info("---->maus_set_worker_transforms")
    print panel
    print dir(panel)
    print panel.app
    print dir(panel.app)
    print panel.app.control
    print dir(panel.app.control)

    print panel.consumer
    print dir(panel.consumer)
    print panel.consumer.pool
    print dir(panel.consumer.pool)
    print "--------------------------------------------------"
    print panel.consumer.pool.info
    print "--------------------------------------------------"
    worker_process_init.send("A")
#    panel.consumer.pool.stop()
#    panel.consumer.pool.start()
    panel.logger.info("Transforms: %s" % transforms)
    for celery_task_name in registry.tasks:
        celery_task = registry.tasks[celery_task_name]
        if hasattr(celery_task, "set_transforms"):
            print "----maus_set_worker_transforms: %s. Invoking set_transform()" \
                % celery_task_name
            celery_task.set_transforms(eval(transforms))
    panel.logger.info("<----maus_set_worker_transforms")
    return {"ok": "maus_worker_transforms"}

from celery.task.sets import subtask

class MapPyDoNothingTask(Task):
    """ Task """

    def __init__(self):
        """ 
        Initialise transform to None.
        @param self Object reference.
        """
        print "%s.__init__(): id: %s " % (type(self).__name__, id(self))
        self._transform = MAUS.MapPyDoNothing()

    def birth(self, json_config_doc):
        """
        Setup transform then invoke birth with the JSON
        configuration document.
        @param self Object reference.
        @param json_config_doc JSON configuration document.       
        """
        print "%s.birth(): id: %s " % (type(self).__name__, id(self))
        self._transform.birth(json_config_doc)

    def run(self, spill, callback=None):
        """
        Apply the transform to the spill and return the new spill.
        @param self Object reference.
        @param spill JSON document string holding spill.
        @return JSON document string holding new spill.
        """
        print "%s.run(): id: %s " % (type(self).__name__, id(self))
#        spill = self._transform.process(spill)
        print spill
        spill = spill + "PYDONOTHINH"
        print spill
        if callback:
            # spill = subtask(callback).delay(spill)
            subtask(callback).delay(spill)
        print spill
        print "<--------MapPyDoNothingTask"
        return spill

registry.tasks[MapPyDoNothingTask.name] # pylint:disable=W0104, E1101

class MapPyGroupTask(Task):
    """ Task """

    def __init__(self):
        """ 
        Initialise transform to None.
        @param self Object reference.
        """
        print "%s.__init__(): id: %s " % (type(self).__name__, id(self))

#    def run(self, group, spill):
    def run(self, spill):
        """
        Apply the transform to the spill and return the new spill.
        @param self Object reference.
        @param spill JSON document string holding spill.
        @return JSON document string holding new spill.
        """
        print "%s.run(): id: %s " % (type(self).__name__, id(self))
        spill = "PYGROUP"
        print spill
        # return MapPyDoNothingTask.delay(spill, callback=subtask(MapPyDoNothingTask))
        MapPyDoNothingTask.delay(spill, callback=subtask(MapPyDoNothingTask))
        print "<--------MapPyGroupTask"

registry.tasks[MapPyGroupTask.name] # pylint:disable=W0104, E1101
