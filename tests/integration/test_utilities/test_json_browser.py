"""
Tests for the json browser utility
"""

import unittest
import os
import time
import json
import subprocess

OUT_FILE_1 = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp", \
                        "test_json_browser_1.json")
OUT_FILE_2 = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp", \
                        "test_json_browser_2.json")
BROWSER = os.path.join(os.environ["MAUS_ROOT_DIR"], "bin", "utilities", \
                       "json_browser.py")
SLEEP_TIME = 1

class TestJsonBrowser(unittest.TestCase): #pylint: disable=R0904
    """
    Tests for the json browser utility
    """

    def test_formatting(self):
        """Check json browser --input-file, --output-file, --indent, -l"""
        json_in = {"a":[{"b":0}, {}, []]}
        json_ref_out = \
"""{
   "a": [
      {
         "b": 0
      }, 
      {}, 
      []
   ]
}"""
        json_ref_out = json_ref_out.split('\n')
        fout = open(OUT_FILE_1, "w")
        print >> fout, json.dumps(json_in)
        fout.close()
        subprocess.Popen(["python", BROWSER, "--input-file", OUT_FILE_1, \
                         "--output-file", OUT_FILE_2, "-l", "--indent", "3"])
        time.sleep(SLEEP_TIME)
        fin = open(OUT_FILE_2)
        json_test_out = fin.readlines()

        for i in range(len(json_ref_out)):
            self.assertEqual(json_ref_out[i]+'\n', json_test_out[i])
        os.remove(OUT_FILE_1)
        os.remove(OUT_FILE_2)

    def test_lines(self):
        """Check json browser --start-line, --end-line"""
        json_in = []
        fout = open(OUT_FILE_1, "w")
        for i in range(5):
            json_in += ['"'+str(i+1)+'":"a"']
            print >> fout, json.dumps(json_in[-1])
        fout.close()

        subprocess.Popen(["python", BROWSER, "--input-file", OUT_FILE_1,
                         "--output-file", OUT_FILE_2, "-l", "--start-line",
                         "5"])
        time.sleep(SLEEP_TIME)
        fin = open(OUT_FILE_2)
        line = fin.readline()
        self.assertEqual(line, '"\\"5\\":\\"a\\""\n')
        os.remove(OUT_FILE_2)

        subprocess.Popen(["python", BROWSER, "--input-file", OUT_FILE_1,
                         "--output-file", OUT_FILE_2, "-l", "--start-line",
                         "6"])
        time.sleep(SLEEP_TIME)
        fin = open(OUT_FILE_2)
        line = fin.readline()
        self.assertEqual(line, "")
        os.remove(OUT_FILE_2)

        subprocess.Popen(["python", BROWSER, "--input-file", OUT_FILE_1, \
                         "--output-file", OUT_FILE_2, "-l", "--start-line", "3",
                         "--end-line", "4"])
        time.sleep(SLEEP_TIME)
        fin = open(OUT_FILE_2)
        line = fin.readline()
        self.assertEqual(line, '"\\"3\\":\\"a\\""\n')
        line = fin.readline()
        self.assertEqual(line, '"\\"4\\":\\"a\\""\n')
        os.remove(OUT_FILE_1)
        os.remove(OUT_FILE_2)

    def test_less(self):
        """Check json browser -L"""
        fout = open(OUT_FILE_1, "w")
        print >> fout, json.dumps({})
        fout.close()

        out = open(OUT_FILE_2, "w")
        proc = subprocess.Popen(["python", BROWSER, "--input-file", OUT_FILE_1],
                             stdin=subprocess.PIPE, stdout=out)
        time.sleep(SLEEP_TIME)
        out.close()
        proc.poll()
        self.assertEqual(proc.returncode, 0)
        fin = open(OUT_FILE_2)
        self.assertEqual(fin.readline(), "{}\n")
        os.remove(OUT_FILE_1)
        os.remove(OUT_FILE_2)

if __name__ == "__main__":
    unittest.main()


