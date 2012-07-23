"""
This file defines the const decorator function
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
from API.DecoratorUtils import smart_decorator


class ConstMethodAttrModError(Exception):
    """Exception thrown when an attempt is made to modify
    local variables within a const method"""
    def __init__(self, *args):
        super(ConstMethodAttrModError, self).__init__(*args)

## works for both old and new style python classes
@smart_decorator
def const(func):
    """
    Constness decorator
    
    Decorator to force class methods to obey const-like
    behaviour. Warning, this is not thread safe.

    @param func the function to be wrapped
    """
    
    def wrapper(self, *args, **kw):#pylint: disable=C0111
        def nullset(self, name, value):#pylint: disable=C0111,W0613
            raise ConstMethodAttrModError("Method '%s' is declared const so "\
                                          "attribute assignment is not "\
                                          "allowed." % func.__name__)
        def nulldel(self, name):#pylint: disable=C0111,W0613
            raise ConstMethodAttrModError("Method '%s' is declared const so "\
                                          "attribute deletion is not allowed."\
                                          % func.__name__)
        
        cls = self.__class__
        old_setter = None
        old_deleter = None
        try:
            old_setter  = cls.__dict__['__setattr__']
        except KeyError:
            pass
        try:
            old_deleter = cls.__dict__['__delattr__']
        except KeyError:
            pass
        
        cls.__setattr__ = nullset
        cls.__delattr__ = nulldel
        try:
            return func(self, *args, **kw)
        finally:
            if old_setter is not None:
                cls.__setattr__ = old_setter
            else: del cls.__setattr__
            if old_deleter is not None:
                cls.__delattr__ = old_deleter
            else: del cls.__delattr__

    return wrapper
