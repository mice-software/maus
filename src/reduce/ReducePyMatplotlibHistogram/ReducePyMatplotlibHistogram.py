"""
PyMatplotlibHistogram creates histograms of TDC and ADC counts within
spills and histograms summarising all spills to date.
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

class ReducePyMatplotlibHistogram:
    """
    @class ReducePyMatplotlibHistogram.PyMatplotlibHistogram
    PyMatplotlibHistogram maintains a histogram of the TDC and
    ADC counts for all spills processed to date. It uses the 
    "adc_counts" and "tdc_counts" for each digit within a "digits" 
    list. For each spill input it outputs a new histogram.

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
        self.image_type = "eps"
        self.spill_count = 0
        self.__max_adc_count = 1
        self.__max_tdc_count = 1
        self.auto_number = False
        self.__histogram = None

    def birth(self, config_json):
        """
        Configure worker from data cards. If "image_type" is not
        in those supported then a ValueError is thrown.
        @param self Object reference.
        @param config_json JSON document string.
        @returns True
        """
        # Create a FigureCanvas for the summary histogram.
        self.__histogram = self.__create_histogram(
            "Total TDC and ADC counts to spill %d" % self.spill_count,
            "TDC count", "ADC count")

        # Configure the worker.
        config_doc = json.loads(config_json)

        key = "histogram_auto_number"
        if key in config_doc:
            self.auto_number = config_doc[key]

        key = "histogram_image_type"
        if key in config_doc:
            self.image_type = config_doc[key]
        else:
            self.image_type = "eps"

        if self.image_type not in \
            self.__histogram.get_supported_filetypes().keys():
            error = "Unsupported histogram image type: %s Expect one of %s" \
                    % (
                    self.image_type, 
                    self.__histogram.get_supported_filetypes().keys())
            raise ValueError(error)

        self.spill_count = 0
        self.__max_adc_count = 1
        self.__max_tdc_count = 1
        return True

    def process(self, json_string):
        """
        Update the histogram with data from the current spill
        and output the histogram.        
        @param self Object reference.
        @param json_string String with current JSON document.
        @returns JSON document containing current histogram.
        """
        try:
            json_doc = json.loads(json_string.rstrip())
        except ValueError:
            json_doc = {"errors": {"bad_json_document":
                                "unable to do json.loads on input"} }
            return json.dumps(json_doc)

        if "digits" not in json_doc:
            if "errors" not in json_doc:
                json_doc["errors"] = {}
            json_doc["errors"]["no_digits"] = "no digits"
            return json.dumps(json_doc)
        digits = json_doc["digits"]

        # Extract just those that are for the Tracker.
        trackerdigits = \
            [digit for digit in digits if self.__filter_trackers(digit)]

        # Get the TDC and ADC counts.    
        tdc_counts = [self.__get_counts(digit, "tdc_counts") 
                      for digit in trackerdigits]
        adc_counts = [self.__get_counts(digit, "adc_counts") 
                      for digit in trackerdigits]
        # Calculate maximums for axis rescaling.
        spill_max_tdc_count = 0
        if (len(tdc_counts) > 0):
            spill_max_tdc_count = max(tdc_counts)
        self.__max_tdc_count = max(self.__max_tdc_count, spill_max_tdc_count)
        spill_max_adc_count = 0
        if (len(adc_counts) > 0):
            spill_max_adc_count = max(adc_counts)
        self.__max_adc_count = max(self.__max_adc_count, spill_max_adc_count)

        json_doc = {}
        json_doc["image"] = {}
        if (self.auto_number):
            tag = "tdcadc%d" % self.spill_count
        else:
            tag = "tdcadc"
        histogram_title = "Total TDC and ADC counts to spill %d" \
            % self.spill_count
        self.__plot(self.__histogram,  histogram_title,
                    tdc_counts, adc_counts)
        # Rescale axis so 0 is always visible.
        # +0.5 are fudge factors to avoid matplotlib warning about
        # "Attempting to set identical bottom==top" which arises if
        # the axes are set to be exactly the maximum of the data.
        self.__histogram.figure.get_axes()[0].set_xlim( \
            [0, self.__max_tdc_count + 0.5])
        self.__histogram.figure.get_axes()[0].set_ylim( \
            [0, self.__max_adc_count + 0.5])
        data = self.__convert_to_binary(self.__histogram)
        json_doc["image"]["content"] = histogram_title
        json_doc["image"]["tag"] = tag
        json_doc["image"]["image_type"] = self.image_type
        json_doc["image"]["data"] = data
        self.spill_count += 1
        return json.dumps(json_doc)

    def death(self): #pylint: disable=R0201
        """
        A no-op
        @returns True
        """
        return True

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

    def __create_histogram(self, title, xlabel, ylabel): #pylint: disable=R0201
        """
        Create a histogram with the given title and labels.
        @param self Object reference.
        @param title Title.
        @param xlabel X axis label.
        @param ylabel Y axis label.
        @returns matplotlib FigureCanvas representing the histogram.
        """
        figure = Figure(figsize=(6, 6))
        histogram = FigureCanvas(figure)
        axes = figure.add_subplot(111)
        axes.set_title(title, fontsize=14)
        axes.set_xlabel(xlabel, fontsize=12)
        axes.set_ylabel(ylabel, fontsize=12)
        axes.grid(True, linestyle="-", color="0.75")
        return histogram

    def __plot(self, histogram, title, tdcs, adcs): #pylint: disable=R0201
        """
        Plot the TDC/ADC counts on the histogram.
        @param self Object reference.
        @param histogram matplotlib FigureCanvas representing the histogram.
        @param title Title.
        @param tdcs List of TDC counts.
        @param adcs List of ADC counts.
        """
        histogram.figure.get_axes()[0].set_title(title, fontsize=14)
        if (len(tdcs) > 0):
            histogram.figure.get_axes()[0].scatter(tdcs, 
                                                   adcs, 10, "b")

    def __convert_to_binary(self, histogram): #pylint: disable=R0201
        """
        Convert histogram to binary format.
        @param self Object reference.
        @param histogram matplotlib FigureCanvas representing the histogram.
        @returns representation of histogram in base 64-encoded image 
        type format.
        """
        data_file = StringIO.StringIO() 
        histogram.print_figure(data_file, dpi=500, format=self.image_type)
        data_file.seek(0)
        data = data_file.read()
        return base64.b64encode(data)
