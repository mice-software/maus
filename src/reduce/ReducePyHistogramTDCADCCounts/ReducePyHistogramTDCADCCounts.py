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

from ReducePyMatplotlibHistogram import ReducePyMatplotlibHistogram

class ReducePyHistogramTDCADCCounts(ReducePyMatplotlibHistogram):
    """
    @class ReducePymatplotlibHistogram.PyHistogramTDCADCCounts
    maintains a histogram of the TDC and ADC counts for all spills
    processed to date. It uses the "adc_counts" and "tdc_counts" for
    each digit within a "digits" list. For each spill input it outputs
    a new histogram. 

    Histograms are output as JSON documents of form:

    @verbatim
    {"image": {"content":"Total TDC and ADC counts to spill 2",
               "tag": TAG,
               "image_type": "eps", 
               "data": "...base 64 encoded image..."}}
    @endverbatim

    where "TAG" is "tdcadc". If "histogram_auto_number" (see
    below) is "true" then the TAG will have a number N appended where
    N means that the histogram was produced as a consequence of the (N
    + 1)th spill processed  by the worker. The number will be
    zero-padded to form a six digit string e.g. "00000N". If
    "histogram_auto_number" is false then no such number is appended

    In cases where a spill is input that contains errors (e.g. is
    badly formatted or is missing the data needed to update a
    histogram) then a spill is output which is just the input spill
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
        ReducePyMatplotlibHistogram.__init__(self)
        # Do initialisation specific to this class.
        self.__max_adc_count = 1
        self.__max_tdc_count = 1
        self._tag = "tdcadc"
        self._content = ""
        self._tdcadchistogram = None

    def _configure_at_birth(self, config_doc):
        """
        Configure worker from data cards. Overrides super-class method. 
        @param self Object reference.
        @param config_doc JSON document.
        @returns True if successful.
        """
        # Do configuration specific to this class.
        # Can assume that self._tdcadchistogram now has a histogram
        # (matplotlib FigureCanvas object) available which can now be
        # customised.
        self._tdcadchistogram = \
            ReducePyMatplotlibHistogram._create_histogram(self)
        self._tdcadchistogram.figure.get_axes()[0].set_xlabel(
            "TDC count", fontsize=12)
        self._tdcadchistogram.figure.get_axes()[0].set_ylabel(
            "ADC count", fontsize=12)
        self.__max_adc_count = 1
        self.__max_tdc_count = 1
        return True

    def _update_histograms(self, spill):
        """
        Process a spill, update the histograms then
        create a JSON document in the format described above. 
        @param self Object reference.
        @param spill Current spill.
        @returns list with histogram JSON document.
        @throws KeyError if "digits" is not in spill.
        """
        if "END_OF_RUN" in spill:
            return [{}]

        # Do validation specific to this class while getting the
        # data to be graphed.
        if "digits" not in spill:
            raise KeyError("digits field is not in spill")
        digits = spill["digits"]

        # Extract just those digits that are for the Tracker.
        trackerdigits = \
            [digit for digit in digits if self.__filter_trackers(digit)]
        # Get the lists of TDC and ADC counts.    
        tdcs = [self.__get_counts(digit, "tdc_counts") 
                for digit in trackerdigits]
        adcs = [self.__get_counts(digit, "adc_counts") 
                for digit in trackerdigits]

        # Calculate maximum TDC and ADC counts for axis rescaling.
        spill_max_tdc_count = 0
        if (len(tdcs) > 0):
            spill_max_tdc_count = max(tdcs)
        self.__max_tdc_count = max(self.__max_tdc_count, spill_max_tdc_count)
        spill_max_adc_count = 0
        if (len(adcs) > 0):
            spill_max_adc_count = max(adcs)
        self.__max_adc_count = max(self.__max_adc_count, spill_max_adc_count)

        # Set description of content of histogram and histogram title.
        self._content = \
            "Total TDC and ADC counts"
        self._tdcadchistogram.figure.get_axes()[0].set_title(
            self._content, fontsize=14)

        # Plot the data.
        if (len(tdcs) > 0):
            self._tdcadchistogram.figure.get_axes()[0].scatter(
                tdcs, adcs, 10, "b")

        # Rescale axis so 0 is always visible.
        ReducePyMatplotlibHistogram._rescale_axes( \
            self, self._tdcadchistogram, \
            0, self.__max_tdc_count, 0, self.__max_adc_count)

        image_doc = ReducePyMatplotlibHistogram._create_image_json( \
            self, self._content, self._tag, self._tdcadchistogram)
        return [image_doc]

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

    def _cleanup_at_death(self):
        """
        No sub-class-specific cleanup is needed.
        @param self Object reference.
        @returns True
        """
        ReducePyMatplotlibHistogram._cleanup_at_death(self)
        # Do cleanup specific to this class, none in this case.
        return True
