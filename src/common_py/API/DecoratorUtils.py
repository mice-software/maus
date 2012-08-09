"""
This file defines the smart_decorator
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

## Now handles default args correctly and __doc__ strings
## wrapper defined by user decorator should generally take just *args and **kw
## as these will be modified anyway. Any doc strings defined by user wrapper
## will be picked up
def smart_decorator(decor):
    """
    Smart decorator for decorating decorators
    
    Decorator which wraps a decorator function
    and allows it to dynamically assume the same signature
    as the wrapped function. It also allows for pass through of
    method doc strings

    @param decor the decorator function to be wrapped
    """

    from inspect import getargspec
    arg_spec = getargspec(decor)
    
    arg_string = ''
    arg_string_less_defaults = ''

    if arg_spec[0] is not None:
        default_start = None
        if arg_spec[3] is not None:
            default_start = len(arg_spec[0]) - len(arg_spec[3])
        for i in range(len(arg_spec[0])):
            arg_string += '%s' % arg_spec[0][i]
            arg_string_less_defaults += '%s' % arg_spec[0][i]
            if default_start is not None and i >= default_start:
                arg_string += ' = %s' % arg_spec[3][i-default_start]
            arg_string += ', '
            arg_string_less_defaults += ', '
    else:
        raise AttributeError("Expected decorator method to at least "\
                             "have a 'function' arg")

    if arg_spec[1] is not None:
        arg_string += '*%s, '% arg_spec[1]
        arg_string_less_defaults += '*%s, '% arg_spec[1]
    if arg_spec[2] is not None:
        arg_string += '**%s, '% arg_spec[2]
        arg_string_less_defaults += '**%s, '% arg_spec[2]
    arg_string = arg_string[:-2]
    arg_string_less_defaults = arg_string_less_defaults[:-2]

    dynamic_wrapper = \
"""
def %s(%s):
    from inspect import getargspec
    argSpec = getargspec(%s)

    argString = ''
    argStringLessDefaults = ''

    if argSpec[0] is not None:
        defaultStart = None
        if argSpec[3] is not None:
            defaultStart = len(argSpec[0]) - len(argSpec[3])
        for i in range(len(argSpec[0])):
            argString += '%s' %s
            argStringLessDefaults += '%s' %s
            if defaultStart is not None and i >= defaultStart:
                argString += ' = %s' %s
            argString += ', '
            argStringLessDefaults += ', '
    
    if argSpec[1] is not None:
        argString += '*%s, '%s
        argStringLessDefaults += '*%s, '%s
    if argSpec[2] is not None:
        argString += '**%s, '%s
        argStringLessDefaults += '**%s, '%s
    argString = argString[:-2]
    argStringLessDefaults = argStringLessDefaults[:-2]

    fnDocString = %s.__doc__
    dynamic_wrapper = \
\"\"\"
def %s(%s):
    return decor(%s)(%s)
if decor(%s).__doc__: %s.__doc__ = decor(%s).__doc__
else: %s.__doc__ = fnDocString
\"\"\" %s

    ns = {%s, 'decor':decor, 'fnDocString':fnDocString}
    exec dynamic_wrapper in ns
    return ns[%s]

%s.__doc__ = decor.__doc__

""" % (decor.__name__,
       arg_string,
       arg_spec[0][0],
       '%s',
       '% argSpec[0][i]',
       '%s',
       '% argSpec[0][i]',
       '%s',
       '% argSpec[3][i-defaultStart]',
       '%s',
       '% argSpec[1]',
       '%s',
       '% argSpec[1]',
       '%s',
       '% argSpec[2]',
       '%s',
       '% argSpec[2]',
       arg_spec[0][0],
       '%s',
       '%s',
       arg_string_less_defaults,
       '%s',
       arg_string_less_defaults,
       '%s',
       arg_string_less_defaults,
       '%s',
       '% ('+arg_spec[0][0] + '.__name__, argString, argStringLessDefaults,'+ \
       arg_spec[0][0] + '.__name__, '+arg_spec[0][0] + '.__name__)',
       '\''+arg_spec[0][0]+'\':'+arg_spec[0][0],
       arg_spec[0][0] + '.__name__',
       decor.__name__
       )

    namespace = {'decor':decor}
    exec dynamic_wrapper in namespace #pylint: disable=W0122
    return namespace[decor.__name__]
