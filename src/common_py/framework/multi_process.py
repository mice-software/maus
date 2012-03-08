"""
Multi-process dataflows module.
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

import json

from framework.input_transform import InputTransformExecutor
from framework.merge_output import MergeOutputExecutor
from framework.utilities import DocumentStoreUtilities

class MultiProcessExecutor:
    """
    @class MultiProcessExecutor
    Execute MAUS dataflows using a Celery distributed task queue and
    nodes and a document store to cache spills after being
    output from transformers, before they are consumed by mergers. 

    This class expects a document store class to be specified in
    the JSON configuration e.g.
    @verbatim
    doc_store_class = "MongoDBDocumentStore.MongoDBDocumentStore"
    @endverbatim
    The document store class itself may have additional configuration
    requirements.  
    """

    def __init__(self, inputer, transformer, merger, outputer, config_doc): # pylint: disable=R0913,C0301
        """
        Save references to arguments and parse the JSON configuration
        document, then connect to the document store. 
        @param self Object reference.
        @param inputer Input task.
        @param transformer Transformer task.
        @param merger Merger task.
        @param outputer Output task.
        @param config_doc JSON configuration document.
        @throws KeyError If any required configuration parameters are
        missing.
        @throws ValueError If any configuration values are invalid.
        @throws TypeError If any dynamic class does not implement
        a required interface.
        @throws DocumentStoreException if there is a problem
        connecting to the document store.
        """
        configuration = json.loads(config_doc)
        self.doc_store = \
            DocumentStoreUtilities.setup_doc_store(configuration)
        self.input_transform = InputTransformExecutor( \
           inputer, transformer, config_doc, self.doc_store)
        self.merge_output = MergeOutputExecutor( \
           merger, outputer, config_doc, self.doc_store,
           self.input_transform.collection)
  
    def execute(self):
        """
        Execute the dataflow - delegate to
        InputTransfomExecutor.execute and
        MergeOutputExecutor.execute. 
         @param self Object reference.
        @throws RabbitMQException if RabbitMQ cannot be contacted.
        @throws NoCeleryNodeException if no Celery nodes.
        @throws CeleryNodeException if Celery nodes fail to 
        configure, birth or death.
        @throws DocumentStoreException if there is a problem
        using the document store.
        """
        self.input_transform.execute()
        self.merge_output.execute()

    @staticmethod
    def get_dataflow_description():
        """
        Get dataflow description.
        @return description.
        """
        description = "Run MAUS in multi-processing mode. This\n"  
        description += "requires Celery and MongoDB to be installed. See\n" 
        description += "the wiki links on how to do this at\n"
        description += \
            "http://micewww.pp.rl.ac.uk/projects/maus/wiki/MAUSDevs\n"
        return description
