"""
ReducePyROOTHistogram is a base class for classes that create
histograms using ROOT.
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
#
# Turn off false positives related to ROOT
#pylint: disable = E1101

import json

import ErrorHandler
import ROOT
import maus_cpp.converter

import framework.utilities

class ReducePyROOTHistogram: # pylint: disable=R0902, R0921
    """
    @class ReducePyROOTHistogram.ReducePyROOTHistogram is a base class
    for classes that create histograms using ROOT. 

    Histograms are output as JSON documents of form:

    @verbatim
    {"image": {"keywords": [...list of image keywords...],
               "description":"...a description of the image...",
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
     supported by ROOT (currently "ps", "eps", "gif", "jpg", "jpeg",
     "pdf", "svg", "png"). Default "eps".
    -Auto-number ("histogram_auto_number"). Default: false. Flag
     that determines if the image tag (see above) has the spill count
     appended to it or not.
    -ROOT batch mode ("root_batch_mode"). Default: 0 (false). Flag
     indicating whether ROOT should be run in batch or interactive 
     mode.
    -Sub-classes may support additional configuration parameters.

    Sub-classes must override the following functions:

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
        self.supported_types = \
            ["ps", "eps", "gif", "jpg", "jpeg", "pdf", "svg", "png"]
        self.spill_count = 0 
        self.image_type = "eps"
        self.auto_number = False
        self.root_batch_mode = 0 # False = Interactive mode.

    def birth(self, config_json):
        """
        Configure worker from data cards. If "image_type" is not
        in those supported then a ValueError is thrown.
        @param self Object reference.
        @param config_json JSON document string.
        @returns True if configuration succeeded. 
        """
        config_doc = json.loads(config_json)

        key = "root_batch_mode"
        if key in config_doc:
            self.root_batch_mode = int(config_doc[key])
        ROOT.gROOT.SetBatch(False) # Interactive mode.
        if self.root_batch_mode == 1:
            ROOT.gROOT.SetBatch(True)

        key = "histogram_auto_number"
        if key in config_doc:
            self.auto_number = config_doc[key]

        key = "histogram_image_type"
        if key in config_doc:
            self.image_type = config_doc[key]
        else:
            self.image_type = "eps"

        if self.image_type not in self.supported_types:
            error = "Unsupported histogram image type: %s Expect one of %s" \
                % (self.image_type, self.supported_types)
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

    def process(self, data):
        """
        Update the histogram with data from the current spill
        and output the histogram.        
        @param self Object reference.
        @param json_string String with current JSON document.
        @returns JSON document containing current histogram.
        """
        # Load and validate the JSON document.
        def_doc = {"maus_event_type":"Image", "image_list":[]}

        # check if input is MAUS::Data, if it's not do a json conversion
        # online celery/mongo returns json string
        if data.__class__.__name__ == 'MAUS::Data':
            json_doc = maus_cpp.converter.json_repr(data)
        else:
            try:
                json_doc = json.loads(data.rstrip())
            except Exception: # pylint:disable=W0703
                def_doc = ErrorHandler.HandleException(def_doc, self)
                print def_doc
                return unicode(json.dumps(def_doc))

        self.spill_count = self.spill_count + 1

        # Process spill and update histograms.
        try:
            result = self._update_histograms(json_doc)
        except Exception: # pylint:disable=W0703
            def_doc = ErrorHandler.HandleException(def_doc, self)
            return unicode(json.dumps(def_doc))
        image_list = [image['image'] for image in result]
        # Convert results to strings.
        return json.dumps({"maus_event_type":"Image", "image_list":image_list})

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
        raise NotImplementedError("Should be overloaded to update histos")

    def death(self): #pylint: disable=R0201
        """
        Invokes _cleanup_at_death() then removes any zombie ROOT
        objects. 
        @returns True
        """
        result = self._cleanup_at_death()
        # Remove any zombie ROOT objects.
        ROOT.gDirectory.GetList().Delete()
        return result

    def _cleanup_at_death(self): #pylint: disable=R0201
        """
        A no-op. 
        Sub-classes can override this function to do
        sub-class-specific clean-up at death time.
        @param self Object reference.
        @returns True
        """
        raise NotImplementedError("Should be overloaded to clean out histos")

    def get_image_doc(self, keywords, description, tag, canvas):
        """
        Build a JSON document holding image data. This saves the
        canvas to a temporary file and then reloads it.

        @param self Object reference.
        @param keywords List of image keywords.
        @param description String describing the image.
        @param tag Image tag.
        @param canvas ROOT canvas.
        @returns JSON document.
        """
        if (self.auto_number):
            image_tag = "%s%06d" % (tag, self.spill_count)
        else:
            image_tag = tag
        # Save to and reload from temporary file.
        file_name = "%s_tmp.%s" % (image_tag, self.image_type)
        canvas.Print(file_name)
        encoded_data = framework.utilities.convert_binary_to_string(file_name,
                                                                    True)
        # Build JSON document.
        json_doc = {}
        json_doc["image"] = {}
        json_doc["image"]["keywords"] = keywords
        json_doc["image"]["description"] = description
        json_doc["image"]["tag"] = image_tag
        json_doc["image"]["image_type"] = self.image_type
        json_doc["image"]["data"] = encoded_data
        return json_doc
 
    def get_root_doc(self, keywords, description, tag, histos):
        """
        Build a JSON document holding ROOT tfile data. This saves
        the list of ROOT histo objects to a temporary file and 
        then reloads it.

        @param self Object reference.
        @param keywords List of image keywords.
        @param description String describing the image.
        @param tag ROOT TFile tag.
        @param histos List of ROOT histo objects.
        @returns JSON document.
        """
        if (self.auto_number):
            image_tag = "%s%06d" % (tag, self.spill_count)
        else:
            image_tag = tag
        # Save to and reload from temporary file.
        file_name = "%s_tmp.%s" % (image_tag, 'root')

        rfile = ROOT.TFile(file_name, 'RECREATE')
        for histo in histos:
            histo.Write()
        rfile.Close()
        encoded_data = framework.utilities.convert_binary_to_string(file_name,
                                                                    True)
        # Build JSON document.
        json_doc = {}
        json_doc["image"] = {}
        json_doc["image"]["keywords"] = keywords
        json_doc["image"]["description"] = description
        json_doc["image"]["tag"] = image_tag
        json_doc["image"]["image_type"] = 'root'
        json_doc["image"]["data"] = encoded_data
        return json_doc
