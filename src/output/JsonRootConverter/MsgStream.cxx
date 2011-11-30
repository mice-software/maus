#include "MsgStream.h"


MsgStream::MsgStream(std::string name,
		     MsgStream::LEVEL loglevel,
		     size_t nameWidth,
		     size_t loglevelWidth):
  m_name(name),
  m_loglevel(loglevel),
  m_nameWidth(nameWidth),
  m_loglevelWidth(loglevelWidth){
    m_levelmap[MsgStream::VERBOSE] = "VERBOSE";
    m_levelmap[MsgStream::DEBUG] = "DEBUG";
    m_levelmap[MsgStream::INFO] = "INFO";
    m_levelmap[MsgStream::WARNING] = "WARNING";
    m_levelmap[MsgStream::ERROR] = "ERROR";
    m_levelmap[MsgStream::FATAL] = "FATAL";
}

MsgStream::MsgStream(const MsgStream::MsgStream& msg){
  m_name = msg.m_name;
  m_loglevel = msg.m_loglevel;
  m_nameWidth = msg.m_nameWidth;
  m_loglevelWidth = msg.m_loglevelWidth;
  m_levelmap = std::map<MsgStream::LEVEL,std::string>(m_levelmap);
}


