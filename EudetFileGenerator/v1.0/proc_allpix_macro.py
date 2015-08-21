#!/usr/bin/python
import sys
import re
# ------------------------------------------------
commands = {
    'detIdx' : '/allpix/det/setId',
    'detPos' : '/allpix/det/setPosition',
    'detRot' : '/allpix/det/setRotation',
    'ofpath' : '/allpix/timepixtelescope/setFolderPath',
    'frames' : '/allpix/beam/frames'
}

# ------------------------------------------------
def callback(match):
    return match.group(1) + '\t%(value)s' % {'value':callback.value.next()}

# ------------------------------------------------
def substitute(command, value, data, display = False):
    '''
    >>> To change 'command' value in data <<< \n
    '''        
    # define pattern
    pattern = re.compile(r'(%(command)s)(\s+)(.+)'%{'command':command})

    # check pre match
    pre_match = pattern.findall(data)
    print pre_match
    # make substitution
    callback.value = iter(value) # setup substitution values
    resu, num = pattern.subn(callback, data)

    # check post match
    pos_match = pattern.findall(resu)
    print pos_match

    # check consistance
    if num < 1:
        print '>>> Zero matches found'
        return None
    if len(pre_match) != len(pos_match):
        print '>>> Wrong substitution number'

    # display comparison 
    if display:
        for pre, pos in zip(pre_match, pos_match):
            print '>>> Original line:'
            print '>>>    ' + ''.join(pre)
            print '>>> changed to:'
            print '>>>    ' + ''.join(pos)
    return resu

# ------------------------------------------------
# setup for opening from terminal
def usage():
    print 'Usage:\n python %s <srcDir> <outDir> <commond> <value>' % (sys.argv[0])

# ------------------------------------------------
if __name__ == '__main__':
    if len(sys.argv) < 5:
        usage()
        sys.exit(1)
    else:
        # -- pre-define variables
        data = None
        value = None
        command = None  

        # -- open files
        f = open(sys.argv[1], 'r+')
        w = open(sys.argv[2], 'w')
        data = f.read()

        # -- setup command
        if commands.has_key(sys.argv[3]):
            command = commands[sys.argv[3]]
        else:
            print '>>> new command "%s"' % (sys.argv[3])
            command = sys.argv[3]

        print "Executing command: " + command
        
        # -- do substitution
        exec "value = " + sys.argv[4]
        print sys.argv[4]
        print value
        output = substitute(command, value, data, True)
        
        # -- save results
        if output != None:
            w.write(output)
        else:
            print '>>> No data is written in "%s"' % (sys.argv[1])
