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
from DecoratorUtils import smartDecorator


class ConstMethodAttrModError(Exception):
    def __init__(self, *args):
        super(ConstMethodAttrModError, self).__init__(*args)

## works for both old and new style python classes
@smartDecorator
def const(fn):
    """
    Constness decorator
    
    Decorator to force class methods to obey const-like
    behaviour. Warning, this is not thread safe.

    @param fn the function to be wrapped
    """
    
    def wrapper(self, *args, **kw):
        def nullset(self, name, value):
            raise ConstMethodAttrModError("Method '%s' is declared const so attribute assignment is not allowed." % fn.__name__)
        def nulldel(self, name):
            raise ConstMethodAttrModError("Method '%s' is declared const so attribute deletion is not allowed." % fn.__name__)
        
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
            ret = fn(self, *args, **kw)
        finally:
            if old_setter is not None:
                cls.__setattr__ = old_setter
            else: del cls.__setattr__
            if old_deleter is not None:
                cls.__delattr__ = old_deleter
            else: del cls.__delattr__
    
    return wrapper


## DEMO
##############################################################################

if __name__ == '__main__':
    
    ## old style
    class test:

        def a(self):
            self.a = 14
        
        @const
        def b(self):
            self.b = 98
            
        @const
        def c(self):
            del self.c

    ## new style
    class test2(object):
        
        def a(self):
            self.a = 14
        
        @const
        def b(self):
            self.b = 98
            
        @const
        def c(self):
            del self.c

    

    f=test()
    g=test2()

    f.a()
    g.a()

    print f.__dict__
    print g.__dict__

    try:
        f.b()
    except Exception, e:
        print e
        print f.__dict__

    try:
        g.b()
    except Exception, e:
        print e
        print f.__dict__

    try:
        f.c()
    except Exception, e:
        print e
        print f.__dict__

    try:
        g.c()
    except Exception, e:
        print e
        print f.__dict__

    del f.a
    del g.a

    print f.__dict__
    print g.__dict__

    f.hello = 8
    g.World = 7

    print f.__dict__
    print g.__dict__

    

