// TextFileIoBase.h
//
// M.Ellis 22/8/2005 - based on version developed for HARP


#ifndef PERSIST_TEXTFILEIOBASE_H
#define PERSIST_TEXTFILEIOBASE_H

#include <string>
#include <cstdio>
#include <cstdlib>
#include <vector>
// template <class T>

class TextFileIoBase
{
  public :

    TextFileIoBase()					{ m_index = -1; };

    virtual ~TextFileIoBase() 				{};

    int			txtIoIndex() const		{ return m_index; };
    void		setTxtIoIndex( int index )	{ m_index = index; };

    virtual std::string	storeObject() = 0;
    virtual void	restoreObject( std::string ) = 0;
    virtual void	completeRestoration() = 0;
//     std::string vectorToString(std::vector<T>);
    std::string vectorToString(std::vector<double> dVect);
    std::string vectorToString(std::vector<int>    iVect);
    std::vector<double> stringToVectorDouble(std::string s);
    std::vector<int>    stringToVectorInteger(std::string s);
    std::vector<double> streamToVectorDouble(std::istringstream &s);
    std::vector<int>    streamToVectorInteger(std::istringstream &s);
  private :

   int m_index;
};

#endif

