#include "JsonRootConverter.h"
#include "MsgStream.h"
#include <fstream>

int main(int argc, char* argv[]){
  MsgStream m_log("main",MsgStream::INFO);
  
  if(argc!=3){
    m_log << MsgStream::ERROR <<"Expected two args, the first being the filname to convert and the second being the output filename." << std::endl;
    return 1;
  }



  JsonRootConverter j(m_log.getLogLevel());


  std::vector<std::string> trees;
  trees.push_back("digits");
  //trees.push_back("mc");


  if(!j.initialise(trees)){
    m_log<< MsgStream::ERROR <<"Failure in the initialisation"<<std::endl;
    return 2;
  }


  
  std::ifstream file(argv[1],std::ios::in);
  if (!file.is_open()){
    m_log << MsgStream::ERROR <<"file '"<<argv[1] <<"' did not open correctly"<<std::endl;
    return 3;
  }

  int eventNo = 0;
  std::string event;
  while(getline(file,event)){
    ++eventNo;
    if(!j.execute(event)){
      m_log<< MsgStream::ERROR <<"Failure in the execution of event "<<eventNo<<std::endl;
      return 4;
    }
  }

  m_log<< MsgStream::INFO <<"Writing out "<< eventNo <<" events."<<std::endl;
  if(!j.write(argv[2])){
    m_log<< MsgStream::ERROR <<"Failure in writing to output file."<<std::endl;
    return 5;
  }
  return 0;
}
