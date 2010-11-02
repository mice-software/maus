// XMLMessage.cc
//
// Interface to handle the XML messages that will travel between G4MICE and the Configuration DB's API
//
// M.Ellis June 2010

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>

#include "XMLMessage.hh"

XMLMessage::XMLMessage()
{
  _message = std::string( "" );
}

XMLMessage::XMLMessage( const std::string& text )
{
  _message = text;
}

XMLMessage::~XMLMessage()
{
}

void		XMLMessage::readFile( const std::string& filename )
{
  // attempt to open the xml file

  std::ifstream inf( filename.c_str() );

  if( ! inf )
  {
    std::cerr << "ERROR! Unable to open the file " << filename << std::endl;
    _message = std::string( "" );
    return;
  }

  // read the XML file in

  std::filebuf* pbuf = inf.rdbuf();

  long size = pbuf->pubseekoff( 0, std::ios::end, std::ios::in );

  pbuf->pubseekpos( 0, std::ios::in );

  char* buffer = new char[size];

  pbuf->sgetn( buffer, size );

  inf.close();

  _message = std::string( buffer );
}

void XMLMessage::MakeRunInfoRequest(int run)
{
  std::stringstream str;
  str << "<?xml version='1.0' encoding='UTF-8'?>" << endl;
  str << "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">" << endl;
  str << "<SOAP-ENV:Body>" << endl;
  str << "<ns1:getSetValues xmlns:ns1=\"urn:miceapi6\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">" << endl;
  str << "<ns1:input xsi:type=\"xsd:int\">" << run <<"</ns1:input>" << endl;
  str << "</ns1:getSetValues>" << endl;
  str << "</SOAP-ENV:Body>" << endl;
  str << "</SOAP-ENV:Envelope>" << endl;

  _message =  str.str();
}

std::string	XMLMessage::text() const
{
  return _message;
}

int		XMLMessage::size() const
{
  return _message.size();
}

string XMLMessage::GetValueString(string name, bool raw)
{
  string s1("&lt;"+name+"&gt;"), s2("&lt;/"+name+"&gt;");
  //cout<<s1<<"  "<<s2<<endl;
  int begin = this->text().find( s1 );
  int end = this->text().find( s2 );

  string value;
  if(begin>0)
  {
	 value = this->text().substr(begin, end-begin);
	 if(!raw){
		value.erase( 0, value.find("\n")+4 );
		value.erase( value.find("\n"), value.find("\n")+3 );
	 }
  }
  return value;
}

string XMLMessage::GetRValueString(string name1, string name2, bool raw)
{
  string value;
  XMLMessage buffer( this->GetValueString(name1, true) );
  //cout<<"buffer - "<<buffer<<endl;
  value = buffer.GetValueString( name2 );

  return value;
}

int XMLMessage::GetValueInt(string name)
{
  stringstream str;
  str << this->GetValueString(name, false);
  if( !str.str().size() )  return -99999;

  int value;
  str >> value;
  return value;
}
double XMLMessage::GetValueDouble(string name)
{
  stringstream str;
  str << this->GetValueString( name, false );
  if( !str.str().size() )  return -99999;

  double value;
  str >> value;
  return value;
}

int XMLMessage::GetRValueInt(string name1, string name2)
{
  stringstream str;
  str << this->GetRValueString( name1, name2, false );
  if( !str.str().size() )  return -99999;

  int value;
  str >> value;
  return value;
}

double  XMLMessage::GetRValueDouble(string name1, string name2)
{
  stringstream str;
  str << this->GetRValueString( name1, name2, false );
  if( !str.str().size() )  return -99999;

  double value;
  str >> value;
  return value;
}







