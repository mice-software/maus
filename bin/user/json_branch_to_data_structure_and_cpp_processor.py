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

TEST_BRANCH = {
    "a_float":1.,
    "a_object":{},
    "a_string":"",
    "a_bool":True,
    "a_int":1,
    "a_list":[],
    "a_pointer":1,
}

DATA_DIR = "src/common_cpp/TestDataStructure"
PROC_DIR = "src/common_cpp/TestJsonCppProcessors"

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
        extras += "    "+variable['element_type']+" Get"+\
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
        includes = []  # should really be a set...
        for variable in self.variables.values():
            if variable["is_array"]:
                includes += ['#include <vector>']
            if variable['type'] == "std::string":
                includes += ['#include <string>']
            if variable['type'] == "std::map":
                includes += ['#include <map>']
            if variable['is_class']:
                class_file = os.path.join(DATA_DIR, variable['type']+'.hh')
                includes += ['#include "'+class_file+'"']
        includes += ['#include "Rtypes.h"  // ROOT']
        return '\n'.join(includes)

    def typedefs(self):
        typedefs = {}
        for variable in self.variables.values():
            if variable["is_array"]:
                typedefs["typedef std::vector<"+variable["upper_name"]+"> "+\
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
        return "};\n}\n\n#endif  // "+self.ifdef+"\n"

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
        return getter

    def getter_array_extras(self, variable):
        extras = variable['element_type']+" "+self.class_name+"::Get"+\
                      variable['upper_name']+"Element(size_t index) const{\n"
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
        ctor += self.__get_initialisation()+" {\n"
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
                includes['#include "'+\
                      os.path.join(PROC_DIR, var['element_proc']+'.hh')+'"'] = 1
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
                a_ctor += 'new '+var['element_proc']
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
        self.is_array = False
        self.is_pointer = False
        self.is_class = False
        self.branch_name = None
        self.var_name = None
        self.upper_name = None
        self.type = None
        self.proc_type = None
        self.element_type = None
        self.element_proc = None
        self.user_name = None
        self.proc_name = None
        self.is_required = True

    def parse_key_value(self, key, value, is_pointer):
            words = key.split('_')
            self.branch_name = key
            self.var_name = '_'.join(['']+words)
            self.upper_name = ''
            for word in words:
                self.upper_name += string.capwords(word)
            if type(value) == type([]):
                self.type = self.upper_name+'Array'
                self.is_array = True
                self.element_type = self.upper_name
                self.proc_type = "ValueArrayProcessor<"+\
                                                    self.element_type+">"
                self.element_proc = self.element_type+"Processor"
            elif type(value) == type({}) or value == None:
                self.type = self.upper_name
                self.proc_type = self.type+'Processor'
                self.is_class = True
            else:
                self.type, self.proc_type = self.get_primitive(key, value)
            self.is_pointer = is_pointer
            self.user_name = self.var_name[1:]
            self.proc_name = self.var_name+"_proc"
            if self.is_array:
                my_list = [x for x in value if x != None]
                if len(my_list) > 0:
                    if type(my_list[0]) == type({}):
                        self.is_class = True
                    elif type(my_list[0]) == type([]):
                        MESSAGES['warn'].append('Cannot parse list of lists '+\
                                                 'in branch '+self.branch_name)
                    else:
                        self.element_type, self.element_proc = \
                                           self.get_primitive('[]', my_list[0])
                for item in my_list:
                    if type(item) != type(my_list[0]):
                        MESSAGES['warn'].append('Cannot parse list '+self.branch_name+\
                                   ' with mixed types '+str(type(my_list[0]))+\
                                   ' and '+str(type(item)))

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

    def  __setitem__(self, key, value):
        self.__dict__[key] = value

    def  __getitem__(self, key):
        return self.__dict__[key]

class DataBranch:
    def __init__(self, json_object, class_name, json_path, parent=None):
        self.class_name = None
        self.get_class_name(class_name, json_path)
        self.json_path = json_path
        if self.class_name in self.branch_rename.keys():
            self.class_name = self.branch_rename[self.class_name]
        self.json_object = json_object
        self.parent_branch = parent
        self.variables = {}
        self.child_branches = {}
        if type(self.json_object) == type({}):
            self.analyse_object()
        elif type(self.json_object) == type([]):
            self.analyse_array() 
        self.add_to_branch_dict()

    def analyse_object(self):
        if self.class_name in self.class_to_pointers.keys():
            pointers = self.class_to_pointers[self.class_name]
        else:
            pointers = [] 
        for key, value in self.json_object.iteritems():
            variable = DataStructureItem()
            variable.parse_key_value(key, value, key in pointers)
            if (variable.is_class or variable.is_array) and value != None:
                branch_name = variable.upper_name
                if variable.is_array:
                    branch_name += "Array"
                new_branch = DataBranch\
                        (value, branch_name, self.json_path+[key], self)
                self.child_branches[key] = new_branch
            self.variables[key] = variable
        self.add_to_branch_dict()
        return self.variables

    def analyse_array(self):
        if self.class_name in self.class_to_pointers.keys():
            pointers = self.class_to_pointers[self.class_name]
        else:
            pointers = []
        if len(self.json_object) > 0:
            key, value = "[]", self.json_object[0]
            variable = DataStructureItem()
            variable.parse_key_value(self.json_path[-1], value, self.json_path[-1] in pointers)
            if (variable.is_class or variable.is_array) and value != None:
                branch_name = variable.upper_name
                if variable.is_array:
                    branch_name += "Array"
                new_branch = DataBranch\
                        (value, branch_name, self.json_path+[key], self)
                self.child_branches[key] = new_branch
            self.variables[key] = variable
        self.add_to_branch_dict()
        return self.variables

    def get_class_name(self, class_name, json_path):
        if str(json_path) in self.branch_rename.keys():
            self.class_name = self.branch_rename[str(json_path)]
        else:
            self.class_name = class_name

    def add_to_branch_dict(self):
        if self.class_name in self.branch_dict.keys():
            if self.json_path == self.branch_dict[self.class_name].json_path:
                self.merge(self.branch_dict[self.class_name])
            elif str(self.json_path) in self.merge_force:
                self.merge(self.branch_dict[self.class_name])
            else:
                MESSAGES['warn'].append('Classes with same name '+\
                               self.class_name+' but different path')
                parent = self.parent_branch
                class_name =  self.class_name
                while class_name in self.branch_dict and parent != None:
                    if type(parent.json_object) != type([]):
                        class_name = parent.class_name+'_'+class_name
                    parent = parent.parent_branch
                print 'class_name',self.json_path, class_name, self.class_name
                self.class_name = class_name
        self.branch_dict[self.class_name] = self

    def merge(self, another_branch):
        for key in self.variables.keys():
            if key not in another_branch.variables.keys():
                self.variables[key].is_required = False
        for key in another_branch.variables.keys():
            if key not in self.variables.keys():
                self.variables[key] = another_branch.variables[key]
                self.variables[key].is_required = False
        return self

    def similarity(self, another_branch):
        n_in, n_out = 0, 0
        for var in self.variables.keys():
            if var in another_branch.variables.keys():
                n_in += 1
            else:
                n_out += 1
        for var in another_branch.variables.keys():
            if var not in self.variables.keys():
                n_out += 1
        if n_in == 0 and n_out == 0:
            return 1.
        return float(n_in)/float(n_in+n_out)

    def build_data_structure(self):
        MESSAGES['info'].append("Building "+self.class_name)
        fout = open(os.path.join(DATA_DIR, self.class_name+".hh"), "w")
        my_decl = DataStructureDeclaration(self.class_name, self.variables)
        print >>fout, my_decl.class_declaration()
        fout.close()

        fout = open(os.path.join(DATA_DIR, self.class_name+".cc"), "w")
        my_impl = DataStructureImplementation(self.class_name, self.variables)
        print >>fout, my_impl.class_implementation()
        fout.close()

        json_decl = JsonCppProcessorDeclaration(self.class_name, self.variables)
        fout = open(os.path.join(PROC_DIR, json_decl.proc_name+".hh"), "w")
        print >>fout, json_decl.class_declaration()
        fout.close()

        json_impl = JsonCppProcessorImplementation(self.class_name, self.variables)
        fout = open(os.path.join(PROC_DIR, json_impl.proc_name+".cc"), "w")
        print >>fout, json_impl.class_implementation()
        fout.close()

        if len(MESSAGES['at_end']) == 0:
            MESSAGES['at_end'].append("Now add the following lines to LinkDef.hh")
        MESSAGES['at_end'].append("#pragma link C++ class MAUS::"+self.class_name+"+")

    def get_primitive(self, key, value):
        my_tp = type(value)
        if my_tp == type(1):
            return 'int', "IntProcessor"
        elif my_tp == type(""):
            return 'std::string', "StringProcessor"
        elif my_tp == type(1.):
            return 'double', "DoubleProcessor"
        elif my_tp == type(True):
            return 'bool', "BoolProcessor"
        elif my_tp == type([]) or my_tp == type({}):
            return None, None
        else:
            raise TypeError("Did not recognise type of json value "+\
                                         str(key)+":"+str(value))

    class_to_pointers = {}
    branch_dict = {}
    merge_force = {
    }
    branch_rename = {
    }

class DotGenerator:
    def __init__(self):
        self.nodes = {}
        self.connectors = {}

    def get_nodes(self, branch):
        a_json_name = branch.json_path[-1]
        self.nodes[branch.class_name] = ' [shape=record, label="{'+\
                 branch.class_name+'|'+a_json_name+'}"]'
        for name, variable in branch.variables.iteritems():
            if variable.is_array:
                self.nodes[variable.type] = ' [shape=record, label="{'+\
                         variable.type+'| [] }"]'

    def get_connections(self, branch):
        for child_branch in branch.child_branches.values():
            self.connectors[branch.class_name+' -> '+child_branch.class_name] = 1
            for name, variable in branch.variables.iteritems():
                if variable.is_array:
                    self.connectors[branch.class_name+' -> '+variable.type] = 1
                    self.connectors[variable.type+' -> '+variable.element_type] = 1

    def top_matter(self):
        return "digraph G {\n    node [shape=record];"

    def bottom_matter(self):
        return "}"

    def generate_dot_text(self):
        dot_text = self.top_matter()+"\n"
        for key, value in DataBranch.branch_dict.iteritems():
            self.get_nodes(value)
        for key, value in self.nodes.iteritems():
            dot_text += key+value+"\n"
        for name, data_branch in DataBranch.branch_dict.iteritems():
            self.get_connections(data_branch)
        for key in self.connectors.keys():
            dot_text += key+"\n"
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
        fin = open("simulation.out")
        DataBranch.class_to_pointers = {}
        for line in fin.readlines():
            recon_branch = json.loads(line)  
            if recon_branch["daq_event_type"] == "physics_event":
                DataBranch(recon_branch, "Spill", [""])
        for value in DataBranch.branch_dict.values():
            value.build_data_structure()
        DotGenerator().write_dot('test_datastructure.dot')
    except Exception:
        sys.excepthook(*sys.exc_info())
    print "Log:"
#    for msg in MESSAGES['info']:
#        print 'INFO', msg
#    for msg in MESSAGES['warn']:
#        print 'WARN', msg
#    for msg in MESSAGES['at_end']:
#        print msg

if __name__ == "__main__":
    main()



