#!/usr/bin/env python3
import sys
import argparse
from struct import *

setting_list = [
        {'name':'get_url', 'type':'str'},
        {'name':'post_url', 'type':'str'},
        {'name':'password', 'type':'str'},
        {'name':'interval', 'type':'num'},
        {'name':'magic', 'type':'str'}
]
magic_structure = "<128s128s64sI8s";

def setting_index(setting_list, name):
    for i in range(len(setting_list)):
        if setting_list[i]['name'] == name:
            return i

class magic_tail:
    def __init__(self, magic_data = None):
        if not magic_data:
            magic_data = b"\0" * calcsize(magic_structure)
        self.data = unpack(magic_structure, magic_data)

    def size():
        return calcsize(magic_structure)

    def read(self, filename):
        with open(filename, 'rb') as f:
            magic_size = calcsize(magic_structure)
            f.seek(-magic_size, 2)
            magic_data = f.read(magic_size)

            self.data = unpack(magic_structure, magic_data)
        return self

    def update(self, argv):
        magic_list = []
        for i, setting in enumerate(setting_list):
            data = b''
            if argv[setting['name']]:
                if setting['type'] == 'str':
                    data = bytes(argv[setting['name']].ljust(64, "\0"), 'ascii')
                elif setting['type'] == 'num':
                    data = int(argv[setting['name']])
                magic_list.append(data)
            else:
                magic_list.append(self.data[i])
        self.data = tuple(magic_list)
        return self

    def pack(self):
        # print(pack(magic_structure, "abc".encode('ascii'), "def".encode('ascii'), "pass".encode('ascii'), 60, "magic".encode('ascii')))
        return pack(magic_structure, *self.data)

    def dump(self):
        for setting, data in zip(setting_list, self.data):
            if setting['type'] == 'str':
                d = data.decode()
            elif setting['type'] == 'num':
                d = data
            print("{0}: [{1}]".format(setting['name'], d))

    def is_magic_existed(self):
        return self.data[setting_index(setting_list, 'magic')].decode() == "hsmagic\0"

def do_append(filename, data):
    with open(filename, 'rb') as f:
        d = f.read()
    with open(filename, 'wb') as f:
        f.write(d)
        f.write(data)

def do_remove(filename):
    with open(filename, 'rb') as f:
        data = f.read()
    with open(filename, 'wb') as f:
        size = magic_tail.size()
        f.write(data[0:-size])

def do_modify(filename, data):
    with open(filename, 'rb') as f:
        d = f.read()
    with open(filename, 'wb') as f:
        size = magic_tail.size()
        f.write(d[0:-size])
        f.write(data)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Help script for static setting hs.')
    parser.add_argument('--get_url', help='set get_url')
    parser.add_argument('--post_url', help='set post_url')
    parser.add_argument('--password', help='set password')
    parser.add_argument('--interval', help='set interval')
    parser.add_argument('-a', '--append', nargs='?', const='1', help='set interval')
    parser.add_argument('-r', '--remove', nargs='?', const='1', help='set interval')
    parser.add_argument('hs', help='hs_binary')
    args = parser.parse_args()
    argv = vars(args)
    argv['magic'] = 'hsmagic'

    file = argv['hs']

    mt = magic_tail()
    mt.read(file)
    is_magic_existed = mt.is_magic_existed()
    if not is_magic_existed:
        mt = magic_tail()

    if argv['append'] and argv['remove']:
        print("Only one action is allowed.")
        sys.exit(1)
    elif argv['append'] and is_magic_existed:
        print("Magic tail already existed.")
        sys.exit(1)
    elif argv['remove'] and (not is_magic_existed):
        print('No magic tail found.')
        sys.exit(1)
    elif (not argv['append']) and (not argv['remove']):
        if not is_magic_existed:
            print('No magic tail found.')
            sys.exit(1)
        else:
            do_modify(file, mt.update(argv).pack())
    elif argv['append']:
        do_append(file, mt.update(argv).pack())
    elif argv['remove']:
        do_remove(file)

    if not argv['remove']:
        mt.dump()
