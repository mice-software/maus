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

## @class ReducePyMatplotlibHistogram.PyMatplotlibHistogram
# PyMatplotlibHistogram creates histograms of data within spills.
#
# PyMatplotlibHistogram extracts the tdc_counts and adc_counts for
# each digit within a spill and plots these. The histogram is saved
# in the current directory with either a user-specified or UUID
# prefix. The user can specify the image file type - conditional
# on those supported by matplot lib (currently 
# 'svg', 'ps', 'emf', 'rgba', 'raw', 'svgz', 'pdf', 'eps', 'png').
# 
# The output JSON document is their concatenation with a line break
# between them.

import datetime
import json
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib.mlab as mlab
import os
import uuid

class ReducePyMatplotlibHistogram:

    def birth(self, configJson):
        configDoc = json.loads(configJson)
	# Create a FigureCanvas so we can validate the image type.
        figure = Figure()
        canvas = FigureCanvas(figure)

        key = "histogram_image_type"
        if key in configDoc:
            self._imageType = configDoc[key]
	else:
            self._imageType = "eps"
        print "Histogram image type: %s" % self._imageType

        if self._imageType not in canvas.get_supported_filetypes().keys():
            error = "Unsupported histogram image type: %s Expect one of %s" % (
                    self._imageType, 
                    canvas.get_supported_filetypes().keys())
            print error
            raise ValueError(error)

        key = "histogram_file_prefix"
        if key in configDoc:
            self._filePrefix = configDoc[key]
	else:
            self._filePrefix = uuid.uuid4()
        print "Histogram file prefix: %s" % self._filePrefix

        key = "histogram_directory"
        if key in configDoc:
            self._directory = configDoc[key]
            if not os.path.exists(self._directory): 
                os.makedirs(self._directory) 
	else:
            self._directory = os.getcwd()
        print "Histogram directory: %s" % self._directory
        # Number of files saved so far.
        self._fileCount = 0
        return True

    def process(self, ignore, jsonString):
        try:
            jsonDoc = json.loads(jsonString.rstrip())
        except ValueError:
            print "Bad JSON document"
            jsonDoc = {"errors": {"bad_json_document":
                                "unable to do json.loads on input"} }
            return self.returnJSON(ignore, json.dumps(jsonDoc))        
        if "digits" not in jsonDoc:
            print "No digits in JSON document"
            if 'errors' not in jsonDoc:
                jsonDoc['errors'] = {}
            jsonDoc['errors']['no digits'] = 'no digits'
            return self.returnJSON(ignore, json.dumps(jsonDoc))
        digits = jsonDoc['digits']
        print "Number of digits: %d" % len(digits)
        # TODO alternatively filter and TDC/ADC count extraction in one
        # list traversal? Otherwise, digit index may get out of synch.
        trackerdigits = filter(self.filtertrackers, digits)
        print "Number of tracker digits: %d" % len(trackerdigits)
	tdc_counts = [self.get_counts(digit, 'tdc_counts') 
                      for digit in trackerdigits]
        print "tdc_counts: %s" % tdc_counts
	adc_counts = [self.get_counts(digit, 'adc_counts') 
                      for digit in trackerdigits]
        print "adc_counts: %s" % adc_counts
        canvas = self.histogram(tdc_counts, adc_counts)
        self.saveHistogram(canvas)
        return self.returnJSON(ignore, jsonString)

    def death(self):
        return True

    def returnJSON(self, jsonStrings, jsonString):
	# Replicate ReducePyDoNothing for now.
        return "%s\n%s" % (jsonStrings.rstrip(), jsonString.rstrip())

    def filtertrackers(self, digit):
        if "channel_id" not in digit:
            return False
        if "type" not in digit["channel_id"]:
            return False
        else:
            return digit['channel_id']['type'] == 'Tracker'

    def get_counts(self, digit, type):
        if type in digit:
            return digit[type]
        else:
            return 0

    def histogram(self, tdc_counts, adc_counts): 
        print "About to histogram..."
        figure = Figure(figsize=(6, 6))
        canvas = FigureCanvas(figure)
        axes = figure.add_subplot(111)
        axes.set_title('TDC and ADC counts', fontsize=14)
        axes.set_xlabel('Digit', fontsize=12)
        axes.set_ylabel('Count', fontsize=12)
        axes.grid(True,linestyle='-', color='0.75')
#        if (len(tdc_counts) > 0):
#            axes.scatter(range(0, len(tdc_counts)), tdc_counts, 10, 'r', label='TDC counts')
#        if (len(adc_counts) > 0):
#            axes.scatter(range(0, len(adc_counts)), adc_counts, 10, 'b', label='ADC counts')        
        axes.plot(tdc_counts, 'r', label='TDC counts') 
        axes.plot(adc_counts, 'b', label='ADC counts') 
        axes.legend()
        print "Histogrammed"
        return canvas

    def saveHistogram(self, canvas):
        # Create a unique file name based on the prefix, current date
        # and the count. Use the date since strftime doesn't (yet)
        # support milli-seconds.
        fileName = "%s%s%d.%s" % (
            self._filePrefix, 
            datetime.datetime.now().strftime("%Y%m%d%H%M%S"), 
            self._fileCount,
            self._imageType)
        file = os.path.join(self._directory, fileName)
        print "Saving in " + file
        # File extension determines format. Default if omitted is .png.
        canvas.print_figure(file, dpi=500)
        self._fileCount += 1
        print "Saved!"
