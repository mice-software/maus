"""
ReducePyHistogramTDCADCCounts manages a histogram of TDC and ADC
counts within spills, updating the histogram as subsequent
spills are processed.
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

import base64
import json
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure
import StringIO

from ReducePyMatplotlibHistogram import ReducePyMatplotlibHistogram

class ReducePyHistogramTDCADCCounts(ReducePyMatplotlibHistogram):
    """
    @class ReducePymatplotlibHistogram.PyHistogramTDCADCCounts
    maintains a histogram of the TDC and ADC counts for all spills
    processed to date. It uses the "adc_counts" and "tdc_counts" for
    each digit within a "digits" list. For each spill input it outputs
    a new histogram. 

    The histogram output is a JSON document of form:

    @verbatim
    {"image": {"content":"Total TDC and ADC counts to spill 2",
               "tag": TAG,
               "image_type": "eps", 
               "data": "...base 64 encoded image..."}}
    @endverbatim

    where "TAG" is "tdcadc" if "histogram_auto_number" (see below)
    is "false" or is "tdcadcN" if "histogram_auto_number" is
    "true", where N means this is the (N + 1)th spill processed
    by the worker.

    "histogram_summary_only" is false:

    In case of errors the output document is just the input document
    with an "errors" field containing the error e.g.

    @verbatim
    {"errors": {..., "bad_json_document": "unable to do json.loads on input"}}
    {"errors": {..., "no_digits": "no digits"}}
    @endverbatim

    The caller can configure the worker and specify:

    -Image type ("histogram_image_type"). Must be one of those
     supported by matplot lib (currently "svg", "ps", "emf", "rgba",
     "raw", "svgz", "pdf", "eps", "png"). Default: "eps".
    -Auto-number ("histogram_auto_number"). Default: false.
     If "true" then each output image tag has an auto-number appended
     e.g. "tdcadc0"..."tdcadcN" and "tdcadc1...tdcadcN" where N is the
     (N + 1)th spill processed. If "false" then there is no 
     auto-numbering of tags. 
    """

    def __init__(self):
        """
        Set initial attribute values.
        @param self Object reference.
        """
        ReducePyMatplotlibHistogram.__init__(self)
        self.__max_adc_count = 1
        self.__max_tdc_count = 1
        self._tag = "tdcadc"

    def birth(self, config_json):
        """
        Configure worker from data cards.
        @param self Object reference.
        @param config_doc JSON document.
        @returns True if successful, False otherwise.
        """
        # Invoke super-class default_birth method to perform
        # general configuration.
        # default_birth returns the JSON document with config_json
        # if this is needed to extract sub-class-specific
        # configuration. 
        ReducePyMatplotlibHistogram._default_birth(self, config_json)
        self.__max_adc_count = 1
        self.__max_tdc_count = 1
        self._histogram.figure.get_axes()[0].set_xlabel("TDC count", fontsize=12)
        self._histogram.figure.get_axes()[0].set_ylabel("ADC count", fontsize=12)
        return True

    def process(self, json_string):
        """
        Update the histogram with data from the current spill
        and output the histogram.        
        @param self Object reference.
        @param json_string String with current JSON document.
        @returns JSON document containing current histogram.
        """

        # Load and validate the JSON document.
        try:
            json_doc = json.loads(json_string.rstrip())
        except ValueError:
            json_doc = {"errors": {"bad_json_document":
                                "unable to do json.loads on input"} }
            return json.dumps(json_doc)

        # Check the data of interest is in the spill.
        if "digits" not in json_doc:
            if "errors" not in json_doc:
                json_doc["errors"] = {}
            json_doc["errors"]["no_digits"] = "no digits"
            return json.dumps(json_doc)
        digits = json_doc["digits"]

        # Extract just those that are for the Tracker.
        trackerdigits = \
            [digit for digit in digits if self.__filter_trackers(digit)]
        # Get the lists of TDC and ADC counts.    
        tdcs = [self.__get_counts(digit, "tdc_counts") 
                for digit in trackerdigits]
        adcs = [self.__get_counts(digit, "adc_counts") 
                for digit in trackerdigits]

        # Calculate maximums for axis rescaling.
        spill_max_tdc_count = 0
        if (len(tdcs) > 0):
            spill_max_tdc_count = max(tdcs)
        self.__max_tdc_count = max(self.__max_tdc_count, spill_max_tdc_count)
        spill_max_adc_count = 0
        if (len(adcs) > 0):
            spill_max_adc_count = max(adcs)
        self.__max_adc_count = max(self.__max_adc_count, spill_max_adc_count)

        # Set content/title.
        content = "Total TDC and ADC counts to spill %d" % self.spill_count

        # Plot the data.
        self._histogram.figure.get_axes()[0].set_title(content, fontsize=14)
        if (len(tdcs) > 0):
            self._histogram.figure.get_axes()[0].scatter(
                tdcs, adcs, 10, "b")

        # Rescale axis so 0 is always visible.
        # +0.5 are fudge factors to avoid matplotlib warning about
        # "Attempting to set identical bottom==top" which arises if
        # the axes are set to be exactly the maximum of the data.
        self._histogram.figure.get_axes()[0].set_xlim( \
            [0, self.__max_tdc_count + 0.5])
        self._histogram.figure.get_axes()[0].set_ylim( \
            [0, self.__max_adc_count + 0.5])

        return ReducePyMatplotlibHistogram._create_image_json(
            self, content)

    def __filter_trackers(self, digit): #pylint: disable=R0201
        """
        Is the digit a tracker?
        @param self Object reference.
        @param digit "digits" list member (a dictionary).
        @returns True if digit is a Tracker, False otherwise.
        """
        if "channel_id" not in digit:
            return False
        if "type" not in digit["channel_id"]:
            return False
        else:
            return digit["channel_id"]["type"] == "Tracker"

    def __get_counts(self, digit, digit_key): #pylint: disable=R0201
        """
        Return number of counts of the given type in digit.
        @param self Object reference.
        @param digit "digits" list member (a dictionary).
        @param digit_type Either "adc_counts" or "tdc_counts".
        @returns Count
        """
        if digit_key in digit:
            return digit[digit_key]
        else:
            return 0
