// MAUS WARNING: THIS IS LEGACY CODE.
// Reader.hh
//
// Interface for classes that will read data from any source
//
// M.Ellis 6th October 2006

#ifndef READER_HH
#define READER_HH 1

class Reader
{
  public :

    virtual ~Reader() {};

    virtual bool readEvent() = 0;

    virtual bool IsOk() const = 0;

    virtual void closeInputFile() = 0;

    virtual void skipEvents( int ) = 0;
};

#endif

