"CalibrationSuperMouse module."

from xml.sax import parseString

from cdb._calibration import Calibration
from cdb._exceptions import CdbPermanentError

__all__ = ["CalibrationSuperMouse"]

class CalibrationSuperMouse(Calibration):
    
    """
The CalibrationSuperMouse class is used to set and retrieve calibration data.

For the control system a channel is described by its location with reference to
a crate and module. For the detectors and trackers a channel is described by its
location with reference to board, bank and channel. Each channel has data
associated with it. Old versions of the data are stored for diagnostic purposes.
Data can be retrieved for a given time using get_calibration_for_date and for a
given run using get_calibration_for_run.

    """
    
    def __init__(self, url=""):
        """
Construct a CalibrationSuperMouse.

@param url: the url of the server in the form 'http://host.domain:port'

@exception CdbPermanentError: Unable to contact CDB server or invalid URL

        """
        super(CalibrationSuperMouse, self).__init__(url,
            "/cdb/calibrationSuperMouse?wsdl")
       
    def __str__(self):
        _help_super = (
        "\n\tset_tracker(string device, dict data)")
        return "CalibrationSuperMouse" + self._help + _help_super

    def set_tracker(self, device, data):
        """
Update the calibration data for a Tracker. A timestamp is associated with the data.
@param device: the name of the tracker
@param data: a dictionary containing the calibration data set with the following keys/values:<pre>
    key - board, value - an int representing the board
    key - bank, value - an int the bank
    key - channel, value - an int representing the channel
    key - adc_pedestal, value - a float representing the adc pedestal
    key - adc_gain, value - a float representing the adc gain
    key - tdc_pedestal, value - a float representing the tdc pedestal
    key - tdc_slope, value - a float representing the tdc slope
</pre>

@return a string containing a status message

@exception CdbTemporaryError: The problem maybe transient and retrying the
request MAY succeed
@exception CdbPermanentError: Maybe due to to bad data being passed in or an
unexpected internal error

        """
        xml = "<calibrations>"        
        try:
            if data != None:
                for calibration in data:
                    xml = (xml + "<tracker board='" + str(calibration['board'])
                           + "' bank='" + str(calibration['bank'])
                           + "' channel='" + str(calibration['channel'])
                           + "' adcPedestal='" 
                           + str(calibration['adc_pedestal'])
                           + "' adcGain='" + str(calibration['adc_gain'])
                           + "' tdcPedestal='" 
                           + str(calibration['tdc_pedestal'])
                           + "' tdcSlope='" + str(calibration['tdc_slope'])
                           + "'/>")
            xml = xml + "</calibrations>"
        except KeyError, exception:
            raise CdbPermanentError("Missing value for " + str(exception))
        print xml
        return_xml = str(self._client.setTracker(str(device), str(xml)))
        parseString(return_xml, self._status_handler)
        return self._status_handler.get_message()

