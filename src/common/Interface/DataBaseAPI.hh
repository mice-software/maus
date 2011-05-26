// DataBaseAPI.hh
//
// Interface to handle the network connection to the Configuration Database API
//
// M.Ellis June 2010

#ifndef DATABASEAPI_HH
#define DATABASEAPI_HH 1

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <string>

#include "Interface/XMLMessage.hh"

class HttpMessage
{
  public :

    HttpMessage( const std::string& hostname, const int& portno, const XMLMessage& xml );

    virtual ~HttpMessage();

    std::string	  completeMessage();

    int        	  size();

  private :

    std::string _header;
    std::string _xml;
    std::string _hostname;
    int         _portno;
};

class DataBaseAPI
{
  public :

    DataBaseAPI();
    DataBaseAPI( const std::string& hostname );
    DataBaseAPI( const int& portno );
    DataBaseAPI( const std::string& hostame, const int& portno );

	 bool 	openConnection();
    XMLMessage   sendQuery( const XMLMessage& query );

    virtual ~DataBaseAPI();

	 bool IsConnected()	const	{return connected;};


  private :

    void 	setDefault();

    int			_sockfd;
    std::string 	_hostname;
    int         	_portno;
    struct hostent* 	_server;

	 bool connected;
};

#endif

