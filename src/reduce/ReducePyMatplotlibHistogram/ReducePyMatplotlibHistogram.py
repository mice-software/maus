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

import ErrorHandler

class ReducePyMatplotlibHistogram: # pylint: disable=R0903 
    """
    @class ReducePyMatplotlibHistogram.PyMatplotlibHistogram is 
    a base class for classes that create histograms using matplotlib. 

    Histograms are output as JSON documents of form:

    @verbatim
    {"image": {"content":"...a description of the image...",
               "tag": TAG,
               "image_type": "eps", 
               "data": "...base 64 encoded image..."}}
    @endverbatim

    "TAG" is specified by the sub-class. If "histogram_auto_number"
    (see below) is "true" then the TAG will have a number N appended
    where N means that the histogram was produced as a consequence of
    the (N + 1)th spill processed  by the worker. The number will be
    zero-padded to form a six digit string e.g. "00000N". If
    "histogram_auto_number" is false then no such number is appended.

    In cases where a spill is input that contains errors (e.g. is
    badly formatted or is missing the data needed to update a
    histogram) then a spill is output which is just the input spill
    with an "errors" field containing the error e.g.

    @verbatim
    {"errors": {..., "bad_json_document": "unable to do json.loads on input"}}
    {"errors": {..., "...": "..."}}
    @endverbatim

    The caller can configure the worker and specify:

    -Image type ("histogram_image_type"). Must be one of those
     supported by matplot lib (currently "svg", "ps", "emf", "rgba",
     "raw", "svgz", "pdf", "eps", "png"). Default: "eps".
    -Auto-number ("histogram_auto_number"). Default: false. Flag
     that determines if the image tag (see above) has the spill count
     appended to it or not.
    -Sub-classes may support additional configuration parameter

    Sub-classes must override:

    -_configure_at_birth - to extract any additional
     sub-class-specific configuration from data cards.
    -_update_histograms. This checks that a spill has the data
     necessary to update any histograms then creates JSON documents in
     the format described above.
    -_cleanup_at_death - to do any sub-class-specific cleanup.
    """

    def __init__(self):
        """
        Set initial attribute values.
        @param self Object reference.
        """
        # matplotlib histogram - for validation.
        figure = Figure(figsize=(6, 6))
        self.__histogram = FigureCanvas(figure)
        self.spill_count = 0 # Number of spills processed to date.
        self.image_type = "eps"
        self.auto_number = False

    def birth(self, config_json):
        """
        Configure worker from data cards. If "image_type" is not
        in those supported then a ValueError is thrown.
        @param self Object reference.
        @param config_json JSON document string.
        @returns True if configuration succeeded. 
        """
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
                % (self.image_type, 
                   self.__histogram.get_supported_filetypes().keys())
            raise ValueError(error)

        self.spill_count = 0

        # Do sub-class-specific configuration.
        return self._configure_at_birth(config_doc)

    def _configure_at_birth(self, config_doc):
        """
        Perform sub-class-specific configuration from data cards.
        Sub-classes must define this function.
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
        except Exception: # pylint:disable=W0703
            json_doc = {}
            ErrorHandler.HandleException(json_doc, self)
            return unicode(json.dumps(json_doc))

        self.spill_count = self.spill_count + 1

        # Process spill and update histograms.
        try:
            result = self._update_histograms(json_doc)
        except Exception: # pylint:disable=W0703
            ErrorHandler.HandleException(json_doc, self)
            return unicode(json.dumps(json_doc))

        # Convert results to strings.
        doc_list = []
        for doc in result:
            doc_list.append(json.dumps(doc))
            doc_list.append("\n")
        return unicode("".join(doc_list))

    def _update_histograms(self, spill):
        """
        Check that the spill has the data necessary to update the
        histograms then creates JSON documents in the format described
        above. 
        Sub-classes must define this function.
        @param self Object reference.
        @param spill Current spill.
        @returns list of JSON documents. If the sub-class only updates 
        histograms every N spills then this list can just contain 
        the input spill.
        Otherwise it should consist of 1 or more JSON documents 
        containing image data in the form described above. 
        @throws Exception if various sub-class specific errors arise.
        """

    def death(self): #pylint: disable=R0201
        """
        Invokes _cleanup_at_death().
        @returns True
        """
        return self._cleanup_at_death()

    def _cleanup_at_death(self): #pylint: disable=R0201
        """
        A no-op. 
        Sub-classes can override this function to do
        sub-class-specific clean-up at death time.
        @param self Object reference.
        @returns True
        """
        return True
 
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

    def _rescale_axes(self, histogram, xmin, xmax, ymin, ymax, xfudge = 0.5, yfudge = 0.5): #pylint: disable=C0301, R0913, R0201
        """
        Rescale the X and Y axes of the histogram to show the given
        axis ranges. Fudge factors are used avoid matplotlib warning
        about "Attempting to set identical bottom==top" which arises
        if the axes are set to be exactly the maximum of the data.
        @param self Object reference.
        @param histogram FigureCanvas representing a histogram.
        @param xmin Minimum X value.
        @param xmax Maximum X value.
        @param ymin Minimum Y value.
        @param ymin Maximum Y value.
        @param xfudge X fudge factor.
        @param yfudge Y fudge factor.
        """
        # Fudge factors are used
        histogram.figure.get_axes()[0].set_xlim( \
            [xmin, xmax + xfudge])
        histogram.figure.get_axes()[0].set_ylim( \
            [ymin, ymax + yfudge])

    def _create_image_json(self, content, tag, canvas): #pylint: disable=R0201
        """
        Build a JSON document holding image data.

        @param self Object reference.
        @param content String describing the image.
        @param tag Image tag.
        @param histogram FigureCanvas representing a histogram.
        @returns JSON document.
        """
        json_doc = {}
        json_doc["image"] = {}
        if (self.auto_number):
            image_tag = "%s%06d" % (tag, self.spill_count)
        else:
            image_tag = tag
        data = self.__convert_to_binary(canvas)
        json_doc["image"]["content"] = content
        json_doc["image"]["tag"] = image_tag
        json_doc["image"]["image_type"] = self.image_type
        json_doc["image"]["data"] = data
        return json_doc

    def __convert_to_binary(self, canvas): #pylint: disable=R0201
        """
        Convert histogram to binary format.
        @param self Object reference.
        @param canvas matplotlib FigureCanvas representing a
        histogram. 
        @returns representation of histogram in base 64-encoded image 
        type format.
        """
        data_file = StringIO.StringIO()
        canvas.print_figure(data_file, dpi=500, format=self.image_type)
        data_file.seek(0)
        data = data_file.read()
        return base64.b64encode(data)

