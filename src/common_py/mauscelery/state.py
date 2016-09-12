"""
MAUS Celery configuration and transform wrapper.
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

import logging
import sys

import ROOT
from framework.workers import WorkerUtilities
from framework.workers import WorkerBirthFailedException
from framework.workers import WorkerDeathFailedException
from framework.workers import WorkerDeadException

class MausConfiguration(): # pylint:disable = W0232, R0903
    """
    MAUS transform configuration consisting of a MAUS JSON
    configuration document, a transform specification an ID and the
    current MAUS version. The transform specification can be a single
    name - representing a single transform - or a list of transforms -
    representing a MapPyGroup. Sub-lists are treated as nested
    MapPyGroups. For example: 
    @verbatim
    ["MapCppTOFDigits", "MapCppTOFSlabHits", "MapCppTOFSpacePoint"]
    or
    ["MapCppTOFDigits", ["MapCppTOFSlabHits", "MapCppTOFSpacePoint"]]
    @endverbatim
    It is assumed that transform specification consists of valid
    transform names and that configuration is a valid JSON
    document. Callers can ensure this via calls to
    WorkerUtilities.validate_transform or json.dumps.
    This serves as a container for configuration for both the Celery
    master and sub-processes.
    """
    transform = "MapPyDoNothing"
    configuration = "{}"
    config_id = 0
    version = ""

class MausTransform(): # pylint:disable = W0232
    """
    MAUS transform wrapper.
    The caller must ensure that initialize is called before birth,
    process or death are used, and likewise ensure that birth is
    called before process is used. 
    This is unused by the Celery master process but each sub-process
    uses it to manage their current transform which they use to
    process client requests.
    """
    # Transform object.
    transform = None
    # Dead flag. If the transform has been death()ed this prevents
    # re-invocations of death() until birth() has been called.
    is_dead = True
        
    @classmethod
    def initialize(cls, transform_specification):
        """
        Given a transform specification, create a new transform. If
        there are existing transforms then these are death-ed first.
        @param cls Class reference.
        @param transform_specification Either a single name can be
        given - representing a single transform - or a list of
        transforms - representing a MapPyGroup. Sub-lists are treated
        as nested MapPyGroups. If None then the current transform is
        deathed and rebirthed. 
        @throw Exceptions::Exception if there are any problems with the
        death or initialisation.
        """
        # Death current transform.
        cls.death()
        logger = logging.getLogger(__name__)
        if logger.isEnabledFor(logging.DEBUG):
            logger.debug("Initializing transform")
            logger.debug(transform_specification)
        # Set ROOT to batch mode in case certain transforms
        # use ROOT and kick up a canvas, which could give rise
        # to an exception depending on terminal window display
        # settings.
        ROOT.gROOT.SetBatch(True) # pylint: disable=E1101
        # Create new transform.
        cls.transform = \
            WorkerUtilities.create_transform(transform_specification)

    @classmethod
    def birth(cls, configuration):
        """
        Birth the transform with the JSON configuration document. The
        transform is death-ed first if not already done.
        @param cls Class reference.
        @param configuration Valid JSON configuration document.
        @throws Exception if there is a problem when birth is called.
        @throws WorkerBirthFailedException if birth returns False.
        """
        # Death current transform.
        cls.death()
        logger = logging.getLogger(__name__)
        if logger.isEnabledFor(logging.DEBUG):
            logger.debug("Birthing transform")
            logger.debug(configuration)
        # return_value can be None and it is okay
        exc_str = ""
        try:
            exc_str = cls.transform.__class__.__name__
            return_value = cls.transform.birth(configuration)
        except Exception: # pylint: disable=W0703
            return_value = False
            exc_str += " exception: "+str(sys.exc_info()[1]) 
        if return_value == False:
            raise WorkerBirthFailedException(exc_str)
        cls.is_dead = False

    @classmethod
    def process(cls, spill):
        """
        Invoke transform.process on the spill if the transform has 
        not been death-ed.
        @param cls Class reference.
        @param spill Spill.
        @return new spill.
        @throws Exception if there is a problem.
        @throws WorkerDeadException if the worker has been death-ed.
        """
        logger = logging.getLogger(__name__)
        if logger.isEnabledFor(logging.DEBUG):
            logger.debug("Processing spill")
            logger.debug(spill)
        if not cls.is_dead:
            return cls.transform.process(spill)
        else:
            raise WorkerDeadException(cls.transform.__class__)

    @classmethod
    def death(cls):
        """
        Death the current transform if it has not been done
        already. After this call MausTransform.is_dead will
        be True irrespective as to whether death was successful
        or not. The caller can decide whether to reuse the object
        or not.
        @param cls Class reference.
        @throws Exception if there is a problem when death is called.
        @throws WorkerDeathFailedException if death returns False.
        """
        logger = logging.getLogger(__name__)
        if logger.isEnabledFor(logging.DEBUG):
            logger.debug("Deathing transform")
        if not cls.is_dead:
            cls.is_dead = True
            # return_value can be None and it is okay
            exc_str = ""
            try:
                exc_str = cls.transform.__class__.__name__
                return_value = cls.transform.death()
            except Exception: # pylint: disable=W0703
                return_value = False
                exc_str += " exception: "+str(sys.exc_info()[1]) 
            if return_value == False:
                raise WorkerDeathFailedException(exc_str)
