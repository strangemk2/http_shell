import sys
import argparse
import subprocess
import mmap

setting_list = [
        {'name':'GET_URL', 'type':'str'},
        {'name':'POST_URL', 'type':'str'},
        {'name':'PASSWORD', 'type':'str'},
        {'name':'INTERVAL', 'type':'num'}
]

def get_setting_offset(filename, setting_name):
    cmd = "objdump -x {0} | grep {1}".format(filename, setting_name)
    output = subprocess.check_output(cmd, shell=True, universal_newlines=True)
    offset = int(output[:output.find(' ')].lstrip('0'), 16) - 0x400000
    return offset

def read_value(mm, setting):
    mm.seek(setting['offset'])
    if setting['type'] == 'str':
        s_offset = (int.from_bytes(mm.read(8), byteorder='little') - 0x400000)
        mm.seek(s_offset)
        data = mm.read(63)
        return data.decode('ascii')
    elif setting['type'] == 'num':
        return int.from_bytes(mm.read(8), byteorder='little')
    else:
        pass

def write_value(mm, value, setting):
    mm.seek(setting['offset'])
    if setting['type'] == 'str':
        s_offset = (int.from_bytes(mm.read(8), byteorder='little') - 0x400000)
        mm.seek(s_offset)
        data = bytes(value.ljust(63, "\0"), 'ascii')
        mm.write(data)
    elif setting['type'] == 'num':
        value = int(value)
        data = value.to_bytes(8, byteorder='little')
        mm.write(data)
    else:
        pass

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Help script for static setting hs.')
    parser.add_argument('--get_url', help='set get_url')
    parser.add_argument('--post_url', help='set post_url')
    parser.add_argument('--password', help='set password')
    parser.add_argument('--interval', help='set interval')
    parser.add_argument('hs', help='hs_binary')
    args = parser.parse_args()
    argv = vars(args)

    file = argv['hs']

    for s in setting_list:
        s["offset"] = get_setting_offset(file, s["name"])

    with open("hs", "r+b") as f:
        with mmap.mmap(f.fileno(), 0) as mm:
            for s in setting_list:
                v = argv[s['name'].lower()]
                if v:
                    print("{0}: {1} -> {2}".format(s['name'], read_value(mm, s), v))
                    write_value(mm, v, s)
                else:
                    print("{0}: {1}".format(s['name'], read_value(mm, s)))
