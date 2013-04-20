# Copyright (c) 2013, Scott Tsai
# 
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


# Usage: gdb -x debugbreak-gdb.py
# (gdb) debugbreak-stepi

import gdb
import re
        
def _gdb_show_version_parse(version_str):
    '''
    >>> s0 = 'This GDB was configured as "x86_64-redhat-linux-gnu".'
    >>> s1 = 'This GDB was configured as "--host=i686-build_pc-linux-gnu --target=arm-linux-gnueabihf".'
    >>> s2 = 'This GDB was configured as "x86_64-unknown-linux-gnu".'
    >>> _gdb_show_version_parse(s0) == dict(target='x86_64-redhat-linux-gnu')
    True
    >>> _gdb_show_version_parse(s1) == dict(host='i686-build_pc-linux-gnu', target='arm-linux-gnueabihf')
    True
    >>> _gdb_show_version_parse(s2) == dict(target='x86_64-unknown-linux-gnu')
    True
    '''
    
    t = version_str
    msg = 'This GDB was configured as "'
    s = t.find(msg)
    if s == -1:
        raise ValueError
    s += len(msg)
    e = t.find('".', s)
    if e == -1:
        raise ValueError

    config = t[s:e]
    d = {}
    for i in config.split():
        i = i.strip()
        if i.startswith('--'):
            (k, v) = i[2:].split('=')
            d[k] = v
        else:
            if not i:
                continue
            d['target'] = i
    return d

def _target_triplet():
    '''
    -> 'arm-linux-gnueabihf' or 'x86_64-redhat-linux-gnu' or ...
        
    >>> import re
    >>> not not re.match(r'\w*-\w*-\w*', target_triplet())
    True
    '''
    t = gdb.execute('show version', to_string=True)
    return _gdb_show_version_parse(t)['target']

def _debugbreak_stepi():
    t = _target_triplet()
    if re.match(r'^arm-', t):
        instn_len = 4 # FIXME: wrong for Thumb mode
        gdb.execute('tbreak *($pc + %d)' % (instn_len,))
        gdb.execute('jump  *($pc + %d)' % (instn_len,))
    else:
        gdb.execute('stepi')

class _DebugBreakStepI(gdb.Command):
    '''Usage: debugbreak-stepi
    Step one instruction after a debug_break() breakpoint hit'''

    def __init__(self):
        gdb.Command.__init__(self, 'debugbreak-stepi', gdb.COMMAND_BREAKPOINTS, gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        _debugbreak_stepi()

_DebugBreakStepI()
