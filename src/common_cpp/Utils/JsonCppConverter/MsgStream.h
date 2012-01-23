#ifndef MSGSTREAM_H
#define MSGSTREAM_H

#include <iostream>
#include <string>
#include <map>

class MsgStream : public std::ostream{
 public:
  enum LEVEL{
    VERBOSE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
  };
  
  MsgStream(std::string, LEVEL =INFO, size_t =20, size_t =8);

  MsgStream(const MsgStream& msg);
  
  
  void setNameWidth(short nw){m_nameWidth = nw;}  
  void setLogLevelWidth(short llw){m_loglevelWidth = llw;}
  void setName(std::string name){m_name = name;}
  void setLogLevel(LEVEL loglevel){m_loglevel = loglevel;}
  const std::string& getName() const{ return m_name;}
  const LEVEL& getLogLevel() const{return m_loglevel;}
  const size_t& getNameWidth() const{return m_nameWidth;}
  const size_t& getLogLevelWidth() const{return m_loglevelWidth;}
  template<typename T> std::ostream& operator<<(T);
  
 private:
  std::string m_name;
  LEVEL m_loglevel;
  size_t m_nameWidth;
  size_t m_loglevelWidth;
  std::map<LEVEL,std::string> m_levelmap;
};


//this template catches the case when someone passes without a log
//level and just ignores whatever they pass. Annoying but enables
//us to ignore lines that are lower than loglevel.
template<typename T>
std::ostream& MsgStream::operator<<(T){
  return *this;
}
template<>
inline std::ostream& MsgStream::operator<< <MsgStream::LEVEL>(MsgStream::LEVEL loglevel)
{
  
  if(loglevel<m_loglevel) return *this;
  
  
  std::ostream* stream = &std::cout;
  if(loglevel == MsgStream::ERROR || loglevel == MsgStream::FATAL){
    stream = &std::cerr;
  }
  
  stream->width(m_nameWidth);
  if(m_name.length() > m_nameWidth - 1){
      *stream << std::left << m_name.substr(0,m_nameWidth - 4) <<"... : ";
  }
  else{
    *stream << std::left << m_name<<": ";
  }
  
  stream->width(m_loglevelWidth);
  *stream << std::left << m_levelmap[loglevel]<<": ";
  
  return *stream;
}

#endif
