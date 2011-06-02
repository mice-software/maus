"""@package cdb
The cdb module provides the client interface to the Configuration Database
server. The client makes use of a cdb server via a web service. The address of
the cdb server is determined from the from the environment variable CDB_SERVER.
If the is not set then the property cdb.server is obtained from the
configuration file /opt/mice/etc/cdb-client/cdb.props. If this is not foundA
then the default http://micewww.pp.rl.ac.uk:4443 is used.

The AlarmHandler class is used to retrieve Alarm Handlers (ALH).

The Beamline class is used to retrieve data about beam line settings.

The Control class is used to retrieve control parameter values.

The Geometry class is used to retrieve a GDML data set.

"""

from cdb._exceptions import CdbError
from cdb._exceptions import CdbPermanentError
from cdb._exceptions import CdbTemporaryError
from cdb._alarmhandler import AlarmHandler
from cdb._beamline import Beamline
from cdb._control import Control
from cdb._geometry import Geometry

__all__ = ["CdbError", "CdbPermanentError", "CdbTemporaryError", "AlarmHandler",
"Beamline", "Control", "Geometry"]

