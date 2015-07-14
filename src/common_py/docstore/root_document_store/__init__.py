"""
root_document_store contains tools necessary to make a ROOT-based doc queue

* RootDocumentDB holds the actual documents stored; this provides the 
  server-side database
* RootDocumentStore provides client-side access to the DocDB
* SocketManager handles connection to and messaging between sockets 
* SocketError is thrown if SocketManager makes and error
* ControlMessage provides an application-level API for messaging between sockets
  and provides a low level messaging protocol based around ROOT data types
"""

from docstore.root_document_store._socket_manager import SocketManager
from docstore.root_document_store._socket_error import SocketError
from docstore.root_document_store._control_message import ControlMessage
from docstore.root_document_store._root_document_db import RootDocumentDB
from docstore.root_document_store._root_document_store import RootDocumentStore

__all__ = [SocketManager, SocketError, ControlMessage, RootDocumentDB,
       RootDocumentStore]
