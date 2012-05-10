# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
#

"""
json_branch_to_ ... builds a C++ tree from a json data file.

json_branch_to_ ... builds both the C++ classes necessary to represent the data
file in C++, a set of converters that will perform conversions between C++ and
JSON, and a .dot file that can be used to create a graphical representation of
the data tree.

Output files are placed in 
DATA_DIR - for DataStructure files
PROC_DIR - for Json <-> C++ converter files
Generally one file/class is created for each object (python dictionary) at each
branch in the tree. Arrays (python lists) are handled by STL vectors.

Note that this script is intended to create a framework that can be developed
on top of, but it is intended that a real person checks the output and makes
alterations, for example specifying that some branches are optional, tweaking
names, etc.

The script is not tested.

Several classes are implemented:
- DataStructureItem performs the analysis of the data structure at a particular
  level in the tree. DataStructureItem is called recursively on the tree to
  build a representation of a given data structure. There is some class data
  in here which can be used to e.g. override the default naming scheme for
  classes. 
- DataStructureDeclaration handles the header file of a class in the data
  structure. There is one main member function, class_declaration(...), which
  generates the file.
- DataStructureImplementation handles the source file of a class in the data
  structure. There is one main member function, class_implementation(...), which
  generates the file.
- JsonCppProcessorDeclaration handles the header file of the C++ <-> JSON 
  converter. There is one main member function, class_declaration(...), which
  generates the file.
- JsonCppProcessorImplementation handles the source file of each C++ <-> JSON
  converter. There is one main member function, class_implementation(...), which
  generates the file.
- DotGenerator handles the generation of the dot (graphviz) file.

The main() function works as an example for calling the code... note that there
are some hardcoded input files in here (hack as you see fit).
"""

import logging
import string
import os   
import sys
import copy
import json

COPYRIGHT= """/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 */
"""

DATA_DIR = "/home/cr67/MAUS/work/delete/DataStructure"
PROC_DIR = "/home/cr67/MAUS/work/delete/JsonCppProcessors"
TEST_DATA_DIR = "/home/cr67/MAUS/work/delete/TestDataStructure"
TEST_PROC_DIR = "/home/cr67/MAUS/work/delete/TestJsonCppProcessors"

MESSAGES = {
"info":[],
"warn":[],
"at_end":[]
}

class DataStructureDeclaration:
    def __init__(self, class_name, variables):
        self.class_name = class_name
        self.class_var = "_"+class_name.lower()
        self.variables = variables
        self.ifdef = "_SRC_COMMON_CPP_DATASTRUCTURE_"+class_name.upper()+"_"

    def getter(self, variable):
        getter = "    /** Returns "+variable["upper_name"]+" */\n"
        getter += "    "+variable["type"]+" "
        getter += "Get"+variable['upper_name']+"() const;"
        if variable["is_array"]:
            getter += "\n\n"+self.getter_array_extras(variable)
        return getter

    def getter_pointer(self, variable):
        getter = "    /** Returns "+variable["upper_name"]+"\n"
        getter += "     *\n"
        getter += "     *   Memory is owned by "+self.class_name+"\n"
        getter += "     */\n"
        getter += "    "+variable["type"]+"* "
        getter += "Get"+variable['upper_name']+"() const;"
        if variable["is_array"]:
            self.getter_array_extras(variable)
        return getter

    def getter_array_extras(self, variable):
        extras = "    /** Get an element from "+variable['upper_name']+\
                                                    " (needed for PyROOT) */\n"
        extras += "    "+variable.element_type()+" Get"+\
                      variable['upper_name']+"Element(size_t index) const;\n\n"
        extras += "    /** Get size of "+variable['upper_name']+\
                                                    " (needed for PyROOT) */\n"
        extras += "    size_t Get"+variable['upper_name']+"Size"\
                                                                   "() const;"
        return extras

    def setter(self, variable):
        setter = "    /** Sets "+variable["upper_name"]+" */\n"
        setter += "    void "
        setter += "Set"+variable['upper_name']+"("
        setter += variable['type']+' '+variable['user_name']+");"
        return setter

    def setter_pointer(self, variable):
        setter = "    /** Sets "+variable["upper_name"]+"\n"
        setter += "     *\n"
        setter += "     *   Memory ownership passes to "+self.class_name+"\n"
        setter += "     */\n"
        setter += "    void "
        setter += "Set"+variable['upper_name']+"("
        setter += variable['type']+'* '+variable['user_name']+");"
        return setter

    def var_definitions(self):
        var_out = "  private:\n"
        for var in self.variables.values():
            var_out += "    "+var['type']
            if var['is_pointer']:
                var_out += "*"
            var_out += " "+var['var_name']+";\n"
        return var_out

    def includes(self):
        includes = {}  # should really be a set...
        for variable in self.variables.values():
            if variable["is_array"]:
                includes['#include <vector>'] = 1
            if variable['type'] == "std::string":
                includes['#include <string>'] = 1
            if variable['type'] == "std::map":
                includes['#include <map>'] = 1
            if variable['is_class']:
                class_file = os.path.join(DATA_DIR, variable['type']+'.hh')
                includes['#include "'+class_file+'"'] = 1
            if variable['is_array']:
                array_el = variable.child_items.values()[0]
                if array_el.is_class:
                    class_file = os.path.join(DATA_DIR, array_el.type+'.hh')
                    includes['#include "'+class_file+'"'] = 1
        includes['#include "Rtypes.h"  // ROOT'] = 1
        return '\n'.join(includes)

    def typedefs(self):
        typedefs = {}
        for variable in self.variables.values():
            if variable["is_array"]:
                typedefs["typedef std::vector<"+variable.element_type()+"> "+\
                                                       variable["type"]+";"] = 1
        return '\n'.join(typedefs.keys())

    def top_matter(self):
        global COPYRIGHT
        top_matter = COPYRIGHT+"\n"

        top_matter += "#ifndef "+self.ifdef+"\n"
        top_matter += "#define "+self.ifdef+"\n\n"
        top_matter += self.includes()+"\n\n"
        top_matter += "namespace MAUS {\n"
        typedefs = self.typedefs()
        if len(typedefs) > 0:
            top_matter += "// Needed for ROOT\n"+typedefs+"\n"
        top_matter += "\n/** @class "+self.class_name+" comment\n *\n"
        for variable in self.variables.keys():
            top_matter += " *  @var "+variable+"  <--description-->\n"
        top_matter += " */\n\n"
        top_matter += "class "+self.class_name+" {\n"
        top_matter += "  public:\n"
        return top_matter

    def bottom_matter(self):
        bottom_matter = "\n    ClassDef("+self.class_name+", 1)\n"
        bottom_matter += "};\n}\n\n#endif  // "+self.ifdef+"\n"
        return bottom_matter

    def ctors(self):
        decl = "    /** Default constructor - initialises to 0/NULL */\n"
        decl += "    "+self.class_name+"();\n\n"
        decl += "    /** Copy constructor - any pointers are deep copied */\n"
        decl += "    "+self.class_name+\
                          "(const "+self.class_name+"& "+self.class_var+");\n\n"
        decl += "    /** Equality operator - any pointers are deep copied */\n"
        decl += "    "+self.class_name+"& operator="+\
                          "(const "+self.class_name+"& "+self.class_var+");\n\n"
        decl += "    /** Destructor - any member pointers are deleted */\n"
        decl += "    ~"+self.class_name+"();\n"
        return decl

    def class_declaration(self):
        class_out = ""
        class_out += self.top_matter()
        class_out += self.ctors()
        for key, var in self.variables.iteritems():
            if var['is_pointer']:
                class_out += "\n\n"+self.getter_pointer(var)
                class_out += "\n\n"+self.setter_pointer(var)
            else:
                class_out += "\n\n"+self.getter(var)
                class_out += "\n\n"+self.setter(var)
        class_out += "\n\n"+self.var_definitions()
        class_out += self.bottom_matter()
        return class_out

class DataStructureImplementation:
    def __init__(self, class_name, variables):
        self.class_name = class_name
        self.class_var = "_"+class_name.lower()
        self.variables = variables

    def getter(self, variable):
        getter = variable["type"]
        if variable['is_pointer']:
            getter += "*"
        getter += " "+self.class_name+"::"
        getter += "Get"+variable['upper_name']+"() const {\n"
        getter += "    return "+variable['var_name']+";\n}"
        if variable.is_array:
            getter += "\n\n"+self.getter_array_extras(variable)
        return getter

    def getter_array_extras(self, variable):
        extras = variable.element_type()+" "+self.class_name+"::Get"+\
                      variable['upper_name']+"Element(size_t index) const {\n"
        extras += "    return "+variable['var_name']+"[index];\n}\n\n"
        extras += "size_t "+self.class_name+"::Get"+variable['upper_name']+\
                                                              "Size() const {\n"
        extras += "    return "+variable['var_name']+".size();\n}"
        return extras

    def setter(self, variable):
        setter = "void "+self.class_name+"::Set"+variable['upper_name']+"("
        setter += variable['type']+' '+variable['user_name']+") {\n"
        setter += "    "+variable['var_name']+" = "+variable['user_name']+";\n}"  
        return setter

    def setter_pointer(self, variable):
        setter = "void "+self.class_name+"::Set"+variable['upper_name']+"("
        setter += variable['type']+'* '+variable['user_name']+") {\n"
        setter += "    if ("+variable['var_name']+" != NULL) {\n"
        setter += "        delete "+variable['var_name']+";\n    }\n"
        setter += "    "+variable['var_name']+" = "+variable['user_name']+";\n"
        setter += "}"
        return setter

    def includes(self):
        return '#include "'+os.path.join(DATA_DIR, self.class_name+".hh")+'"\n'

    def top_matter(self):
        global COPYRIGHT
        top_matter = COPYRIGHT+"\n"
        top_matter += self.includes()+"\n\n"
        top_matter += "namespace MAUS {"
        return top_matter

    def bottom_matter(self):
        return "}\n"

    def __get_initialisation(self):
        if len(self.variables) == 0:
            return ""
        ctor = ["\n    : "]
        for value in self.variables.values():
            extra = value['var_name']+"("
            if value['is_pointer']:
                extra += 'NULL'
            elif value['type'] == 'int':
                extra += '0'
            elif value['type'] == 'double':
                extra += '0.'
            elif value['type'] == 'std::string':
                extra += '""'
            extra += "), "
            if len(ctor[-1]+extra) > 80:
                ctor[-1] = ctor[-1][:-1]
                ctor.append("      ")
            ctor[-1] += extra
        return '\n'.join(ctor)[:-2]

    def default_constructor(self):
        ctor = self.class_name+"::"+self.class_name+"()"
        ctor += self.__get_initialisation()+" {\n}"
        return ctor

    def copy_constructor(self):
        ctor = self.class_name+"::"+self.class_name+\
                             "(const "+self.class_name+"& "+self.class_var+")"
        ctor += self.__get_initialisation(
)+" {\n"
        ctor += "    *this = "+self.class_var+";\n}"
        return ctor

    def equality_operator(self):
        ctor = self.class_name+"& "+self.class_name+"::operator="+\
                           "(const "+self.class_name+"& "+self.class_var+") {\n"
        ctor += "    if (this == &"+self.class_var+") {\n"
        ctor += "        return *this;\n    }\n"
        for var in self.variables.values():
            rhs_var = self.class_var+"."+var['var_name']
            if var['is_pointer']:
                ctor += "    if ("+rhs_var+" == NULL) {\n"
                ctor += "        "+var['var_name']+" = NULL;\n    } else {\n"
                ctor += "        "+var['var_name']+" = new "+var['type']+"(*("+\
                    rhs_var+"));\n    }\n"
            else:
                ctor += "    Set"+var['upper_name']+"("+rhs_var+");\n"
        ctor += "    return *this;\n}"
        return ctor

    def destructor(self):
        ctor = self.class_name+"::~"+self.class_name+"() {\n"
        for var in self.variables.values():
            if var['is_pointer']:
                ctor += "    Set"+var['upper_name']+"(NULL);\n"
        ctor += "}"
        return ctor
        

    def class_implementation(self):
        class_out = ""
        class_out += self.top_matter()+"\n\n"
        class_out += self.default_constructor()+"\n\n"
        class_out += self.copy_constructor()+"\n\n"
        class_out += self.equality_operator()+"\n\n"
        class_out += self.destructor()
        for key, var in self.variables.iteritems():
            if var['is_pointer']:
                class_out += "\n\n"+self.getter(var)
                class_out += "\n\n"+self.setter_pointer(var)
            else:
                class_out += "\n\n"+self.getter(var)
                class_out += "\n\n"+self.setter(var)
        class_out += "\n"+self.bottom_matter()
        return class_out

class JsonCppProcessorDeclaration:
    def __init__(self, class_name, variables):
        self.class_name = class_name
        self.class_var = "_"+class_name.lower()
        self.variables = variables
        self.proc_name = class_name+"Processor"
        self.ifdef = "_SRC_COMMON_CPP_JSONCPPPROCESSORS_"+\
                                                      self.proc_name.upper()+"_"

    def includes(self):
        includes = {}
        includes['#include "'+\
                           os.path.join(DATA_DIR,self.class_name+'.hh')+'"'] = 1
        for var in self.variables.values():
            if var['proc_type'] in ["IntProcessor", "StringProcessor",
                "DoubleProcessor", "BoolProcessor", "UIntProcessor"]:
                includes['#include "'+\
                      os.path.join(PROC_DIR, 'PrimitivesProcessors.hh')+'"'] = 1
            if var['is_array']:
                includes['#include "'+\
                           os.path.join(PROC_DIR, 'ArrayProcessors.hh')+'"'] = 1
                if var.element_is_class():
                    includes['#include "'+\
                          os.path.join(PROC_DIR, var.element_proc_type()+'.hh')+'"'] = 1
            if var['is_class']:
                includes['#include "'+\
                     os.path.join(PROC_DIR, var['type']+'Processor.hh')+'"'] = 1
        includes['#include "'+\
                           os.path.join(PROC_DIR, 'ObjectProcessor.hh')+'"'] = 1
        return "\n".join(sorted(includes.keys()))

    def top_matter(self):
        global COPYRIGHT
        top_matter = COPYRIGHT+"\n"

        top_matter += "#ifndef "+self.ifdef+"\n"
        top_matter += "#define "+self.ifdef+"\n\n"
        top_matter += self.includes()+"\n\n"
        top_matter += "namespace MAUS {\n"
        top_matter += "\n/** @class "+self.proc_name+" Conversions for "+\
                                  self.class_name+" between C++ and Json \n *\n"
        for variable in self.variables.values():
            top_matter += " *  @var "+variable['proc_name']+"_proc "+\
                          "Processor for "+variable['var_name']+"\n"
        top_matter += " */\n\n"
        return top_matter

    def bottom_matter(self):
        bottom_matter = "}  // namespace MAUS\n\n"
        bottom_matter += "#endif  // #define "+self.ifdef+"\n"
        return bottom_matter

    def class_body(self):
        class_body = "class "+self.proc_name+" : public ObjectProcessor<"+\
                                                         self.class_name+"> {\n"
        class_body += "  public:\n"
        class_body += "    /** Set up processors and register branches\n"
        class_body += "     *\n"
        class_body += "     *  Everything else is handled by the base class\n"
        class_body += "     */\n"
        class_body += "    "+self.proc_name+"();\n\n"
        class_body += "  private:\n"
        for var in self.variables.values():
            class_body += "    "+var["proc_type"]+" "+var["proc_name"]+";\n"
        class_body += "};\n"
        return class_body

    def class_declaration(self):
        decl = self.top_matter()+self.class_body()+self.bottom_matter()
        return decl

class JsonCppProcessorImplementation:
    def __init__(self, class_name, variables):
        self.class_name = class_name
        self.class_var = "_"+class_name.lower()
        self.variables = variables
        self.proc_name = class_name+"Processor"

    def top_matter(self):
        global COPYRIGHT
        top_matter = COPYRIGHT+"\n"
        top_matter += '#include "'+\
                            os.path.join(PROC_DIR, self.proc_name+'.hh')+'"\n\n'
        top_matter += "namespace MAUS {\n\n"
        return top_matter

    def constructor(self):
        constructor = self.proc_name+"::"+self.proc_name+"()"
        var_ctors = "  "
        if len(self.variables) > 0:
            var_ctors  = "\n    : "
        for var in self.variables.values():
            a_ctor = var['proc_name']+"("
            if var['is_array']:
                a_ctor += 'new '+var.element_proc_type()
            a_ctor += "), "
            if len(var_ctors+a_ctor) <= 80:
                var_ctors = var_ctors+a_ctor
            else:
                constructor += var_ctors[0:-1]+"\n"
                var_ctors   = "      "+a_ctor
        constructor += var_ctors[0:-2]+" {\n"
        for var in self.variables.values():
            branch_register = "    Register"
            if var['is_pointer']:
                branch_register += "Pointer"
            else:
                branch_register += "Value"
            branch_register += 'Branch\n          ("'+var["branch_name"]+'", &'+\
                        var["proc_name"]+", &"+\
                        self.class_name+"::Get"+var["upper_name"]+",\n      "+\
                        "    &"+self.class_name+"::Set"+var["upper_name"]+","
            if var['is_required']:
                 branch_register += " true);\n"
            else:
                 branch_register += " false);\n"
            constructor += branch_register
        constructor += "}"
        return constructor

    def bottom_matter(self):
        bottom_matter = "}  // namespace MAUS\n\n"
        return bottom_matter

    def class_implementation(self):
        impl = self.top_matter()
        impl += self.constructor()+"\n"
        impl += self.bottom_matter()
        return impl

class DataStructureItem:
    def __init__(self):
        self.is_array = False  # true if this is an array (python list, c++ vector)
        self.is_pointer = False  # true if this is a pointer
        self.is_class = False  # true if this is a class (python dict, json object)

        self.type = None # type - by default same as upper_name for classes and arrays
        self.proc_type = None # type of processor

        self.branch_name = None  # json name of the branch - arrays use parent's branch_name
        self.upper_name = None # name used for C++ function and class (cpp name of the branch)
        self.user_name = None # variable name used in public (set function calls etc)
        self.proc_name = None # name used for private member processor name
        self.var_name = None  # name used to reference private member variables

        self.is_required = True # set to true to force the data to have relevant member
        self.json_path = [] # path to get here through the whole json data tree
        self.parent_branch = None # reference to the parent data structure item - None if root branch
        self.child_items = {} # reference to any child data structure items - arrays only ever have one entry

    def parse_key_value(self, key, value, is_pointer, json_path, parent):
        words = key.split('_')
        self.parent_branch = parent
        self.json_path = json_path
        self.branch_name = key
        self.var_name = '_'.join(['']+words)
        self.upper_name = ''
        if str(self.json_path) in self.branch_to_upper_name.keys():
            self.upper_name = self.branch_to_upper_name[str(json_path)]
        else:
            for word in words:
                self.upper_name += string.capwords(word)
        if type(value) == type([]):
            if self.upper_name[-1] == 's':
                self.upper_name = self.upper_name[:-1]
            self.upper_name += 'Array'
            self.type = self.upper_name
            self.is_array = True
        elif type(value) == type({}) or value == None:
            self.type = self.upper_name
            self.proc_type = self.type+'Processor'
            self.is_class = True
        else:
            self.type, self.proc_type = self.get_primitive(key, value)
        self.is_pointer = is_pointer
        self.user_name = self.var_name[1:]
        self.proc_name = self.var_name+"_proc"
        if self.is_class:
            self.branch_from_object(value, self.json_path)
        if self.is_array:
            self.branch_from_array(value, self.json_path)

    def branch_from_object(self, json_object, json_path):
        if json_object == None:
            json_object = {}
        if self.upper_name in self.class_to_pointers.keys():
            pointers = self.class_to_pointers[self.upper_name]
        else:
            pointers = []
        for key, value in json_object.iteritems():
            variable = DataStructureItem()
            variable.parse_key_value(key, value, key in pointers, self.json_path+[key], self)
            self.child_items[key] = variable
        self.add_to_branch_dict()

    def branch_from_array(self, json_array, json_path):
        my_list = [x for x in json_array if x != None]
        for item in my_list:
            if type(item) != type(my_list[0]):
                MESSAGES['warn'].append('Cannot parse list '+self.branch_name+\
                           ' with mixed types '+str(type(my_list[0]))+\
                           ' and '+str(type(item))+ ' - ignoring')
                return
        if len(my_list) > 0:
            if type(my_list[0]) == type([]):
                MESSAGES['warn'].append('Cannot parse list of lists '+\
                                         'in branch '+self.branch_name)
            else:
                key = self.branch_name
                if key[-1] == 's':
                    key = key[0:-1]
                variable = DataStructureItem()
                variable.parse_key_value(key, my_list[0], False, self.json_path+['[]'], self)
                self.child_items[key] = variable
                self.proc_type = "ValueArrayProcessor<"+\
                                                self.element_type()+">"
        else:
            key = self.branch_name
            if key[-1] == 's':
                key = key[0:-1]
            variable = DataStructureItem()
            variable.parse_key_value(key, {}, False, self.json_path+['[]'], self)
            self.child_items[key] = variable
            self.proc_type = "ValueArrayProcessor<"+\
                                            self.element_type()+">"

        self.add_to_branch_dict()

    def element_is_class(self):
        if self.is_array and len(self.child_items.values()) == 1:
            return self.child_items.values()[0].is_class
        else:
            raise TypeError('Attempt to get element type for non-array')

    def element_type(self):
        if self.is_array and len(self.child_items.values()) == 1:
            return self.child_items.values()[0].type
        else:
            raise TypeError('Attempt to get element type for non-array')

    def element_proc(self):
        if self.is_array and len(self.child_items.values()) == 1:
            return self.child_items.values()[0].proc_name
        else:
            raise TypeError('Attempt to get element type for non-array')

    def element_proc_type(self):
        if self.is_array and len(self.child_items.values()) == 1:
            return self.child_items.values()[0].proc_type
        else:
            raise TypeError('Attempt to get element type for non-array')

    def get_primitive(self, key, value):
        my_tp = type(value)
        if my_tp == type(1):
            return 'int', "IntProcessor"
        elif my_tp == type("") or my_tp == type(u""):
            return 'std::string', "StringProcessor"
        elif my_tp == type(1.):
            return 'double', "DoubleProcessor"
        elif my_tp == type(True):
            return 'bool', "BoolProcessor"
        elif my_tp == type([]) or my_tp == type({}):
            return None, None
        else:
            raise TypeError("Did not recognise type '"+str(my_tp)+\
                                     "' of json value "+str(key)+":"+str(value))

    def build_data_structure(self):
        if self.upper_name in self.disable_dict.keys():
            return
        self.disable_dict[self.upper_name] = 1 # only build once
        MESSAGES['info'].append("Building class "+self.upper_name)
        fout = open(os.path.join(DATA_DIR, self.upper_name+".hh"), "w")
        my_decl = DataStructureDeclaration(self.upper_name, self.child_items)
        print >>fout, my_decl.class_declaration()
        fout.close()

        fout = open(os.path.join(DATA_DIR, self.upper_name+".cc"), "w")
        my_impl = DataStructureImplementation(self.upper_name, self.child_items)
        print >>fout, my_impl.class_implementation()
        fout.close()

        json_decl = JsonCppProcessorDeclaration(self.upper_name, self.child_items)
        fout = open(os.path.join(PROC_DIR, json_decl.proc_name+".hh"), "w")
        print >>fout, json_decl.class_declaration()
        fout.close()

        json_impl = JsonCppProcessorImplementation(self.upper_name, self.child_items)
        fout = open(os.path.join(PROC_DIR, json_impl.proc_name+".cc"), "w")
        print >>fout, json_impl.class_implementation()
        fout.close()

        if len(MESSAGES['at_end']) == 0:
            MESSAGES['at_end'].append("Now add the following lines to LinkDef.hh")
        MESSAGES['at_end'].append("#pragma link C++ class MAUS::"+self.upper_name+"+;")

    def add_to_branch_dict(self):
        self.branch_dict[str(self.json_path)] = self


    def  __setitem__(self, key, value):
        self.__dict__[key] = value

    def  __getitem__(self, key):
        return self.__dict__[key]

    class_to_pointers = {}
    branch_to_upper_name = { # index json_path to cpp branch name
        str([u'recon_events', '[]', u'ckov_event', u'ckov_digits', '[]', u'A']):'CkovA',
        str([u'recon_events', '[]', u'ckov_event', u'ckov_digits', '[]', u'B']):'CkovB',
        str([u'daq_data']):'DAQData',
        str([u'daq_data', u'ckov', '[]']):'CkovDaq',
        str([u'daq_data', u'tof0', '[]']):'TOFDaq',
        str([u'daq_data', u'tof1', '[]']):'TOFDaq',
        str([u'daq_data', u'tof2', '[]']):'TOFDaq',
        str([u'daq_data', u'tof0']):'TOF0Daq',
        str([u'daq_data', u'tof1']):'TOF1Daq',
        str([u'daq_data', u'tof2']):'TOF2Daq',
        str([u'daq_data', u'kl']):'KL',
        str([u'daq_data', u'kl', '[]']):'KLDaq',
        str([u'recon_events', '[]', u'kl_event']):'KLEvent',
        str([u'recon_events', '[]', u'tof_event']):'TOFEvent',
        str([u'recon_events', '[]', u'emr_event']):'EMREvent',
        str([u'recon_events', '[]', u'tof_event', u'tof_digits']):'TOFEventDigit',
        str([u'recon_events', '[]', u'tof_event', u'tof_digits', u'tof0']):'TOF0Digit',
        str([u'recon_events', '[]', u'tof_event', u'tof_digits', u'tof1']):'TOF1Digit',
        str([u'recon_events', '[]', u'tof_event', u'tof_digits', u'tof2']):'TOF2Digit',
        str([u'recon_events', '[]', u'tof_event', u'tof_digits', u'tof0', '[]']):'TOFDigit',
        str([u'recon_events', '[]', u'tof_event', u'tof_digits', u'tof1', '[]']):'TOFDigit',
        str([u'recon_events', '[]', u'tof_event', u'tof_digits', u'tof2', '[]']):'TOFDigit',
        str([u'recon_events', '[]', u'tof_event', u'tof_space_points']):'TOFEventSpacePoint',
        str([u'recon_events', '[]', u'tof_event', u'tof_space_points', u'tof0']):'TOF0SpacePoint',
        str([u'recon_events', '[]', u'tof_event', u'tof_space_points', u'tof1']):'TOF1SpacePoint',
        str([u'recon_events', '[]', u'tof_event', u'tof_space_points', u'tof2']):'TOF2SpacePoint',
        str([u'recon_events', '[]', u'tof_event', u'tof_space_points', u'tof0', '[]']):'TOFSpacePoint',
        str([u'recon_events', '[]', u'tof_event', u'tof_space_points', u'tof1', '[]']):'TOFSpacePoint',
        str([u'recon_events', '[]', u'tof_event', u'tof_space_points', u'tof2', '[]']):'TOFSpacePoint',
        str([u'recon_events', '[]', u'tof_event', u'tof_slab_hits']):'TOFEventSlabHit',
        str([u'recon_events', '[]', u'tof_event', u'tof_slab_hits', u'tof0']):'TOF0SlabHit',
        str([u'recon_events', '[]', u'tof_event', u'tof_slab_hits', u'tof1']):'TOF1SlabHit',
        str([u'recon_events', '[]', u'tof_event', u'tof_slab_hits', u'tof2']):'TOF2SlabHit',
        str([u'recon_events', '[]', u'tof_event', u'tof_slab_hits', u'tof0', '[]']):'TOFSlabHit',
        str([u'recon_events', '[]', u'tof_event', u'tof_slab_hits', u'tof1', '[]']):'TOFSlabHit',
        str([u'recon_events', '[]', u'tof_event', u'tof_slab_hits', u'tof2', '[]']):'TOFSlabHit',
        str([u'mc_events']):'MCEvents',
        str([u'mc_events', '[]', u'primary', u'position']):'ThreeVector',
        str([u'mc_events', '[]', u'primary', u'momentum']):'ThreeVector',
        str([u'mc_events', '[]', u'sci_fi_hits', '[]', u'position']):'ThreeVector',
        str([u'mc_events', '[]', u'sci_fi_hits', '[]', u'momentum']):'ThreeVector',
        str([u'mc_events', '[]', u'sci_fi_hits', '[]', u'channel_id']):'SciFiChannelID',
        str([u'mc_events', '[]', u'special_virtual_hits', '[]', u'position']):'ThreeVector',
        str([u'mc_events', '[]', u'special_virtual_hits', '[]', u'momentum']):'ThreeVector',
        str([u'mc_events', '[]', u'special_virtual_hits', '[]', u'channel_id']):'SpecialVirtualChannelID',
        str([u'mc_events', '[]', u'tof_hits', '[]', u'position']):'ThreeVector',
        str([u'mc_events', '[]', u'tof_hits', '[]', u'momentum']):'ThreeVector',
        str([u'mc_events', '[]', u'tof_hits', '[]', u'channel_id']):'TOFChannelID',
        str([u'mc_events', '[]']):'MCEvent',
    }
    branch_to_type = { # index json path to cpp class type (NOT IMPLEMENTED)
    }
    optional_branches = {
    }
    branch_dict = {}
    disable_dict = {'ReconEvent':1, 'Root':1}


class DotGenerator:
    def __init__(self):
        self.nodes = {}
        self.connectors = {}
        self.node_indents = {}

    def node_name(self, branch):
        node = '_'.join(branch.json_path)
        node = node.replace('[]', 'array_item')
        if node == "":
            node = "root_item"
        return node

    def indent(self, branch):
        if len(branch.json_path) == 0:
          return '  '
        return ' '.join(['']*(len(branch.json_path)+2))        

    def get_nodes(self, branch):
        a_json_name = ""
        if len(branch.json_path) > 0:
            a_json_name = branch.json_path[-1]
        self.nodes[self.node_name(branch)] = ' [shape=record, label="{'+\
                 branch.type+'|'+a_json_name+'}"]'
        self.node_indents[self.node_name(branch)] = self.indent(branch)

    def get_connections(self, branch):
        for child_branch in branch.child_items.values():
            if child_branch.is_class or child_branch.is_array:
                self.connectors[self.node_name(branch)+' -> '+self.node_name(child_branch)] = self.indent(branch)

    def top_matter(self):
        return "digraph G {\n  node [shape=record];"

    def bottom_matter(self):
        return "}"

    def generate_dot_text(self):
        dot_text = self.top_matter()+"\n"
        for key, value in DataStructureItem.branch_dict.iteritems():
            self.get_nodes(value)
        for key in sorted(self.nodes.keys()):
            dot_text += self.node_indents[key]+key+self.nodes[key]+"\n"
        dot_text += '\n\n'
        for name, data_branch in DataStructureItem.branch_dict.iteritems():
            self.get_connections(data_branch)
        for key in sorted(self.connectors.keys()):
            dot_text += self.connectors[key]+key+"\n"
        dot_text += self.bottom_matter()+"\n"
        return dot_text

    def write_dot(self, file_name):
        fout = open(file_name, 'w')
        print >>fout, self.generate_dot_text()
        fout.close()

def main():
    try:
        if os.getcwd() != os.environ["MAUS_ROOT_DIR"]:
            raise OSError("Must be in MAUS_ROOT_DIR to run script")
        for file_name in ["tmp/test_root_io_offline_analysis_IN.json", "tmp/test_root_io_simulate_mice_IN.json"]:
          fin = open(file_name)
          for line in fin.readlines():
              recon_branch = json.loads(line)  
              if recon_branch["daq_event_type"] == "physics_event":
                  branch = DataStructureItem()#recon_branch, "Spill", [""]
                  branch.parse_key_value("spill", recon_branch, False, [], None)
        MESSAGES['warn'].append('Not building '+\
                                    str(DataStructureItem.disable_dict.keys()))
        for value in DataStructureItem.branch_dict.values():
            MESSAGES['info'].append('Found '+str(value.json_path))
            if value.is_class:
                print value.upper_name
                value.build_data_structure()
        DotGenerator().write_dot('test_datastructure.dot')
    except Exception:
        sys.excepthook(*sys.exc_info())
    print "Log:"
    for msg in MESSAGES['info']:
        print 'INFO', msg
    for msg in MESSAGES['warn']:
        print 'WARN', msg
    for msg in MESSAGES['at_end']:
        print msg
    print "NOTE I REDIRECTED OUTPUT TO A STUPID DIRECTORY so that I could play without breaking stuff"

if __name__ == "__main__":
    main()



