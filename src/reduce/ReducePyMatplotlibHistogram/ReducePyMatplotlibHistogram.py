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
    matplotlib. Sub-classes determine the axes, scale, and data
    within the histogram and invoke the methods of this class.

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
    by the worker. If "histogram_auto_number" is false then no
    such number is appended.

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
    """

    def __init__(self):
        """
        Set initial attribute values.
        @param self Object reference.
        """
        self.image_type = "eps"
        self.spill_count = 0
        self.auto_number = False
        self._histogram = None
        self._tag = "graph"

    def _default_birth(self, config_json):
        """
        Configure worker from data cards. If "image_type" is not
        in those supported then a ValueError is thrown.
        @param self Object reference.
        @param config_json JSON document string.
        @returns JSON document with configuration in case sub-class
        wants to extract sub-class-specific configuration.
        """
        config_doc = json.loads(config_json)

        # Create a FigureCanvas for the histogram.
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
                    % (
                    self.image_type, 
                    self._histogram.get_supported_filetypes().keys())
            raise ValueError(error)

        self.spill_count = 0
        return config_doc

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

    def _create_image_json(self, content):
        """
        Create JSON document for output by the worker with the
        content description, image type, tag and base-64 encoded
        data from the histogram.
        @param self Object reference.
        @param content Description of histogram content.
        @returns JSON document containing current histogram.
        """
        json_doc = {}
        json_doc["image"] = {}
        if (self.auto_number):
            tag = "%s%d" % (self._tag, self.spill_count)
        else:
            tag = "%s" % (self._tag)
        data = self.__convert_to_binary(self._histogram)
        json_doc["image"]["content"] = content
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

    def death(self): #pylint: disable=R0201
        """
        A no-op
        @returns True
        """
        return True
