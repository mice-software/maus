#!/usr/bin/env python

#pylint: disable = C0111

import json
import argparse
import sys
import subprocess
import os

TEMP_FILE = 'json_to_human_readable_out.tmp'

DESCRIPTION = \
"""
Convert json output to human readable format. 

By default, output is written to """+TEMP_FILE+""" and piped to 'less'; then
deleted at the end of the session. Optionally can save the output to a
user-specified file for browsing in your favourite text editor.
"""

# Dynamically set the docstring
__doc__ = DESCRIPTION #pylint: disable = W0622

def arg_parser():
    """
    Parse command line arguments.

    Use -h switch at the command line for information on command line args used.
    """
    parser = argparse.ArgumentParser(description=DESCRIPTION)
    parser.add_argument('--input-file', dest='input_file', \
                        help='Read in json file with this name', required=True)
    parser.add_argument('--output-file', dest='output_file', \
                        help='Write reformatted output to file with this'+\
                             ' name, overwriting any existing files', \
                        default=TEMP_FILE)
    parser.add_argument('--indent', dest='indent', \
                        help='Specify the number of spaces to indent each '+\
                             'level of the data tree', \
                        default=2, type=int)
    parser.add_argument('-L', '--less', help='Run less on output file after '+\
                        'parsing', dest='less', action='store_true',
                        default=True)
    parser.add_argument('-l', '--no-less', help="Don't run less on output '+\
                        'file after parsing", dest='less', action='store_false',
                        default=True)
    parser.add_argument('--start-line', help="First line to parse, counting "+\
                        "from 1. Issues an error if --start-line is > number "+\
                        "of lines", type=int, default=1)
    parser.add_argument('--end-line', help="Last line to parse, counting "+\
                        'from 1. Ignored if < 1', type=int, default=0)
    return parser

def write_json(sys_args):
    """
    Reformat the input json file then write to output file

    Skips the required number of lines to start_line, and loops until end_line.
    If the line is not empty or EOF, but can't be parsed into json format,
    raise an IOError. Closes the output file after writing
    """
    line = '1'
    line_number = 1
    fin = open(sys_args.input_file)
    fout = open(sys_args.output_file, 'w')
    while line_number < sys_args.start_line:
        line = fin.readline()
        line_number += 1
        if line == "":
            raise IOError("File finished before reaching start_line at "+\
                          "line "+str(line_number))
    line = fin.readline()
    while (line_number <= sys_args.end_line or sys_args.end_line < 1) \
          and line != '':
        try:
            json_in = json.loads(line)
            json_out = json.dumps(json_in, indent=sys_args.indent, \
                                                                sort_keys=True)
        except ValueError:
            if line != "\n" and line != "":
                raise IOError("Input file could not be parsed into json "+\
                              "format at line "+str(line_number))  
        print >> fout, json_out
        line_number += 1
        line = fin.readline()
    fout.close() # force to clear any buffer

def main(argv):
    """
    Main loop. Parse any input arguments, then reformat and write the input
    file to some output file. Run less if required. if no output filename was
    specified assume the output isn't for keeping so delete it.
    """
    args = arg_parser()
    args_in = args.parse_args(argv)
    write_json(args_in)
    if args_in.less:
        less_proc = subprocess.Popen(['less', args_in.output_file],
                                     stdout=sys.stdout)
        less_proc.wait()
    if args_in.output_file == TEMP_FILE:
        os.remove(args_in.output_file)

if __name__ == "__main__":
    main(sys.argv[1:])

