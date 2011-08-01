import json
import SpillSchema

def parse_json_schema(json_schema, file_handle):
    __parse_json_schema_recursive("spill", json_schema, file_handle)
  

def __parse_json_schema_recursive(item_name, json_schema, file_handle, depth=0):
    item_name = string_to_latex(item_name)
    print >>file_handle, "\\indent "*depth+item_name,
    if "description" in json_schema: 
        description = string_to_latex(json_schema["description"])
        print >>file_handle, "-",description+"\\newline"
    else: print >>file_handle,"\\newline"
    if "type" in json_schema and json_schema["type"] == "object":
        for key, value in json_schema["properties"].iteritems():
            __parse_json_schema_recursive(key, value, file_handle, depth+1)
    if "type" in json_schema and json_schema["type"] == "array":
        for value in json_schema["items"]:
            __parse_json_schema_recursive("\emph{array item}", value, file_handle, depth+1)

def string_to_latex(string_in):
    return string_in.replace('_', '\\_')

if __name__ == "__main__":
  spill_schema_fh = open('spill_schema.tex', 'w')
  json_schema = SpillSchema.spill
  parse_json_schema(json_schema, spill_schema_fh)

