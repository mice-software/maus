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
#include <vector>

using namespace std;

class XMLMessage
{
  public :

    XMLMessage();
    XMLMessage( const std::string& text );

    virtual ~XMLMessage();

    void		readFile( const std::string& filename );
	 void		MakeRunInfoRequest(int run);
	 void		MakeCalibRequest(string v1, string detector);
	 void		NewTOFCalibRequest(string v1, int trigger, string xml);

    string		text() const;
    int			size() const;

	 string  GetValueString(string name, bool raw=false);
	 string  GetRValueString(string name1, string name2, bool raw=false);

	 string  GetCalibElement(int n, vector<string>& params);
	 //! The element n is returned. All parameters of this element are pushed in the vector params.

	 string GetCalibHeader(string& name, string& detector);
	 //! The header of the calibration is returned.

	 int     GetValueInt(string name);
	 double  GetValueDouble(string name);
	 int  	GetRValueInt(string name1, string name2);
	 double  GetRValueDouble(string name1, string name2);

  private :

	 string beginRequest();
	 string endRequest();

    string _message;

};

#endif

