import json
import argparse
import sys
import linecache
import subprocess
import os

DESCRIPTION=\
"""
Convert json output to human readable format. 

By default, output is piped to 'more'. Optionally can save the output to a
user-specified file for browsing in your favourite text editor.
"""

__doc__ = DESCRIPTION

def arg_parser():
    parser = argparse.ArgumentParser(description=DESCRIPTION)
    parser.add_argument('--input_file', dest='input_file', \
                        help='Read in json file with this name', required=True)
    parser.add_argument('--output_file', dest='output_file', \
                        help='Write reformatted output to file with this name, overwriting any existing files', \
                        default='json_to_human_readable_out.tmp')
    parser.add_argument('--indent', dest='indent', \
                        help='Specify the number of spaces to indent each level of the data tree', \
                        default=2, type=int)
    parser.add_argument('-M', '--more', help='Run more on output file after parsing', dest='more', action='store_true', default=True)
    parser.add_argument('-m', '--no_more', help="Don't run more on output file after parsing", dest='more', action='store_false', default=True)
    parser.add_argument('-D', '--delete', help="Delete human readable file before program exit", dest='delete', action='store_true', default=True)
    parser.add_argument('-d', '--no_delete', help="Don't delete human readable file before program exit", dest='delete', action='store_false', default=True)
    parser.add_argument('--start_line', help="First line to parse, counting from 1. Issues an error if --start-line is > number of lines", type=int, default=1)
    parser.add_argument('--end_line', help="Last line to parse, counting from 1. Ignored if < 1.", type=int, default=0)
    return parser

def write_json(sys_args):
        line = '1'
        line_number = 1
        fin = open(sys_args.input_file)
        fout = open(sys_args.output_file, 'w')
        while line_number < sys_args.start_line:
            line = fin.readline()
            line_number += 1
            if line == "":
                raise IOError("File finished before reaching start_line at line "+str(line_number))
        while (line_number <= sys_args.end_line or sys_args.end_line < 0) \
              and line != '':
            line = fin.readline()
            try:
                json_in = json.loads(line)
                json_out = json.dumps(json_in, indent=sys_args.indent)
            except:
                raise IOError("Input file could not be parsed into json format at line "+str(line_number))
            print >>fout, json_out
            line_number += 1
        fout.close() # force to clear any buffer

def main(argv):
    args = arg_parser()
    args_in = args.parse_args(argv)
    write_json(args_in)
    if args_in.more:
        ps = subprocess.Popen(['more', args_in.output_file], stdout=sys.stdout)
        ps.wait()
    if args_in.delete:
        os.remove(args_in.output_file)

if __name__ == "__main__":
    main(sys.argv[1:])

