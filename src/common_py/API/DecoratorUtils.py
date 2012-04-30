## Now handles default args correctly and __doc__ strings
## wrapper defined by user decorator should generally take just *args and **kw
## as these will be modified anyway. Any doc strings defined by user wrapper will
## be picked up

def smartDecorator(decor):
    from inspect import getargspec
    argSpec = getargspec(decor)
    
    argString = ''
    argStringLessDefaults = ''

    if argSpec[0] is not None:
        defaultStart = None
        if argSpec[3] is not None: defaultStart = len(argSpec[0]) - len(argSpec[3])
        for i in range(len(argSpec[0])):
            argString += '%s' % argSpec[0][i]
            argStringLessDefaults += '%s' % argSpec[0][i]
            if defaultStart is not None and i >= defaultStart: argString += ' = %s' % argSpec[3][i-defaultStart]
            argString += ', '
            argStringLessDefaults += ', '
    else:
        raise AttributeError("Expected decorator method to at least have a 'function' arg")

    if argSpec[1] is not None:
        argString += '*%s, '% argSpec[1]
        argStringLessDefaults += '*%s, '% argSpec[1]
    if argSpec[2] is not None:
        argString += '**%s, '% argSpec[2]
        argStringLessDefaults += '**%s, '% argSpec[2]
    argString = argString[:-2]
    argStringLessDefaults = argStringLessDefaults[:-2]

    dynamic_wrapper = \
"""
def %s(%s):
    from inspect import getargspec
    argSpec = getargspec(%s)

    argString = ''
    argStringLessDefaults = ''

    if argSpec[0] is not None:
        defaultStart = None
        if argSpec[3] is not None: defaultStart = len(argSpec[0]) - len(argSpec[3])
        for i in range(len(argSpec[0])):
            argString += '%s' %s
            argStringLessDefaults += '%s' %s
            if defaultStart is not None and i >= defaultStart: argString += ' = %s' %s
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
    decor(%s)(%s)
if decor(%s).__doc__: %s.__doc__ = decor(%s).__doc__
else: %s.__doc__ = fnDocString
\"\"\" %s

    ns = {%s, 'decor':decor, 'fnDocString':fnDocString}
    exec dynamic_wrapper in ns
    return ns[%s]

%s.__doc__ = decor.__doc__

""" % (decor.__name__,
       argString,
       argSpec[0][0],
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
       argSpec[0][0],
       '%s',
       '%s',
       argStringLessDefaults,
       '%s',
       argStringLessDefaults,
       '%s',
       argStringLessDefaults,
       '%s',
       '% ('+argSpec[0][0] + '.__name__, argString, argStringLessDefaults,'+argSpec[0][0] + '.__name__, '+argSpec[0][0] + '.__name__)',
       '\''+argSpec[0][0]+'\':'+argSpec[0][0],
       argSpec[0][0] + '.__name__',
       decor.__name__
       )

    ns={'decor':decor}
    exec dynamic_wrapper in ns
    return ns[decor.__name__]


## DEMO
##############################################################################

if __name__ == '__main__':
    @smartDecorator
    def myDec(fn):
        """Hello World"""
        def wrapper(*args,**kw):
            """NEW f label""" #comment this line out to see doc pass-through
            print "Wrapped", args, kw
            return fn(*args,**kw)
        return wrapper

    @myDec
    def f(a,d,c=78,*args,**kw):
        """Function f doc"""
        print "f",a,d,c
        
    def f2(a,d,c=78, *args,**kw):
        """Function f2 doc"""
        print "f2",a,d,c

    # allows us to check the function signature
    from inspect import getargspec as g 

    print "function f name =" ,f.__name__
    print "function f2 name =",f2.__name__
    print "function f signature =" ,g(f)
    print "function f signature =" ,g(f2)
    try: f()
    except Exception, e:
        print e
        
    try: f2()
    except Exception, e:
        print e
    f(1,2,3,4,5,6)
    f2(1,2,3,4,5,6)

    f(a=5,c=98,d='bob',j='hello')
    f2(a=5,c=98,d='bob',j='hello')
    help(f)
    help(f2)
