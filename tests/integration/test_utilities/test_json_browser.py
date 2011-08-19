import unittest
import os
import json
import subprocess

OUT_FILE_1 = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp", \
                        "test_json_browser_1.json")
OUT_FILE_2 = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp", \
                        "test_json_browser_2.json")
BROWSER = os.path.join(os.environ["MAUS_ROOT_DIR"], "bin", "utilities", \
                       "json_browser.py")

class test_json_browser(unittest.TestCase):
    def test_formatting(self):
        json_in = {"a":[{"b":0}, {}, []]}
        json_ref_out = \
"""{
   a:[
      {
         b:0
      },
      {
      },
      [
      ]
   ]
}"""
        fout = open(OUT_FILE_1, "w")
        print >>fout, json.dumps(json_in)
        subprocess.Popen(["python", BROWSER, "--input_file", OUT_FILE_1, \
                         "--output_file", OUT_FILE_2, "-m", "--indent", "3"])
        fin = open(OUT_FILE_2)
        json_test_out = fin.read()
        self.assertEqual(json_ref_out, json_test_out)

if __name__ == "__main__":
    unittest.main()


