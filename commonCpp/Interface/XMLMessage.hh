// XMLMessage.hh
//
// Interface to handle the XML messages that will travel between G4MICE and the Configuration DB's API
//
// M.Ellis June 2010

#ifndef XMLMESSAGE_HH
#define XMLMESSAGE_HH 1

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

class XMLMessage
{
  public :

    XMLMessage();
    XMLMessage( const std::string& text );

    virtual ~XMLMessage();

    void		readFile( const std::string& filename );
	 void		MakeRunInfoRequest(int run);
    string		text() const;
    int			size() const;

	 string  GetValueString(string name, bool raw=false);
	 string  GetRValueString(string name1, string name2, bool raw=false);

	 int     GetValueInt(string name);
	 double  GetValueDouble(string name);
	 int  	GetRValueInt(string name1, string name2);
	 double  GetRValueDouble(string name1, string name2);

  private :

    string _message;
};

#endif

