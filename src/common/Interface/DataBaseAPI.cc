// DataBaseAPI.cc
//
// Interface to handle the network connection to the Configuration Database API
//
// M.Ellis June 2010

#include "Interface/DataBaseAPI.hh"

#include <sstream>

HttpMessage::HttpMessage( const std::string& hostname, const int& portno, const XMLMessage& xml )
{
  _hostname = hostname;
  _portno = portno;
  _xml = xml.text();

  std::stringstream str;

  str << "POST /soap/servlet/rpcrouter HTTP/1.1" << std::endl;
  str << "Content-Length: " << ( xml.size() + 1 ) << std::endl;
  str << "Content-Type: text/xml; charset=utf-8" << std::endl;
  str << "SOAPAction:" << std::endl;
  str << "User-Agent: Java/1.6.0_16" << std::endl;
  str << "Host: " << _hostname << ":" << _portno << std::endl;
  str << "Accept: text/html, image/gif, image/jpeg, *; q=.2, */*; q=.2" << std::endl;
  str << "Connection: keep-alive" << std::endl << std::endl;

  _header = str.str();

}

HttpMessage::~HttpMessage()
{
}

std::string	HttpMessage::completeMessage()
{
  std::string mess = _header + _xml;
  mess += std::string( "\n" );

  return mess;
}

int		HttpMessage::size()
{
  return ( _header.size() + _xml.size() + 1 );
}

DataBaseAPI::DataBaseAPI()
{
  setDefault();
}

DataBaseAPI::DataBaseAPI( const std::string& hostname )
{
  setDefault();
  _hostname = hostname;
}

DataBaseAPI::DataBaseAPI( const int& portno )
{
  setDefault();
  _portno = portno;
}

DataBaseAPI::DataBaseAPI( const std::string& hostname, const int& portno )
{
  setDefault();
  _hostname = hostname;
  _portno = portno;
  //openConnection();
}

XMLMessage DataBaseAPI::sendQuery( const XMLMessage& query )
{
  if( !openConnection() ) // then we can't send this query!
  {
	 connected = false;
	 return XMLMessage( std::string( "ERROR! No Connection" ) );
  }
  connected = true;
  // send the query to the socket
  HttpMessage mess( _hostname, _portno, query );
  std::string request = mess.completeMessage();
  //std::cout<< request <<std::endl;
  int n = write( _sockfd, request.c_str(), mess.size() );

  if( n < 0 )
  {
    perror( "Error sending the query" );
    return XMLMessage();
  }

  // now read the response
  char responseBuffer[4096];
  std::string response;
  bool done = false;
  while( !done )
  {
	 n = read( _sockfd, responseBuffer, 4096 );
	 //std::cout<<"nRead = "<<n<<std::endl;
	 if( n==0 )
	 {
		done = true;
		continue;
	 }
	 if( n < 0 )
	 {
		perror( "Error reading response from API" );
		return XMLMessage();
	 }
	 response.append( responseBuffer, n);
	 if(response.size()>20)
		done = !response.compare(response.size()-19,17,"SOAP-ENV:Envelope");
	 //std::cout<<"end - "<<response.substr(response.size()-19,17)<<std::endl;
	 //std::cout<<"response size = "<<response.size()<<"   done = "<<done<<std::endl;
  }

  close( _sockfd );

  return XMLMessage( response );
}

DataBaseAPI::~DataBaseAPI()
{}

void DataBaseAPI::setDefault()
{
  _sockfd = -1;
  _hostname = "micewww.pp.rl.ac.uk";
  _portno = 4443;
  connected = false;
}

bool DataBaseAPI::openConnection()
{
  // attempt to open a socket

  _sockfd = socket( AF_INET, SOCK_STREAM, 0 );

  if( _sockfd < 0 )
  {
    perror( "Error opening socket" );
    return false;
  }

  // resolve the server's name

  _server = gethostbyname( _hostname.c_str() );

  if( ! _server )
  {
     std::cerr << "Unable to resolve the server name " << _hostname << std::endl;
     return false;
  }

  // attempt to establish a connection to the server

  struct sockaddr_in serv_addr;

  // setup the server address information

  bzero( (char *) &serv_addr, sizeof( serv_addr ) );

  serv_addr.sin_family = AF_INET;

  bcopy( (char*) _server->h_addr, (char*) &serv_addr.sin_addr.s_addr, _server->h_length );

  // add the port number

  serv_addr.sin_port = htons( _portno );

  // open a connection

  if( connect( _sockfd, (const sockaddr*) &serv_addr, sizeof( serv_addr ) ) < 0 )
  {
    perror( "Error connect" );
    close( _sockfd );
    return false;
  }

  // the connection has been established

  return true;
}
