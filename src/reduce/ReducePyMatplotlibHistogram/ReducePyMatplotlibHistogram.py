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
import os
import uuid

class ReducePyMatplotlibHistogram:
    """
    @class ReducePyMatplotlibHistogram.PyMatplotlibHistogram
    PyMatplotlibHistogram creates histograms of TDC and ADC counts
    within spills and histograms summarising all spills to date.
  
    It plots adc_counts against tdc_counts for each digit within a
    spill, then saves the histogram. It also maintains a histogram
    that aggregates the data from all spills to date.

    The caller can configure the reducer and specify.
    -Image type ("histogram_image_type"). Must be one of those
     supported by matplot lib (currently 'svg', 'ps', 'emf', 'rgba',
     'raw', 'svgz', 'pdf', 'eps', 'png'). Default: 'eps'.
    -File prefix ("histogram_file_prefix"). Default: auto-generated UUID.
    -Directory for files ("histogram_directory"). Default: current
     working directory. If the given directory does not exist it
     will be created.
    The output is a string of JSON documents read to date and
    separated by line breaks.
    """

    def __init__(self):
        self._summary_histogram = None
        self._image_type = "eps"
        self._file_prefix = ""
        self._directory = os.getcwd()
        self._spill_count = 0

    def birth(self, config_json):
        """
        Configure reducer from data cards.
        @param self Object reference.
        @param config_json JSON document string.
        @returns True
        """
        # Create a FigureCanvas for the summary histogram.
        self._summary_histogram = self.create_histogram(
            "Total TDC and ADC counts to spill %d" % self._spill_count,
            'TDC count', 'ADC count')

        # Configure the worker.
        config_doc = json.loads(config_json)

        key = "histogram_image_type"
        if key in config_doc:
            self._image_type = config_doc[key]
        else:
            self._image_type = "eps"
        print "Histogram image type: %s" % self._image_type

        if self._image_type not in \
            self._summary_histogram.get_supported_filetypes().keys():
            error = "Unsupported histogram image type: %s Expect one of %s" \
                    % (
                    self._image_type, 
                    self._summary_histogram.get_supported_filetypes().keys())
            print error
            raise ValueError(error)

        key = "histogram_file_prefix"
        if key in config_doc:
            self._file_prefix = config_doc[key]
        else:
            self._file_prefix = uuid.uuid4()
        print "Histogram file prefix: %s" % self._file_prefix

        key = "histogram_directory"
        if key in config_doc:
            self._directory = config_doc[key]
            if not os.path.exists(self._directory): 
                os.makedirs(self._directory) 
        else:
            self._directory = os.getcwd()
        print "Histogram directory: %s" % self._directory

        self._spill_count = 0
        return True

    def process(self, ignore, json_string):
        """
        Create histogram from data and update summary histogram.
        @param self Object reference.
        @param ignore String with JSON documents.
        @param json_string String with current JSON document.
        @returns String of JSON documents with a line break and the 
        current one appended.
        """
        try:
            json_doc = json.loads(json_string.rstrip())
        except ValueError:
            print "Bad JSON document"
            json_doc = {"errors": {"bad_json_document":
                                "unable to do json.loads on input"} }
            return self.return_json(ignore, json.dumps(json_doc))        
        if "digits" not in json_doc:
            print "No digits in JSON document"
            if 'errors' not in json_doc:
                json_doc['errors'] = {}
            json_doc['errors']['no digits'] = 'no digits'
            return self.return_json(ignore, json.dumps(json_doc))
        digits = json_doc['digits']
        print "Number of digits: %d" % len(digits)

        # Extract just those that are for the Tracker.
        trackerdigits = \
            [digit for digit in digits if self.filter_trackers(digit)]
        print "Number of tracker digits: %d" % len(trackerdigits)
        # Get the TDC and ADC counts.    
        tdc_counts = [self.get_counts(digit, 'tdc_counts') 
                      for digit in trackerdigits]
        print "tdc_counts: %s" % tdc_counts
        adc_counts = [self.get_counts(digit, 'adc_counts') 
                      for digit in trackerdigits]
        print "adc_counts: %s" % adc_counts
   
        # Create a FigureCanvas for a histogram for the current spill.
        histogram = self.create_histogram(
            "TDC and ADC counts for spill %d" % self._spill_count,
            'TDC count', 'ADC count')

        if (len(tdc_counts) > 0):
            self.histogram(histogram, 
                           "TDC and ADC counts for spill %d" \
                           % self._spill_count,
                           tdc_counts, adc_counts)
            self.histogram(self._summary_histogram, 
                           "Total TDC and ADC counts to spill %d" \
                           % self._spill_count,
                           tdc_counts, adc_counts)

        self.save_histogram(histogram, "spill")
        self.save_histogram(self._summary_histogram, "spills")
        self._spill_count += 1
        return self.return_json(ignore, json_string)

    def death(self):
        """
        A no-op
        @returns True
        """
        return True

    def return_json(self, json_strings, json_string):
        """
        Add JSON document to list of documents
        @param json_strings String with JSON documents.
        @param json_string String with current JSON document.
        @returns String of JSON documents with a line break and the 
        current one appended.
        """
        # Replicate ReducePyDoNothing for now.
        return "%s\n%s" % (json_strings.rstrip(), json_string.rstrip())

    def filter_trackers(self, digit):
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
            return digit['channel_id']['type'] == 'Tracker'

    def get_counts(self, digit, digit_key):
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

    def create_histogram(self, title, xlabel, ylabel):
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
        axes.grid(True, linestyle='-', color='0.75')
        return histogram

    def histogram(self, histogram, title, tdc_counts, adc_counts): 
        """
        Plot the TDC/ADC counts on the histogram.
        @param self Object reference.
        @param histogram matplotlib FigureCanvas representing the histogram.
        @param title Title.
        @param tdc_counts List of TDC counts.
        @param adc_counts List of ADC counts.
        """
        print "About to histogram..."
        histogram.figure.get_axes()[0].set_title(title, fontsize=14)
        histogram.figure.get_axes()[0].scatter(tdc_counts, adc_counts, 10, 'b')
        print "Histogrammed"

    def save_histogram(self, histogram, histogram_type):
        """
        Save histogram in current directory, in file named by file
        name prefix, spill count, histogram type and image type.
        @param self Object reference.
        @param histogram matplotlib FigureCanvas representing the histogram.
        @param histogram_type String containing histogram type
        (e.g. "spill" or "spills") to add to file name.
        """
        file_name = "%s%s%s.%s" % (
            self._file_prefix, 
            self._spill_count,
            histogram_type,
            self._image_type)
        file_path = os.path.join(self._directory, file_name)
        print "Saving in " + file_path
        # File extension determines format. Default if omitted is .png.
        histogram.print_figure(file_path, dpi=500)
        print "Saved!"
