#!/usr/bin/env python

#pylint: disable = C0111

import argparse
import couchdb
import sys

DESCRIPTION = \
"""
Delete a database from CouchDB.
"""

# Dynamically set the docstring
__doc__ = DESCRIPTION #pylint: disable = W0622

def arg_parser():
    """
    Parse command line arguments.

    Use -h switch at the command line for information on command line
    args used. 
    """
    parser = argparse.ArgumentParser(description=DESCRIPTION)
    parser.add_argument('--url', dest='url', \
                        help='CouchDB URL',
                        default="http://localhost:5984")
    parser.add_argument('--database', dest='database', \
                        help='Database', required=True)
    return parser

def delete_database(url, database_name):
    """
    Delete the database. If it cannot be found then this is just a 
    no-op.
    @param url CouchDB URL.
    @param database_name Database name.
    """
    server = couchdb.Server(url)
    if database_name in server:
        server.delete(database_name)

def main(argv):
    """
    Main function. Parse any input arguments then delete the database.
    @param argv Command-line arguments.
    """
    args = arg_parser()
    args_in = args.parse_args(argv)
    delete_database(args_in.url, args_in.database)

if __name__ == "__main__":
    main(sys.argv[1:])
