#!/usr/bin/env python

#pylint: disable = C0111

import argparse
import pymongo
import sys

DESCRIPTION = \
"""
Summarise the contents of MongoDB.
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
                        help='MongoDB URL',
                        default="localhost:27017")
    parser.add_argument('--database', dest='database', \
                        help='Database name or ALL for all', 
                        default="mausdb")
    return parser

def summarise_database(url, database_name):
    """
    Summarise the database.
    @param url URL.
    @param database_name Database name or "ALL" for all.
    """
    mongo = pymongo.Connection(url)
    database_names = mongo.database_names()
    if (database_name != "ALL"):
        if (database_name not in database_names):
            print "Database %s not found" % database_name
            return
        else:
            database_names = [database_name]
    for database_name in database_names:
        print "Database: %s" % database_name
        mongodb = mongo[database_name]
        collection_names = mongodb.collection_names()
        if ("system.indexes" in collection_names):
            collection_names.remove("system.indexes")
        if (len(collection_names) == 0):
            print "  No collections"
            continue
        for collection_name in collection_names:
            collection = mongodb[collection_name]
            space = mongodb.validate_collection(collection_name)\
                ["datasize"]
            space_kb = space / 1024
            space_mb = space_kb / 1024
            print "  Collection: %s : %d documents (%d bytes %d Kb %d Mb)" \
                % (collection_name, collection.count(), space, \
                space_kb, space_mb)

def main(argv):
    """
    Main function. Parse any input arguments then delete the database.
    @param argv Command-line arguments.
    """
    args = arg_parser()
    args_in = args.parse_args(argv)
    summarise_database(args_in.url, args_in.database)

if __name__ == "__main__":
    main(sys.argv[1:])
