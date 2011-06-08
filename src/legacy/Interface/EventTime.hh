#ifndef EVENTTIME_HH
#define EVENTTIME_HH 1

// EventTime.hh - class to hold the time of an event as retrieved from the LDC EVent information
//
// M. Ellis - 20th May 2009

class EventTime
{
  public :

    EventTime( unsigned int sec, unsigned int usec )
    {
       _sec = sec;
       _usec = usec;
    }

    ~EventTime()
    {};

    unsigned int 	seconds() const 	{ return _sec; };

    unsigned int	microSeconds() const	{ return _usec; };

    double		time() const;

  private :

    unsigned int 	_sec;

    unsigned int 	_usec;
};

#endif

