# json schema to define (and validate) a json schema. Hack draft-03 to make it
# consistent with validator

# CHANGES (Rogers):
#  * capitalise true, false to make it pythonic
#  * Remove unsupported tags: uniqueItems, $schema, additionalProperties, $ref 

schema = {
	"id" : "http://json-schema.org/draft-03/schema#",
	"type" : "object",
	
	"properties" : {
		"type" : {
			"type" : ["string", "array"],
			"items" : {
				"type" : ["string"]
			},
			"default" : "any"
		},
		
		"properties" : {
			"type" : "object",
			"default" : {}
		},
		
		"patternProperties" : {
			"type" : "object",
			"default" : {}
		},
		
		"items" : {
			"type" : ["array"],
			"items" : {},
			"default" : {}
		},
		
		"additionalItems" : {
			"type" : ["boolean"],
			"default" : {}
		},


    "optionalItems" : {
      "type" : ["array"],
      "default": {}
    },
		
		"required" : {
			"type" : "boolean",
			"default" : False
		},
		
		"minimum" : {
			"type" : "number"
		},
		
		"maximum" : {
			"type" : "number"
		},
		
		"exclusiveMaximum" : {
			"type" : "boolean",
			"default" : False
		},
		
		"minItems" : {
			"type" : "integer",
			"minimum" : 0,
			"default" : 0
		},
		
		"maxItems" : {
			"type" : "integer",
			"minimum" : 0
		},
		
		"pattern" : {
			"type" : "string",
			"format" : "regex"
		},
		
		"minLength" : {
			"type" : "integer",
			"minimum" : 0,
			"default" : 0
		},
		
		"maxLength" : {
			"type" : "integer"
		},
		
		"enum" : {
			"type" : "array",
			"minItems" : 1,
		},
		
		"default" : {
			"type" : "any"
		},
		
		"title" : {
			"type" : "string"
		},
		
		"description" : {
			"type" : "string"
		},
		
		"format" : {
			"type" : "string"
		},
		
		"divisibleBy" : {
			"type" : "number",
			"minimum" : 0,
			"default" : 1
		},
		
		"disallow" : {
			"type" : ["string", "array"],
			"items" : {
				"type" : ["string"]
			},
		},
		
		"extends" : {
			"type" : ["array"],
			"items" : {},
			"default" : {}
		},
		
		"id" : {
			"type" : "string",
			"format" : "uri"
		},
		
	},
	
	"default" : {}
}

