#!/usr/bin/env python
Import('env')
import binascii
import os

def CRC32_from_file(filename):
    buf = open(filename,'rb').read()
    buf = (binascii.crc32(buf) & 0xFFFFFFFF)
    return "%08X" % buf
    
def node_checks():
    os.system('node check.js check')

def post_build_action(source, target, env):
    print("Build Complete")
    program_path = target[0].get_abspath()
    print("Program path", program_path)
    print("CRC32 =", CRC32_from_file(program_path))
    node_checks()


env.AddPostAction("$PROGPATH", post_build_action)
