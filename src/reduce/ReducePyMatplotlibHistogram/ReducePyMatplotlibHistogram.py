"""
PyMatplotlibHistogram creates histograms of data within spills.
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

import datetime
import json
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure
import os
import uuid

class ReducePyMatplotlibHistogram:
    """
    @class ReducePyMatplotlibHistogram.PyMatplotlibHistogram
    PyMatplotlibHistogram creates histograms of data within spills.

    It extracts the tdc_counts and adc_counts for each digit within a
    spill and plots these, then saves the histogram.

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
        self._image_type = "eps"
        self._file_prefix = ""
        self._directory = os.getcwd()
        self._file_count = 0

    def birth(self, config_json):
        """
        Configure reducer from data cards.
        @param self Object reference.
        @param config_json JSON document string.
        @returns True
        """
        config_doc = json.loads(config_json)
        # Create a FigureCanvas so we can validate the image type.
        figure = Figure()
        canvas = FigureCanvas(figure)

        key = "histogram_image_type"
        if key in config_doc:
            self._image_type = config_doc[key]
        else:
            self._image_type = "eps"
        print "Histogram image type: %s" % self._image_type

        if self._image_type not in canvas.get_supported_filetypes().keys():
            error = "Unsupported histogram image type: %s Expect one of %s" % (
                    self._image_type, 
                    canvas.get_supported_filetypes().keys())
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
        # Number of files saved so far.
        self._file_count = 0
        return True

    def process(self, ignore, json_string):
        """
        Create histogram from data.
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
        # TODO alternatively filter and TDC/ADC count extraction in one
        # list traversal? Otherwise, digit index may get out of synch.
        trackerdigits = \
            [digit for digit in digits if self.filter_trackers(digit)]
        print "Number of tracker digits: %d" % len(trackerdigits)
        tdc_counts = [self.get_counts(digit, 'tdc_counts') 
                      for digit in trackerdigits]
        print "tdc_counts: %s" % tdc_counts
        adc_counts = [self.get_counts(digit, 'adc_counts') 
                      for digit in trackerdigits]
        print "adc_counts: %s" % adc_counts
        canvas = self.histogram(tdc_counts, adc_counts)
        self.save_histogram(canvas)
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

    def histogram(self, tdc_counts, adc_counts): 
        """
        Create a histogram with the given TDC and ADC counts.
        @param self Object reference.
        @param tdc_counts List of TDC counts.
        @param adc_counts List of ADC counts.
        @returns matplotlib FigureCanvas with histogram.
        """
        print "About to histogram..."
        figure = Figure(figsize=(6, 6))
        canvas = FigureCanvas(figure)
        axes = figure.add_subplot(111)
        axes.set_title('TDC and ADC counts', fontsize=14)
        axes.set_xlabel('Digit', fontsize=12)
        axes.set_ylabel('Count', fontsize=12)
        axes.grid(True, linestyle='-', color='0.75')
#        if (len(tdc_counts) > 0):
#            axes.scatter(range(0, len(tdc_counts)), tdc_counts, 10, \
#                'r', label='TDC counts')
#        if (len(adc_counts) > 0):
#            axes.scatter(range(0, len(adc_counts)), adc_counts, 10, \
#                'b', label='ADC counts')        
        axes.plot(tdc_counts, 'r', label='TDC counts') 
        axes.plot(adc_counts, 'b', label='ADC counts') 
        axes.legend()
        print "Histogrammed"
        return canvas

    def save_histogram(self, canvas):
        """
        Save histogram.
        @param self Object reference.
        @param canvas matplotlib FigureCanvas with histogram.
        """
        # Create a unique file name based on the prefix, current date
        # and the count. Use the date since strftime doesn't (yet)
        # support milli-seconds.
        file_name = "%s%s%d.%s" % (
            self._file_prefix, 
            datetime.datetime.now().strftime("%Y%m%d%H%M%S"), 
            self._file_count,
            self._image_type)
        file_path = os.path.join(self._directory, file_name)
        print "Saving in " + file_path
        # File extension determines format. Default if omitted is .png.
        canvas.print_figure(file_path, dpi=500)
        self._file_count += 1
        print "Saved!"
