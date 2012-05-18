#!/usr/bin/env python

#pylint: disable = C0111

import argparse
import pymongo
import sys

DESCRIPTION = \
"""
Delete a collection from MongoDB.
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
                        help='Database', 
                        default="mausdb")
    parser.add_argument('--collection', dest='collection', \
                        help='Collection', 
                        default=None)
    return parser

def delete_database(url, database_name, collection_name):
    """
    Delete the database. If it cannot be found then this is just a 
    no-op.
    @param url URL.
    @param database_name Database name.
    @param collection_name Collection name. If None then the
    database is dropped, else the collection is dropped.
    """
    mongo = pymongo.Connection(url)
    if database_name in mongo.database_names():
        mongodb = mongo[database_name]
        if collection_name == None:
            mongo.drop_database(database_name)
        elif collection_name in mongodb.collection_names():
            mongodb.drop_collection(collection_name)
        else:
            print "Collection %s not found" % collection_name
    else:
        print "Database %s not found" % database_name

def main(argv):
    """
    Main function. Parse any input arguments then delete the database.
    @param argv Command-line arguments.
    """
    args = arg_parser()
    args_in = args.parse_args(argv)
    delete_database(args_in.url, args_in.database, args_in.collection)

if __name__ == "__main__":
    main(sys.argv[1:])
