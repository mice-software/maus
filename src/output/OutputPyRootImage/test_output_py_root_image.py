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


"""OutputPyRootImage test case"""

#pylint: disable=E1101

import os
import json
import unittest
import shutil

import ROOT

from MAUS import OutputPyRootImage

class OutputPyRootImageTestCase(unittest.TestCase): # pylint: disable=R0904
    """OutputPyRootImage test case"""

    def setUp(self): # pylint: disable=C0103
        """OutputPyRootImage setup"""
        ROOT.gROOT.SetBatch(True)
        self.test_cards = {
            "reduce_plot_refresh_rate":1,
            "image_file_prefix":"image",
            "image_directory":os.path.expandvars(
                  "${MAUS_TMP_DIR}/test_output_py_root_image"
            ),
            "end_of_run_image_directory":os.path.expandvars(
                  "${MAUS_TMP_DIR}/test_output_py_root_image/end_of_run"
            ),
            "image_types":["png"],
        }
        self.test_image = ROOT.MAUS.ImageData()
        canvas = ROOT.TCanvas("name", "title")
        hist = ROOT.TH1D("", "title;axis;axis", 100, 0., 1.)
        hist.Draw()
        canvas.Update()
        canvas_wrapper = ROOT.MAUS.CanvasWrapper()
        canvas_wrapper.SetCanvas(canvas)
        canvas_wrapper.SetFileTag("TestCanvas")
        image = ROOT.MAUS.Image()
        image.SetRunNumber(555)
        image.SetSpillNumber(0)
        image.CanvasWrappersPushBack(canvas_wrapper)
        self.test_image.SetImage(image)        
        try:
            shutil.rmtree(self.test_cards["image_directory"])
        except OSError: # probably the dir does not exist
            pass

    def test_birth(self):
        """OutputPyRootImage test birth()"""
        output = OutputPyRootImage()
        output.birth(json.dumps(self.test_cards))
        self.assertEqual(output.file_prefix,
                         self.test_cards["image_file_prefix"])
        self.assertEqual(output.directory,
                         self.test_cards["image_directory"])
        self.assertEqual(output.end_of_run_directory,
                         self.test_cards["end_of_run_image_directory"])
        self.assertEqual(output.image_types,
                         self.test_cards["image_types"])

    def test_death(self): # pylint: disable=R0201
        """OutputPyRootImage test death()"""
        output = OutputPyRootImage()
        output.death()

    def test_save(self):
        """OutputPyRootImage test save()"""
        output = OutputPyRootImage()
        output.birth(json.dumps(self.test_cards))
        output.save(self.test_image)
        test_path = os.path.join(self.test_cards["image_directory"],
                                 "imageTestCanvas.png")
        self.assertTrue(os.path.exists(test_path))
        self.test_image.GetImage().SetSpillNumber(1)
        output.save(self.test_image)
        self.test_image.GetImage().SetSpillNumber(2)
        output.save(self.test_image)
        self.assertTrue(os.path.exists(test_path))
        os.remove(test_path)
        self.test_image.GetImage().SetSpillNumber(3)
        output.save(self.test_image)
        self.assertTrue(os.path.exists(test_path))

    def test_save_end_of_run(self):
        """OutputPyRootImage test end of run save()"""
        output = OutputPyRootImage()
        output.birth(json.dumps(self.test_cards))
        self.test_image.GetImage().SetSpillNumber(4)
        output.save(self.test_image)
        output.save(ROOT.MAUS.RunFooterData())
        test_path = os.path.join(self.test_cards["end_of_run_image_directory"],
                                 str(self.test_image.GetImage().GetRunNumber()),
                                 "imageTestCanvas.png")
        self.assertTrue(os.path.exists(test_path))
        output.save(ROOT.MAUS.RunFooterData())
        self.assertTrue(os.path.exists(test_path))


if __name__ == "__main__":
    unittest.main()

