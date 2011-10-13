"""
ReducePyMatplotlibHistogram is a base class for classes that create
histograms using matplotlib.
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

class ReducePyMatplotlibHistogram: # pylint: disable=R0903 
    """
    @class ReducePyMatplotlibHistogram.PyMatplotlibHistogram is 
    a base class for classes that create histograms using matplotlib. 
    PyMatplotlibHistogram maintains a histogram created using
    matplotlib.

    The histogram output is a JSON document of form:

    @verbatim
    {"image": {"content":"...a description of the image...",
               "tag": TAG,
               "image_type": "eps", 
               "data": "...base 64 encoded image..."}}
    @endverbatim

    where "TAG" is specified by the sub-class. If 
    "histogram_auto_number" (see below) is "true" then the TAG will
    have a number N appended where N means that the histogram was
    produced as a consequence of the (N + 1)th spill processed 
    by the worker. The number will be zero-padded to form a six digt
    string e.g. "00000N". If "histogram_auto_number" is false 
    then no such number is appended.

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
    -Auto-number ("histogram_auto_number"). Default: false. Flag
     that determines if the image tag (see above) has the spill count
     appended to it or not.

    Sub-classes must override:

    -_configure_at_birth - to extract any additional
     sub-class-specific configuration from data cards.
    -_update_histogram. This should update the histogram, axes, scale
     and make any other desired changes. The _content attribute can
     be updated with a textual description of the content of the
     histogram.
    -_cleanup_at_death - to do any sub-class-specific cleanup.
    """

    def __init__(self):
        """
        Set initial attribute values.
        @param self Object reference.
        """
        self.spill_count = 0 # Number of spills processed to date.
        self.image_type = "eps"
        self.auto_number = False
        self._histogram = None # matplotlib histogram.
        self._tag = "graph" # Histogram name tag.
        self._content = "" # Description of histogram content.

    def birth(self, config_json):
        """
        Configure worker from data cards. If "image_type" is not
        in those supported then a ValueError is thrown.
        @param self Object reference.
        @param config_json JSON document string.
        @returns True if configuration succeeded. 
        """
        config_doc = json.loads(config_json)

        self._histogram = self._create_histogram()

        key = "histogram_auto_number"
        if key in config_doc:
            self.auto_number = config_doc[key]

        key = "histogram_image_type"
        if key in config_doc:
            self.image_type = config_doc[key]
        else:
            self.image_type = "eps"

        if self.image_type not in \
            self._histogram.get_supported_filetypes().keys():
            error = "Unsupported histogram image type: %s Expect one of %s" \
                % (self.image_type, 
                   self._histogram.get_supported_filetypes().keys())
            raise ValueError(error)

        self.spill_count = 0
        self._content = ""

        # Do sub-class-specific configuration.
        return self._configure_at_birth(config_doc)

    def _configure_at_birth(self, config_doc):
        """
        Perform sub-class-specific configuration from data cards. When
        this is called a sub-class can assume that self._histogram 
        points to a matplotlib FigureCanvas that they can then customise.
        @param self Object reference.
        @param config_json JSON document.
        @returns True if configuration succeeded. 
        """

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

        result = self._update_histogram(json_doc)
        if (result != None):
            return json.dumps(result)

        return self._create_image_json()

    def _update_histogram(self, json_doc):
        """
        Update the histogram with data from the current spill
        and output the histogram. Sub-classes must define this.
        @param self Object reference.
        @param json_doc Current spill..
        @returns None if histogram was created or a JSON document with
        error messages if there were problems (e.g. information was
        missing from the spill).
        """

    def death(self): #pylint: disable=R0201
        """
        Invokes _cleanup_at_death().
        @returns True
        """
        return self._cleanup_at_death()

    def _cleanup_at_death(self):
        """
        A no-op. Can be overridden by sub-classes for sub-class-specific
        clean-up at death time.
        @param self Object reference.
        @returns True
        """
 
    def _create_histogram(self): #pylint: disable=R0201
        """
        Create a histogram using matplotlib.
        @param self Object reference.
        @returns matplotlib FigureCanvas representing the histogram.
        """
        figure = Figure(figsize=(6, 6))
        histogram = FigureCanvas(figure)
        axes = figure.add_subplot(111)
        axes.grid(True, linestyle="-", color="0.75")
        return histogram

    def _rescale_axes(self, xmin, xmax, ymin, ymax, xfudge = 0.5, yfudge = 0.5): #pylint: disable=C0301, R0913
        """
        Rescale the X and Y axes of the histogram to show the given
        axis ranges. Fudge factors are used avoid matplotlib warning
        about "Attempting to set identical bottom==top" which arises
        if the axes are set to be exactly the maximum of the data.
        @param self Object reference.
        @param xmin Minimum X value.
        @param xmax Maximum X value.
        @param ymin Minimum Y value.
        @param ymin Maximum Y value.
        @param xfudge X fudge factor.
        @param yfudge Y fudge factor.
        @returns matplotlib FigureCanvas representing the histogram.
        """
        # Fudge factors are used
        self._histogram.figure.get_axes()[0].set_xlim( \
            [xmin, xmax + xfudge])
        self._histogram.figure.get_axes()[0].set_ylim( \
            [ymin, ymax + yfudge])

    def _create_image_json(self):
        """
        Create JSON document for output by the worker with the
        content description, image type, tag and base-64 encoded
        data from the histogram.
        @param self Object reference.
        @returns JSON document containing current histogram.
        """
        json_doc = {}
        json_doc["image"] = {}
        if (self.auto_number):
            tag = "%s%06d" % (self._tag, self.spill_count)
        else:
            tag = "%s" % (self._tag)
        data = self.__convert_to_binary(self._histogram)
        json_doc["image"]["content"] = self._content
        json_doc["image"]["tag"] = tag
        json_doc["image"]["image_type"] = self.image_type
        json_doc["image"]["data"] = data
        self.spill_count += 1
        return json.dumps(json_doc)

    def __convert_to_binary(self, histogram): #pylint: disable=R0201
        """
        Convert histogram to binary format.
        @param self Object reference.
        @param histogram matplotlib FigureCanvas representing the
        histogram. 
        @returns representation of histogram in base 64-encoded image 
        type format.
        """
        data_file = StringIO.StringIO() 
        histogram.print_figure(data_file, dpi=500, format=self.image_type)
        data_file.seek(0)
        data = data_file.read()
        return base64.b64encode(data)

