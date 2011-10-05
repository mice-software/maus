"""
Parses json schema + "description" field into a bunch of latex itemized lists.
Sees latex maximum itemization depth and splits into a bunch of sublists as it
goes.
"""

import Queue
import json
import SpillSchema

LIST_BEGIN = '\\begin{itemize}' #[itemsep=0pt] -- latex2html cant do this
LIST_END = '\\end{itemize}'
UNPARSED = Queue.Queue()

def parse_json_schema(my_json_schema, my_ancestors, file_handle, separator="/"):
    """
    Parse json schema description fields into a latex document. 
        my_json_schema = recursively walk the json schema tree and add items to
                      latex itemize lists. Need to break every time we get to
                      four deep as this is the latex max itemize depth. If we
                      are forced to break, will add unparsed items to the
                      unparsed queue (for parsing in a second loop).
        my_ancestors = list of ancestor items. Used for naming the root of the
                    documentation tree, given by ancestors split by separator
        file_handle = file handle to which schema docs are written
        separator = separates items in the ancestor list
    """
    first_word = my_ancestors[0]
    for item in my_ancestors[1:]:
        first_word += str(separator)+str(item)
    print >> file_handle, '\\noindent', first_word+":"
    __parse_json_schema_recursive \
                         (my_json_schema, my_ancestors, file_handle)
    print >> file_handle

def __parse_schema(my_json_schema):
    """
    Parse items in the documentation into a format acceptable for latex (just does
    string find/replace on the schema unicode)
    """
    schema_as_text = str(json.dumps(my_json_schema))
    # escape once for python, once for json
    schema_as_text = schema_as_text.replace('_', '\\\\_')
    return json.loads(unicode(schema_as_text))

def __write_description(item_name, item, file_handle, depth):
    """
    Write the description for a particular item
        item_name = the name to be written
        item = json schema tree for this item and below. Looks for description
               value in the item and uses this for documentation
        depth = depth of recursion in the tree. Prepend spaces to the latex
                source to make it easier to read
    """
    file_handle.write(depth*' '+'\\item '+str(item_name))
    if "description" in item:
        file_handle.write(': '+str(item["description"])+'\n')
    else:
        file_handle.write('\n')

def __will_break_recursion(my_json_schema, depth):
    """
    Check to see if we should break the recursion at this level
       json_schema = schema tree for this level and below
       depth = depth of recursion reached so far
    This is necessary because latex can only itemize to a maximum depth of 5.
    The enumitem package is supposed to let us go deeper, but I couldn't figure
    it out (broken?)
    """
    if depth == 4 and \
        (my_json_schema["type"] == "object" or \
         my_json_schema["type"] == "array"):
        return True
    if "breakdoc" in my_json_schema.keys() and my_json_schema["breakdoc"] \
       and depth != 0:
        return True
    return False
    

def __parse_json_schema_recursive(my_schema, my_ancestors_, \
                                   file_handle, depth=0):
    """
    Recursively walk the json schema; if depth exceeds latex maximum, break and
    add anything that didn't get parsed to the unparsed queue.
    """
    if __will_break_recursion(my_schema, depth):
        UNPARSED.put( (my_schema, my_ancestors_) )
        return
    if "type" in my_schema and my_schema["type"] == "object" and \
        len(my_schema["properties"]) > 0:
        print >> file_handle, ' '*depth, LIST_BEGIN
        for key, value in my_schema["properties"].iteritems():
            __write_description(key, value, file_handle, depth+1)
            __parse_json_schema_recursive \
                         (value, my_ancestors_+[key], file_handle, depth+1)
        print >> file_handle, ' '*depth, LIST_END

    if "type" in my_schema and \
        my_schema["type"] == "array" and len(my_schema["items"]) > 0:
        print >> file_handle, ' '*depth, LIST_BEGIN
        key = "\\emph{array item}"
        for value in my_schema["items"]:
            __write_description(key, value, file_handle, depth+1)
            __parse_json_schema_recursive \
                         (value, my_ancestors_+[key], file_handle, depth+1)
        print >> file_handle, ' '*depth, LIST_END

def head_matter(file_handle, schema_name):
    """
    Make a header based on the schema name
    """
    pass

def main():
    """
    Run the main program - creates documentation for:
    * SpillSchema.py
    """
    spill_schema_fh = open('spill_schema.tex', 'w')
    head_matter(spill_schema_fh, "Spill data structure")
    json_schema = __parse_schema(SpillSchema.spill)
    parse_json_schema(json_schema, ["spill"], spill_schema_fh)
    while not UNPARSED.empty():
        (json_schema, ancestors) = UNPARSED.get()
        parse_json_schema(json_schema, ancestors, spill_schema_fh)

if __name__ == "__main__":
    main()

