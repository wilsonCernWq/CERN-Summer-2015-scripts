#!/usr/bin/python
import sys, re
vars_dirt = {}
vars_name = []
vars_rank = []
vars_indx = []
NODE_NUM = '0'
#-------------------------------------------------------------------------------------------
# @brief variable type: iter (iterator
class mvar:
    '''
    Iterative variable type, definition contains: init, step, snum, func
    >>> Sample:
    <var> 
      snum = 3,
      func = 4 * x + 5 
    <var>
    '''
    # Initialization
    def __init__(self, name, contents):
        self.__level = len(vars_dirt) - 1
        self.__vname = name
        self.__scope = {} # private variable
        self.__parse(contents) # private function
    
    # general definition paser
    def __parse(self, items):
        # items must be in form of: [(key, value),...]
        for (key, value) in items:
            self.__scope[key] = value # stored as string

        # check error
        if not self.__scope.has_key("rank"):
            raise NameError('missing "rank" value in <%s>' % (self.__vname))
        if not self.__scope.has_key("func"):
            raise NameError('missing "func" value in <%s>' % (self.__vname))

    # functor
    def __func(self, x):
        # define scope variables
        for key, value in self.__scope.items():
            if key != 'func':
                exec '%s = %s' % (key, value)
        # calcute function
        return eval(self.__scope['func'])

    # value accessor
    def value(self, x):
        return self.__func(x)

    # terms accessor
    def terms(self, name):
        return self.__scope[name]

    # terms accessor
    def level(self):
        return self.__level

#-------------------------------------------------------------------------------------------
def read_var_def(filename):
    # openfile
    f = open(filename, 'r+')
    d = f.read()
    f.close()

    # format: search each local definition
    p = re.compile(r'<(\w+)>(.+)<\1>',16)
    i = p.finditer(d)
    
    # loop results
    m = None
    while True:
        # test iteration
        try:
            m = i.next()
        except StopIteration:
            break

        # record variable
        var_name = m.group(1)
        var_defs = m.group(2)
        vars_dirt[var_name] = None

        # secondary search 
        msub = []
        items = var_defs.split(',')
        for item in items:
            pair = item.strip('\n').split('=')
            if len(pair) == 1 and len(items) == 1:
                msub = [('rank', '1'), ('func', pair[0].strip())]
            else:
                try:
                    msub.append((pair[0].strip(), pair[1].strip()))
                except IndexError:
                    raise Exception('invalid definition: <%s> %s' % (var_name, var_defs))        

        var = mvar(var_name, msub)
        vars_dirt[var_name] = var
        vars_name.append(var_name)
        vars_rank.append(eval(var.terms("rank")))
        vars_indx.append(0)

#-------------------------------------------------------------------------------------------
def node_num(format):
    s = 0
    for i in range(len(vars_dirt)):
        if i != len(vars_dirt)-1:
            s += vars_rank[i+1] * vars_indx[i]
        else:
            s += vars_indx[i]
    print "built newfile -->", vars_indx, "-->", s
    return ('%' + format + 'd') % (s)

def update_var():
    for i in range(len(vars_name)):
        var = vars_dirt[vars_name[i]]
        value = var.value(vars_indx[var.level()])
        exec 'global %s\n%s = value' % (vars_name[i],vars_name[i])
    
def callback(m):
    var = vars_dirt[m.group(1)]
    return str(var.value(vars_indx[var.level()]))

def substitute(ifile, ofile, surfix, format):
    # openfile
    i = open(ifile, 'r+')
    d = i.read()
    i.close()
    
    p = re.compile(r'@(\w+)@')
    # make exec line
    line = ''
    indentation = ''

    # build loop
    for i in range(len(vars_rank)):
        line += indentation + 'for vars_indx[%d] in range(%d):\n' % (i,vars_rank[i])
        indentation += '\t'

    # build in loop command
    line += indentation + 'global NODE_NUM\n'
    line += indentation + 'NODE_NUM = node_num("%s")\n' % (format)
    line += indentation + 'update_var()\n'
    line += indentation + 'o = open(ofile + "-" + NODE_NUM + "%s", "w")\n' % (surfix)
    line += indentation + 'r = p.sub(callback, d)\n'
    line += indentation + 'o.write(r)'
    exec line
    
if __name__ == "__main__":
    read_var_def(sys.argv[2])
    substitute(sys.argv[1], sys.argv[3], sys.argv[4], sys.argv[5])
