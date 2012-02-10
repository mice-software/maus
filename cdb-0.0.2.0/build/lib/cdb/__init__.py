"""@package cdb
The cdb module provides the client interface to the Configuration Database
server. The client makes use of a cdb server via a web service. The address of
the cdb server is determined from the from the environment variable CDB_SERVER.
If the is not set then the property cdb.server is obtained from the
configuration file /opt/mice/etc/cdb-client/cdb.props. If this is not found
then the default http://micewww.pp.rl.ac.uk:4443 is used.

The AlarmHandler class is used to retrieve Alarm Handlers (ALH).

The AlarmHandlerSuperMouse class is used to store and retrieve Alarm Handlers
(ALH).

The Beamline class is used to retrieve data about beam line settings.

The BeamlineSuperMouse class is used to set and retrieve data about beam line
settings.

The Cabling class is used to retrieve cabling data.

The CablingSuperMouse class is used to set and retrieve cabling data.

The Calibration class is used to retrieve calibration data.

The CalibrationSuperMouse class is used to set and retrieve calibration data.

The Control class is used to retrieve control parameter values.

The ControlSuperMouse class is used to set and retrieve control parameter
values.

The Geometry class is used to retrieve a GDML data set.

The GeometrySuperMouse class is used to set and retrieve GDML data set.

The StateMachine class is used to retrieve state machine data.

The StateMachineSuperMouse class is used to set and retrieve state machine
settings.

The Target class is used to retrieve target data.

The TargetSuperMouse class is used to set and retrieve target data.
"""

from cdb._exceptions import CdbError
from cdb._exceptions import CdbPermanentError
from cdb._exceptions import CdbTemporaryError
from cdb._alarmhandler import AlarmHandler
from cdb._alarmhandler_supermouse import AlarmHandlerSuperMouse
from cdb._beamline import Beamline
from cdb._beamline_supermouse import BeamlineSuperMouse
from cdb._cabling import Cabling
from cdb._cabling_supermouse import CablingSuperMouse
from cdb._calibration import Calibration
from cdb._calibration_supermouse import CalibrationSuperMouse
from cdb._control import Control
from cdb._control_supermouse import ControlSuperMouse
from cdb._geometry import Geometry
from cdb._geometry_supermouse import GeometrySuperMouse
from cdb._statemachine import StateMachine
from cdb._statemachine_supermouse import StateMachineSuperMouse
from cdb._target import Target
from cdb._target_supermouse import TargetSuperMouse

__all__ = ["CdbError", "CdbPermanentError", "CdbTemporaryError", "AlarmHandler",
"AlarmHandlerSuperMouse", "Beamline", "BeamlineSuperMouse", "Cabling",
"CablingSuperMouse", "Calibration", "CalibrationSuperMouse", "Control",
"ControlSuperMouse", "Geometry", "GeometrySuperMouse", "StateMachine",
"StateMachineSuperMouse", "Target", "TargetSuperMouse"]

__version__ = '0.0.2.0'

