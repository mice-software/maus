"""
An automatically generated wrapper for C++ maps using the C / Python API.
This comes from a template file in src/map/Templates/MapCpp Template.py,
with python file src/common_py/maus_build_tools/module_builder.py applying
a search / replace for the map name.
(space added so that the name isn't changed when python does it's search
/ replace).
"""
import _MapCppTemplate # pylint:disable = F0401

class MapCppTemplate():
    """ Automatically generated wrapper for _MapCppTemplate"""

    def birth(self, *args):
        """
        birth calls the birth of the C class
        """
        return _MapCppTemplate.birth(self.this, *args)

    def death(self):
        """
        death calls the death of the C class
        """
        return _MapCppTemplate.death(self.this)

    def process(self, *args):
        """
        process calls the process of the C class
        """
        return _MapCppTemplate.process(self.this, *args)

    def __init__(self):
        this = _MapCppTemplate.module_new()
        try:
            self.this.append(this)
        except: # pylint:disable = W0702
            self.this = this

    def __del__(self):
        _MapCppTemplate.module_delete(self.this)

    def set_input(self, *args):
        """
        Sets the input type (MAUS::Data or Json::Value) that a
        MapBase inherited class will receive.  This is provided by the
        templating fucntionality of MapBase.
        """
        return _MapCppTemplate.set_input(self.this, *args)

    def get_output(self):
        """
        Returns the output type (MAUS::Data or Json::Value) that a
        MapBase inherited class will return.  This is provided by the
        templating fucntionality of MapBase.
        """
        return _MapCppTemplate.get_output(self.this)

    def can_convert(self):# pylint:disable = R0201
        """
        Confirm that this class is cable of converting between the
        different inputs of MAUS::Data, Json::Value and std::string.
        It will be passed whichever output was provided by the
        previous mapper.
        """
        return True
