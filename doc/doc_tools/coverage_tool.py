#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

"""
Handler for lcov output.

Parses the lcov output to generate a list of coverage items; then we use a
series of filters to select the appropriate files on which to report; and a
function to sum over the list to return total reporting numbers for the list.
"""

import copy
import os
import json
import HTMLParser

CPP_COV = os.path.expandvars('$MAUS_ROOT_DIR/doc/cpp_coverage')

class Coverage(): # pylint: disable=R0903
    """
    Data structure reflecting lcov coverage information
    """
    def __init__(self):
        """Initialise data to 0."""
        self.file = ""
        self.lines = {"percentage":0., "covered":0., "total":0., "excluded":0.}
        self.functions = copy.deepcopy(self.lines)
        self.branches = copy.deepcopy(self.lines)

    def json_repr(self):
        """Return json representation of the data"""
        return {
            "file":self.file,
            "line":self.lines,
            "function":self.functions,
            "branch":self.branches
        }

    def __str__(self):
        """String representation of the data is a json.dumps of json_repr"""
        return json.dumps(self.json_repr())

class PyCoverageParser(HTMLParser.HTMLParser): # pylint: disable=R0904
    """
    Parse a py coverage html file to extract test coverage information
    """
    def __init__(self):
        """Initialise coverage and some flags"""
        HTMLParser.HTMLParser.__init__(self)
        self.coverage = []
        self.in_a = False
        self.in_tbody = False
        self.in_td = False
        self.td_index = -1

    def handle_starttag(self, tag, attrs_list):
        """
        PyCoverageParser Handler for opening e.g. <tag> tag

        Handles tr, td, a tag
        """
        attrs_dict = {}
        for attr in attrs_list:
            attrs_dict[attr[0]] = attr[1]
        if tag == "tbody":
            self.in_tbody = True
        if tag == "tr":
            self.handle_tr_start(tag, attrs_dict)
        if tag == "td":
            self.td_index += 1
            self.in_td = True
        if tag == "a":
            self.in_a = True

    def handle_endtag(self, tag):
        """
        PyCoverageParser Handler for closing e.g. </tag> tag

        Handles tr, td, a tag
        """
        if tag == "tbody":
            self.in_tbody = False
        if tag == "td":
            self.in_td = False
        if tag == "a":
            self.in_a = False

    def handle_tr_start(self, tag, attrs_dict): # pylint: disable=W0613
        """PyCoverageParser Handler for new table row"""
        if 'class' in attrs_dict and attrs_dict['class'] == 'tablehead':
            return
        self.coverage.append(Coverage())
        self.td_index = -1

    def handle_data(self, data):
        """PyCoverageParser Handler for data"""
        if self.td_index == -1 or not self.in_td or not self.in_tbody:
            return
        if self.td_index == 0:
            if self.in_a:
                self.coverage[-1].file = data
        if self.td_index == 1:
            self.coverage[-1].lines['total'] = float(data)
        if self.td_index == 2:
            self.coverage[-1].lines['covered'] = \
                                    self.coverage[-1].lines['total']-float(data)
        if self.td_index == 3:
            self.coverage[-1].functions['percentage'] = float(data)

    def parse_file(self, file_name):
        """PyCoverageParser Parse a file with the specified file name"""
        html_file = open(file_name)
        for line in html_file.readlines():
            self.feed(line)
        for item in self.coverage:
            if item.lines['total'] < 0.5:
                item.lines['percentage'] = 100.
            else:
                item.lines['percentage'] = \
                                  100.*item.lines['covered']/item.lines['total']
        return self.coverage

class LcovCoverageParser(HTMLParser.HTMLParser): # pylint: disable=R0904
    """
    Parse a lcov coverage html file to extract test coverage information

    To parse a file, use parse_file(file_name) which returns a list of Coverage
    objects.

    HTMLParser module uses a linear mode to parse html. Makes it quite horrible
    to figure out where in the html structure I am, especially with e.g. nested
    tables and such like. I use an obscure system of flags to do it - not nice.
    """
    def __init__(self):
        """Initialise coverage and some flags"""
        HTMLParser.HTMLParser.__init__(self)
        self.coverage = []
        self.td_index = -1
        self.in_td = -1
        self.in_a = 0

    def handle_starttag(self, tag, attrs_list):
        """
        Handler for opening e.g. <tag> tag

        Handles tr, td, a tag
        """
        attrs_dict = {}
        for attr in attrs_list:
            attrs_dict[attr[0]] = attr[1]
        if tag == "tr":
            self.handle_tr_start(tag, attrs_dict)
        if tag == "td":
            self.handle_td_start(tag, attrs_dict)
        if tag == "a":
            self.in_a += 1


    def handle_endtag(self, tag):
        """
        Handler for closing e.g. </tag> tag

        Handles tr, td, a tag
        """
        if tag == "tr":
            if self.td_index == -1:
                self.coverage.pop()
        if tag == "a":
            self.in_a -= 1
        if tag == "td":
            self.in_td = -1


    def handle_tr_start(self, tag, attrs_dict): # pylint: disable=W0613
        """Handler for new table row"""
        if self.td_index > -1 and self.td_index < 3:
            return
        self.coverage.append(Coverage())
        self.td_index = -1

    def handle_td_start(self, tag, attrs_dict): # pylint: disable=W0613
        """Handler for new table element"""
        if 'class' in attrs_dict:
            if attrs_dict['class'][0:5] == 'cover' and \
               attrs_dict['class'][5:8] != 'Bar':
                self.td_index += 1
        self.in_td = 0

    def handle_data(self, data):
        """Handler for data"""
        if self.td_index == -1:
            return
        if self.in_td != 0:
            return
        if self.td_index == 0:
            if self.in_a > 0:
                self.coverage[-1].file = data
        if self.td_index == 1:
            self.coverage[-1].lines['percentage'] = data
        if self.td_index == 2:
            words = data.split('/')
            self.coverage[-1].lines['covered'] = int(words[0])
            self.coverage[-1].lines['total'] = int(words[1])
        if self.td_index == 3:
            self.coverage[-1].functions['percentage'] = data
        if self.td_index == 4:
            words = data.split('/')
            self.coverage[-1].functions['covered'] = int(words[0])
            self.coverage[-1].functions['total'] = int(words[1])
        if self.td_index == 5:
            self.coverage[-1].branches['percentage'] = data
        if self.td_index == 6:
            words = data.split('/')
            self.coverage[-1].branches['covered'] = int(words[0])
            self.coverage[-1].branches['total'] = int(words[1])
        if self.in_td > -1:
            self.in_td += 1

    def parse_file(self, file_name):
        """Parse a file with the specified file name"""
        html_file = open(file_name)
        for line in html_file.readlines():
            self.feed(line)
        return self.coverage

def total_coverage(coverage_items):
    """
    Sum the coverage of all items in the coverage_items list and return a
    Coverage() item with appropriate elements
    """    
    cover_sum = Coverage()
    cover_sum.file = 'total'
    for item in 'lines', 'functions', 'branches':
        covered = 0
        total = 0
        for coverage in coverage_items:
            covered += coverage.__dict__[item]['covered']
            total += coverage.__dict__[item]['total']
        cover_sum.__dict__[item]['covered'] = covered
        cover_sum.__dict__[item]['total'] = total
        if total > 1e-3:
            cover_sum.__dict__[item]['percentage'] = \
                                                100.*float(covered)/float(total)
    return cover_sum

def maus_filter(coverage_item):
    """
    Return true for items that are in MAUS (i.e. not stl and 
    third_party). Else return false
    """    
    if coverage_item.file[-5:] == 'build':
        return False
    if coverage_item.file.find('MausDataStructure.') > -1:
        return False
    test_path = os.path.expandvars('${MAUS_ROOT_DIR}/src/'+coverage_item.file)
    if os.path.exists(test_path):
        return True    
    return False

def datastructure_filter(coverage_item):
    """
    Return true for items that are in the DataStructure directory
    """
    if coverage_item.file.find('common_cpp/DataStructure') == 0:
        return True
    return False

def datastructure_getter():
    """
    Datastructure needs special handling to ignore ROOT generated
    MausDataStructure code.
    """
    ds_list = LcovCoverageParser().parse_file(
      CPP_COV+'/common_cpp/DataStructure/index.html')
    ds_list = [ds for ds in ds_list if ds.file.find('MausDataStructure.') == -1]
    return total_coverage(ds_list)

def non_legacy_filter(coverage_item):
    """
    Return true for items that are not legacy (i.e. not in src/legacy). Else
    return false
    """
    if coverage_item.file[0:6] == 'legacy':
        return False
    return True

def py_maus_filter(coverage_item):
    """
    Return false for items that are third party python. Else return True.
    """
    filters = ['amqplib', 'bson', 'cdb', 'celery', 'ctypes', 'dateutil',
     'email', 'encodings', 'json', 'kombu', 'matplotlib', 'maus_cpp', 'numpy', 
     'pymongo', 'pytz',
     'stringprep', 'subprocess', 'suds', 'urllib2', 'uu', 'uuid', 'xboa', 'xml',
     'zipfile', 'FileDialog', 'Dialog', 'ROOT']+\
    ['Tkconstants', 'Tkinter', 'UserList', '_LWPCookieJar', '_MozillaCookieJar',
     '_strptime', 'anyjson', 'argparse', 'calendar', 'contextlib', 'cookielib',
     'csv', 'decimal', 'distutils', 'gzip', 'hmac', 'httplib', 'importlib', 
     'io', 'libxml2', 'libxslt', 'md5', 'mimetools', 'numbers', 'platform', 
     'pydoc', 'pylab', 'quopri', 'rfc822', 'rlcompleter', 'shelve', 'shlex', 
     'smtplib']

    module = coverage_item.file.split('.')[0]
    if module in filters:
        return False
    return True

def py_gen_filter(coverage_item):
    """
    Return false for items that were auto-generated

    If items are of the form MapCpp, ReduceCpp, InputCpp, OutputCpp, they were
    generated by SWIG and should be ignored.

    Also return False for item "MAUS", which is auto generated by scons. Else
    return True
    """
    for test in ['InputCpp', 'MapCpp', 'ReduceCpp', 'OutputCpp']:
        if coverage_item.file.find(test) == 0:
            return False
    if coverage_item.file == "MAUS":
        return False
    return True

def lcov_main():
    """
    Extract coverage information from the lcov output and make a few different
    summary reports
    """
    coverage_list = LcovCoverageParser().parse_file(
        os.path.expandvars('$MAUS_ROOT_DIR/doc/cpp_coverage/index.html')
    )
    maus_coverage_list = [x for x in coverage_list if maus_filter(x)]
    maus_coverage_list = [x for x in maus_coverage_list \
                                                 if not datastructure_filter(x)]
    maus_coverage_list.append(datastructure_getter())
    for item in maus_coverage_list:
        print item.file, item.json_repr()['line']['percentage']
    print 'ALL MAUS\n', total_coverage(maus_coverage_list)
    non_legacy_list = [x for x in maus_coverage_list if non_legacy_filter(x)]
    print 'NON-LEGACY MAUS\n', total_coverage(non_legacy_list)
    legacy_list = [x for x in maus_coverage_list if not non_legacy_filter(x)]
    print 'LEGACY MAUS\n', total_coverage(legacy_list)

def py_main():
    """
    Extract coverage information from the nose output and make a few different
    summary reports
    """
    coverage_list = PyCoverageParser().parse_file(
        os.path.expandvars('$MAUS_ROOT_DIR/doc/python_coverage/index.html')
    )
    maus_coverage_list = [x for x in coverage_list if py_maus_filter(x)]
    maus_coverage_list = [x for x in maus_coverage_list if py_gen_filter(x)]
    print 'ALL MAUS\n', total_coverage(maus_coverage_list)


if __name__ == "__main__":
    lcov_main()
    py_main()

