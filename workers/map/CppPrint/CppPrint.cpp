/** @class CppPrint
 *  Print events to some stream
 *
 *  @author Christopher Tunnell <c.tunnell1@physics.ox.ac.uk>
 *
 *  processor which simulates the trigger.
 *
 *  The "update" parameter controls the update interval.  See SetI()
 *
 */
#include <iostream>
#include <string>
#include <TMap.h>
#include "CppPrint.h"

CppPrint::CppPrint()
  { 
  }

  /** Destroy count processor. */
//CppPrint::~CppPrint() 
//  { } 
  
/*int CppPrint::ProcessEvent(TMap run, TMap spill, TMap event) 
  { 
    return x+y;
    }*/

char* CppPrint::Process(char* document){
  Json::Value root;   // will contains the root value after parsing.
  Json::Reader reader;
  std::string somestring(document);
  bool parsingSuccessful = reader.parse(somestring, root);
  if ( !parsingSuccessful )
    {
      // report to the user the failure and their locations in the document.
      std::cout  << "Failed to parse configuration\n"
		 << reader.getFormatedErrorMessages();
      return NULL;
    }

  // Get the value of the member of root named 'encoding', return 'UTF-8' if there is no
  // such member.
  std::string encoding = root.get("encoding", "UTF-8" ).asString();
  // Get the value of the member of root named 'encoding', return a 'null' value if
  // there is no such member.
  const Json::Value plugins = root["plug-ins"];
  for ( int index = 0; index < plugins.size(); ++index )  // Iterates over the sequence elements.
    std::cout<< plugins[index].asString()<<std::endl;
   

  Json::StyledWriter writer;
  // Make a new JSON document for the configuration. Preserve original comments.
  std::string outputConfig = writer.write( root );

  // You can also use streams.  This will put the contents of any JSON
  // stream at a particular sub-value, if you'd like.
  //std::cin >> root["subtree"];

  // And you can write to a stream, using the StyledWriter automatically.
  std::cout << root;


    std::cout<<document<<std::endl;

    return NULL;
  }
