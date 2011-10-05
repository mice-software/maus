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

import json
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure
import StringIO

class ReducePyMatplotlibHistogram:
    """
    @class ReducePyMatplotlibHistogram.PyMatplotlibHistogram
    PyMatplotlibHistogram creates histograms of TDC and ADC counts
    within spills and a summary histogram for all spills processed.
  
    It uses the "adc_counts" and "tdc_counts" for each digit within a
    "digits" list. 

    The caller can configure the worker and specify:

    -Image type ("histogram_image_type"). Must be one of those
     supported by matplot lib (currently "svg", "ps", "emf", "rgba",
     "raw", "svgz", "pdf", "eps", "png"). Default: "eps".

    The output is a sequence of JSON documents separated by line
    breaks e.g.:

    @verbatim
    {"images": [{"content":"TDC and ADC counts for spill 2",
                 "tag": "spill2",
                 "image_type": "eps", 
                 "data": "..."},
                {"content":"Total TDC and ADC counts to spill 2",
                 "tag": "spills2",
                 "image_type": "eps", 
                 "data": "..." }]}
    @endverbatim

    The "tag" allows the types of histogram to be discriminated. The
    N (e.g. 2) means that the histograms were produced during processing
    of the Nth spill (N = 0..)

    In case of errors the output document is just the input document
    with an "errors" field containing the error e.g.

    @verbatim
    {"errors": {..., "bad_json_document": "unable to do json.loads on input"}}
    {"errors": {..., "no_digits": "no digits"}}
    @endverbatim
    """

    def __init__(self):
        self.image_type = "eps"
        self.spill_count = 0
        self.__max_adc_count = 1
        self.__max_tdc_count = 1
        self.__summary_histogram = None

    def birth(self, config_json):
        """
        Configure worker from data cards. If "image_type" is not
        in those supported then a ValueError is thrown.
        @param self Object reference.
        @param config_json JSON document string.
        @returns True
        """
        # Create a FigureCanvas for the summary histogram.
        self.__summary_histogram = self.__create_histogram(
            "Total TDC and ADC counts to spill %d" % self.spill_count,
            "TDC count", "ADC count")

        # Configure the worker.
        config_doc = json.loads(config_json)

        key = "histogram_image_type"
        if key in config_doc:
            self.image_type = config_doc[key]
        else:
            self.image_type = "eps"
        print "Histogram image type: %s" % self.image_type

        if self.image_type not in \
            self.__summary_histogram.get_supported_filetypes().keys():
            error = "Unsupported histogram image type: %s Expect one of %s" \
                    % (
                    self.image_type, 
                    self.__summary_histogram.get_supported_filetypes().keys())
            print error
            raise ValueError(error)

        self.spill_count = 0
        self.__max_adc_count = 1
        self.__max_tdc_count = 1
        return True

    def process(self, json_strings, json_string):
        """
        Create histogram from data and update summary histogram.
        @param self Object reference.
        @param json_strings String with JSON documents handled to date.
        @param json_string String with current JSON document.
        @returns String of JSON documents.
        """
        try:
            json_doc = json.loads(json_string.rstrip())
        except ValueError:
            print "Bad JSON document"
            json_doc = {"errors": {"bad_json_document":
                                "unable to do json.loads on input"} }
            return self.__return_json(json_strings, json.dumps(json_doc))
        if "digits" not in json_doc:
            print "No digits in JSON document"
            if "errors" not in json_doc:
                json_doc["errors"] = {}
            json_doc["errors"]["no_digits"] = "no digits"
            return self.__return_json(json_strings, json.dumps(json_doc))
        digits = json_doc["digits"]
        print "Number of digits: %d" % len(digits)

        # Extract just those that are for the Tracker.
        trackerdigits = \
            [digit for digit in digits if self.__filter_trackers(digit)]
        print "Number of tracker digits: %d" % len(trackerdigits)

        # Get the TDC and ADC counts.    
        tdc_counts = [self.__get_counts(digit, "tdc_counts") 
                      for digit in trackerdigits]
        print "tdc_counts: %s" % tdc_counts
        adc_counts = [self.__get_counts(digit, "adc_counts") 
                      for digit in trackerdigits]
        print "adc_counts: %s" % adc_counts
        # Calculate maximums for axis rescaling.
        spill_max_tdc_count = 0
        if (len(tdc_counts) > 0):
            spill_max_tdc_count = max(tdc_counts)
        self.__max_tdc_count = max(self.__max_tdc_count, spill_max_tdc_count)
        spill_max_adc_count = 0
        if (len(adc_counts) > 0):
            spill_max_adc_count = max(adc_counts)
        self.__max_adc_count = max(self.__max_adc_count, spill_max_adc_count)

        # Create a FigureCanvas for a histogram for the current spill.
        histogram = self.__create_histogram(
            "TDC and ADC counts for spill %d" % self.spill_count,
            "TDC count", "ADC count")

        json_doc = {}
        json_histograms = []
        json_doc["images"] = json_histograms

        histogram_title = "TDC and ADC counts for spill %d" \
                           % self.spill_count
        self.__histogram(histogram,  histogram_title,
                         tdc_counts, adc_counts)
        # Rescale axis so 0 is always visible.
        histogram.figure.get_axes()[0].set_xlim([0, spill_max_tdc_count])
        histogram.figure.get_axes()[0].set_ylim([0, spill_max_adc_count])

        data = self.__convert_to_binary(histogram)
        json_histograms.append({"content": histogram_title,
                                "tag": "%dspill" % self.spill_count,
                                "image_type": self.image_type,
                                "data": data})

        histogram_title = "Total TDC and ADC counts to spill %d" \
                          % self.spill_count
        self.__histogram(self.__summary_histogram,  histogram_title,
                         tdc_counts, adc_counts)
        # Rescale axis so 0 is always visible.
        self.__summary_histogram.figure.get_axes()[0].set_xlim( \
            [0, self.__max_tdc_count])
        self.__summary_histogram.figure.get_axes()[0].set_ylim( \
            [0, self.__max_adc_count])
        data = self.__convert_to_binary(self.__summary_histogram)

        json_histograms.append({"content": histogram_title,
                                "tag": "%dspills" % self.spill_count,
                                "image_type": self.image_type,
                                "data": data})

        self.spill_count += 1
        return self.__return_json(json_strings, json.dumps(json_doc))

    def death(self):
        """
        A no-op
        @returns True
        """
        return True

    def __return_json(self, json_strings, json_string):
        """
        Add JSON document to list of documents
        @param json_strings String with JSON documents.
        @param json_string String with current JSON document.
        @returns String of JSON documents with a line break and the 
        current one appended.
        """
        return "%s\n%s" % (json_strings.rstrip(), json_string.rstrip())

    def __filter_trackers(self, digit):
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

    def __get_counts(self, digit, digit_key):
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

    def __create_histogram(self, title, xlabel, ylabel):
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

    def __histogram(self, histogram, title, tdc_counts, adc_counts): 
        """
        Plot the TDC/ADC counts on the histogram.
        @param self Object reference.
        @param histogram matplotlib FigureCanvas representing the histogram.
        @param title Title.
        @param tdc_counts List of TDC counts.
        @param adc_counts List of ADC counts.
        """
        print "About to histogram %s..." % title
        histogram.figure.get_axes()[0].set_title(title, fontsize=14)
        if (len(tdc_counts) > 0):
            histogram.figure.get_axes()[0].scatter(tdc_counts, 
                                                   adc_counts, 10, "b")

    def __convert_to_binary(self, histogram): 
        """
        Convert histogram to binary format.
        @param self Object reference.
        @param histogram matplotlib FigureCanvas representing the histogram.
        @returns representation of histogram in image type format.
        """
        print "Converting to binary format..."
        data_file = StringIO.StringIO() 
        histogram.print_figure(data_file, dpi=500, format=self.image_type)
        data_file.seek(0)
        data = data_file.read()
        print "Converted"
        return data
