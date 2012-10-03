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

class Coverage(): # pylint: disable=R0903
    """
    Data structure reflecting lcov coverage information
    """
    def __init__(self):
        """Initialise data to 0."""
        self.file = ""
        self.lines = {"percentage":0., "covered":0., "total":0.}
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

class CoverageParser(HTMLParser.HTMLParser): # pylint: disable=R0904
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
        cover_sum.__dict__[item]['percentage'] = float(covered)/float(total)
    return cover_sum

def maus_filter(coverage_item):
    """
    Return true for items that are in MAUS (i.e. not stl and 
    third_party). Else return false
    """    
    if coverage_item.file[0:3] != 'src':
        return False
    if coverage_item.file[-5:] == 'build':
        return False
    return True    

def datastructure_filter(coverage_item):
    """
    Return true for items that are in the DataStructure directory
    """
    if coverage_item.file.find('src/common_cpp/DataStructure') == 0:
        return True
    return False

def non_legacy_filter(coverage_item):
    """
    Return true for items that are not legacy (i.e. not in src/legacy). Else
    return false
    """
    if coverage_item.file[4:10] == 'legacy':
        return False
    return True

def main():
    """
    Extract coverage information from the lcov output and make a few different
    summary reports
    """
    coverage_list = CoverageParser().parse_file(
        os.path.expandvars('$MAUS_ROOT_DIR/doc/cpp_coverage/index.html')
    )   
    maus_coverage_list = [x for x in coverage_list if maus_filter(x)]
    for item in maus_coverage_list:
        print item.file
    print 'ALL MAUS\n', total_coverage(maus_coverage_list)
    non_legacy_list = [x for x in maus_coverage_list if non_legacy_filter(x)]
    print 'NON-LEGACY MAUS\n', total_coverage(non_legacy_list)
    legacy_list = [x for x in maus_coverage_list if not non_legacy_filter(x)]
    print 'LEGACY MAUS\n', total_coverage(legacy_list)
    not_ds_list = [x for x in maus_coverage_list if not datastructure_filter(x)]
    print 'MAUS EXCLUDING DATASTRUCTURE\n', total_coverage(not_ds_list)
    not_legacy_not_ds_list = [x for x in not_ds_list if non_legacy_filter(x)]
    print 'MAUS EXCLUDING DATASTRUCTURE AND LEGACY\n', \
          total_coverage(not_legacy_not_ds_list)

if __name__ == "__main__":
    main()
